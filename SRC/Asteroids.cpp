#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include <iostream>
#include <fstream> 
#include <sstream>
#include <vector>
#include <string>
#include <vector>

//function provided by https://medium.com/@ryan_forrester_/splitting-strings-in-c-a-complete-guide-cf162837f4ba
std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delimiter, start);
	}

	tokens.push_back(str.substr(start));
	return tokens;
}

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char* argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
	mScreen = "start";
	mDifficulty = true;
	doubleShoot = false;
	invulrable = false;
}

shared_ptr<GUIComponent> newt;
shared_ptr<GUIComponent> instruction4;
shared_ptr<GUIComponent> askHighScore;
shared_ptr<GUIComponent> askName;
shared_ptr<GUILabel> table_label;
shared_ptr<GUILabel> difficulty_label;
shared_ptr<GUILabel> name_label;
shared_ptr<GUILabel> topScore_label;
shared_ptr<GUILabel> top1_label;
shared_ptr<GUILabel> top2_label;
shared_ptr<GUILabel> top3_label;
string mname = "";
/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation* explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation* asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation* spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

	//Create the GUI
	CreateGUI();



	mStartGameLabel->SetVisible(true);

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);




	//Start the game
	GameSession::Start();

}

/** Stop the current game. */
void Asteroids::Stop()
{

	// Stop the game
	GameSession::Stop();
}

//starts the game 
void Asteroids::StartGame() {
	mScoreLabel->SetVisible(true);
	mLivesLabel->SetVisible(true);
	// Create a spaceship and add it to the world
	mGameWorld->AddObject(CreateSpaceship());
	// Create some asteroids and add them to the world
	CreateAsteroids(10);
}

void Asteroids::CloseStart() {
	mStartGameLabel->SetVisible(false);
	mInstructionLabel->SetVisible(false);
	difficulty_label->SetVisible(false);
}

void Asteroids::ShowInstructions() {
	mInstructions->SetVisible(true);
	mInstructions2->SetVisible(true);
	mInstructions3->SetVisible(true);
	newt->SetVisible(true);
	instruction4->SetVisible(true);
}

void Asteroids::AddLife() {
	mPlayer.setLives(mPlayer.getLives() + 1);
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << mPlayer.getLives();
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);
}

bool Asteroids::getDiff() {
	return mDifficulty;
}

bool Asteroids::getDoub() {
	return doubleShoot;
}
void Asteroids::turnDoub() {
	if (!doubleShoot) {
		doubleShoot = true;
		SetTimer(10000, TURN_OFF_DOUB);
	}
}

bool Asteroids::getInv() {
	return invulrable;
}
void Asteroids::turnInv() {
	if (!invulrable) {
		invulrable = true;
		SetTimer(10000, TURN_OFF_INV);
	}
}
// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	if (mScreen == "enter name" && mname.length() < 9) {
		string k(1, key);
		mname = mname + k;
		name_label->SetText("name: " + mname);
	}
	switch (key)
	{
	case ' ':
		if (mScreen == "game") { mSpaceship->Shoot(); }

		break;
		//starts the game when s is pressed
	case 's':
		if (mScreen == "start") {
			mScreen = "game";
			CloseStart();
			StartGame();
		}
		break;
	case 'i':
		if (mScreen == "start") {
			mScreen = "instruction";
			CloseStart();
			ShowInstructions();
		}
		break;
	case 'd':
		if (mScreen == "start") {
			mDifficulty = !mDifficulty;
			if (mDifficulty) {
				difficulty_label->SetText("press 'd' to set difficulty, set to: easy");
			}
			else {
				difficulty_label->SetText("press 'd' to set difficulty, set to: hard");
			}
		}
	case 'h':
		if (mScreen == "game over") {
			mScreen = "enter name";
			askHighScore->SetVisible(false);
			askName->SetVisible(true);
			name_label->SetVisible(true);
		}
		break;
	default:

		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {

}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	if (mScreen == "game") {
		switch (key)
		{
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
			// If left arrow key is pressed start rotating anti-clockwise
		case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
			// If right arrow key is pressed start rotating clockwise
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
			// Default case - do nothing
		default: break;
		}
	}
	else if (key == GLUT_KEY_HOME) {
		if (mScreen == "instruction") {
			mScreen = "start";
			OpenStart();
			CloseInstructions();
		}
	}
	else if (key == GLUT_KEY_INSERT && mScreen == "enter name") {
		mScreen = "high score";
		askName->SetVisible(false);
		name_label->SetVisible(false);
		int score = mScoreKeeper.Getscore();
		// Format the score message using an string-based stream
		std::ostringstream msg_stream;

		string highscores;

		msg_stream << mname << " Score: " << score;
		// Get the score message as a string
		std::string score_msg = msg_stream.str();
		ofstream write_file("highscore.txt", ios::app);
		write_file << "," << mname<<":"<<score;
		write_file.close();

		ifstream highscores_file("highscore.txt");
		//read file
		highscores_file >> highscores;
		//write file
		highscores_file.close();
			
		vector<string> scores = split(highscores,',');

		int n = scores.size();

		//provided by https://www.geeksforgeeks.org/bubble-sort-in-cpp/
		// Outer loop that corresponds to the number of
		// elements to be sorted
		for (int i = 0; i < n - 1; i++) {

			// Last i elements are already
			// in place
			for (int j = 0; j < n - i - 1; j++) {

				int pos1 = scores[j].find_first_of(':');
				int num1 = stoi(scores[j].substr(pos1 + 1));


				int pos2 = scores[j+1].find_first_of(':');
				int num2 = stoi(scores[j+1].substr(pos2 + 1));
				// Comparing adjacent elements
				if (num2 > num1)

					// Swapping if in the wrong order
					swap(scores[j], scores[j + 1]);
			}
		}

		table_label->SetText(score_msg);
		table_label->SetVisible(true);
		topScore_label->SetVisible(true);
		top1_label->SetText(scores[0]);
		top1_label->SetVisible(true);
		top2_label->SetText(scores[1]);
		top2_label->SetVisible(true);
		top3_label->SetText(scores[2]);
		top3_label->SetVisible(true);
	}
}
void Asteroids::OpenStart() {
	mStartGameLabel->SetVisible(true);
	mInstructionLabel->SetVisible(true);
	difficulty_label->SetVisible(true);
}

void Asteroids::CloseInstructions() {
	mInstructions->SetVisible(false);
	mInstructions2->SetVisible(false);
	mInstructions3->SetVisible(false);
	newt->SetVisible(false);
	instruction4->SetVisible(false);
}
void Asteroids::OnSpecialKeyReleased(int key, int x, int y)

{
	if (mScreen == "game") {
		switch (key)
		{
			// If up arrow key is released stop applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
			// If left arrow key is released stop rotating
		case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
			// If right arrow key is released stop rotating
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
			// Default case - do nothing
		default: break;
		}
	}
}



// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mAsteroidCount <= 0)
		{
			SetTimer(500, START_NEXT_LEVEL);
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
	}
	if (value == GO_HIGH_SCORE) {
		mScreen = "game over";
		mGameOverLabel->SetVisible(false);
		askHighScore->SetVisible(true);
	}
	if (value == TURN_OFF_DOUB) {
		doubleShoot = false;
	}
	if (value == TURN_OFF_INV) {
		invulrable = false;
	}

}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	mSpaceship->SetGame(this);
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Set the visibility of the label to false (hidden)
	mScoreLabel->SetVisible(false);

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Set the visibility of the label to false (hidden)
	mLivesLabel->SetVisible(false);

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mStartGameLabel = shared_ptr<GUILabel>(new GUILabel("Start: press 's' to start"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mStartGameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mStartGameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	shared_ptr<GUIComponent> start_game_component
		= static_pointer_cast<GUIComponent>(mStartGameLabel);

	mGameDisplay->GetContainer()->AddComponent(start_game_component, GLVector2f(0.5f, 0.8f));



	//instruction label
	// Create a new GUILabel and wrap it up in a shared_ptr
	mInstructionLabel = shared_ptr<GUILabel>(new GUILabel("Start: press 'i' for instructions"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mInstructionLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mInstructionLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	shared_ptr<GUIComponent> instruction_componet
		= static_pointer_cast<GUIComponent>(mInstructionLabel);
	mGameDisplay->GetContainer()->AddComponent(instruction_componet, GLVector2f(0.5f, 0.6f));

	//instruction label
// Create a new GUILabel and wrap it up in a shared_ptr
	mInstructions = shared_ptr<GUILabel>(new GUILabel("Score high by blasting asteroids"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mInstructions->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mInstructions->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructions->SetVisible(false);
	shared_ptr<GUIComponent> instructions_component
		= static_pointer_cast<GUIComponent>(mInstructions);
	mGameDisplay->GetContainer()->AddComponent(instructions_component, GLVector2f(0.5f, 0.9f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mInstructions2 = shared_ptr<GUILabel>(new GUILabel("press space to shoot and up arrow to thrust"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mInstructions2->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mInstructions2->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructions2->SetVisible(false);
	shared_ptr<GUIComponent> instructions_component2
		= static_pointer_cast<GUIComponent>(mInstructions2);
	mGameDisplay->GetContainer()->AddComponent(instructions_component2, GLVector2f(0.5f, 0.8f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mInstructions3 = shared_ptr<GUILabel>(new GUILabel("use your left and right arrows key to steer"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mInstructions3->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mInstructions3->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructions3->SetVisible(false);
	shared_ptr<GUIComponent> instructions_component3
		= static_pointer_cast<GUIComponent>(mInstructions3);
	mGameDisplay->GetContainer()->AddComponent(instructions_component3, GLVector2f(0.5f, 0.7f));

	newt = shared_ptr<GUILabel>(new GUILabel("to avoid the asteroids"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	newt->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	newt->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	newt->SetVisible(false);
	shared_ptr<GUIComponent> newt_comp
		= static_pointer_cast<GUIComponent>(newt);
	mGameDisplay->GetContainer()->AddComponent(newt_comp, GLVector2f(0.5f, 0.6f));

	instruction4 = shared_ptr<GUILabel>(new GUILabel("press HOME to go back"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	instruction4->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	instruction4->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	instruction4->SetVisible(false);
	shared_ptr<GUIComponent> instruction4_comp
		= static_pointer_cast<GUIComponent>(instruction4);
	mGameDisplay->GetContainer()->AddComponent(instruction4_comp, GLVector2f(0.5f, 0.5f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	difficulty_label = make_shared<GUILabel>("press 'd' to set difficulty, set to: easy");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	difficulty_label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	difficulty_label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> difficulty_label_component
		= static_pointer_cast<GUIComponent>(difficulty_label);
	mGameDisplay->GetContainer()->AddComponent(difficulty_label_component, GLVector2f(0.5f, 0.4f));

	askHighScore = shared_ptr<GUILabel>(new GUILabel("Press 'h' for to save your score "));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	askHighScore->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	askHighScore->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	askHighScore->SetVisible(false);
	shared_ptr<GUIComponent> askHighScore_comp
		= static_pointer_cast<GUIComponent>(askHighScore);
	mGameDisplay->GetContainer()->AddComponent(askHighScore_comp, GLVector2f(0.5f, 0.5f));

	askName = shared_ptr<GUILabel>(new GUILabel("enter name, press INSERT to submit"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	askName->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	askName->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	askName->SetVisible(false);
	shared_ptr<GUIComponent> askName_comp
		= static_pointer_cast<GUIComponent>(askName);
	mGameDisplay->GetContainer()->AddComponent(askName_comp, GLVector2f(0.5f, 0.7f));

	name_label = make_shared<GUILabel>("name: ");
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	name_label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	name_label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	name_label->SetVisible(false);
	shared_ptr<GUIComponent> name_label_comp
		= static_pointer_cast<GUIComponent>(name_label);
	mGameDisplay->GetContainer()->AddComponent(name_label_comp, GLVector2f(0.5f, 0.5f));


	table_label = make_shared<GUILabel>("Your score");
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	table_label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	table_label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	table_label->SetVisible(false);
	shared_ptr<GUIComponent> table_label_comp
		= static_pointer_cast<GUIComponent>(table_label);
	mGameDisplay->GetContainer()->AddComponent(table_label_comp, GLVector2f(0.5f, 0.9f));

	topScore_label = make_shared<GUILabel>("top score");
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	topScore_label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	topScore_label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	topScore_label->SetVisible(false);
	shared_ptr<GUIComponent> topScore_label_comp
		= static_pointer_cast<GUIComponent>(topScore_label);
	mGameDisplay->GetContainer()->AddComponent(topScore_label_comp, GLVector2f(0.5f, 0.8f));


	top1_label = make_shared<GUILabel>("top score");
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	top1_label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	top1_label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	top1_label->SetVisible(false);
	shared_ptr<GUIComponent> top1_label_comp
		= static_pointer_cast<GUIComponent>(top1_label);
	mGameDisplay->GetContainer()->AddComponent(top1_label_comp, GLVector2f(0.5f, 0.7f));

	top2_label = make_shared<GUILabel>("top score");
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	top2_label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	top2_label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	top2_label->SetVisible(false);
	shared_ptr<GUIComponent> top2_label_comp
		= static_pointer_cast<GUIComponent>(top2_label);
	mGameDisplay->GetContainer()->AddComponent(top2_label_comp, GLVector2f(0.5f, 0.6f));

	top3_label = make_shared<GUILabel>("top score");
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	top3_label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	top3_label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	top3_label->SetVisible(false);
	shared_ptr<GUIComponent> top3_label_comp
		= static_pointer_cast<GUIComponent>(top3_label);
	mGameDisplay->GetContainer()->AddComponent(top3_label_comp, GLVector2f(0.5f, 0.5f));
}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0)
	{
		SetTimer(1000, CREATE_NEW_PLAYER);
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
		SetTimer(1000, GO_HIGH_SCORE);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}




