#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Gizmos.h"

glm::vec2 PhysicsScene::m_gravity = glm::vec2(0, -70); 

Application2D::Application2D() {

}

Application2D::~Application2D() {

}

bool Application2D::startup() {

	// increase the 2d line count to maximize the number of objects we can draw
	aie::Gizmos::create(255U, 255U, 65535U, 65535U);
	
	m_2dRenderer = new aie::Renderer2D();
	m_font = new aie::Font("./font/consolas.ttf", 32);
	// these 3 were in the sample but not the tut
	m_texture = new aie::Texture("./textures/numbered_grid.tga");
	m_shipTexture = new aie::Texture("./textures/ship.png");
	m_timer = 0;

	m_physicsScene = new PhysicsScene();

	//Physics(); 

	// remove gravity for the below scenarios
	m_physicsScene->setGravity(glm::vec2(0, 0)); 
	//RotationDemo();
	//DVDPlayer();
	Billiards();
	//Pong();
	//BubbleBobble();

	return true;
}

void Application2D::shutdown() {
	
	delete m_font;
	delete m_texture;
	delete m_shipTexture;
	delete m_2dRenderer;

	delete m_physicsScene;
}

void Application2D::update(float deltaTime) {

	m_timer += deltaTime;

	// input example
	aie::Input* input = aie::Input::getInstance();
	static bool held = false;

	switch (m_gameID) 
	{

	case DVD:
		if (input->isKeyDown(aie::INPUT_KEY_DOWN)) {
			if (held == false)
			{
				m_physicsScene->getActor(0)->setVelocity(glm::vec2(m_physicsScene->getActor(0)->getVelocity().x / 2,
					m_physicsScene->getActor(0)->getVelocity().y / 2));
				printf("%F %f\n", m_physicsScene->getActor(0)->getVelocity().x, m_physicsScene->getActor(0)->getVelocity().y);
				held = true;
			}
		}
		else if (input->isKeyDown(aie::INPUT_KEY_UP)) {
			if (held == false)
			{
				m_physicsScene->getActor(0)->setVelocity(glm::vec2(m_physicsScene->getActor(0)->getVelocity().x * 2,
					m_physicsScene->getActor(0)->getVelocity().y * 2));
				held = true;
			}
		}
		else if (input->isKeyDown(aie::INPUT_KEY_BACKSPACE)) {
			if (held == false)
			{
				m_physicsScene->getActor(0)->setVelocity(glm::vec2(-m_physicsScene->getActor(0)->getVelocity().x,
					-m_physicsScene->getActor(0)->getVelocity().y));
				held = true;
			}
		}
		else held = false;
		break;

	case BILLIARDS:
		if (input->isKeyDown(aie::INPUT_KEY_LEFT))
		{
			if (held == false )
			{
				m_physicsScene->getActor(10)->setVelocity(glm::vec2(-500, 0));
				held = true;
			}
		}
		else 
		{
			// only do calculations if we need to
			if (held)
			{
				// check all objects to see if they are moving
				for (size_t i = 10; i < m_physicsScene->getActorCount(); i++)
				{
					if (m_physicsScene->getActor(i)->getVelocity() != glm::vec2(0, 0))
					{
						// exit early if object is moving
						break;
					}
					// if we've made it to the end of the loop and all have been still, allow us to take our next shot
					else if (i == m_physicsScene->getActorCount() - 1)
					{
						Sphere* cueBall = dynamic_cast<Sphere*>(m_physicsScene->getActor(10));
						if (cueBall->getPosition().x == 200) cueBall->setPosition(glm::vec2(54, 0));
						held = false;
					}
				}
			}
		}

		// pocket collisions
		for (size_t i = 4; i < 10; i++) // pockets
		{
			for (size_t j = 10; j < m_physicsScene->getActorCount(); j++) // balls
			{
				// try to cast objects to sphere and sphere
				Sphere* pocket = dynamic_cast<Sphere*>(m_physicsScene->getActor(i)); 
				Sphere* ball = dynamic_cast<Sphere*>(m_physicsScene->getActor(j)); 
				// if we are successful then test for collision
				if (pocket != nullptr && ball != nullptr)
				{
					float distance = glm::distance(pocket->getPosition(), ball->getPosition());
					if (distance <= pocket->getRadius() + 1)
					{
						if (j == 10) // is cue ball
						{
							ball->setVelocity(glm::vec2(0, 0));
							ball->setPosition(glm::vec2(200, 0));
						}
						else
							m_physicsScene->removeActor(ball);
					}
				}
			}
		}
		break;

	case PONG:

		break;

	case BUBBLEBOBBLE:

		break;

	default:
		if (input->isKeyDown(aie::INPUT_KEY_UP))
			m_physicsScene->getActor(3)->setVelocity(glm::vec2(0, 10));

		//// Update the camera position using the arrow keys - camera stuff in sample but not tut
		//float camPosX;
		//float camPosY;
		//m_2dRenderer->getCameraPos(camPosX, camPosY);

		//if (input->isKeyDown(aie::INPUT_KEY_UP))
		//	camPosY += 500.0f * deltaTime;

		//if (input->isKeyDown(aie::INPUT_KEY_DOWN))
		//	camPosY -= 500.0f * deltaTime;

		//if (input->isKeyDown(aie::INPUT_KEY_LEFT))
		//	camPosX -= 500.0f * deltaTime;

		//if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
		//	camPosX += 500.0f * deltaTime;

		//m_2dRenderer->setCameraPos(camPosX, camPosY);
	}

	aie::Gizmos::clear();

	m_physicsScene->update(deltaTime);
	m_physicsScene->draw();

	//printf("%f\n", m_physicsScene->getTotalEnergy());

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application2D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	//// These objects drawn were part of the sample but not the tut

	//// demonstrate animation
	//m_2dRenderer->setUVRect(int(m_timer) % 8 / 8.0f, 0, 1.f / 8, 1.f / 8);
	//m_2dRenderer->drawSprite(m_texture, 200, 200, 100, 100);

	//// demonstrate spinning sprite
	//m_2dRenderer->setUVRect(0,0,1,1);
	//m_2dRenderer->drawSprite(m_shipTexture, 600, 400, 0, 0, m_timer, 1);

	//// draw a thin line
	//m_2dRenderer->drawLine(300, 300, 600, 400, 2, 1);

	//// draw a moving purple circle
	//m_2dRenderer->setRenderColour(1, 0, 1, 1);
	//m_2dRenderer->drawCircle(sin(m_timer) * 100 + 600, 150, 50);

	//// draw a rotating red box
	//m_2dRenderer->setRenderColour(1, 0, 0, 1);
	//m_2dRenderer->drawBox(600, 500, 60, 20, m_timer);

	//// draw a slightly rotated sprite with no texture, coloured yellow
	//m_2dRenderer->setRenderColour(1, 1, 0, 1);
	//m_2dRenderer->drawSprite(nullptr, 400, 400, 50, 50, 3.14159f * 0.25f, 1);

	////

	// draw your stuff here!
	static float aspectRatio = 16 / 9.f;
	aie::Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / aspectRatio, 100 / aspectRatio, -1.0f, 1.0f));
	
	// output some text
	m_2dRenderer->setRenderColour(0, 1, 0, 1); // green
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->drawText(m_font, fps, 0, 720 - 32);
	m_2dRenderer->drawText(m_font, "Press ESC to quit!", 0, 720 - 64);

	// done drawing sprites
	m_2dRenderer->end();
}

void Application2D::Physics()
{
	Sphere* ball1 = new Sphere(glm::vec2(-54, 0), glm::vec2(20, 17), 4.0f, 4, 0.8, glm::vec4(0, 1, 0, 1));	    
	Sphere* ball2 = new Sphere(glm::vec2(58, 0), glm::vec2(-20, 15), 8.0f, 8, 0.8, glm::vec4(0, 1, 0, 1));	   
	Sphere* ball3 = new Sphere(glm::vec2(0, 0), glm::vec2(0, 0), 4.0f, 4, 0.8, glm::vec4(0, 1, 0, 1));		    
	Sphere* ball4 = new Sphere(glm::vec2(10, 0), glm::vec2(0, 0), 4.0f, 4, 0.8, glm::vec4(0, 1, 0, 1));		
	ball4->setKinematic(true);
	Box* box1 = new Box(glm::vec2(-20, 10), glm::vec2(20, 20), 4.0f, 2, 4, 0, 0.8, glm::vec4(0, 1, 0, 1));		   
	Box* box2 = new Box(glm::vec2(-40, 10), glm::vec2(0, 60), 4.0f, 2, 4, 1, 0.8, glm::vec4(0, 1, 0, 1));		    
	Box* box3 = new Box(glm::vec2(-40, -20), glm::vec2(0, 60), 4.0f, 2, 4, 1, 0.8, glm::vec4(0, 1, 0, 1));		
	box3->setKinematic(true); 
	m_physicsScene->addActor(ball1);																	   
	m_physicsScene->addActor(ball2);																	   
	m_physicsScene->addActor(ball3);																	   
	m_physicsScene->addActor(ball4);																	   
	m_physicsScene->addActor(box1);																		   
	m_physicsScene->addActor(box2);			 															   
	m_physicsScene->addActor(box3);																		   
																										   
	Plane* plane1 = new Plane(glm::vec2(-1, 0), -85, 0.6, glm::vec4(0, 0, 1, 1));							   
	Plane* plane2 = new Plane(glm::vec2(1, 0), -85, 0.6, glm::vec4(0, 0, 1, 1));								   
	Plane* plane3 = new Plane(glm::vec2(0, 1), -40, 0.6, glm::vec4(0, 0, 1, 1));								   
	m_physicsScene->addActor(plane1);																	   
	m_physicsScene->addActor(plane2);																	   
	m_physicsScene->addActor(plane3);																	   
}

void Application2D::RotationDemo()
{
	Sphere* ballYellow = new Sphere(glm::vec2(-30, 30), glm::vec2(30, 0), 1.f, 2, 1, glm::vec4(1, 1, 0, 1));
	Sphere* ballWhite = new Sphere(glm::vec2(-30, -2), glm::vec2(30, 0), 1.f, 2, 1, glm::vec4(1, 1, 1, 1));
	Sphere* ballGreen = new Sphere(glm::vec2(-30, -34), glm::vec2(30, 0), 1.f, 2, 1, glm::vec4(0, 1, 0, 1));
	ballYellow->setDrag(0);
	ballWhite->setDrag(0);
	ballGreen->setDrag(0);
	m_physicsScene->addActor(ballYellow);
	m_physicsScene->addActor(ballWhite);
	m_physicsScene->addActor(ballGreen);

	Box* boxTop = new Box(glm::vec2(30, 30), glm::vec2(0, 0), 10, 2, 8, 0, 1, glm::vec4(0, 1, 1, 1));
	Box* boxMiddle = new Box(glm::vec2(30, 0), glm::vec2(0, 0), 10, 2, 8, 0, 1, glm::vec4(0, 1, 1, 1));
	Box* boxBottom = new Box(glm::vec2(30, -30), glm::vec2(0, 0), 10, 2, 8, 0, 1, glm::vec4(0, 1, 1, 1));
	boxTop->setDrag(0);
	boxMiddle->setDrag(0);
	boxBottom->setDrag(0);
	m_physicsScene->addActor(boxTop);
	m_physicsScene->addActor(boxMiddle);
	m_physicsScene->addActor(boxBottom);
}

void Application2D::DVDPlayer()
{
	m_gameID = DVD;

	Sphere* ball1 = new Sphere(glm::vec2(0, 0), glm::vec2(20, 20), 4.0f, 4, 1, glm::vec4(0, 1, 0, 1));
	ball1->setDrag(0);
	m_physicsScene->addActor(ball1); 

	Plane* plane1 = new Plane(glm::vec2(-1, 0), -85, 1,  glm::vec4(0, 0, 1, 1));  // -40 instead of -85 for perfect bounce	  
	Plane* plane2 = new Plane(glm::vec2(1, 0),  -85, 1,  glm::vec4(0, 0, 1, 1));  // -40 instead of -85 for perfect bounce	  
	Plane* plane3 = new Plane(glm::vec2(0, 1),  -40, 1,  glm::vec4(0, 0, 1, 1));												  
	Plane* plane4 = new Plane(glm::vec2(0, -1), -40, 1,  glm::vec4(0, 0, 1, 1));											  
	m_physicsScene->addActor(plane1);																					  
	m_physicsScene->addActor(plane2);																					  
	m_physicsScene->addActor(plane3);																					  
	m_physicsScene->addActor(plane4);																					  
}																														  

void Application2D::Billiards()
{
	m_gameID = BILLIARDS;
	setBackgroundColour(0.11, 0.30, 0.18, 1);

	Plane* top = new Plane(glm::vec2(0, -1), -40, 0.6, glm::vec4(1, 1, 1, 0.1));
	Plane* left = new Plane(glm::vec2(1, 0), -85, 0.6, glm::vec4(1, 1, 1, 0.1));
	Plane* right = new Plane(glm::vec2(-1, 0), -85, 0.6, glm::vec4(1, 1, 1, 0.1));
	Plane* bottom = new Plane(glm::vec2(0, 1), -40, 0.6, glm::vec4(1, 1, 1, 0.1));
	m_physicsScene->addActor(top);
	m_physicsScene->addActor(left);
	m_physicsScene->addActor(right);
	m_physicsScene->addActor(bottom);

	Sphere* pocket1 = new Sphere(glm::vec2(-84, 39),  glm::vec2(0, 0), 160.0f, 6, 0.8, glm::vec4(0, 0, 0, 1));
	Sphere* pocket2 = new Sphere(glm::vec2(0, 39),    glm::vec2(0, 0), 160.0f, 6, 0.8, glm::vec4(0, 0, 0, 1));
	Sphere* pocket3 = new Sphere(glm::vec2(84, 39),   glm::vec2(0, 0), 160.0f, 6, 0.8, glm::vec4(0, 0, 0, 1));
	Sphere* pocket4 = new Sphere(glm::vec2(-84, -39), glm::vec2(0, 0), 160.0f, 6, 0.8, glm::vec4(0, 0, 0, 1));
	Sphere* pocket5 = new Sphere(glm::vec2(0, -39),   glm::vec2(0, 0), 160.0f, 6, 0.8, glm::vec4(0, 0, 0, 1));
	Sphere* pocket6 = new Sphere(glm::vec2(84, -39),  glm::vec2(0, 0), 160.0f, 6, 0.8, glm::vec4(0, 0, 0, 1));
	pocket1->setIsHole(true); 
	pocket2->setIsHole(true);
	pocket3->setIsHole(true);
	pocket4->setIsHole(true);
	pocket5->setIsHole(true);
	pocket6->setIsHole(true);
	pocket1->setKinematic(true); 
	pocket2->setKinematic(true);
	pocket3->setKinematic(true);
	pocket4->setKinematic(true);
	pocket5->setKinematic(true);
	pocket6->setKinematic(true);
	m_physicsScene->addActor(pocket1);
	m_physicsScene->addActor(pocket2);
	m_physicsScene->addActor(pocket3);
	m_physicsScene->addActor(pocket4);
	m_physicsScene->addActor(pocket5);
	m_physicsScene->addActor(pocket6);

	Sphere* cueBall = new Sphere(glm::vec2(54, 0),  glm::vec2(0, 0), 170.0f, 4, 0.8, glm::vec4(1, 1, 1, 1));
	Sphere* ball1 = new Sphere(glm::vec2(-44, 0),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(1, 1, 0, 1));
	Sphere* ball2 = new Sphere(glm::vec2(-51, -4),  glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0, 0, 1, 1));
	Sphere* ball3 = new Sphere(glm::vec2(-51, 4),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(1, 0, 0, 1));
	Sphere* ball4 = new Sphere(glm::vec2(-58, -8),  glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0.53, 0, 0.68, 1));
	Sphere* ball8 = new Sphere(glm::vec2(-58, 0),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0, 0, 0, 1));
	Sphere* ball5 = new Sphere(glm::vec2(-58, 8),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(1, 0.39, 0, 1));
	Sphere* ball6 = new Sphere(glm::vec2(-65, -12), glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0, 0.57, 0.27, 1));
	Sphere* ball7 = new Sphere(glm::vec2(-65, -4),  glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0.64, 0.25, 0.17, 1));
	Sphere* ball9 = new Sphere(glm::vec2(-65, 4),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(1, 1, 0.5, 1));		 
	Sphere* ball10 = new Sphere(glm::vec2(-65, 12), glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0.5, 0.5, 1, 1));		 
	m_physicsScene->addActor(cueBall);		  
	m_physicsScene->addActor(ball1);
	m_physicsScene->addActor(ball2);		  
	m_physicsScene->addActor(ball3);		  
	m_physicsScene->addActor(ball4);		  
	m_physicsScene->addActor(ball5);		  
	m_physicsScene->addActor(ball6);		  
	m_physicsScene->addActor(ball7);		   
	m_physicsScene->addActor(ball8);		   
	m_physicsScene->addActor(ball9);		  	  
	m_physicsScene->addActor(ball10);		  	  
}

void Application2D::Pong()
{
	m_gameID = PONG; 
}

void Application2D::BubbleBobble()
{
	m_gameID = BUBBLEBOBBLE; 
}
