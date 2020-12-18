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

	_redGhost = new MovingEnemy();
	_redGhost->direction = 1;
	_redGhost->speed = 0.25f;

	_blueGhost = new MovingEnemy();
	_blueGhost->direction = 2;
	_blueGhost->speed = 0.25f;

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

	// Initialise Block(s)
	_blocks = new Enemy[BLOCKS];

	// Initialise Sounds
	_music = new SoundEffect();
	_munchieEat = new SoundEffect();
	_cherryEat = new SoundEffect();
	_loseSound = new SoundEffect();
	_winSound = new SoundEffect();
	
	// Initialise rest...
	_pause = new Menu();
	_pause->paused = false;
	_pause->pKeyDown = false;

	_start = new Menu();
	_start->menu = true;

	_gameOver = new Menu();
	_gameOver->menu = false;

	_winScreen = new Menu();
	_winScreen->menu = false;
	_gameWin = false;

	_rKeyDown = false;
	_musicStatus = false;

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
	
	delete _redGhost->texture;
	delete _redGhost->sourceRect;
	delete _redGhost;

	delete _blueGhost->texture;
	delete _blueGhost->sourceRect;
	delete _blueGhost;

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

	// Delete Block(s)
	delete _blocks[0].texture;
	for (i = 0; i < BLOCKS; i++)
	{
		delete _blocks[i].position;
		delete _blocks[i].sourceRect;
		delete _blocks;
	}
	delete[] _blocks;

	// Delete Sounds
	delete _munchieEat;
	delete _music;
	delete _cherryEat;
	delete _loseSound;
	delete _winSound;

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
	std::uniform_int_distribution<int> RandomWidth(15, 1013);
	std::uniform_int_distribution<int> RandomHeight(15, 753);

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

	_winScreen->background = new Texture2D();
	_winScreen->background->Load("Textures/WinScreen.tga", false);
	_winScreen->sourceRect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_winScreen->stringPosition = new Vector2(512, 434);

	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.tga", false);
	_pacman->position = new Vector2(530.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	_redGhost->texture = new Texture2D();
	_redGhost->texture->Load("Textures/GhostRed.png", false);
	_redGhost->position = new Vector2(400.0f, 400.0f);
	_redGhost->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	_blueGhost->texture = new Texture2D();
	_blueGhost->texture->Load("Textures/GhostBlue.png", false);
	_blueGhost->position = new Vector2(200.0f, 600.0f);
	_blueGhost->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Ghost(s)
	_ghosts[0]->texture = new Texture2D();
	_ghosts[0]->texture->Load("Textures/GhostPink.png", false);
	_ghosts[1]->texture = new Texture2D();
	_ghosts[1]->texture->Load("Textures/GhostGreen.png", false);

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		int RandWidth = RandomWidth(random);
		int RandHeight = RandomHeight(random);
		_ghosts[i]->position = new Vector2(RandWidth, RandHeight);
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

		// Checks if Ghosts are spawning too close to Pacman and if so, runs loop for that ghost again
		if (_ghosts[i]->position->X > 500 && _ghosts[i]->position->X < 600 && _ghosts[i]->position->Y < 400 && _ghosts[i]->position->Y > 300)
		{
			i--;
		}
	}
	// Load Cherry
	_cherry->texture = new Texture2D();
	_cherry->texture->Load("Textures/Cherry.png", false);
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

	// Load Block(s)
	Texture2D* blockTex = new Texture2D();
	blockTex->Load("Textures/block.tga", false);

	for (i = 0; i < BLOCKS; i++)
	{
		_blocks[i].texture = blockTex;
		_blocks[i].sourceRect = new Rect(0.0f, 0.0f, 60, 60);
	}

	_blocks[0].position = new Vector2(60.0f, 70.0f);
	_blocks[1].position = new Vector2(904.0f, 70.0f);
	_blocks[2].position = new Vector2(60.0f, 666.0f);
	_blocks[3].position = new Vector2(904.0f, 666.0f);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f); 

	// Load Sounds
	_music->Load("Sounds/music.wav");
	_munchieEat->Load("Sounds/pop.wav");
	_cherryEat->Load("Sounds/nom.wav");
	_loseSound->Load("Sounds/lose.wav");
	_winSound->Load("Sounds/win.wav");
}

void Pacman::Update(int elapsedTime)
{
	int i;
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState(); 
	// Gets the current state of the mouse
	Input::MouseState* mouseState = Input::Mouse::GetState();
		
	// Start Game if RETURN key is pressed

	CheckStart(keyboardState, Input::Keys::RETURN);
	CheckPaused(keyboardState, Input::Keys::P);		

	if (!_pacman->dead)
		{
			if (!_gameWin)
			{
				if (!_pause->paused)
				{
					Input(elapsedTime, keyboardState, mouseState);
					UpdatePacman(elapsedTime);
					UpdateCherry(elapsedTime);
					CheckGhostCollisions();
					CheckViewportCollision();
					CheckMunchieCollisions();
					CheckCherryCollision();
					CheckBlockCollisions();
					CheckWin();
					RedGhost(_redGhost, elapsedTime);
					BlueGhost(_blueGhost, elapsedTime);

					for (i = 0; i < GHOSTCOUNT; i++)
					{
						UpdateGhost(_ghosts[i], elapsedTime);
					}

					for (i = 0; i < MUNCHIECOUNT; i++)
					{
						UpdateMunchies(_munchies[i], elapsedTime);
					}
				}
			}
		}
	
	if (_pacman->dead)
		CheckReplay(keyboardState, Input::Keys::RETURN, Input::Keys::ESCAPE);

	if (_gameWin)
	{
		CheckReplay(keyboardState, Input::Keys::RETURN, Input::Keys::ESCAPE);
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



		// Draws Block(s)
		for (i = 0; i < BLOCKS; i++)
		{
			SpriteBatch::Draw(_blocks[i].texture, _blocks[i].position, _blocks[i].sourceRect);
		}

		// Draws Cherry
		SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect);

		// Draws String
		SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

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

		SpriteBatch::Draw(_redGhost->texture, _redGhost->position, _redGhost->sourceRect);
		SpriteBatch::Draw(_blueGhost->texture, _blueGhost->position, _blueGhost->sourceRect);



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
			std::stringstream gameOverScore;
			gameOverScore << _score;
			SpriteBatch::DrawString(gameOverScore.str().c_str(), _gameOver->stringPosition, Color::Yellow);
		}

		if (_gameWin)
		{
			SpriteBatch::Draw(_winScreen->background, _winScreen->sourceRect, nullptr);
			std::stringstream winScreenScore;
			winScreenScore << _score;
			SpriteBatch::DrawString(winScreenScore.str().c_str(), _winScreen->stringPosition, Color::Yellow);
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

	// Moves Pacman up the Y axis
	if (keyboardState->IsKeyDown(Input::Keys::W))
	{
	_pacman->position->Y -= pacmanSpeed;
	_pacman->direction = 3;
	}

	// Moves Pacman left across the X axis
	else if (keyboardState->IsKeyDown(Input::Keys::A))
	{
		_pacman->position->X -= pacmanSpeed;
		_pacman->direction = 2;
	}

	// Moves Pacman down the Y axis
	else if (keyboardState->IsKeyDown(Input::Keys::S))
	{
		_pacman->position->Y += pacmanSpeed;
		_pacman->direction = 1;
	}

	// Moves Pacman right across the X axis
	else if (keyboardState->IsKeyDown(Input::Keys::D))
	{
		_pacman->position->X += pacmanSpeed;
		_pacman->direction = 0;
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
	{
		_start->menu = false;

		// If Music already playing, don't do anything, else play music
		if (_musicStatus == false)
		{
			Audio::Play(_music);
			_musicStatus = true;
		}
	}
}

void Pacman::CheckPaused(Input::KeyboardState* keyboardState, Input::Keys pauseKey)
{
	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pause->pKeyDown)
	{
		_pause->pKeyDown = true;
		_pause->paused = !_pause->paused;

		// If music playing, pause it upon pressing P
		if (_musicStatus == true)
		{
			Audio::Pause(_music);
			_musicStatus = false;
		}

		// If music paused, play it upon pressing P
		else if (_musicStatus == false)
		{
			Audio::Resume(_music);
			_musicStatus = true;
		}
	}

	if (keyboardState->IsKeyUp(Input::Keys::P))
		_pause->pKeyDown = false;

}

void Pacman::CheckReplay(Input::KeyboardState* keyboardState, Input::Keys replayKey, Input::Keys closeKey)
{

	if (keyboardState->IsKeyDown(Input::Keys::RETURN) && !_rKeyDown)
	{
		// Music would loop when replaying so this is to stop that
		if (_musicStatus == true)
		{
			Audio::Stop(_music);
			_musicStatus = false;
		}

		// Removes Gameover Menu, Score, Cherry Counter and resurrects Pacman 
		_rKeyDown = true;
		_gameOver->menu = false;
		_winScreen->menu = false;
		_gameWin = false;
		_pacman->dead = false;
		_score = 0;
		_cherryCounter = 0;
		_winCounter = 0;

		// I think this creates a memory leak but only way I can think of restarting game and not sure how to fix
		LoadContent();

		// Starts playing music again once content has loaded
		if (_musicStatus == false)
		{
			Audio::Play(_music);
			_musicStatus = true;
		}
	}

	if (keyboardState->IsKeyUp(Input::Keys::RETURN))
		_rKeyDown = false;

	if (keyboardState->IsKeyDown(Input::Keys::ESCAPE))
	{
		exit(0);
	}

}

void Pacman::CheckWin()
{
	if (_winCounter == MUNCHIECOUNT)
	{
		_gameWin = true;

		if (_musicStatus == true)
		{
			Audio::Stop(_music);
			_musicStatus = false;
		}
		Audio::Play(_winSound);

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
	int bottom3 = 0;
	int bottom4 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int left3 = 0;
	int left4 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int right3 = 0;
	int right4 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;
	int top3 = 0;
	int top4 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// Populate variables with Ghost data
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;
		_ghosts[i]->sourceRect->Y = _ghosts[i]->sourceRect->Height * _ghosts[i]->direction;

		// If Ghost collides with Pacman then stops current music, plays death sound and sets Pacman to dead.
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			_pacman->position->X = -1500;
			_pacman->position->X = -1500;
			Audio::Stop(_music);
			_musicStatus = false;
			Audio::Play(_loseSound);
			i = GHOSTCOUNT;
		}
	}

	bottom3 = _redGhost->position->Y + _redGhost->sourceRect->Height;
	left3 = _redGhost->position->X;
	right3 = _redGhost->position->X + _redGhost->sourceRect->Width;
	top3 = _redGhost->position->Y;
	_redGhost->sourceRect->Y = _redGhost->sourceRect->Height * _redGhost->direction;

	if ((bottom1 > top3) && (top1 < bottom3) && (right1 > left3) && (left1 < right3))
	{
		_pacman->dead = true;
		_pacman->position->X = -1500;
		_pacman->position->X = -1500;
		Audio::Stop(_music);
		_musicStatus = false;
		Audio::Play(_loseSound);
		i = GHOSTCOUNT;
	}

	bottom4 = _blueGhost->position->Y + _blueGhost->sourceRect->Height;
	left4 = _blueGhost->position->X;
	right4 = _blueGhost->position->X + _blueGhost->sourceRect->Width;
	top4 = _blueGhost->position->Y;
	_blueGhost->sourceRect->Y = _blueGhost->sourceRect->Height * _blueGhost->direction;

	if ((bottom1 > top4) && (top1 < bottom4) && (right1 > left4) && (left1 < right4))
	{
		_pacman->dead = true;
		_pacman->position->X = -1500;
		_pacman->position->X = -1500;
		Audio::Stop(_music);
		_musicStatus = false;
		Audio::Play(_loseSound);
		i = GHOSTCOUNT;
	}

}

// Checks collision and offsets pacman if he collides with box.
void Pacman::CheckBlockCollisions()
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

		for (i = 0; i < BLOCKS; i++)
		{
			// Populate variables with block data
			bottom2 = _blocks[i].position->Y + _blocks[i].sourceRect->Height;
			left2 = _blocks[i].position->X;
			right2 = _blocks[i].position->X + _blocks[i].sourceRect->Width;
			top2 = _blocks[i].position->Y;

			// If Pacman collides with block then stops him.
			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
			{
				_pacman->position->X -= 5;
			}
			if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
			{
				_pacman->position->Y -= 5;
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
		// Populate variables with Munchie data
		bottom2 = _munchies[i].position->Y + _munchies[i].sourceRect->Height;
		left2 = _munchies[i].position->X;
		right2 = _munchies[i].position->X + _munchies[i].sourceRect->Width;
		top2 = _munchies[i].position->Y;

		// If munchie collides with Pacman then it plays audio file, moves munchie offscreen and also adds to score, winCounter and cherryCounter
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			Audio::Play(_munchieEat);
			_munchies[i].position->Y = -200;
			_munchies[i].position->X = -200;
			_winCounter += 1;
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

	// Populate variables with Cherry data
	bottom2 = _cherry->position->Y + _cherry->sourceRect->Height;
	left2 = _cherry->position->X;
	right2 = _cherry->position->X + _cherry->sourceRect->Width;
	top2 = _cherry->position->Y;

	// If Cherry collides with Pacman, plays audio file, adds to score and moves Cherry offscreen
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

		// Adds 1 to cherry counter to stop cherry moving rapidly around the screen once cherry counter = 1200
		_cherryCounter += 1;
	}

	// Spawns Cherry once 28 munchies collected, provided Cherry has been eaten already.
	if (_cherryCounter == 2801 && !_cherry->alive)
	{
		_cherry->position->X = 32 + rand() % Graphics::GetViewportWidth();
		_cherry->position->Y = 32 + rand() % Graphics::GetViewportHeight();

		// Minus 1 to cherry counter to stop cherry moving rapidly around the screen once cherry counter = 2801
		_cherryCounter -= 1;
	}
		
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

// Red Ghost moves left to right
void Pacman::RedGhost(MovingEnemy* ghost, int elapsedTime)
{

	// Moves Ghost Right
	if (ghost->direction == 0)
	{
		ghost->position->X += ghost->speed * elapsedTime;
	}
	// Moves Ghost Left
	else if (ghost->direction == 1)
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}

		if (ghost->position->X + ghost->sourceRect->Width > Graphics::GetViewportWidth())
		{
			// Changes direction
			ghost->position->X = Graphics::GetViewportWidth() - ghost->sourceRect->Width;
			ghost->direction = 1;
		}
		// If Ghost hits Left edge
		else if (ghost->position->X < 0)
		{
			// Changes ghost direction randomly out of 4 values
			ghost->position->X = 0;
			ghost->direction = 0;
		}
}

void Pacman::BlueGhost(MovingEnemy* ghost, int elapsedTime)
{

	// Moves Ghost Up
	if (ghost->direction == 2)
	{
		ghost->position->Y += ghost->speed * elapsedTime;
	}
	// Moves Ghost Down
	else if (ghost->direction == 3)
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
	}

	// If Ghost hits Top edge
	if (ghost->position->Y + ghost->sourceRect->Height > Graphics::GetViewportHeight())
	{
		// Changes ghost direction to down
		ghost->position->Y = Graphics::GetViewportHeight() - ghost->sourceRect->Height;
		ghost->direction = 3;
	}
	// If Ghost hits Bottom edge
	else if (ghost->position->Y < 0)
	{
		// Changes ghost direction to up
		ghost->position->Y = 0 + ghost->sourceRect->Height;
		ghost->direction = 2;
	}
}


void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	std::random_device random;
	std::mt19937 mt(random());
	std::uniform_int_distribution<int> randomDir(0, 3);

	// Moves Ghost Right
	if (ghost->direction == 0) 
	{	
		ghost->position->X += ghost->speed * elapsedTime;
	}
	// Moves Ghost Left
	else if (ghost->direction == 1) 
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}
	// Moves Ghost Down
	else if (ghost->direction == 2)
	{
		
		ghost->position->Y += ghost->speed * elapsedTime;
	}
	// Moves Ghost Up
	else if (ghost->direction == 3)
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
	}

	// Populate variables with box surrounding ghost
	bottom2 = ghost->position->Y + ghost->sourceRect->Height;
	left2 = ghost->position->X - ghost->sourceRect->Height;
	right2 = ghost->position->X + ghost->sourceRect->Width;
	top2 = ghost->position->Y - ghost->sourceRect->Width;


	// Track and move towards Pacman if too close
	if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
	{
		if (ghost->position->X <= (_pacman->position->X + 10))
			ghost->direction = 0;

		else if (ghost->position->X >= (_pacman->position->X - 10))
			ghost->direction = 1;

		else if (ghost->position->Y <= (_pacman->position->Y + 10))
			ghost->direction = 2;

		else if (ghost->position->Y >= (_pacman->position->Y - 10))
			ghost->direction = 3;
	}


	// If Ghost hits Right edge
	if (ghost->position->X + ghost->sourceRect->Width > Graphics::GetViewportWidth())
	{
		// Changes direction
		ghost->position->X = Graphics::GetViewportWidth() - ghost->sourceRect->Width;
		ghost->direction = randomDir(random);
		
	}
	// If Ghost hits Left edge
	else if (ghost->position->X < 0)
	{
		// Changes ghost direction randomly out of 4 values
		ghost->position->X = 0;
		ghost->direction = randomDir(random);
	}

	// If Ghost hits Top edge
	else if (ghost->position->Y + ghost->sourceRect->Height > Graphics::GetViewportHeight())
	{
		// Changes ghost direction randomly out of 4 values
		ghost->position->Y = Graphics::GetViewportHeight() - ghost->sourceRect->Height;
		ghost->direction = randomDir(random);
	}
	// If Ghost hits Bottom edge
	else if (ghost->position->Y < 0)
	{
		// Changes ghost direction randomly out of 4 directions
		ghost->position->Y = 0 + ghost->sourceRect->Height;
		ghost->direction = randomDir(random);
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

void Pacman::UpdateMunchies(Enemy& munchies, int elapsedTime)
{
		munchies.currentFrameTime += elapsedTime;

		if (munchies.currentFrameTime > munchies.cFrameTime)
		{
			munchies.frameCount++;

			if (munchies.frameCount >= 2)
				munchies.frameCount = 0;

			munchies.currentFrameTime = 0;

			// Move Spritesheets depending on Frame
		}
	
}


