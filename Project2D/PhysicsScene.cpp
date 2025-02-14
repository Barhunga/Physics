#include "PhysicsScene.h"
#include "Sphere.h"
#include "Plane.h"

PhysicsScene::~PhysicsScene()
{
	for (int i = 0; i < m_actors.size(); i++) {
		delete m_actors[i];
		m_actors[i] = nullptr;
	}
	m_actors.clear();
}

void PhysicsScene::addActor(PhysicsObject* actor)
{
	m_actors.push_back(actor);
}

void PhysicsScene::removeActor(PhysicsObject* actor)
{
	for (std::vector<PhysicsObject*>::iterator itr = m_actors.begin(); itr != m_actors.end(); itr++) {
		if (*itr == actor) {
			delete *itr;
			*itr = nullptr;
			m_actors.erase(itr);
			return;
		}
	}
	printf("\nActor not found in array when attempting remove\n");
}

// define function pointer type with 2 object parameters
typedef bool(*fn)(PhysicsObject*, PhysicsObject*);

// create array of above function pointers
static fn collisionFunctionArray[] =
{
	PhysicsScene::plane2Plane, PhysicsScene::plane2Sphere, PhysicsScene::plane2Box,
	PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere, PhysicsScene::sphere2Box,
	PhysicsScene::box2Plane, PhysicsScene::box2Sphere, PhysicsScene::box2Box,
};

const int SHAPE_COUNT = LAST;

void PhysicsScene::update(float dt)
{
	// update physics at a fixed time step

	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;

	while (accumulatedTime >= m_timeStep)
	{
		for (auto pActor : m_actors)
		{
			pActor->fixedUpdate(m_gravity, m_timeStep);
		}

		accumulatedTime -= m_timeStep;

		// check for collisions (ideally you'd want to have some sort of 
		// scene management in place)
		int actorCount = m_actors.size();

		// need to check for collisions against all objects except this one.
		for (int outer = 0; outer < actorCount - 1; outer++)
		{
			for (int inner = outer + 1; inner < actorCount; inner++)
			{
				PhysicsObject* object1 = m_actors[outer];
				PhysicsObject* object2 = m_actors[inner];

				int shapeId1 = object1->getShapeID();
				int shapeId2 = object2->getShapeID();

				// create function pointer index value
				int functionIdx = (shapeId1 * SHAPE_COUNT) + shapeId2;
				//create and assign function pointer to correct index
				fn collisionFunctionPtr = collisionFunctionArray[functionIdx];
				if (collisionFunctionPtr != nullptr)
				{
					// run function pointer
					collisionFunctionPtr(object1, object2);
				}
			}
		}
	}
}

void PhysicsScene::draw()
{
	for (auto pActor : m_actors) {
		pActor->draw();
	}
}

bool PhysicsScene::sphere2Sphere(PhysicsObject* obj1, PhysicsObject* obj2)
{
	// try to cast objects to sphere and sphere
	Sphere* sphere1 = dynamic_cast<Sphere*>(obj1);
	Sphere* sphere2 = dynamic_cast<Sphere*>(obj2);
	// if we are successful then test for collision
	if (sphere1 != nullptr && sphere2 != nullptr)
	{
		// compare the distance to the sum of the radii
		if (glm::distance(sphere1->getPosition(), sphere2->getPosition()) <= sphere1->getRadius() + sphere2->getRadius()) {
			// process collision
			sphere1->resolveCollision(sphere2, 0.5f * (sphere1->getPosition() + sphere2->getPosition()));
			return true;
		}
	}
	return false;
}

bool PhysicsScene::sphere2Box(PhysicsObject* obj1, PhysicsObject* obj2)
{
	return false;
}

bool PhysicsScene::box2Plane(PhysicsObject* obj1, PhysicsObject* obj2)
{
	return false;
}

bool PhysicsScene::box2Sphere(PhysicsObject* obj1, PhysicsObject* obj2)
{
	return false;
}

bool PhysicsScene::box2Box(PhysicsObject* obj1, PhysicsObject* obj2)
{
	return false;
}

float PhysicsScene::getTotalEnergy()
{
	float total = 0;
	for (auto it = m_actors.begin(); it != m_actors.end(); it++)
	{
		PhysicsObject* obj = *it;
		total += obj->getEnergy();
	}
	return total;
}

bool PhysicsScene::plane2Box(PhysicsObject* obj1, PhysicsObject* obj2)
{
	return false;
}

bool PhysicsScene::sphere2Plane(PhysicsObject* obj1, PhysicsObject* obj2)
{
	Sphere* sphere = dynamic_cast<Sphere*>(obj1);
	Plane* plane = dynamic_cast<Plane*>(obj2);
	//if we are successful then test for collision
	if (sphere != nullptr && plane != nullptr)
	{
		glm::vec2 collisionNormal = plane->getNormal();
		//distance from sphere center to plane
		float sphereToPlane = glm::dot(sphere->getPosition(), plane->getNormal()) - plane->getDistance();

		float intersection = sphere->getRadius() - sphereToPlane;
		float velocityOutOfPlane = glm::dot(sphere->getVelocity(), plane->getNormal());
		// check that the sphere is impacting the plane and moving towards it
		if (intersection > 0 && velocityOutOfPlane < 0)
		{
			// apply impact force
			plane->resolveCollision(sphere, sphere->getPosition() + (collisionNormal * -sphere->getRadius())); 
			return true;
		}
	}
	return false;
}

