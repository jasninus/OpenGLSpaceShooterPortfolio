#pragma once
#include "../include/freeglut.h"

#define VERTEX_COUNT 3
#define RAD_TO_DEG 57.29577951

class Rocket
{
private:
	GLfloat xSpeed, ySpeed, zRotation, colorR = 1, colorG = 0, colorB = 0, xPos, yPos, radius = 7;
	static constexpr GLfloat vertices[]
	{
		10, 0,
		-1, 4,
		-1, -4
	};

public:
	Rocket(GLint xPos, GLint yPos, GLfloat speed, GLfloat direction);

	void Draw() const;
	void UpdatePosition() { xPos += xSpeed;	yPos += ySpeed; }

	GLfloat GetXPos() const { return xPos; }
	GLfloat GetYPos() const { return yPos; }
	GLfloat GetRadius() const { return radius; }

	bool OutOfBounds(GLfloat xMin, GLfloat xMax, GLfloat yMin, GLfloat yMax) const;

	bool operator==(const Rocket& right) const
	{
		return GetXPos() == right.GetXPos() && GetYPos() == right.GetYPos();
	}

	~Rocket();
};
