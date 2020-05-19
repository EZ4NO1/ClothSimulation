#pragma once
#include"include.h"
using glm::vec3;
using glm::vec4;
class Particle
{
public:
    vec3 pos;
    vec3 velocity;
    vec3 accelerate;
    vec4 color;
    vec3 normal;
    bool fixed=false;
    Particle(vec3 pos, float time,  vec4 color = vec4(1, 1, 1, 1), vec3 velocity = vec3(0, 0, 0), vec3 accelerate = vec3(0, 0, 0));
    Particle();
    void update(float time);
    void print();
};

