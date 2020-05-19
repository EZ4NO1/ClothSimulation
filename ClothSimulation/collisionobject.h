#pragma once
#include "Particle.h"
class collisionobject
{
public:
	virtual void collision(Particle* p) = 0;
	virtual ~collisionobject() = 0;
};

