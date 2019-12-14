#include <time.h>
#include <random>
#include <iostream>
#include <list>
#include <string>
#include <chrono>
#include "../include/freeglut.h"
#include "Asteroid.h"
#include "LevelGeneration.h"
#include "SpaceShip.h"
#include "Rocket.h"
#include "Background.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define WINDOW_TITLE "SpaceShooter"
#define COLOR_BLACK 0, 0, 0, 0
#define TICK_LENGTH 10 // In ms
#define ASCII_SPACE 32
#define ROCKET_SPAWN_OFFSET 31
#define DEG_TO_RAD 0.01745329252
#define ROCKET_SPEED 2
#define ASTEROID_POINTS 10
#define Y_TEXT_MARGIN 40
#define X_TEXT_MARGIN 110
#define ASTEROID_COUNT 15
#define PATTERN_MENU_VALUE 0
#define GRID_MENU_VALUE 1

std::list<Asteroid> asteroids;
std::list<Rocket> rockets;
SpaceShip spaceShip(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 1);
GLint currentPoints = 0;
Background backgroundType = Background::Pattern;

void GenerateNewLevel()
{
	rockets.clear();

	for (size_t i = 0; i < ASTEROID_COUNT; i++)
	{
		asteroids.push_back(level_generation::GetNewAsteroid(&asteroids, 100, WINDOW_WIDTH - 100, 100, WINDOW_HEIGHT - 100, 100));
	}
}

bool IsRocketCollidingWithAsteroid(const std::list<Rocket>::iterator r, const Asteroid& a)
{
	return sqrt(pow(r->GetXPos() - a.GetXPos(), 2) + pow(r->GetYPos() - a.GetYPos(), 2)) <= r->GetRadius() + a.GetRadius();
}

void CheckCollisions()
{
	// Loop through all rockets and all asteroids per rocket
	for (std::list<Rocket>::iterator r = rockets.begin(); r != rockets.end(); r++)
	{
		for (Asteroid& a : asteroids)
		{
			// If a collision is found remove the asteroid and rocket
			if (IsRocketCollidingWithAsteroid(r, a))
			{
				std::list<Rocket>::iterator toRemove = r;
				r++;
				rockets.remove(*toRemove);
				asteroids.remove(a);
				currentPoints += ASTEROID_POINTS;
				break;
			}
		}

		// Since removal of rockets happens mid-loop it is required to check for end and break if it has been reached
		if (r == rockets.end())
		{
			break;
		}
	}
}

std::list<Rocket> GetOutOfBoundsRockets()
{
	std::list<Rocket> outOfBoundsRockets;
	for (Rocket& r : rockets)
	{
		if (r.OutOfBounds(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT))
		{
			outOfBoundsRockets.push_back(r);
		}
	}
	return outOfBoundsRockets;
}

void DestroyOutOfBoundsRockets()
{
	for (Rocket& r : GetOutOfBoundsRockets())
	{
		rockets.remove(r);
	}
}

void DrawRockets()
{
	for (Rocket& r : rockets)
	{
		r.Draw();
	}
}

void UpdateRocketPositions()
{
	for (Rocket& r : rockets)
	{
		r.UpdatePosition();
	}
}

void DrawAsteroids()
{
	for (Asteroid& a : asteroids)
	{
		a.Draw();
	}
}

void DrawObjects()
{
	DrawRockets();
	DrawAsteroids();
	spaceShip.Draw();
}

void DrawPoints()
{
	glColor3f(1, 1, 1);
	glRasterPos2f(WINDOW_WIDTH - X_TEXT_MARGIN, WINDOW_HEIGHT - Y_TEXT_MARGIN);
	std::string str = "Points: " + std::to_string(currentPoints);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(str.c_str()));
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);

	CheckCollisions();
	background::DrawBackground(backgroundType, WINDOW_WIDTH, WINDOW_HEIGHT);

	if (asteroids.size() == 0)
	{
		GenerateNewLevel();
	}

	DestroyOutOfBoundsRockets();
	UpdateRocketPositions();

	DrawObjects();
	DrawPoints();

	glutSwapBuffers();
}

void Reshape(const GLsizei width, const GLsizei height)
{
	if (height == 0 || width == 0)
	{
		return;
	}

	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, width, height);
	glLoadIdentity();

	// Set glOrtho size
	if (width <= height)
	{
		glOrtho(0.0f, WINDOW_WIDTH, 0.0f, WINDOW_HEIGHT * height / width, 1.0f, -1.0f);
	}
	else
	{
		glOrtho(0.0f, WINDOW_WIDTH * width / height, 0.0f, WINDOW_HEIGHT, 1.0f, -1.0f);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void TimerFunction(const GLint value)
{
	glutPostRedisplay();
	glutTimerFunc(TICK_LENGTH, TimerFunction, 0);
}

void HandleKeyDown(const unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		spaceShip.SetTurningDirection(Direction::Left);
		break;

	case 'd':
		spaceShip.SetTurningDirection(Direction::Right);
		break;

	case ASCII_SPACE:
		// Shoot rocket
		rockets.push_back(Rocket(
			ROCKET_SPAWN_OFFSET * cos(spaceShip.GetRotation() * DEG_TO_RAD) + spaceShip.GetXPos(),
			ROCKET_SPAWN_OFFSET * sin(spaceShip.GetRotation() * DEG_TO_RAD) + spaceShip.GetYPos(),
			ROCKET_SPEED,
			spaceShip.GetRotation() * DEG_TO_RAD
		));
		break;
	}
}

void HandleKeyUp(const unsigned char key, int x, int y)
{
	// Stop turning if lifting 'a' or 'd'
	switch (key)
	{
	case 'a':
	case 'd':
		spaceShip.SetTurningDirection(Direction::None);
		break;
	}
}

void HandleMenuInput(const GLint input)
{
	// Switch to the specified background type
	switch (input)
	{
	case PATTERN_MENU_VALUE:
		backgroundType = Background::Pattern;
		break;

	case GRID_MENU_VALUE:
		backgroundType = Background::Grid;
		break;
	}
}

void AddMenuEntries()
{
	glutAddMenuEntry("Pattern", PATTERN_MENU_VALUE);
	glutAddMenuEntry("Grid", GRID_MENU_VALUE);
}

void WriteConsoleInstructions()
{
	std::cout << "Welcome to the asteroid shooter game!\n" <<
		"Press space to shoot, A to turn left and D to turn right.\n" <<
		"You get points by shooting asteroids.\n" <<
		"Shooting all the asteroids will cause new ones to generate.\n" <<
		"Right clicking brings up a menu which lets you change the background.\n" <<
		"Have fun :)" << std::endl;
}

void init()
{
	glClearColor(COLOR_BLACK);
	glClear(GL_COLOR_BUFFER_BIT);
}

void WindowSetup(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);
	glutTimerFunc(TICK_LENGTH, TimerFunction, 0);
	init();
}

void SetupInputCallbacks()
{
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(HandleKeyDown);
	glutKeyboardUpFunc(HandleKeyUp);
}

void MenuSetup()
{
	glutCreateMenu(HandleMenuInput);
	AddMenuEntries();
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv)
{
	WindowSetup(argc, argv);
	SetupInputCallbacks();
	MenuSetup();
	WriteConsoleInstructions();

	srand(time(nullptr));
	GenerateNewLevel();
	glutMainLoop();

	return 0;
}