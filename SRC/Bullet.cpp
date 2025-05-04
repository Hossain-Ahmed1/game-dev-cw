#include "GameWorld.h"
#include "Bullet.h"
#include "BoundingSphere.h"
#include  "Asteroids.h"
#include <random>
// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Bullets live for 2s by default. */
Bullet::Bullet()
	: GameObject("Bullet"), mTimeToLive(2000)
{
}

/** Construct a new bullet with given position, velocity, acceleration, angle, rotation and lifespan. */
Bullet::Bullet(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl)
	: GameObject("Bullet", p, v, a, h, r), mTimeToLive(ttl)
{
}

/** Copy constructor. */
Bullet::Bullet(const Bullet& b)
	: GameObject(b),
	  mTimeToLive(b.mTimeToLive)
{
}

/** Destructor. */
Bullet::~Bullet(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update bullet, removing it from game world if necessary. */
void Bullet::Update(int t)
{
	// Update position/velocity
	GameObject::Update(t);
	// Reduce time to live
	mTimeToLive = mTimeToLive - t;
	// Ensure time to live isn't negative
	if (mTimeToLive < 0) { mTimeToLive = 0; }
	// If time to live is zero then remove bullet from world
	if (mTimeToLive == 0) {
		if (mWorld) mWorld->FlagForRemoval(GetThisPtr());
	}

}

bool Bullet::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Asteroid")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Bullet::OnCollision(const GameObjectList& objects)
{

	// Initialize a random number generator
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> powerchance(1, 2);
	uniform_int_distribution<> powertype(1, 3);

	int chance = powerchance(gen);
	int power = powertype(gen);

	if (chance == 1 && game->getDiff()) {
		if (power == 1)game->AddLife();
		if (power == 2)game->turnDoub();
		if (power == 3)game->turnInv();
	}
	mWorld->FlagForRemoval(GetThisPtr());
}

void Bullet::SetGame(Asteroids* set_game) {
	game = set_game;
}