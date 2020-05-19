#pragma once
#include "include.h"
#include "Cloth.h"
class AirPillow
{
public:
	Cloth cloth1,cloth2;
public:
	float pressure=10;
	bool line_mode = true;
	void setwind(vec3 (*wind)(vec3));
	AirPillow();
	AirPillow(int width, int height, unsigned int ID, float distancew = 1, float distanceh = 1, bool gravity = true, vec3 center = vec3(0), vec2 rotate = vec2(0), float struct_w = 20, float shear_w = 20, float bend_w = 20);
	void display();
	void setcollision(collisionobject* co);
};

