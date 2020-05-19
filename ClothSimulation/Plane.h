#pragma once
#include "collisionobject.h"
#include "include.h"
class Plane :
	public collisionobject
{
public:
	int cutnumber;
	GLuint vertexbuffer;
	GLfloat* arraydate;
	vec3 center;
	vec3 u;
	vec3 v;
	vec2 scale;
	vec2 rotate;
	vec3 normal;
	float frictionu=0.2;
	float limit = 0.2;
	unsigned int ShaderID;
	Plane(unsigned int ID, vec3 center = vec3(0), vec2 scale=vec2(1.0f),vec2 rotate=vec2(0));
	void display();
	void collision(Particle* p);
	Plane();
	static unsigned int textureID;
	static bool line_mode;
};

