/*
	This awesome code was written by Chris Roxby.
	This project was created using SDL.
*/

#include "classes.h"
#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>
#include <algorithm>
#include <ctime>

#define PROG_NAME "SDLitaire"
#define GAME_VERSION "0.01.00"

/* Window Defaults */
#define WINDOW_TITLE PROG_NAME
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

#define TRANSPARENT_COLOR 0xFF, 0, 0xFF /* Purple is rendered clear. */
#define RENDER_BGCOLOR 0x0, 0x64, 0x0, 0xFF /* Areas with no objects are medium-green. */

#define DOUBLECLICK_DELAY 250

const char* nameOfSuit(int suit)
{
	switch (suit)
	{
	case CLUBS:
		return "Clubs";
	case DIAMONDS:
		return "Diamonds";
	case HEARTS:
		return "Hearts";
	}
	return "Spades";
}

bool pointWithinBounds(int test_x, int test_y, int pos_x, int pos_y, int width, int height)
{
	if (pos_x <= test_x)
		if (test_x <= pos_x + width)
		{
			if (pos_y <= test_y)
				if (test_y <= pos_y + height)
				{
					return true;
				}
		}
	return false;
}

bool testRectCollision(SDL_Rect &rect1, SDL_Rect &rect2)
{
	bool xCollision = false;
	bool yCollision = false;

	if (rect1.x < rect2.x)
	{
		if (rect1.x + rect1.w >= rect2.x)
		{
			xCollision = true;
		}
	}
	else /* (rect1.x >= rect2.x) */
	{
		if (rect2.x + rect2.w >= rect1.x)
		{
			xCollision = true;
		}
	}

	if (rect1.y < rect2.y)
	{
		if (rect1.y + rect1.h >= rect2.y)
		{
			yCollision = true;
		}
	}
	else /* (rect1.y >= rect2.y) */
	{
		if (rect2.y + rect2.h >= rect1.y)
		{
			yCollision = true;
		}
	}
	return xCollision && yCollision;
}


Timer::Timer()
{
	stop(); /* set vars */
}

void Timer::start()
{
	/* Start the timer */
	mStarted = true;

	/* Unpause the timer */
	mPaused = false;

	/* Get the current clock time */
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void Timer::stop()
{
	mPaused = false;
	mStarted = false;

	mStartTicks = 0;
	mPausedTicks = 0;
}

void Timer::pause()
{
	/* If the timer is running and isn't already paused */
	if (mStarted && !mPaused)
	{
		/* Pause the timer */
		mPaused = true;

		/* Calculate the paused ticks */
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void Timer::unpause()
{
	/* If the timer is running and paused */
	if (mStarted && mPaused)
	{
		/* Unpause the timer */
		mPaused = false;

		/* Reset the starting ticks */
		mStartTicks = SDL_GetTicks() - mPausedTicks;

		/* Reset the paused ticks */
		mPausedTicks = 0;
	}
}

Uint32 Timer::getTicks()
{
	/* The actual timer time */
	Uint32 time = 0;

	/* If the timer is running */
	if (mStarted)
	{
		/* If the timer is paused */
		if (mPaused)
		{
			/* Return the number of ticks when the timer was paused */
			time = mPausedTicks;
		}
		else
		{
			/* Return the current time minus the start time */
			time = SDL_GetTicks() - mStartTicks;
		}
	}

	return time;
}

bool Timer::isStarted()
{
	/* Timer is running and paused or unpaused */
	return mStarted;
}

bool Timer::isPaused()
{
	/* Timer is running and paused */
	return mPaused && mStarted;
}


Texture::Texture()
{
	clear();
}
Texture::~Texture()
{
	free();
}

bool Texture::loadFromFile(std::string path, SDL_Renderer* renderer)
{
	free(); /* Get rid of any preexisting texture */
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (!loadedSurface)
	{
		printf("This image could not be loaded: %s\nSDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, TRANSPARENT_COLOR)); /* Color key image */
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface); /* Create texture from surface pixels */
		if (!newTexture)
		{
			printf("A texture could not be created from %s\nSDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
		SDL_FreeSurface(loadedSurface);
	}
	mTexture = newTexture;
	return mTexture != NULL;
}

bool Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* renderer)
{
	free(); /* Get rid of any preexisting texture */

	/* Render text surface */
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
	if (!textSurface)
	{
		printf("The text surface could not be rendered!\nSDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		/* Create texture from surface pixels */
		mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (!mTexture)
		{
			printf("The text texture could not be created!\nSDL Error: %s\n", SDL_GetError());
		}
		else
		{
			/* Get image dimensions */
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}
		/* Get rid of old surface */
		SDL_FreeSurface(textSurface);
	}
	return mTexture != NULL;
}

void Texture::free()
{
	/* Free texture if it exists */
	if (mTexture)
	{
		SDL_DestroyTexture(mTexture);
		clear();
	}
}

void Texture::clear()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

void Texture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	/* Modulate texture color */
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void Texture::setBlendMode(SDL_BlendMode blending)
{
	/* Set blending function */
	SDL_SetTextureBlendMode(mTexture, blending);
}
void Texture::setAlpha(Uint8 alpha)
{
	/* Modulate texture alpha */
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void Texture::setWidth(int width)
{
	if (width != 0)
		mWidth = abs(width);
}
void Texture::setHeight(int height)
{
	if (height != 0)
		mHeight = abs(height);
}

void Texture::aspectScale(int width, int height)
{
	if (!height && !width)
		return;

	/*
		Since this function only needs to know about the dimension that should be used for scaling,
		the following logic allows a zero to passed. (Since dimension determination is value-based)
	*/
	if (!height)
		height = INT_MAX;
	if (!width)
		height = INT_MAX;

	if (height < width)
	{
		if (mHeight == height)
			return;
		mWidth = (int)roundf(mWidth * ((float)height / (float)mHeight));
		mHeight = height;
	}
	else
	{
		if (mWidth == width)
			return;
		mHeight = (int)roundf(mHeight * ((float)width / (float)mWidth));
		mWidth = width;
	}
}

void Texture::render(SDL_Renderer* renderer, int x, int y,
	SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	/* Set rendering space and render to screen */
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	/* Set clip rendering dimensions */
	if (clip)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	/* Render to screen */
	//* Hack!
	if (mTexture < (void*)0x40000)
	{
		printf("ATTEMPTED TO RENDER AN UNKNOWN TEXTURE!\n");
		return;
	}
	//*/
	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
}


Card::Card()
{
	mTexture = NULL;
	mClickable =
		mDragging =
		mSliding =
		mFaceUp = false;
	mOffsetX =
	mOffsetY =
	mPosX =
		mPosY =
		mVelX =
		mVelY =
		mFile =
		mRank =
		mDestRank = 0;
	mFace.suit = SPADES;
	mFace.value = ACE;
	mLastClickTime = GetCurrentTime();
}

void Card::handleEvent(SDL_Event& e)
{
	if (e.type == SDL_MOUSEMOTION)
	{
		if (mDragging)
		{
			/* Get mouse position */
			int x, y;
			SDL_GetMouseState(&x, &y);
			mPosX = x + mOffsetX;
			mPosY = y + mOffsetY;
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if ((e.button.button == SDL_BUTTON_LEFT) && (mClickable))
		{
			/* Get mouse position */
			int x, y;
			SDL_GetMouseState(&x, &y);
			if (pointWithinBounds(x, y, mPosX, mPosY, mTexture->getWidth(), mTexture->getHeight()))
			{
				DWORD newClickTime = GetCurrentTime();
				if (mFaceUp)
				{
					/* If mLastClickTime was recent */
					if (newClickTime - DOUBLECLICK_DELAY <= mLastClickTime)
					{
						//Try to auto-move
					}
					else /* Maybe this else should be like an if(!automove) ? */
					{
						mDragging = true;
						mOffsetX = mPosX - x;
						mOffsetY = mPosY - y;
					}
				}
				else
				{
					if (mRank == 0)
					{
						dealTo(1);
					}
					flip();
				}
				mLastClickTime = newClickTime;
			}
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (mDragging)
			{
				mDragging = false;
				mOffsetX = 0;
				mOffsetY = 0;
				land();
			}
		}
	}
}

void Card::move(int timeStep)
{
	if (mDestRank == mRank)
	{
		return;
	}

	/* FIXME: Is there any y-drift when the DestRank fills up? */
	point* dest = mTable->getCardPlace(mDestRank);

	bool xCond = mPosX != dest->x;
	bool yCond = mPosY != dest->y;

	/*
		FIXME:BUGBUG: Sometimes cards miss and fly forever
			Moving in big steps with little tolerance?
	*/
	if (mTable->options()->animation)
	{
		if (xCond)
		{
			mPosX += mVelX * timeStep;
		}

		if (yCond)
		{
			mPosY += mVelY * timeStep;
		}
	}
	else
	{
		mPosX = dest->x;
		mPosY = dest->y;
	}

	xCond = mPosX != dest->x;
	yCond = mPosY != dest->y;

	if (yCond || xCond)
	{
		mSliding = true;
	}
	else
	{
		mSliding = false;
		land();
	}
}

void Card::flip()
{
	if (!mFaceUp)
	{
		setTexture(mTable->getCardTexture(mFace.suit, mFace.value));
	}
	else
	{
		setTexture(mTable->getCardBack());
	}
	mFaceUp = !mFaceUp;
}

void Card::setTexture(Texture* texture)
{
	mTexture = texture;
}

void Card::render(SDL_Renderer* renderer, point* slot)
{
	if (!mDragging && !mSliding)
	{
		mPosX = slot->x;
		mPosY = slot->y;
		if (mRank > 5)
		{
			mPosY += (mFile * (mTexture->getHeight() / 10 ));
		}
	}
	mTexture->render(renderer, mPosX, mPosY);
}

void Card::setRank(int rank)
{
	if (0 > rank || rank > CARD_RANKS)
	{
		rank = 0;
	}
	mRank = rank;
	mDestRank = rank;
}
void Card::setFile(int file)
{
	if (0 > file || file > NUM_CARDS)
	{
		file = 0;
	}
	mFile = file;
}

void Card::setClickability(bool state)
{
	mClickable = state;
}

void Card::setFace(cardFace face)
{
	if (((face.suit >= SPADES) && (face.suit < NUM_SUITS)) &&
		((face.value >= ACE) && (face.value <= KING)))
	{
		mFace = face;
	}
}

void Card::dealTo(int rank)
{
	if ((0 <= rank) && (rank < CARD_RANKS))
	{
		if (mRank != rank)
		{
			mDestRank = rank;
			mSliding = true;
			if (mTable->options()->animation)
			{
				mVelY = CARD_VEL;
				mVelX = CARD_VEL;
				if (mTable->getCardPlace(rank)->y < mTable->getCardPlace(mRank)->y) //BUGBUG: Head a little further down for each card that is already there
				{
					mVelY *= -1;
				}
				if (mTable->getCardPlace(rank)->x < mTable->getCardPlace(mRank)->x)
				{
					mVelX *= -1;
				}
			}
		}
	}
}

void Card::setDestRank(int rank)
{
	if ((0 > rank) || (rank > CARD_RANKS))
	{
		rank = 0;
	}
	mDestRank = rank;
}

void Card::land()
{
	mTable->cardDrop(this);
}

void Card::assocGame(AssetManager& game)
{
	mTable = &game;
	mTable->registerCard(this);
}


Window::Window()
{
	mWindow = NULL;
	mRenderer = NULL;
	mMouseFocus =
	mKeyboardFocus =
	mFullScreen =
	mMinimized = false;
	mWidth =
	mHeight = 0;
}

bool Window::init()
{
	mWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!mWindow)
	{
		printf("The window could not be created!\n");
		return false;
	}

	mMouseFocus =
		mKeyboardFocus = true;
	mWidth = SCREEN_WIDTH;
	mHeight = SCREEN_HEIGHT;

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(mWindow, &info))
	{
		std::stringstream ssGmeVer, ssSDLver, ssTTFver, ssMIXver, ssIMGver;
		ssGmeVer << PROG_NAME << " version " << GAME_VERSION;
		ssSDLver << "SDL main version " << SDL_MAJOR_VERSION << "." << SDL_MINOR_VERSION << "." << SDL_PATCHLEVEL;
		ssTTFver << "SDL TTF version " << SDL_TTF_MAJOR_VERSION << "." << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_PATCHLEVEL;
		ssMIXver << "SDL Mixer version " << SDL_MIXER_MAJOR_VERSION << "." << SDL_MIXER_MINOR_VERSION << "." << SDL_MIXER_PATCHLEVEL;
		ssIMGver << "SDL Image version " << SDL_IMAGE_MAJOR_VERSION <<"."<< SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_PATCHLEVEL;

		HMENU hMenubar = CreateMenu();
		HMENU hGame = CreateMenu();
		HMENU hAbout = CreateMenu();
		AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hGame, "&Game");
		AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hAbout, "&About");
		AppendMenu(hGame, MF_STRING, MENU_EXIT, "&Exit");
		AppendMenu(hAbout, MF_STRING, NULL, "This awesome clone was created by Chris Roxby.");
		AppendMenu(hAbout, MF_STRING, NULL, ssGmeVer.str().c_str());
		AppendMenu(hAbout, MF_SEPARATOR, NULL, "");
		AppendMenu(hAbout, MF_STRING, NULL, ssTTFver.str().c_str());
		AppendMenu(hAbout, MF_STRING, NULL, ssMIXver.str().c_str());
		AppendMenu(hAbout, MF_STRING, NULL, ssIMGver.str().c_str());
		SetMenu(info.info.win.window, hMenubar);
		return true;
	}
	else
	{
		printf("The menu bar could not be created!\n");
		return false;
	}
}

SDL_Renderer* Window::createRenderer()
{
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
	return mRenderer;
}

void Window::handleEvent(SDL_Event& e)
{
	if (e.type == SDL_WINDOWEVENT)
	{
		switch (e.window.event)
		{
			/* Get new dimensions and repaint on window size change */
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent(mRenderer);
			break;

			/* Repaint on exposure */
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(mRenderer);
			break;

		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			break;
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			break;

		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			break;

		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimized = true;
			break;
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
			break;
		case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			break;
		}
	}
	/* Enter\exit full screen */
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
	{
		const Uint8 *kBstate = SDL_GetKeyboardState(NULL);
		if (kBstate[SDL_SCANCODE_LALT] || kBstate[SDL_SCANCODE_RALT])
		{
			if (mFullScreen)
			{
				SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
				mFullScreen = false;
			}
			else
			{
				SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
				mFullScreen = true;
				mMinimized = false;
			}
		}
	}
}

void Window::free()
{
	if (mRenderer != NULL)
	{
		SDL_DestroyRenderer(mRenderer);
	}
	if (mWindow != NULL)
	{
		SDL_DestroyWindow(mWindow);
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int Window::getWidth()
{
	return mWidth;
}
int Window::getHeight()
{
	return mHeight;
}
bool Window::hasMouseFocus()
{
	return mMouseFocus;
}
bool Window::hasKeyboardFocus()
{
	return mKeyboardFocus;
}
bool Window::isMinimized()
{
	return mMinimized;
}


AssetManager::AssetManager()
{
	Close(); /* Null init pointers */

	/* The whole deck is in the draw pile for the first tick. */
	mHeldCards[0] = 52;
	for (int i = 1; i < CARD_RANKS; i++)
	{
		mHeldCards[i] = 0;
	}
}

AssetManager::~AssetManager()
{
	/* Deallocate */
	mFPSTextTexture.free();
	mDeckTexture.free();
	mBackgroundTexture.free();

	/* Free the font */
	TTF_CloseFont(mFont);

	/* Free the sound effects */
	Mix_FreeChunk(mSound);

	/* Destroy window */
	mWindow.free();
	SDL_DestroyRenderer(mRenderer);

	/* NULL out pointers */
	Close();

	/* Quit SDL subsystems */
	Mix_CloseAudio();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool AssetManager::Init()
{
	bool success = true;

	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize!\nSDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		/* Set texture filtering quality */
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2")) /* "2" = "best", but currently "best" = "linear" */
		{
			printf("Texture filtering could not be enabled!");
		}

		/* Create window */
		if (!mWindow.init())
		{
			printf("The window could not be created!\nSDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			/* Create renderer for window */
			mRenderer = mWindow.createRenderer();
			if (!mRenderer)
			{
				printf("The renderer could not be created!\nSDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				/* Initialize renderer color */
				SDL_SetRenderDrawColor(mRenderer, RENDER_BGCOLOR);

				/* Initialize PNG loading */
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize!\nSDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				/* Initialize SDL_ttf */
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize!\nSDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}

				/* Initialize SDL_mixer */
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0)
				{
					printf("SDL_mixer could not initialize!\nSDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}
	return success;
}

bool AssetManager::LoadMedia()
{
	bool success = true;

	/* Open the font */
	mFont = TTF_OpenFont("C:/Windows/Fonts/calibri.ttf", 28);
	if (!mFont)
	{
		printf("The font could not be loaded!\nSDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}

	if (!mBackgroundTexture.loadFromFile("table.png", mRenderer))
	{
		printf("The background texture could not be loaded!\n");
		success = false;
	}

	/* Load sound effect */
	mSound = Mix_LoadWAV("fire.wav");
	if (!mSound)
	{
		printf("The sound effect could not be loaded!\nSDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	/* Load deck texture */
	if (!mDeckTexture.loadFromFile("cards/back.png", mRenderer))
	{
		printf("The deck texture could not be loaded!\n");
		success = false;
	}

	/* Load outline texture */
	if (!mOutlineTexture.loadFromFile("cards/outline.png", mRenderer))
	{
		printf("The outline texture could not be loaded!\n");
		success = false;
	}

	/* Load face textures */
	/* Init to back */
	for (int i = 0; i < NUM_SUITS; i++)
	{
		for (int j = 0; j <= NUM_FACES; j++)
		{
			/* FIXME: Can't we just set these = mDeckTexture ? */
			if (!mFaceTextures[i][j].loadFromFile("cards/back.png", mRenderer))
			{
				printf("The card back texture could not be loaded!\n");
				success = false;
				break;
			}
		}
	}

	/*
		Yes, I'm pretending that index 0 doesn't exist.
		This way 1 = Ace, 2 = 2, ... (13 = King)
	*/
	for (int i = 0; i < NUM_SUITS; i++)
	{
		const std::string suitName (nameOfSuit(i));
		for (int j = 1; j <= NUM_FACES; j++)
		{
			std::stringstream filename;
			filename << "cards/" << suitName << "/" << j << ".png";
			if (!mFaceTextures[i][j].loadFromFile(filename.str(), mRenderer))
			{
				printf("The texture could not be loaded for the %i of %s!\n", j, suitName.c_str());
				success = false;
				break;
			}
			/* I don't understand the math either... */
			mAllFaces[(i * NUM_FACES) + (j - 1)].suit = i;
			mAllFaces[(i * NUM_FACES) + (j - 1)].value = j;
		}
	}

	srand(unsigned(time(NULL)));
	/* Serious (new) C++ magic */
	std::random_shuffle(std::begin(mAllFaces), std::end(mAllFaces));

	return success;
}

void AssetManager::Close()
{
	mRenderer = NULL;
	mSDLWindow = NULL;
	mIconSurface = NULL;
	mFont = NULL;
	mSound = NULL;
}

void AssetManager::clearRenderer()
{
	SDL_SetRenderDrawColor(mRenderer, RENDER_BGCOLOR);
	SDL_RenderClear(mRenderer);
}

void AssetManager::computeCardPlaces()
{
	int cardW = mDeckTexture.getWidth();
	int cardH = mDeckTexture.getHeight();

	int margin_y = max(cardW / 8, 4);
	int margin_x = max((mWindow.getWidth() - (cardW * 7)) / 8, 4);

	mCardPlaces[0].y = margin_y;
	mCardPlaces[1].y = margin_y;
	mCardPlaces[2].y = margin_y;
	mCardPlaces[3].y = margin_y;
	mCardPlaces[4].y = margin_y;
	mCardPlaces[5].y = margin_y;

	mCardPlaces[6].y = margin_y * 2 + cardH;
	mCardPlaces[7].y = margin_y * 2 + cardH;
	mCardPlaces[8].y = margin_y * 2 + cardH;
	mCardPlaces[9].y = margin_y * 2 + cardH;
	mCardPlaces[10].y = margin_y * 2 + cardH;
	mCardPlaces[11].y = margin_y * 2 + cardH;
	mCardPlaces[12].y = margin_y * 2 + cardH;

	mCardPlaces[0].x = margin_x;
	mCardPlaces[6].x = margin_x;

	mCardPlaces[1].x = mCardPlaces[0].x + margin_x + cardW;
	mCardPlaces[7].x = mCardPlaces[6].x + margin_x + cardW;

	mCardPlaces[8].x = mCardPlaces[7].x + margin_x + cardW;

	mCardPlaces[2].x = mCardPlaces[8].x + margin_x + cardW;
	mCardPlaces[9].x = mCardPlaces[8].x + margin_x + cardW;

	mCardPlaces[3].x = mCardPlaces[2].x + margin_x + cardW;
	mCardPlaces[10].x = mCardPlaces[2].x + margin_x + cardW;

	mCardPlaces[4].x = mCardPlaces[3].x + margin_x + cardW;
	mCardPlaces[11].x = mCardPlaces[3].x + margin_x + cardW;

	mCardPlaces[5].x = mCardPlaces[4].x + margin_x + cardW;
	mCardPlaces[12].x = mCardPlaces[4].x + margin_x + cardW;
}

void AssetManager::cardDrop(Card* card)
{
	int oldRank = card->getRank();
	int oldFile = card->getFile();

	SDL_Rect cardRect;
	SDL_Rect rankRect;

	cardRect.w = mDeckTexture.getWidth();
	cardRect.h = mDeckTexture.getHeight();
	cardRect.x = card->getX();
	cardRect.y = card->getY();

	rankRect.w = cardRect.w;
	rankRect.h = cardRect.h;
	
	mRanks[oldRank][oldFile] = nullptr;
	if (oldFile > 0)
	{
		if (mRanks[oldRank][oldFile - 1])
		{
			mRanks[oldRank][oldFile - 1]->setClickability(true);
		}
	}

	for (int i = 0; i < CARD_RANKS; i++)
	{
		/* FIXME: Each rank's "drop zone" does not expand down as it fills */
		rankRect.x = mCardPlaces[i].x;
		rankRect.y = mCardPlaces[i].y;

		if (testRectCollision(cardRect, rankRect))
		{
			for (int j = 0; j < NUM_CARDS; j++)
			{
				if (!mRanks[i][j])
				{
					--mHeldCards[card->getRank()];
					card->setRank(i);
					card->setFile(j);
					mRanks[i][j] = card;
					mHeldCards[card->getRank()]++;
					if (j > 0)
					{
						if (mRanks[i][j - 1])
						{
							mRanks[i][j - 1]->setClickability(false);
						}
					}
					card->setClickability(true);
					return;
				}
			}
		}
	}
}

void AssetManager::registerCard(Card* card)
{
	mRanks[card->getRank()][card->getFile()] = card;
}

Texture* AssetManager::getCardTexture(int suit, int value)
{
	if ((suit < SPADES) || (suit >= NUM_SUITS))
		suit = SPADES;
	if ((value < ACE) || (value > NUM_FACES))
		value = ACE;
	return& mFaceTextures[suit][value];
}

void AssetManager::handleEvent(SDL_Event& e)
{
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			/* Get mouse position */
			int x, y;
			SDL_GetMouseState(&x, &y);
			if (pointWithinBounds(x, y, mCardPlaces[0].x, mCardPlaces[0].y, mOutlineTexture.getWidth(), mOutlineTexture.getHeight()))
			{
				if (!mRanks[0][0])
				{
					//This loop runs backward to put the draw pile back like it was.
					for (int i = NUM_CARDS; i >= 0; i--)
					{
						if (mRanks[1][i])
						{
							mRanks[1][i]->dealTo(0);
							mRanks[1][i]->flip();
						}
					}
				}
			}
		}
	}
}