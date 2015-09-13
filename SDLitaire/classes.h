/*
	This awesome code was written by Chris Roxby.
*/
#ifndef _CLASSES_H
#define _CLASSES_H

#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <SDL_SysWM.h>

#define NUM_CARDS 52
#define FIRST_DEAL 28

#define SUIT_SIZE 13
#define CARD_RANKS SUIT_SIZE

#define ID_EXIT 1

class LTimer;
class LTexture;
class LCard;
class LWindow;
class LAssetManager;

struct point
{
	int x;
	int y;
};

static const enum SUITS
{
	ACES,
	CLUBS,
	DIAMONDS,
	HEARTS,
	NUM_SUITS
};

static const enum FACES
{
	ACE = 1,
	JACK = 11,
	QUEEN,
	KING,
	NUM_FACES
};

/* The application-time based timer */
class LTimer
{
public:
	LTimer();

	/* The various clock actions */
	void start();
	void stop();
	void pause();
	void unpause();

	/* Gets the timer's time */
	Uint32 getTicks();

	/* Checks the status of the timer */
	bool isStarted();
	bool isPaused();

private:
	/* The clock time when the timer started */
	Uint32 mStartTicks;

	/* The ticks stored when the timer was paused */
	Uint32 mPausedTicks;

	/* The timer status */
	bool mStarted,
		mPaused;
};

/* Texture wrapper class */
class LTexture
{
public:
	LTexture();
	~LTexture();

	/* Loads image at specified path */
	bool loadFromFile(std::string path, SDL_Renderer* renderer);

	/* Creates image from font string */
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* renderer);

	/* Deallocates texture */
	void free();

	/* Emptys vars */
	void clear();

	/* Set color modulation */
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	/* Set blending */
	void setBlendMode(SDL_BlendMode blending);

	/* Set alpha modulation */
	void setAlpha(Uint8 alpha);

	/* Set dimensions */
	void setWidth(int width);
	void setHeight(int height);
	void aspectScale(int width, int height);

	/* Renders texture at given point */
	void render(SDL_Renderer* renderer, int x, int y,
		SDL_Rect* clip = NULL, double angle = 0.0,
		SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	/* Gets image dimensions */
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }

private:
	/* The actual hardware texture */
	SDL_Texture* mTexture;

	/* Image dimensions */
	int mWidth,
		mHeight;
};

/* The Cards */
class LCard
{
public:
	/* Velocity */
	static const int CARD_VEL = 2;

	/* Initializes the variables */
	LCard();

	/* Assign texture */
	void setTexture(LTexture* texture);

	void handleEvent(SDL_Event& e);

	void move(int timeStep);

	/* Shows the card on the screen */
	void render(SDL_Renderer* renderer, point* rank);

	void setRank(int rank);

	void setFile(int file);

	void setClickability(bool state);

	void dealTo( int rank );

	void setDestRank(int rank);

	void land();

	void assocGame(LAssetManager& game);

	bool getClickability() { return mClickable; }
	int getRank() { return mRank; }
	int getDestRank() { return mDestRank; }
	int getFile() { return mFile; }
	int getX() { return mPosX; }
	int getY() { return mPosY; }

private:
	/* The X,Y Position and Spot on the Table*/
	int mPosX, mPosY, mRank, mFile;
	/* For sliding */
	int mVelX, mVelY, mDestRank;
	bool mSliding;

	/* Mouse data */
	int mOffsetX, mOffsetY;
	bool mDragging;
	bool mClickable;
	DWORD mLastClickTime;

	/* The Texture */
	LTexture* mTexture;

	LAssetManager* mTable;
};

/* Window Wrapper Class */
class LWindow
{
public:
	LWindow();

	/* Creates window */
	bool init();

	/* Creates renderer from internal window */
	SDL_Renderer* createRenderer();

	/* Handles window events */
	void handleEvent(SDL_Event& e);

	/* Deallocates internals */
	void free();

	/* Window handling */
	SDL_Window* getSDLWindow() { return mWindow; }
	SDL_Renderer* getRenderer() { return mRenderer; }

	/* Window dimensions */
	int getWidth();
	int getHeight();

	/* Window focii */
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();

private:
	/* Window data */
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	/* Window dimensions */
	int mWidth,
		mHeight;

	/* Window focii */
	bool mMouseFocus,
		mKeyboardFocus,
		mFullScreen,
		mMinimized;
};

/* Manages Textures and SDL */
class LAssetManager
{
public:
	LAssetManager();
	~LAssetManager();
	bool Init();
	bool LoadMedia();
	void Close();
	void clearRenderer();
	void computeCardPlaces();
	void cardDrop(LCard* card);
	void registerCard(LCard* card);

	LWindow* getWindow() { return& mWindow; }
	SDL_Renderer* getRenderer() { return mRenderer; }
	TTF_Font* getFont() { return mFont; }
	Mix_Chunk* getSound() { return mSound; }
	LTexture* getBackground() { return& mBackgroundTexture; }
	LTexture* getFPSTexture() { return& mFPSTextTexture; }
	LTexture* getCardBack() { return& mDeckTexture; }
	LTexture* getCardOutline() { return& mOutlineTexture; }
	point* getCardPlace(int place) { return& mCardPlaces[place]; }

private:
	/* Window data */
	LWindow mWindow;
	SDL_Window* mSDLWindow;
	SDL_Renderer* mRenderer;
	SDL_Surface* mIconSurface; /* Will be the window icon */
	/* Game Data */
	TTF_Font* mFont; /* Main Font */
	Mix_Chunk* mSound; /* Example Sound Effect */
	LTexture mBackgroundTexture; /* Backdrop (Table) */
	LTexture mFPSTextTexture; /* Rendered FPS Count */
	LTexture mDeckTexture; /* The Card Back */
	LTexture mOutlineTexture; /* The Card Outline */
	point mCardPlaces[CARD_RANKS]; /* Card Holding Spots */
	LCard* mRanks[CARD_RANKS][NUM_CARDS]; /* The card in each position */
};

#endif /* _CLASSES_H */