#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif
#define MUNCHIECOUNT 50
#define GHOSTCOUNT 2
#define BLOCKS 4

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

//Structure Definition
struct Player
{
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
	bool dead;
	float cSpeed;
	float speedMultiplier;
	const int cFrameTime = 250;
	int currentFrameTime;
	int frameCount;
	int direction;

};

struct Enemy
{
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
	const int cFrameTime = 250;
	int currentFrameTime;
	int frameCount;
	bool alive;
};

struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;

};

struct Menu
{
	Rect* sourceRect;
	Texture2D* background;
	Vector2* stringPosition;
	bool paused;
	bool menu;
	bool pKeyDown;
	bool returnKeyDown;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:

	//Input methods
	void Input(int elapsedTime, Input::KeyboardState* keyboardState, Input::MouseState* mouseState);

	//Check methods
	void CheckStart(Input::KeyboardState* keyboardState, Input::Keys startKey);
	void CheckReplay(Input::KeyboardState* keyboardState, Input::Keys replayKey, Input::Keys closeKey);
	void CheckPaused(Input::KeyboardState* keyboardState, Input::Keys pauseKey);
	void CheckWin();
	void CheckViewportCollision();
	void CheckGhostCollisions();
	void CheckMunchieCollisions();
	void CheckCherryCollision();
	void CheckBlockCollisions();

	//Update methods
	void UpdatePacman(int elapsedTime);
	void UpdateCherry(int elapsedTime);
	void UpdateMunchies(Enemy&, int elapsedTime);
	void UpdateGhost(MovingEnemy*, int elapsedTime);
	void RedGhost(MovingEnemy*, int elapsedTime);
	void BlueGhost(MovingEnemy*, int elapsedTime);

	// Data for Sound Effects
	SoundEffect* _munchieEat;
	SoundEffect* _music;
	SoundEffect* _cherryEat;
	SoundEffect* _loseSound;
	SoundEffect* _winSound;

	// Data to represent Pacman
	Player* _pacman;

	// Data to represent Enemy
	Enemy* _munchies;
	Enemy* _cherry;
	Enemy* _blocks;

	// Data to represent MovingEnemy
	MovingEnemy* _ghosts[GHOSTCOUNT];
	MovingEnemy* _redGhost;
	MovingEnemy* _blueGhost;

	// Position for String
	Vector2* _stringPosition;

	// Data for Menu
	Menu* _pause;
	Menu* _start;
	Menu* _gameOver;
	Menu* _winScreen;

	bool _rKeyDown;
	bool _musicStatus;
	bool _gameWin;

	int _score;
	int _cherryCounter;
	int _winCounter;



public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};