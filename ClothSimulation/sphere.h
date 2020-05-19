#pragma once
#include "include.h"
#include "collisionobject.h"
using glm::vec3;
class Sphere:public collisionobject
{
	public:
	int cutnumber;
	GLuint* vertexbuffer;
	GLfloat** arraydate;
	vec3 center;
	float radius;
	unsigned ShaderID;
	Sphere(unsigned int ID,vec3 center= vec3(0) , float radius=1.0f,int cutnumber=20);
	Sphere();
	void display();
	void collision(Particle* p);
	static unsigned int textureID;
	static bool line_mode;
};

