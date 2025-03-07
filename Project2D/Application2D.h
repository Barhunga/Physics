#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "PhysicsScene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include "Spring.h"

enum GameType {
	DVD = 1,
	BILLIARDS,
	PONG,
	BUBBLEBOBBLE
};

class Application2D : public aie::Application {
public:

	Application2D();
	virtual ~Application2D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void SceneSelect();

	void Physics();
	void RotationDemo();
	void Rope(int num);
	void DVDPlayer();
	void Billiards();
	void Pong();
	void BubbleBobble();

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Texture*		m_texture;
	aie::Texture*		m_shipTexture;
	aie::Font*			m_font;

	PhysicsScene* m_physicsScene;

	float m_timer;

	GameType m_gameID;
	int m_sceneID;
};