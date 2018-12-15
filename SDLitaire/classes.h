/*
	This awesome code was written by Chris Roxby.
	This project was created using SDL.
*/
#ifndef _CLASSES_H
#define _CLASSES_H

#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_SysWM.h>
#include <string>
#include <sstream>

#define NUM_CARDS 52
#define FIRST_DEAL 28 /* This is the number of cards dealt at the start of the game. */

#define CARD_RANKS 13 /* The number of slots on the table */

/* Menu Choices */
#define MENU_EXIT 1

class Timer;
class Texture;
class Card;
class Window;
class AssetManager;

struct point
{
	int x,y;
};

struct cardFace
{
	int suit,value;
};

enum SUITS
{
	SPADES,
	CLUBS,
	DIAMONDS,
	HEARTS,
	NUM_SUITS
};

enum FACES
{
	ACE = 1,
	JACK = 11,
	QUEEN,
	KING,
	NUM_FACES = KING /* Card Values per Suit */
};

/* Contains all of the game's configuration options */
struct optionSet
{
	bool animation = false; /* Animate Card Motion */
	bool showFPS = true; /* Display the FPS Counter */
};

const char* nameOfSuit(int suit);

/* The application-time based timer */
class Timer
{
public:
	Timer();

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
class Texture
{
public:
	Texture();
	~Texture();

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
class Card
{
public:
	/* Cards' Velocity */
	static const int CARD_VEL = 2;

	/* Initializes the variables */
	Card();

	/* Assign Texture */
	void setTexture(Texture* texture);

	void handleEvent(SDL_Event& e);

	void move(int timeStep);

	void flip();

	/* Shows the card on the screen */
	void render(SDL_Renderer* renderer, point* rank);

	void setRank(int rank);
	void setFile(int file);

	void setFace(cardFace face);

	void setClickability(bool state);

	void dealTo( int rank );
	void setDestRank(int rank);

	void land();

	void assocGame(AssetManager& game);

	bool getClickability() { return mClickable; }
	bool getFlipState() { return mFaceUp; }
	bool isDragging() { return mDragging; }
	bool isSliding() { return mSliding; }
	int getRank() { return mRank; }
	int getDestRank() { return mDestRank; }
	int getFile() { return mFile; }
	int getX() { return mPosX; }
	int getY() { return mPosY; }
	cardFace getFace() { return mFace; }

private:
	/* Game state */
	bool mFaceUp; /* Flip state */
	cardFace mFace; /* suit and value */

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
	Texture* mTexture;

	AssetManager* mTable;
};

/* Window Wrapper Class */
class Window
{
public:
	Window();

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

/* Manages Cards, Textures, SDL, and more */
class AssetManager
{
public:
	AssetManager();
	~AssetManager();
	bool Init();
	bool LoadMedia();
	void Close();
	void clearRenderer();
	void computeCardPlaces();
	void cardDrop(Card* card);
	void registerCard(Card* card);
	Texture* getCardTexture(int suit, int value);
	void handleEvent(SDL_Event& e);

	Window* getWindow() { return& mWindow; }
	SDL_Renderer* getRenderer() { return mRenderer; }
	Mix_Chunk* getSound() { return mSound; }
	TTF_Font* getFont() { return mFont; }
	Texture* getBackground() { return& mBackgroundTexture; }
	Texture* getFPSTexture() { return& mFPSTextTexture; }
	Texture* getCardBack() { return& mDeckTexture; }
	Texture* getCardOutline() { return&mOutlineTexture; }
	point* getCardPlace(int place) { return& mCardPlaces[place]; }
	int stackedCards(int place) { return mHeldCards[place]; }
	Card* getCard(int rank, int file) { return mRanks[rank][file]; }
	cardFace getFace(int index) { return mAllFaces[index]; }
	optionSet* options() { return &mOptions; }

private:
	/* Window data */
	Window mWindow;
	SDL_Window* mSDLWindow;
	SDL_Renderer* mRenderer;
	SDL_Surface* mIconSurface; /* Will be the window icon */
	/* Game Data */
	TTF_Font* mFont; /* Main Font */
	Mix_Chunk* mSound; /* Example Sound Effect */
	Texture mBackgroundTexture; /* Backdrop (Table) */
	Texture mFPSTextTexture; /* Rendered FPS Count */
	Texture mDeckTexture; /* The Card Back */
	Texture mOutlineTexture; /* The Card Outline */
	Texture mFaceTextures[NUM_SUITS][NUM_FACES + 1]; /* The Card Faces. Index 0 will be ignored to make faces more logical. */
	point mCardPlaces[CARD_RANKS]; /* Card Holding Spots */
	int mHeldCards[CARD_RANKS]; /* The number of cards in each spot */
	Card* mRanks[CARD_RANKS][NUM_CARDS]; /* The card in each position */
	cardFace mAllFaces[NUM_CARDS]; /* All possible card face values */
	optionSet mOptions; /* Game Options */
};

#endif /* _CLASSES_H */