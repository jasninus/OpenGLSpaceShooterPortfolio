#include "Rocket.h"
#include <cmath>

Rocket::Rocket(const GLint xPos, const GLint yPos, const GLfloat speed, const GLfloat initialRotation) : xPos(xPos), yPos(yPos), zRotation(initialRotation * RAD_TO_DEG)
{
	xSpeed = speed * cos(initialRotation);
	ySpeed = speed * sin(initialRotation);
}

void Rocket::Draw() const
{
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(xPos, yPos, 0);
	glRotatef(zRotation, 0, 0, 1);

	glColor3f(colorR, colorG, colorB);
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_POLYGON, 0, VERTEX_COUNT);

	glPopMatrix();
}

bool Rocket::OutOfBounds(const GLfloat xMin, const GLfloat xMax, const GLfloat yMin, const GLfloat yMax) const
{
	return xPos < xMin || xPos > xMax || yPos < yMin || yPos > yMax;
}

Rocket::~Rocket() {}