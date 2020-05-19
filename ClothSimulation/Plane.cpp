#include "Plane.h"
unsigned int Plane::textureID = 0;
bool Plane::line_mode = true;
Plane::Plane(unsigned int ID, vec3 center , vec2 scale , vec2 rotate ) {
	this->ShaderID = ID;
	this->center = center;
	this->scale = scale;
	this->rotate = rotate;
	glGenBuffers(1, &vertexbuffer);
	glm::mat4 RotateM = glm::mat4(1.0);
	RotateM = glm::rotate(RotateM, glm::radians(rotate.x), vec3(1, 0, 0));
	RotateM = glm::rotate(RotateM, glm::radians(rotate.y), vec3(0, 1, 0));
	this->normal = vec3(RotateM * vec4(0, 1, 0, 1));
	this->u = vec3(RotateM * vec4(1, 0, 0, 1));
	this->v = vec3(RotateM * vec4(0, 0, 1, 1));
	arraydate = new GLfloat[4 * 3];
	vec3 tep[4] = { vec3(-1,0,-1),vec3(-1,0,1), vec3(1,0,-1), vec3(1,0,1) };
	for (int i = 0; i < 4; i++) {
		arraydate[3 * i] = tep[i].x;
		arraydate[3 * i+1] = tep[i].y;
		arraydate[3 * i+2] = tep[i].z;
	}
}
void Plane::display()
{
	glUseProgram(ShaderID);
	if (line_mode){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform1i(glGetUniformLocation(ShaderID, "enable"), 0);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1i(glGetUniformLocation(ShaderID, "text"), textureID);
		glUniform1i(glGetUniformLocation(ShaderID, "enable"), 1);
	}
	//glUniform1i(glGetUniformLocation(ShaderID, "enable_texture"), line_mode ? 0 : 1);
	GLuint uniform_model = glGetUniformLocation(ShaderID, "model");
	glm::mat4 RotateM = glm::mat4(1.0);
	RotateM = glm::rotate(RotateM, glm::radians(rotate.x), vec3(1, 0, 0));
	RotateM = glm::rotate(RotateM, glm::radians(rotate.y), vec3(0, 1, 0));
	glm::mat4 Model = glm::translate(center) * RotateM * glm::scale(vec3(scale[0], 1, scale[1]));
	glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(Model));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * (4) * sizeof(GLfloat), arraydate, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	if (!line_mode) {
		GLuint vBtep;
		glGenBuffers(1, &vBtep);;
		glBindBuffer(GL_ARRAY_BUFFER, vBtep);
		static GLfloat tep[8] = { 0,0,0,1,1,0,1,1 };
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tep, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Plane::collision(Particle* p)
{
	glm::mat4 RotateM = glm::mat4(1.0);
	RotateM = glm::rotate(RotateM, glm::radians(rotate.x), vec3(1, 0, 0));
	RotateM = glm::rotate(RotateM, glm::radians(rotate.y), vec3(0, 1, 0));
	glm::mat4 Model = glm::translate(center) * RotateM * glm::scale(vec3(scale[0], 0, scale[1]));
	this->normal = vec3(RotateM * vec4(0, 1, 0, 1));
	this->u = vec3(RotateM * vec4(1, 0, 0, 1));
	this->v = vec3(RotateM * vec4(0, 0, 1, 1));
	float X_u_value = glm::dot(p->pos - center,this->u);
	float  X_v_value = glm::dot(p->pos - center, this->v);
	if (p->fixed) return;
	if ((X_u_value >= -scale[0]) && (X_u_value <= scale[0]) && (X_v_value >= -scale[0]) && (X_v_value <= scale[0])) {
		float X_n_value = glm::dot(p->pos - center, this->normal);
		vec3 V_n = glm::dot(p->velocity, this->normal) * normal;
		if ((X_n_value < 0.001f) && (X_n_value > -limit)) {
			p->pos -= (X_n_value- 0.001f) * normal;
			p->velocity -= V_n;
			if (glm::length(p->velocity) > 1e-4) {
				float A_value= glm::dot(p->accelerate, this->normal);
				if (A_value < 0) {
					p->accelerate += glm::normalize(p->velocity) * A_value * frictionu;
				}
			}//friction force
		}
	}
}

Plane::Plane()
{
}
