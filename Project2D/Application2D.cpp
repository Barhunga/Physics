#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Gizmos.h"

glm::vec2 PhysicsScene::m_gravity = glm::vec2(0, -70); 
const float extents = 100;
const float aspectRatio = 16.0f / 9.0f;
static bool kinematicToggle = false;
static bool canShoot = true;
static float power;
float powerCap = 400;

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
	m_sceneID = 0;
	SceneSelect();

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
	aie::Gizmos::clear();

	// input example
	aie::Input* input = aie::Input::getInstance();
	// get mouse position
	static int xScreen, yScreen;
	input->getMouseXY(&xScreen, &yScreen);
	glm::vec2 mousePos = screenToWorld(glm::vec2(xScreen, yScreen));

	// handle inputs
	switch (m_gameID) 
	{
	case DVD:
		if (input->wasKeyPressed(aie::INPUT_KEY_DOWN)) 
		{
			if(length(m_physicsScene->getActor(0)->getVelocity()) >= 6) // make sure it doesnt reach the lower threshold
				m_physicsScene->getActor(0)->setVelocity(glm::vec2(m_physicsScene->getActor(0)->getVelocity().x / 2,
					m_physicsScene->getActor(0)->getVelocity().y / 2));
		}
		else if (input->wasKeyPressed(aie::INPUT_KEY_UP)) 
		{
			m_physicsScene->getActor(0)->setVelocity(glm::vec2(m_physicsScene->getActor(0)->getVelocity().x * 2,
				m_physicsScene->getActor(0)->getVelocity().y * 2));
		}
		else if (input->wasKeyPressed(aie::INPUT_KEY_BACKSPACE)) 
		{
			m_physicsScene->getActor(0)->setVelocity(glm::vec2(-m_physicsScene->getActor(0)->getVelocity().x,
				-m_physicsScene->getActor(0)->getVelocity().y));
		}
		else if (input->isKeyDown(aie::INPUT_KEY_LEFT))
		{
			Plane* plane = dynamic_cast<Plane*>(m_physicsScene->getActor(1));
			plane->setDistance(plane->getDistance() + 0.1);
		}
		else if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
		{
			Plane* plane = dynamic_cast<Plane*>(m_physicsScene->getActor(1));
			plane->setDistance(plane->getDistance() - 0.1);
		}
		break;

	case BILLIARDS:
		if (canShoot == true)
		{
			Sphere* cueBall = dynamic_cast<Sphere*>(m_physicsScene->getActor(10)); 
			if (cueBall != nullptr)
			{
				glm::vec2 direction = glm::normalize(cueBall->getPosition() - mousePos);
				power = glm::length(cueBall->getPosition() - mousePos) * 10.f;
				// cap the power
				if (power > powerCap) power = powerCap;
				
				aie::Gizmos::add2DLine(power != powerCap ? mousePos : cueBall->getPosition() - (direction * powerCap / 10.f), 
					power != powerCap ? mousePos - direction * 40.f : cueBall->getPosition() - (direction * (powerCap / 10.f + 40.f)), 
					glm::vec4(1, 0, 1, 1));

				if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT))
				{
					cueBall->setVelocity(direction * power);
					canShoot = false;
				}
			}
		}
		else 
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
					canShoot = true;
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
		if (input->isKeyDown(aie::INPUT_KEY_UP))
			m_physicsScene->getActor(1)->setVelocity(glm::vec2(0, 100));
		if(input->isKeyDown(aie::INPUT_KEY_DOWN))
			m_physicsScene->getActor(1)->setVelocity(glm::vec2(0, -100));
		if(input->isKeyDown(aie::INPUT_KEY_W))
			m_physicsScene->getActor(0)->setVelocity(glm::vec2(0, 100));
		if(input->isKeyDown(aie::INPUT_KEY_S))
			m_physicsScene->getActor(0)->setVelocity(glm::vec2(0, -100));
		break;

	case BUBBLEBOBBLE:
		if (input->isKeyDown(aie::INPUT_KEY_LEFT))
			m_physicsScene->getActor(0)->setVelocity(glm::vec2(-50, 0));
		if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
			m_physicsScene->getActor(0)->setVelocity(glm::vec2(50, 0));
		break;

	case FREEPLAY:
		static bool held0;
		static bool held1;
		static RigidBody* object;

		// right click - spawns spheres when released
		if (!held0)
		{
			glm::vec4 colour;
			kinematicToggle ? colour = glm::vec4(1, 1, 0, 1) : colour = glm::vec4(0, 1, 1, 1);
			if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
			{
				held1 = true;
				aie::Gizmos::add2DCircle(mousePos, 5, 32, colour);
			}
			else if (input->wasMouseButtonReleased(aie::INPUT_MOUSE_BUTTON_RIGHT))
			{
				object = new Sphere(screenToWorld(glm::vec2(xScreen, yScreen)), glm::vec2(0), 1, 4, 0.8, colour);
				if (kinematicToggle)
					object->setKinematic(true);
				m_physicsScene->addActor(object);
				object = nullptr;
			}
		}

		// left click - drags objects
		static bool swapped;
		if (!held1)
		{
			if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT))
			{
				held0 = true;
				if (object == nullptr)
				{
					object = dynamic_cast<RigidBody*>(getHoveredObject(mousePos));
					if (object != nullptr)
					{
						if (!object->isKinematic())
						{
							object->setKinematic(true);
							swapped = true;
						}
					}
				}
				if (object != nullptr)
					object->setPosition(mousePos);
			}
			else if (input->wasMouseButtonReleased(aie::INPUT_MOUSE_BUTTON_LEFT))
			{
				if (object != nullptr)
				{
					if (swapped)
						object->setKinematic(false);
					swapped = false;
					object = nullptr;
				}
			}
		}

		// middle click - removes objects
		if (!held1)
		{
			if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_MIDDLE))
			{
				object = dynamic_cast<RigidBody*>(getHoveredObject(mousePos));
				if (object != nullptr)
				{
					m_physicsScene->removeActor(object);
					swapped = false;
					object = nullptr;
				}
			}
		}

		// scroll wheel - toggle kinematic flag for spawns
		static float lastScroll;
		if (input->getMouseScroll() > lastScroll) 
			kinematicToggle = true;		
		if (input->getMouseScroll() < lastScroll) 
			kinematicToggle = false;
		lastScroll = input->getMouseScroll(); 

		if (!input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT)) held0 = false;
		if (!input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT)) held1 = false;
		break;

	default:
		if (m_sceneID == 1)
		{
			RigidBody* actor = dynamic_cast<RigidBody*>(m_physicsScene->getActor(19));
			if (actor != nullptr)
			{
				if (input->isKeyDown(aie::INPUT_KEY_UP))
					actor->addVelocity(glm::vec2(0, 10));;
				if (input->isKeyDown(aie::INPUT_KEY_DOWN))
					actor->addVelocity(glm::vec2(0, -10));;
				if (input->isKeyDown(aie::INPUT_KEY_LEFT))
					actor->addVelocity(glm::vec2(-10, 0));
				if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
					actor->addVelocity(glm::vec2(10, 0));;
				// normalize the speed so diagonals work as intended and don't speed up
				if (actor->getVelocity() != glm::vec2(0))
					actor->setVelocity(glm::normalize(actor->getVelocity()) * 20.f);
			}
		}

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

	m_physicsScene->update(deltaTime);
	m_physicsScene->draw();

	//printf("%f\n", m_physicsScene->getTotalEnergy());

	// switching scenes
	static bool switching = false;
	if (switching == false && input->isKeyDown(aie::INPUT_KEY_EQUAL))
	{
		switching = true;
		if (m_sceneID < 7)
		{
			while (m_physicsScene->getActorCount() > 0)
			{
				m_physicsScene->removeActor(m_physicsScene->getActor(0));
			}
			m_sceneID++;
			SceneSelect();
		}
	}
	if (switching == false && input->isKeyDown(aie::INPUT_KEY_MINUS))
	{
		switching = true;
		if (m_sceneID > 0)
		{
			while (m_physicsScene->getActorCount() > 0)
			{
				m_physicsScene->removeActor(m_physicsScene->getActor(0));
			}
			m_sceneID--;
			SceneSelect();
		}
	}
	if (!input->isKeyDown(aie::INPUT_KEY_EQUAL) && !input->isKeyDown(aie::INPUT_KEY_MINUS)) switching = false;

	// exit the game
	if(input->isKeyDown(aie::INPUT_KEY_ESCAPE))
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
	aie::Gizmos::draw2D(glm::ortho<float>(-extents, extents, -extents / aspectRatio, extents / aspectRatio, -1.0f, 1.0f)); 
	
	// output some text
	m_2dRenderer->setRenderColour(0, 1, 0, 1); // green
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->drawText(m_font, fps, 0, 720 - 32);
	m_2dRenderer->drawText(m_font, "Press + or - to switch scenes!", 0, 720 - 64);
	m_2dRenderer->drawText(m_font, "Press ESC to quit!", 1200 - 240, 720 - 32);

	// display controls
	m_2dRenderer->setRenderColour(0, 1, 0, 1); // green
	std::string controls;
	std::string controls2;
	switch (m_sceneID)
	{
		case 0:
			// Physics
			break;
		case 1:
			// Rope
			controls = "UP/DOWN/LEFT/RIGHT - Move Box";
			break;
		case 2:
			// Rotation
			break;
		case 3:
			// DVD
			controls = "UP/DOWN - Change Speed | BACKSPACE - Reverse | LEFT/RIGHT - Move Wall";
			break;
		case 4:
			// Billiards
			if (canShoot)
			{
				controls = "LEFT CLICK - Take Shot | Power [";
				for (int i = 0; i < 10; i++) {
					if (power / (powerCap / 10) > i)
						controls += "|";
					else
						controls += ".";
				}
				controls += "]";
			}
			break;
		case 5:
			// Pong
			controls = "PLAYER 1 MOVE: W/S  -  PLAYER 2 MOVE: UP/DOWN";
			break;
		case 6:
			// BubbleBobble
			controls = "LEFT/RIGHT - Move";
			break;
		case 7:
			// FreePlay

			if (kinematicToggle)
			{
				controls2 = "RIGHT CLICK - Spawn object | LEFT CLICK - Drag object";
				controls = "MIDDLE CLICK - Remove object | SCROLL - Kinematic spawn (TRUE)";
			}
			else
			{
				controls2 = "RIGHT CLICK - Spawn object | LEFT CLICK - Drag object";
				controls = "MIDDLE CLICK - Remove object | SCROLL - Kinematic spawn (FALSE)";
			}
		default:
			break;
	}
	if (controls.length() > 0)
		m_2dRenderer->drawText(m_font, controls.c_str(), 0, 720 - 710);
	if (controls2.length() > 0)
		m_2dRenderer->drawText(m_font, controls2.c_str(), 0, 720 - 710 + 32);

	// done drawing sprites
	m_2dRenderer->end();
}

glm::vec2 Application2D::screenToWorld(glm::vec2 screenPos)
{
	glm::vec2 worldPos = screenPos;
	// move the centre of the screen to (0,0)
	worldPos.x -= getWindowWidth() / 2;
	worldPos.y -= getWindowHeight() / 2;
	// scale according to our extents
	worldPos.x *= 2.0f * extents / getWindowWidth();
	worldPos.y *= 2.0f * extents / (aspectRatio * getWindowHeight());
	return worldPos;
}

PhysicsObject* Application2D::getHoveredObject(glm::vec2 mousePos)
{
	for (int i = 0; i < m_physicsScene->getActorCount(); i++)
	{
		if (m_physicsScene->getActor(i)->IsInside(mousePos))
			return m_physicsScene->getActor(i);
	}
	return nullptr;
}

void Application2D::SceneSelect()
{
	switch (m_sceneID)
	{
	case 0:
		Physics();
		break;
	case 1:
		Rope(10);
		break;
	case 2:
		// remove gravity for the below scenes
		m_physicsScene->setGravity(glm::vec2(0, 0));
		RotationDemo();
		break;
	case 3:
		DVDPlayer();
		break;
	case 4:
		Billiards();
		break;
	case 5:
		Pong();
		break;
	case 6:
		m_physicsScene->setGravity(glm::vec2(0, 0));
		BubbleBobble();
		break;
	case 7:
		FreePlay();
		break;
	default:
		break;
	}
}

void Application2D::Physics()
{
	m_gameID = DEFAULT;
	m_physicsScene->setGravity(glm::vec2(0, -70));

	Sphere* ball1 = new Sphere(glm::vec2(-54, 0), glm::vec2(20, 17), 4.0f, 4, 0.8, glm::vec4(1, 0.5, 1, 1));	    
	Sphere* ball2 = new Sphere(glm::vec2(58, 0), glm::vec2(-20, 15), 8.0f, 8, 0.8, glm::vec4(1, 0.5, 1, 1));	   
	Sphere* ball3 = new Sphere(glm::vec2(0, 0), glm::vec2(0, 0), 4.0f, 4, 0.8,     glm::vec4(1, 0.5, 1, 1));		    
	Sphere* ball4 = new Sphere(glm::vec2(10, 0), glm::vec2(0, 0), 4.0f, 4, 0.8,    glm::vec4(1, 0.5, 1, 1));		
	ball4->setKinematic(true);																
	Box* box1 = new Box(glm::vec2(-20, 10), glm::vec2(20, 20), 4.0f, 2, 4, 0, 0.8, glm::vec4(1, 0.5, 1, 1));		   
	Box* box2 = new Box(glm::vec2(-40, 10), glm::vec2(0, 60), 4.0f, 2, 4, 1, 0.8,  glm::vec4(1, 0.5, 1, 1));		    
	Box* box3 = new Box(glm::vec2(-40, -20), glm::vec2(0, 60), 4.0f, 2, 4, 1, 0.8, glm::vec4(1, 0.5, 1, 1));		
	box3->setKinematic(true); 
	m_physicsScene->addActor(ball1);																	   
	m_physicsScene->addActor(ball2);																	   
	m_physicsScene->addActor(ball3);																	   
	m_physicsScene->addActor(ball4);																	   
	m_physicsScene->addActor(box1);																		   
	m_physicsScene->addActor(box2);			 															   
	m_physicsScene->addActor(box3);																		   
																										   
	Plane* plane1 = new Plane(glm::vec2(-1, 0), -85, 0.6, glm::vec4(0.53, 0, 0.68, 1));
	Plane* plane2 = new Plane(glm::vec2(1, 0), -85, 0.6, glm::vec4(0.53, 0, 0.68, 1));
	Plane* plane3 = new Plane(glm::vec2(0, 1), -40, 0.6, glm::vec4(0.53, 0, 0.68, 1));
	m_physicsScene->addActor(plane1);																	   
	m_physicsScene->addActor(plane2);																	   
	m_physicsScene->addActor(plane3);																	   
}

void Application2D::RotationDemo()
{
	m_gameID = DEFAULT;

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

void Application2D::Rope(int num)
{
	m_gameID = DEFAULT;
	m_physicsScene->setGravity(glm::vec2(0, -9.82f));
	Sphere* prev = nullptr;
	for (int i = 0; i < num; i++)
	{
		// spawn a sphere to the right and below the previous one, so that the whole rope acts under gravity and swings
		Sphere* sphere = new Sphere(glm::vec2(i * 3, 30 - i * 5), glm::vec2(0), 10, 2, 1, glm::vec4(1, 1, 0, 1));
		if (i == 0)
			sphere->setKinematic(true);
		m_physicsScene->addActor(sphere);
		if (prev)
			m_physicsScene->addActor(new Spring(sphere, prev, 1000, 10, 7));
		prev = sphere;
	}
	// add a kinematic box at an angle for the rope to drape over
	Box* box = new Box(glm::vec2(0, -20), glm::vec2(0), 20, 8, 2, 0.3f, 1, glm::vec4(0, 0, 1, 1));
	box->setKinematic(true);
	m_physicsScene->addActor(box);

	Plane* plane1 = new Plane(glm::vec2(-1, 0), -90, 0.6, glm::vec4(1, 0.5, 0, 1));
	Plane* plane2 = new Plane(glm::vec2(1, 0), -90, 0.6, glm::vec4(1, 0.5, 0, 1));
	Plane* plane3 = new Plane(glm::vec2(0, 1), -45, 0.6, glm::vec4(1, 0.5, 0, 1));
	m_physicsScene->addActor(plane1);
	m_physicsScene->addActor(plane2);
	m_physicsScene->addActor(plane3);

	// softbodies
	std::vector<std::string> sb;
	sb.push_back("000000");
	sb.push_back("000000");
	sb.push_back("00....");
	sb.push_back("00....");
	sb.push_back("000000");
	sb.push_back("000000");
	SoftBody::Build(m_physicsScene, glm::vec2(-50, 0), 5.0f, 1000, 10.0f, sb);

	std::vector<std::string> sb2;
	sb2.push_back("..00..");
	sb2.push_back(".0000.");
	sb2.push_back("00..00");
	sb2.push_back("000000");
	sb2.push_back("00..00");
	sb2.push_back("00..00");
	SoftBody::Build(m_physicsScene, glm::vec2(50, 0), 5.0f, 1000, 10.0f, sb2);
}

void Application2D::DVDPlayer()
{
	m_gameID = DVD;
	setBackgroundColour(0, 0, 0, 1);

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
	Sphere* ball1 = new Sphere(glm::vec2(-34, 0),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(1, 1, 0, 1));
	Sphere* ball2 = new Sphere(glm::vec2(-41, -4),  glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0, 0, 1, 1));
	Sphere* ball3 = new Sphere(glm::vec2(-41, 4),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(1, 0, 0, 1));
	Sphere* ball4 = new Sphere(glm::vec2(-48, -8),  glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0.53, 0, 0.68, 1));
	Sphere* ball8 = new Sphere(glm::vec2(-48, 0),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0, 0, 0, 1));
	Sphere* ball5 = new Sphere(glm::vec2(-48, 8),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(1, 0.39, 0, 1));
	Sphere* ball6 = new Sphere(glm::vec2(-55, -12), glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0, 0.57, 0.27, 1));
	Sphere* ball7 = new Sphere(glm::vec2(-55, -4),  glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0.64, 0.25, 0.17, 1));
	Sphere* ball9 = new Sphere(glm::vec2(-55, 4),   glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(1, 1, 0.5, 1));		 
	Sphere* ball10 = new Sphere(glm::vec2(-55, 12), glm::vec2(0, 0), 160.0f, 4, 0.8, glm::vec4(0.5, 0.5, 1, 1));		 
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
	setBackgroundColour(0, 0, 0, 1);

	Box* player1 = new Box(glm::vec2(-60, 0), glm::vec2(0), 1, 2, 10, 0, 1, glm::vec4(1, 1, 1, 1));
	Box* player2 = new Box(glm::vec2( 60, 0), glm::vec2(0), 1, 2, 10, 0, 1, glm::vec4(1, 1, 1, 1));
	player1->setKinematic(true);
	player2->setKinematic(true);
	m_physicsScene->addActor(player1);
	m_physicsScene->addActor(player2);

	Plane* top = new Plane(glm::vec2(0, -1), -40, 1, glm::vec4(1, 1, 1, 0.1));
	Plane* left = new Plane(glm::vec2(1, 0), -85, 1, glm::vec4(1, 1, 1, 0.1));
	Plane* right = new Plane(glm::vec2(-1, 0), -85, 1, glm::vec4(1, 1, 1, 0.1));
	Plane* bottom = new Plane(glm::vec2(0, 1), -40, 1, glm::vec4(1, 1, 1, 0.1));
	m_physicsScene->addActor(top);
	m_physicsScene->addActor(left);
	m_physicsScene->addActor(right);
	m_physicsScene->addActor(bottom);


	Sphere* ball = new Sphere(glm::vec2(0), glm::vec2(80, 80), 1, 2, 1, glm::vec4(1, 1, 1, 1));
	ball->setDrag(0);
	ball->collisionCallback = [=](PhysicsObject* other) { 
		if (other == left || other == right)
		{
			ball->setPosition(glm::vec2(0));
		}
	};
	m_physicsScene->addActor(ball);
}

void Application2D::BubbleBobble()
{
	m_gameID = BUBBLEBOBBLE; 

	Box* player = new Box(glm::vec2(0, -40), glm::vec2(0), 1, 5, 1, 0, 1, glm::vec4(0, 1, 0, 1));
	player->setKinematic(true);
	m_physicsScene->addActor(player);

	Plane* top = new Plane(glm::vec2(0, -1), -45, 1, glm::vec4(1, 0, 1, 0.5));
	Plane* left = new Plane(glm::vec2(1, 0), -30, 1, glm::vec4(1, 0, 1, 0.5));
	Plane* right = new Plane(glm::vec2(-1, 0), -30, 1, glm::vec4(1, 0, 1, 0.5));
	Plane* bottom = new Plane(glm::vec2(0, 1), -50, 1, glm::vec4(1, 0, 1, 0.5));
	m_physicsScene->addActor(top);
	m_physicsScene->addActor(left);
	m_physicsScene->addActor(right);
	m_physicsScene->addActor(bottom);

	Sphere* ball = new Sphere(glm::vec2(0, 20), glm::vec2(42, -56), 1, 1.5, 1, glm::vec4(1, 1, 0, 1));
	ball->setDrag(0);
	m_physicsScene->addActor(ball);
	ball->collisionCallback = [=](PhysicsObject* other) {
		if (other == bottom)
		{
			ball->setPosition(glm::vec2(0, 20));
			if(ball->getVelocity().y > 30.f)
				ball->setVelocity(glm::vec2(ball->getVelocity().x, -ball->getVelocity().y));
			else
				ball->setVelocity(glm::vec2(45, -55));
		}
	};
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			Sphere* orb = new Sphere(glm::vec2(j % 2 == 0 ? -24 + (i * 4) : -22 + (i * 4), 40 - (j * 4)), glm::vec2(0), 1, 2, 1, glm::vec4(0, 1, 1, 1));
			orb->setKinematic(true);
			m_physicsScene->addActor(orb);
			orb->collisionCallback = [=](PhysicsObject* other) {
				if (other == ball)
					orb->setPosition(glm::vec2(-200, 0));
			};
			if (j % 2 == 1 && i == 12)
				m_physicsScene->removeActor(orb);
		}
	}
}

void Application2D::FreePlay()
{
	m_gameID = FREEPLAY;
	m_physicsScene->setGravity(glm::vec2(0, -70));

	Plane* plane1 = new Plane(glm::vec2(-1, 0), -85, 0.6, glm::vec4(1, 0, 0, 0.5));
	Plane* plane2 = new Plane(glm::vec2(1, 0), -85, 0.6, glm::vec4(1, 0, 0, 0.5));
	Plane* plane3 = new Plane(glm::vec2(0, 1), -40, 0.6, glm::vec4(1, 0, 0, 0.5));
	m_physicsScene->addActor(plane1);
	m_physicsScene->addActor(plane2);
	m_physicsScene->addActor(plane3);
}
