#include "AirPillow.h"


AirPillow::AirPillow()
{
}

AirPillow::AirPillow(int width, int height, unsigned int ID, float distancew, float distanceh, bool gravity, vec3 center, vec2 rotate, float struct_w, float shear_w, float bend_w)
{
	cloth1 = Cloth(width, height, ID, distancew, distanceh, gravity, center, rotate, struct_w, shear_w, bend_w);
	cloth2 = Cloth(width, height, ID, distancew, distanceh, gravity, center, rotate, struct_w, shear_w, bend_w);
}

void AirPillow::display()
{
	cloth1.pressure = pressure;
	cloth2.pressure = -pressure;
	cloth1.display();
	cloth2.display();
	for (int i = 0; i < cloth1.height; i++) {
		int j = 0;
		Particle* n1 = &cloth1.node[i][j];
		Particle* n2 = &cloth2.node[i][j];
		vec3 midX = (n1->pos + n2->pos) / 2.0f;
		vec3 midV = (n1->velocity + n2->velocity) / 2.0f;
		vec3 midA = (n1->accelerate + n2->accelerate) / 2.0f;
		n1->pos = midX;
		n2->pos = midX;
		n1->velocity = midV;
		n2->velocity = midV;
		n1->accelerate = midA;
		n2->accelerate = midA;
	}

	for (int i = 0; i < cloth1.height; i++) {
		int j = cloth1.width-1;
		Particle* n1 = &cloth1.node[i][j];
		Particle* n2 = &cloth2.node[i][ j ];
		vec3 midX = (n1->pos + n2->pos) / 2.0f;
		vec3 midV = (n1->velocity + n2->velocity) / 2.0f;
		vec3 midA = (n1->accelerate + n2->accelerate) / 2.0f;
		n1->pos = midX;
		n2->pos = midX;
		n1->velocity = midV;
		n2->velocity = midV;
		n1->accelerate = midA;
		n2->accelerate = midA;
	}
	for (int j = 0; j < cloth1.width-1; j++) {
		int i = 0;
		Particle* n1 = &cloth1.node[i][j];
		Particle* n2 = &cloth2.node[i][ j];
		vec3 midX = (n1->pos + n2->pos) / 2.0f;
		vec3 midV = (n1->velocity + n2->velocity) / 2.0f;
		vec3 midA = (n1->accelerate + n2->accelerate) / 2.0f;
		n1->pos = midX;
		n2->pos = midX;
		n1->velocity = midV;
		n2->velocity = midV;
		n1->accelerate = midA;
		n2->accelerate = midA;
	}
	for (int j = 0; j < cloth1.width-1; j++) {
		int i = cloth1.height-1;
		Particle* n1 = &cloth1.node[i][j];
		Particle* n2 = &cloth2.node[i][j];
		vec3 midX = (n1->pos + n2->pos) / 2.0f;
		vec3 midV = (n1->velocity + n2->velocity) / 2.0f;
		vec3 midA = (n1->accelerate + n2->accelerate) / 2.0f;
		n1->pos = midX;
		n2->pos = midX;
		n1->velocity = midV;
		n2->velocity = midV;
		n1->accelerate = midA;
		n2->accelerate = midA;
	}

}
void AirPillow::setcollision(collisionobject* co)
{
	cloth1.collisionSet.push_back(co);
	cloth2.collisionSet.push_back(co);
}
void AirPillow::setwind(vec3(*wind)(vec3))
{
	cloth1.wind = wind;
	cloth2.wind = wind;
}
