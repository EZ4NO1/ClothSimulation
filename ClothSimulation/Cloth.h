#pragma once
#include "Particle.h"
#include "collisionobject.h"
using std::vector;
using glm::vec3;
using glm::vec2;

class Cloth {
public:
	class  MassString{
	public:
		typedef struct deltaxv {
			vec3 x;
			vec3 v;
		}deltaxv;
		MassString(Particle* particleA,Particle* particleB,float l0,float elasticK,float resistenceK);
		Particle* particleA;
		Particle* particleB;
		float l0;
		float elasticK;
		float resistenceK;
		void update();
	};
	vec3(*wind)(vec3) = [](vec3 x) {return vec3(0); };
	float airfrictionK = 0.5f;
	static bool line_mode;
	vector<vector<Particle>> node;
	vector<MassString> MassStringVector;
	vector<collisionobject*> collisionSet;
	bool gravityenable=true;
	float gravitya =-0.3;
	float distancew, distanceh;
	int width, height;
	float struct_w, shear_w, bend_w;
	float resist_w =10;
	float pretime;
	GLuint* vertexbuffer;
	GLfloat** arraydate;
	unsigned int ShaderID;
	Cloth(int width,int height, unsigned int ID,float distancew=1,float distanceh=1,bool gravity=true,  vec3 center = vec3(0), vec2 rotate=vec2(0), float struct_w=20,float shear_w=20,float bend_w=20);
	Cloth();
	void Update();
	void display();
	float pressure=0;
	static unsigned int textureID;
};
