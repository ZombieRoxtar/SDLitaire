/*
	This awesome code was written by Chris Roxby.
*/

#include "classes.h"
#include <sstream>

#define EXIT_FAILED_INIT 2
#define EXIT_FAILED_FILES 3

#define CARD_SCALE 4

int main(int argc, char* args[])
{
	/* Start up SDL and create the window */
	LAssetManager gameManager;
	if (!gameManager.Init())
	{
		printf("Failed to initialize!\n");
		return EXIT_FAILED_INIT;
	}
	if (!gameManager.LoadMedia()) /* Load Media */
	{
		printf("Failed to load media!\n");
		gameManager.Close(); /* Try to free resources and close SDL */
		return EXIT_FAILED_FILES;
	}

	bool quit = false; /* Loop flag */

	LWindow* gameWindow = gameManager.getWindow();
	SDL_Renderer* gameRenderer = gameManager.getRenderer();
	TTF_Font* font = gameManager.getFont();
	LTexture* fpsTexture = gameManager.getFPSTexture();
	LTexture* backgroundTexture = gameManager.getBackground();
	LTexture* deckTexture = gameManager.getCardBack();
	LTexture* outlineTexture = gameManager.getCardOutline();

	/* May be faster to store these */
	int winH = gameWindow->getHeight();
	int winW = gameWindow->getWidth();

	/* The Cards */
	LCard* card[NUM_CARDS];

	for (int i = 0; i < NUM_CARDS; i++)
	{
		card[i] = new LCard;
		card[i]->setFile(NUM_CARDS - i);
		card[i]->assocGame(gameManager);
		card[i]->setTexture(deckTexture);
	}

	/* Initial scaling should happen as soon as possible */
	deckTexture->aspectScale(winW / CARD_SCALE, winH / CARD_SCALE);
	outlineTexture->aspectScale(winW / CARD_SCALE, winH / CARD_SCALE);
	gameManager.computeCardPlaces();

	for (int i = 0; i < NUM_CARDS; i++)
	{
		// Values here?
	}

	/* Deal Cards */
	int rank = 6; /* Rank 6 is the first one in the second row */
	int file = 1;
	for (int i = 0; i < FIRST_DEAL; i++)
	{
		if (rank == CARD_RANKS)
		{
			rank = 6 + file;
			file++;
		}
		card[i]->dealTo(rank);
		rank++;
	}
	card[FIRST_DEAL + 1]->setClickability(true);

	SDL_Event e; /* Event handler */
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE); /* Allow standard window events to process */

	LTimer stepTimer; /* Keeps track of time between card steps */
	LTimer fpsTimer; /* The frames per second timer */

	SDL_Color textColor = { 0, 0, 0, 255 }; /* Set text color */
	std::stringstream timeText; /* In memory text stream */

	/* Start counting frames per second */
	int countedFrames = 0;
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

		/* Set FPS text to be rendered */
		timeText.str("");
		timeText << "FPS: " << roundf(avgFPS);

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

			gameManager.computeCardPlaces();

			backgroundTexture->render(gameRenderer, 0, 0);

			for (int i = 0; i < CARD_RANKS; i++)
			{
				if (i != 1)
				{
					outlineTexture->render(gameRenderer, gameManager.getCardPlace(i)->x, gameManager.getCardPlace(i)->y);
				}
			}

			for (int i = 0; i < CARD_RANKS; i++) /* BOOM! Implicit Z ordering */
			{
				for (int j = 0; j < NUM_CARDS; j++)
				{
					if (gameManager.getCard(i, j))
					{
						gameManager.getCard(i, j)->render(gameRenderer, gameManager.getCardPlace(i));
					}
				}
			}

			fpsTexture->render(gameRenderer, (winW - fpsTexture->getWidth()), 0);

			SDL_RenderPresent(gameRenderer); /* Update screen */
			countedFrames++;
		}
		Sleep(0);
	}
	gameManager.Close(); /* Free resources and close SDL */
	return EXIT_SUCCESS;
}
