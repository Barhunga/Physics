#pragma once
#include <vector>
#include <glm/glm.hpp>

class PhysicsObject;
class RigidBody;

class PhysicsScene
{
public:
	PhysicsScene() : m_timeStep(0.01f) {}
	~PhysicsScene();

	void addActor(PhysicsObject* actor);
	void removeActor(PhysicsObject* actor);
	PhysicsObject* getActor(int index);
	size_t getActorCount() { return m_actors.size(); }
	void update(float dt);
	void draw();

	void setGravity(const glm::vec2 gravity) { m_gravity = gravity; }
	static glm::vec2 getGravity() { return m_gravity; }

	void setTimeStep(const float timeStep) { m_timeStep = timeStep; }
	float getTimeStep() const { return m_timeStep; }

	float getTotalEnergy();

	static bool plane2Plane(PhysicsObject*, PhysicsObject*) { return false; }
	static bool plane2Sphere(PhysicsObject* obj1, PhysicsObject* obj2) { return sphere2Plane(obj2, obj1); };
	static bool plane2Box(PhysicsObject*, PhysicsObject*);
	static bool sphere2Plane(PhysicsObject*, PhysicsObject*);
	static bool sphere2Sphere(PhysicsObject*, PhysicsObject*);
	static bool sphere2Box(PhysicsObject* obj1, PhysicsObject* obj2) { return box2Sphere(obj2, obj1); }
	static bool box2Plane(PhysicsObject* obj1, PhysicsObject* obj2) { return plane2Box(obj2, obj1); }
	static bool box2Sphere(PhysicsObject*, PhysicsObject*);
	static bool box2Box(PhysicsObject*, PhysicsObject*);

	static void ApplyContactForces(RigidBody* body1, RigidBody* body2, glm::vec2 norm, float pen);

protected:
	static glm::vec2 m_gravity;
	float m_timeStep;
	std::vector<PhysicsObject*> m_actors;
};



