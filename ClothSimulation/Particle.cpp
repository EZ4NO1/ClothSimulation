#include "Particle.h"

Particle::Particle(vec3 pos, float time, vec4 color, vec3 velocity, vec3 accelerate)
{
	this->pos = pos;
	this->color = color;
	this->velocity = velocity;
	this->accelerate = accelerate;
}

Particle::Particle()
{
	Particle(vec3(0,0,0),0);
}

void Particle::update(float time)
{

	velocity += time * accelerate;
	pos += time * velocity;
}

void Particle::print()
{
	std::cout <<"pos  "<< glm::to_string(pos)<<std::endl;
	std::cout <<"velocity  "<< glm::to_string(velocity)<<std::endl;
	std::cout << "acc  "<<glm::to_string(accelerate)<<std::endl;

}
