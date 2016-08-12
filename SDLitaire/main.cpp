/*
	This awesome code was written by Chris Roxby.
*/

#include "classes.h"

/* Exit code 0 is success and 1 is a generic failure */
#define EXIT_FAILED_INIT 2
#define EXIT_FAILED_FILES 3

#define CARD_SCALE 4

#define TEXT_COLOR 0, 0, 0 /* Black */

int main(int argc, char* args[])
{
#if _DEBUG
	SetConsoleTitle("Debug Output");
#endif // DEBUG

	/* Start up SDL and create the window */
	AssetManager gameManager;
	if (!gameManager.Init())
	{
#if _DEBUG
		system("pause");
#endif // DEBUG
		return EXIT_FAILED_INIT;
	}

	if (!gameManager.LoadMedia()) /* Load Media */
	{
		gameManager.Close(); /* Try to free resources and close SDL */
#if _DEBUG
		system("pause");
#endif // DEBUG
		return EXIT_FAILED_FILES;
	}

	bool quit = false; /* Loop flag */

	Window* gameWindow = gameManager.getWindow();
	SDL_Renderer* gameRenderer = gameManager.getRenderer();
	TTF_Font* font = gameManager.getFont();
	Texture* fpsTexture = gameManager.getFPSTexture();
	Texture* backgroundTexture = gameManager.getBackground();
	Texture* deckTexture = gameManager.getCardBack();
	Texture* outlineTexture = gameManager.getCardOutline();

	/* May be faster to store these */
	int winH = gameWindow->getHeight();
	int winW = gameWindow->getWidth();

	/* The Cards */
	Card* card[NUM_CARDS];

	for (int i = 0; i < NUM_CARDS; i++)
	{
		card[i] = new Card;
		card[i]->setFile(NUM_CARDS - i);
		card[i]->assocGame(gameManager);
		card[i]->setTexture(deckTexture);
		card[i]->setFace(gameManager.getFace(i)); /* Face values are shuffled earlier */
	}

	/* Initial scaling should happen as soon as possible */
	deckTexture->aspectScale(winW / CARD_SCALE, winH / CARD_SCALE);
	outlineTexture->aspectScale(winW / CARD_SCALE, winH / CARD_SCALE);
	gameManager.computeCardPlaces();

	/* Deal Cards */
	int rank = 6; /* Rank 6 is the first one in the second row */
	int file = 1;
	card[0]->flip(); //FIXME: Write a delayed flipper
	for (int i = 0; i < FIRST_DEAL; i++)
	{
		if (rank == CARD_RANKS)
		{
			rank = 6 + file;
			file++;
			card[i]->flip(); /* When a new row starts, flip the first card */
		}
		card[i]->dealTo(rank);
		rank++;
	}
	card[FIRST_DEAL]->setClickability(true); /* Top Deck Card */

	SDL_Event e; /* Event handler */
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE); /* Allow standard window events to process */

	Timer stepTimer; /* Keeps track of time between card steps */
	Timer fpsTimer; /* The frames per second timer */

	SDL_Color textColor = { TEXT_COLOR }; /* Set text color */
	std::stringstream timeText; /* In-memory text stream */

	Card* draggingCard = NULL; /* This is set in the render loop so it can render last */

	/* Start counting frames per second */
	int countedFrames = 0;
	if (gameManager.options()->showFPS)
		fpsTimer.start();

	/* While it's not quiting time */
	while (!quit)
	{
		/* Handle events in queue */
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			/* Handle input for the cards */
			for (int i = 0; i < NUM_CARDS; i++)
			{
				card[i]->handleEvent(e);
			}

			gameManager.handleEvent(e);

			/* Handle window events */
			gameWindow->handleEvent(e);

			/* Window menu events are of this type */
			if (e.type == SDL_SYSWMEVENT)
			{
				if (e.syswm.msg->msg.win.msg == WM_COMMAND) /* and this type */
				{
					switch (e.syswm.msg->msg.win.wParam)
					{
					case ID_EXIT:
						quit = true;
						break;
					}
				}
			}
		}

		/* Calculate and correct fps */
		float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
		if (gameManager.options()->showFPS)
		{
			/* Set FPS text to be rendered */
			timeText.str("");
			timeText << "FPS: " << roundf(avgFPS);
		}

		/* Object Processing */
		for (int i = 0; i < NUM_CARDS; i++)
		{
			card[i]->move(stepTimer.getTicks());
		}

		stepTimer.start(); /* Restart step timer */

		/* Only draw when not minimized */
		if (!gameWindow->isMinimized())
		{
			gameManager.clearRenderer(); /* Clear screen */

			/* Create FPS text texture*/
			if (gameManager.options()->showFPS)
				if (!fpsTexture->loadFromRenderedText(timeText.str().c_str(), textColor, font, gameRenderer))
				{
					printf("Unable to render FPS texture!\n");
				}

			winH = gameWindow->getHeight();
			winW = gameWindow->getWidth();
			/* Render objects */
			backgroundTexture->setWidth(winW);
			backgroundTexture->setHeight(winH);
			deckTexture->aspectScale(winW / CARD_SCALE, winH / CARD_SCALE);
			outlineTexture->aspectScale(winW / CARD_SCALE, winH / CARD_SCALE);
			for (int i = 0; i < NUM_SUITS; i++)
			{
				for (int j = 1; j <= NUM_FACES; j++)
				{
					gameManager.getCardTexture(i, j)->aspectScale(winW / CARD_SCALE, winH / CARD_SCALE);
				}
			}

			gameManager.computeCardPlaces();

			backgroundTexture->render(gameRenderer, 0, 0);

			for (int i = 0; i < CARD_RANKS; i++)
			{
				if (i != 1) /* No outline for the discard pile */
				{
					outlineTexture->render(gameRenderer, gameManager.getCardPlace(i)->x, gameManager.getCardPlace(i)->y);
				}
			}

			for (int i = 0; i < CARD_RANKS; i++) /* BOOM! Implicit Z ordering */
			{
				Card* tempCard = nullptr;
				for (int j = 0; j < NUM_CARDS; j++)
				{
					if (tempCard = gameManager.getCard(i, j))
					{
						if (!tempCard->isDragging())
						{
							tempCard->render(gameRenderer, gameManager.getCardPlace(i));
						}
						else
						{
							draggingCard = tempCard;
						}
					}
				}
			}
			if (draggingCard) /* Dragging card is rendered last */
			{
				draggingCard->render(gameRenderer, gameManager.getCardPlace(draggingCard->getRank()));
			}

			if (gameManager.options()->showFPS)
			{
				fpsTexture->render(gameRenderer, (winW - fpsTexture->getWidth()), 0);
				countedFrames++;
			}

			SDL_RenderPresent(gameRenderer); /* Update screen */
		}
		Sleep(0);
	}
	gameManager.Close(); /* Free resources and close SDL */
	return EXIT_SUCCESS;
}
