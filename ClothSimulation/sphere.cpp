#include "sphere.h"
unsigned int Sphere::textureID = 0;
bool Sphere::line_mode = true;
Sphere::Sphere( unsigned int ID,vec3 center, float radius, int cutnumber)
{
	this->ShaderID = ID;
	this->center = center;
	this->radius = radius;
	this->cutnumber = cutnumber;
	this->arraydate = new GLfloat * [cutnumber*2];
	vertexbuffer = new GLuint[cutnumber*2];
	glGenBuffers(cutnumber*2, vertexbuffer);
	for (int i = 1; i < 2*cutnumber-1; i++) {
		arraydate[i] = new GLfloat[(cutnumber*2+1)*2* 3];
	}
	arraydate[0]= new GLfloat[(2*cutnumber +2)*3];
	arraydate[2*cutnumber-1]= new GLfloat[(2*cutnumber + 2) * 3];
	arraydate[0][0] = 0;
	arraydate[0][1] = 1;
	arraydate[0][2] = 0;
	for (int i = 1; i < 2*cutnumber+2; i++) {
		arraydate[0][3 * i] = cos(glm::radians(90.0f - 90.0f / cutnumber))* cos(glm::radians(i*180.0f/cutnumber));
		arraydate[0][3 * i + 1] = sin(glm::radians(90.0f - 90.0f / cutnumber));
		arraydate[0][3 * i+2] = cos(glm::radians(90.0f - 90.0f / cutnumber)) * sin(glm::radians(i * 180.0f / cutnumber));
	}
	for (int i = 0; i < 2 * cutnumber + 2; i++) {
		arraydate[2 * cutnumber-1][3 * i] = arraydate[0][3 * i];
		arraydate[2 * cutnumber-1][3 * i + 1] = -arraydate[0][3 * i+1];
		arraydate[2 * cutnumber-1][3 * i + 2] = arraydate[0][3 * i+2];
	}
	for (int i = 1; i < cutnumber ; i++) {
		float ramin = cos(glm::radians(90.0f - 90.0f / cutnumber * i));
		float hmin = sin(glm::radians(90.0f - 90.0f / cutnumber * i));
		float ramax = cos(glm::radians(90.0f - 90.0f / cutnumber * (i + i)));
		float hmax = sin(glm::radians(90.0f - 90.0f / cutnumber * (i + i)));
		for (int j = 0; j < cutnumber * 2+1; j++) {
			arraydate[i][6 * j] = ramin * cos(glm::radians(j * 180.0f / cutnumber));
			arraydate[i][6 * j + 1] = hmin;
			arraydate[i][6 * j + 2] = ramin* sin(glm::radians(j * 180.0f / cutnumber));
			arraydate[i][6 * j+3] = ramax * cos(glm::radians(j * 180.0f / cutnumber));
			arraydate[i][6 * j + 4] = hmax;
			arraydate[i][6 * j + 5] = ramax* sin(glm::radians(j * 180.0f / cutnumber));
		}
	}
	for (int i = 1; i < cutnumber; i++) {
		for (int j = 0; j < (cutnumber * 2+1)*2; j++) {
			arraydate[2 * cutnumber - 1 - i][3 * j] = arraydate[i][3 * j];
			arraydate[2 * cutnumber - 1 - i][3 * j+1] =- arraydate[i][3 * j+1];
			arraydate[2 * cutnumber - 1 - i][3 * j+2] = arraydate[i][3 * j+2];
		}
	}

}

Sphere::Sphere()
{
}

void Sphere::display()
{
	glUseProgram(ShaderID);
	if (line_mode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform1i(glGetUniformLocation(ShaderID, "enable"), 0);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1i(glGetUniformLocation(ShaderID, "text"), textureID);
		glUniform1i(glGetUniformLocation(ShaderID, "enable"), 1);
	}
	glUseProgram(ShaderID);
	GLuint uniform_model = glGetUniformLocation(ShaderID, "model");
	glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(glm::translate(center)*glm::scale(vec3(radius))));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
	glBufferData(GL_ARRAY_BUFFER,  3 * (2 * cutnumber+2) * sizeof(GLfloat), arraydate[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	if (!line_mode) {
		glEnableVertexAttribArray(1);
		GLuint vBtep;
		glGenBuffers(1, &vBtep);
	    GLfloat* tep = new GLfloat[2 * (2 * cutnumber + 2)];
		tep[0] = 0; tep[1] = 0.5;
		for (int j = 1; j < 2 * cutnumber + 2; j++) {
			tep[2* j] = (float)1 / (2*cutnumber);
			tep[2* j+1] = (float)j / (2 * cutnumber);
		}
		glBindBuffer(GL_ARRAY_BUFFER, vBtep);
		glBufferData(GL_ARRAY_BUFFER, 2 * (2 * cutnumber + 2) * sizeof(GLfloat), tep, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		delete[] tep;
	}
	glDrawArrays(GL_TRIANGLE_FAN, 0, 2 * cutnumber + 2);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2*cutnumber-1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * (2 * cutnumber + 2) * sizeof(GLfloat), arraydate[2 * cutnumber - 1], GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	if (!line_mode) {
		glEnableVertexAttribArray(1);
		GLuint vBtep;
		glGenBuffers(1, &vBtep);
		GLfloat* tep = new GLfloat[2 * (2 * cutnumber + 2)];
		tep[0] = 0; tep[1] = 0.5;
		for (int j = 1; j < 2 * cutnumber + 2; j++) {
			tep[2 * j] = 1-(float)1 / (2 * cutnumber);
			tep[2 * j + 1] = (float)j / (2 * cutnumber);
		}
		glBindBuffer(GL_ARRAY_BUFFER, vBtep);
		glBufferData(GL_ARRAY_BUFFER, 2 * (2 * cutnumber + 2) * sizeof(GLfloat), tep, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		delete[] tep;
	}
	glDrawArrays(GL_TRIANGLE_FAN, 0, 2 * cutnumber + 2);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glEnableVertexAttribArray(0);
	for (int i = 1; i < 2 * cutnumber - 1; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, (cutnumber * 2 + 1) * 2 * 3 * sizeof(GLfloat), arraydate[i], GL_STATIC_DRAW);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		if (!line_mode) {
			glEnableVertexAttribArray(1);
			GLuint vBtep;
			glGenBuffers(1, &vBtep);
			GLfloat* tep = new GLfloat[2 * (cutnumber * 2 + 1) * 2];
			tep[0] = 0; tep[1] = 0.5;
			for (int j = 0; j <  (cutnumber * 2 + 1); j++) {
				tep[4 * j] = 1 - (float)i / (2 * cutnumber);
				tep[4 * j + 1] = (float)j / (2 * cutnumber);
				tep[4 * j + 2] = (float)i + 1 / (2 * cutnumber);
				tep[4 * j + 3] = (float)j / (2 * cutnumber);

			}
			glBindBuffer(GL_ARRAY_BUFFER, vBtep);
			glBufferData(GL_ARRAY_BUFFER, 2 * (cutnumber * 2 + 1) * 2 * sizeof(GLfloat), tep, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(
				1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				2,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			delete[] tep;
		}
		glDrawArrays(GL_TRIANGLE_STRIP, 0, (cutnumber * 2 + 1) * 2);
	}	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


}

void Sphere::collision(Particle* p)
{
	if (glm::length((p->pos - center)) < 1.03f*radius) {
		vec3 radiusnorm = glm::normalize((p->pos - center));
		vec3 relationalraiusX = radius * radiusnorm;
		p->pos = center + 1.03f*relationalraiusX;
		vec3 relationalraiusV = glm::dot(p->velocity, radiusnorm) * radiusnorm;
		p->velocity -= relationalraiusV;
	}
}
