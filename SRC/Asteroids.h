#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char* argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

	void AddLife();

	bool getDiff();

	bool getDoub();

	void turnDoub();

	bool getInv();

	void turnInv();

private:
	shared_ptr<Spaceship> mSpaceship;

	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<GUILabel> mStartGameLabel;
	shared_ptr<GUILabel> mInstructionLabel;
	shared_ptr<GUILabel> mInstructions;
	shared_ptr<GUILabel> mInstructions2;
	shared_ptr<GUILabel> mInstructions3;

	uint mLevel;
	uint mAsteroidCount;
	string mScreen;
	bool mDifficulty;
	bool doubleShoot;
	bool invulrable;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();
	void CreateAsteroids(const uint num_asteroids);
	void StartGame();
	void CloseStart();
	void ShowInstructions();
	void OpenStart();
	void CloseInstructions();
	shared_ptr<GameObject> CreateExplosion();

	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;
	const static uint GO_HIGH_SCORE = 3;
	const static uint TURN_OFF_DOUB = 4;
	const static uint TURN_OFF_INV = 5;

	ScoreKeeper mScoreKeeper;
	Player mPlayer;
};

#endif