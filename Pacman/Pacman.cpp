#include "Pacman.h"
#include <sstream>
#include <random>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	int i;

	// Set up random numbers
	std::random_device random;
	std::mt19937 mt(random());
	std::uniform_int_distribution<int> random1(1, 10);
	std::uniform_int_distribution<int> random2(1, 500);

	// Initialise member variables
	_pacman = new Player();
	_pacman->dead = false;
	_pacman->cSpeed = 0.25f;
	_pacman->speedMultiplier = 1.0f;
	_pacman->direction = 0;
	_pacman->currentFrameTime = 0;

	// Initialise ghost character
	for (i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.25f;
	}

	// Initialise cherry
	_cherry = new Enemy();
	_cherry->currentFrameTime = 0;
	_cherry->alive = true;

	// Initialise Munchies
	_munchies = new Enemy[MUNCHIECOUNT];

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		int smallRand = random1(random);
		int bigRand = random2(random);
		_munchies[i].frameCount = smallRand;
		_munchies[i].currentFrameTime = 0;
		_munchies[i].frameCount = bigRand;
	}

	// Initialise Sounds
	_music = new SoundEffect();
	_munchieEat = new SoundEffect();
	_cherryEat = new SoundEffect();
	
	// Initialise rest...
	_pause = new Menu();
	_pause->paused = false;
	_pause->pKeyDown = false;

	_start = new Menu();
	_start->menu = true;

	_gameOver = new Menu();
	_gameOver->menu = false;

	_rKeyDown = false;

	// Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	int i;

	// Delete Pacman
	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _pacman;

	// Delete Ghost(s)

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[i]->position;
		delete _ghosts[i]->sourceRect;
		delete _ghosts[i];
	}
	delete[] _ghosts;

	// Delete Cherry(s)
	delete _cherry->texture;
	delete _cherry->sourceRect;
	delete _cherry;

	// Delete Munchie(s)	
	delete _munchies[0].texture;
	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchies[i].position;
		delete _munchies[i].sourceRect;
		delete _munchies;
	}
	delete[] _munchies;

	// Delete Sounds
	delete _munchieEat;
	delete _music;
	delete _cherryEat;

	// Delete Menus
	delete _pause;
	delete _start;
	delete _gameOver;
}

void Pacman::LoadContent()
{
	int i;

	// Create Random Seeds
	std::random_device random;
	std::mt19937 mt(random());
	std::uniform_int_distribution<int> RandomWidth(1, Graphics::GetViewportWidth());
	std::uniform_int_distribution<int> RandomHeight(1, Graphics::GetViewportHeight());

	// Load Start Screen
	_start->background = new Texture2D();
	_start->background->Load("Textures/StartMenu.tga", true);
	_start->sourceRect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	// Set Menu Parameters
	_pause->background = new Texture2D();
	_pause->background->Load("Textures/Transparency.png", false);
	_pause->sourceRect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_pause->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	_gameOver->background = new Texture2D();
	_gameOver->background->Load("Textures/GameOver.tga", false);
	_gameOver->sourceRect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_gameOver->stringPosition = new Vector2(640, 424);

	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.tga", false);
	_pacman->position = new Vector2(530.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Ghost(s)
	Texture2D* ghostTex = new Texture2D();
	ghostTex->Load("Textures/GhostBlue.png", false);

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		int RandWidth = RandomWidth(random);
		int RandHeight = RandomHeight(random);
		_ghosts[i]->texture = ghostTex;
		_ghosts[i]->position = new Vector2(RandWidth, RandHeight);
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}
	// Load Cherry

	_cherry->texture = new Texture2D();
	_cherry->texture->Load("Textures/Cherry.tga", false);
	_cherry->position = new Vector2(500.0f, 200.0f);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie(s)
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.tga", false);

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		int RandWidth = RandomWidth(random);
		int RandHeight = RandomHeight(random);
		_munchies[i].texture = munchieTex;
		_munchies[i].position = new Vector2(RandWidth, RandHeight);
		_munchies[i].sourceRect = new Rect(0.0f, 0.0f, 12, 12);
	}

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f); 

	// Load Sounds
	_music->Load("Sounds/GameMusic.wav");
	_munchieEat->Load("Sounds/pop.wav");
	_cherryEat->Load("Sounds/nom.wav");
	
}

void Pacman::Update(int elapsedTime)
{
	int i;
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState(); 
	// Gets the current state of the mouse
	Input::MouseState* mouseState = Input::Mouse::GetState();
		
	// Start Game if RETURN key is pressed

	for (i = 0; i < 1; i++)
	{
		CheckStart(keyboardState, Input::Keys::RETURN);
		music();
		CheckPaused(keyboardState, Input::Keys::P);

		if (!_pause->paused)
		{
			Input(elapsedTime, keyboardState, mouseState);
			UpdatePacman(elapsedTime);
			UpdateCherry(elapsedTime);
			CheckGhostCollisions();
			CheckViewportCollision();
			CheckMunchieCollisions();
			CheckCherryCollision();

			for (i = 0; i < GHOSTCOUNT; i++)
			{
				UpdateGhost(_ghosts[i], elapsedTime);
			}

			for (i = 0; i < MUNCHIECOUNT; i++)
			{
				UpdateMunchies(_munchies[i], elapsedTime);
			}

			if (_pacman->dead)
			{
				CheckReplay(i, keyboardState, Input::Keys::RETURN, Input::Keys::ESCAPE);
			}
		}
	}
	
}


void Pacman::Draw(int elapsedTime)
{
	int i;
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Score: " << _score;

	// Starts Drawing
	SpriteBatch::BeginDraw();

	// Draws menu screen at the start of the draw call
	if (_start->menu)
	{
		SpriteBatch::Draw(_start->background, _start->sourceRect, nullptr);

	}

	// Draw the rest of the assets once game has started and menu statement has been set to False
	if (!_start->menu)
	{

		// Draws Cherry
		SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect);

		// Draws Munchie(s)
		for (i = 0; i < MUNCHIECOUNT; i++)
		{
			SpriteBatch::Draw(_munchies[i].texture, _munchies[i].position, _munchies[i].sourceRect);
		}

		// Draws Pacman
		if (!_pacman->dead)
		{
			SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect);
		}

		// Draws Ghost(s)
		for (i = 0; i < GHOSTCOUNT; i++)
		{
			SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
		}

		// Draws String
		SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

		if (_pause->paused)
		{
			std::stringstream menuStream;
			menuStream << "Paused!";
			SpriteBatch::Draw(_pause->background, _pause->sourceRect, nullptr);
			SpriteBatch::DrawString(menuStream.str().c_str(), _pause->stringPosition, Color::Red);
		}

		if (_pacman->dead)
		{
			SpriteBatch::Draw(_gameOver->background, _gameOver->sourceRect, nullptr);
			std::stringstream gameOverStream;
			gameOverStream << _score;
			SpriteBatch::DrawString(gameOverStream.str().c_str(), _gameOver->stringPosition, Color::Yellow);
		}

	}
	// Ends Drawing
	SpriteBatch::EndDraw();
}

//Input methods
void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState, Input::MouseState* mouseState)
{
	float pacmanSpeed = _pacman->cSpeed * elapsedTime * _pacman->speedMultiplier;

	// Speed Multiplier
	if (keyboardState->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		// Apply Multiplier
		_pacman->speedMultiplier = 2.0f;
	}

	else
	{
		// Reset Multiplier
		_pacman->speedMultiplier = 1.0f;
	}

	// Moves Pacman right across the X axis
	if (keyboardState->IsKeyDown(Input::Keys::D))
	{
		_pacman->position->X += pacmanSpeed;
		_pacman->direction = 0;
	}

	// Moves Pacman down the Y axis
	else if (keyboardState->IsKeyDown(Input::Keys::S))
	{
		_pacman->position->Y += pacmanSpeed;
		_pacman->direction = 1;
	}

	// Moves Pacman left across the X axis
	else if (keyboardState->IsKeyDown(Input::Keys::A))
	{
		_pacman->position->X -= pacmanSpeed;
		_pacman->direction = 2;
	}

	// Moves Pacman up the Y axis
	else if (keyboardState->IsKeyDown(Input::Keys::W))
	{
		_pacman->position->Y -= pacmanSpeed;
		_pacman->direction = 3;
	}

	// Handle Mouse Input - Reposition Cherry
	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		_cherry->position->X = mouseState->X;
		_cherry->position->Y = mouseState->Y;
	}
}

//Check methods
void Pacman::CheckStart(Input::KeyboardState* keyboardState, Input::Keys startKey)
{
	// Start Game if RETURN key is pressed
	if (keyboardState->IsKeyDown(Input::Keys::RETURN))
		_start->menu = false;
}

void Pacman::CheckPaused(Input::KeyboardState* keyboardState, Input::Keys pauseKey)
{
	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pause->pKeyDown)
	{
		_pause->pKeyDown = true;
		_pause->paused = !_pause->paused;
	}

	if (keyboardState->IsKeyUp(Input::Keys::P))
		_pause->pKeyDown = false;

}

int Pacman::CheckReplay(int i, Input::KeyboardState* keyboardState, Input::Keys replayKey, Input::Keys closeKey)
{
	if (keyboardState->IsKeyDown(Input::Keys::RETURN))
	{
		_gameOver->menu = false;
		_pacman->dead = false;
		_score = 0;
		_cherryCounter = 0;
		i = 0;
		return i;
	}

	if (keyboardState->IsKeyDown(Input::Keys::ESCAPE))
	{
		exit(0);
	}

}

void Pacman::CheckViewportCollision()
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i].sourceRect->X = _munchies[i].sourceRect->Width * _munchies[i].frameCount;
	}

	_cherry->sourceRect->X = _cherry->sourceRect->Width * _cherry->frameCount;
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frameCount;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;

	// Loop Pacman around to the Left if he goes off screen to the right
	if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth())
		_pacman->position->X = 0;

	// Loop Pacman around to the Right if he goes off screen to the Left
	else if (_pacman->position->X < 0)
		_pacman->position->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width;

	// Loop Pacman to the Top if he goes off screen to the Bottom
	else if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight())
		_pacman->position->Y = 0;

	// Loop Pacman to the Bottom if he goes off screen to the Top
	else if (_pacman->position->Y < 0)
		_pacman->position->Y = Graphics::GetViewportHeight() - _pacman->sourceRect->Height;
}

void Pacman::CheckGhostCollisions()
{
	// Local Variables
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// Populate variables with Ghost data
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;
		_ghosts[i]->sourceRect->Y = _ghosts[i]->sourceRect->Height * _ghosts[i]->direction;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			i = GHOSTCOUNT;
		}
	}
}

void Pacman::CheckMunchieCollisions()
{
	// Local Variables
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		// Populate variables with Ghost data
		bottom2 = _munchies[i].position->Y + _munchies[i].sourceRect->Height;
		left2 = _munchies[i].position->X;
		right2 = _munchies[i].position->X + _munchies[i].sourceRect->Width;
		top2 = _munchies[i].position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			Audio::Play(_munchieEat);
			_munchies[i].position->Y = -200;
			_munchies[i].position->X = -200;
			_score += 100;
			_cherryCounter += 100;

		}
	}

}

void Pacman::CheckCherryCollision()
{
	// Local Variables
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	// Populate variables with Ghost data
	bottom2 = _cherry->position->Y + _cherry->sourceRect->Height;
	left2 = _cherry->position->X;
	right2 = _cherry->position->X + _cherry->sourceRect->Width;
	top2 = _cherry->position->Y;

	if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
	{
		Audio::Play(_cherryEat);
		_cherry->position->Y = -200;
		_cherry->position->X = -200;
		_score += 500;
		_cherry->alive = false;
	}

	// Spawns Cherry once 12 munchies collected, provided Cherry has been eaten already.
	if (_cherryCounter == 1200 && !_cherry->alive)
	{
		_cherry->position->X = 32 + rand() % Graphics::GetViewportWidth();
		_cherry->position->Y = 32 + rand() % Graphics::GetViewportHeight();
		_cherryCounter += 1;
	}

	// Spawns Cherry once 28 munchies collected, provided Cherry has been eaten already.
	if (_cherryCounter == 2801 && !_cherry->alive)
	{
		_cherry->position->X = 32 + rand() % Graphics::GetViewportWidth();
		_cherry->position->Y = 32 + rand() % Graphics::GetViewportHeight();
		_cherryCounter += 1;
	}
		
}

void Pacman::music()
{


}

//Update methods
void Pacman::UpdatePacman(int elapsedTime)
{
	// Move Spritesheets depending on Frame
	_pacman->currentFrameTime += elapsedTime;

	if (_pacman->currentFrameTime > _pacman->cFrameTime)
	{
		_pacman->frameCount++;

		if (_pacman->frameCount >= 2)
			_pacman->frameCount = 0;

		_pacman->currentFrameTime = 0;
	}
}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	// Moves Right
	if (ghost->direction == 0) 
	{	
		ghost->position->X += ghost->speed * elapsedTime;
	}
	// Moves Left
	else if (ghost->direction == 1) 
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}
	// Moves Up
	else if (ghost->direction == 2)
	{
		
		ghost->position->Y += ghost->speed * elapsedTime;
	}
	// Moves Down
	else if (ghost->direction == 3)
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
	}

	// If Ghost hits Right edge
	if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth())
	{
		// Changes direction
		ghost->direction = 1;
	}
	// If Ghost hits Left edge
	else if (ghost->position->X <= 0)
	{
		// Changes direction
		ghost->direction = 0;
	}

	// If Ghost hits Top edge
	else if (ghost->position->Y + ghost->sourceRect->Height > Graphics::GetViewportHeight())
	{
		// Changes direction
		ghost->direction = 0;
	}
	// If Ghost hits Bottom edge
	else if (ghost->position->Y < 0)
	{
		// Changes direction
		ghost->direction = 0;
	}

}

void Pacman::UpdateCherry(int elapsedTime)
{
	_cherry->currentFrameTime += elapsedTime;

	if (_cherry->currentFrameTime > _cherry->cFrameTime)
	{
		_cherry->frameCount++;

		if (_cherry->frameCount >= 2)
			_cherry->frameCount = 0;

		_cherry->currentFrameTime = 0;

		// Move Spritesheets depending on Frame
	}
}

void Pacman::UpdateMunchies(Enemy&, int elapsedTime)
{

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i].currentFrameTime += elapsedTime;

		if (_munchies[i].currentFrameTime > _munchies[i].cFrameTime)
		{
			_munchies[i].frameCount++;

			if (_munchies[i].frameCount >= 2)
				_munchies[i].frameCount = 0;

			_munchies[i].currentFrameTime = 0;

			// Move Spritesheets depending on Frame
		}
	}
}


