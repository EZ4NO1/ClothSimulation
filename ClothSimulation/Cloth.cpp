#include "Cloth.h"

 unsigned int Cloth::textureID=0;
 bool Cloth::line_mode = true;
Cloth::Cloth(int width, int height,  unsigned int ID, float distancew, float distanceh, bool gravity, vec3 center, vec2 rotate, float struct_w, float shear_w, float bend_w)
{
	this->ShaderID = ID;
	this->pretime = glfwGetTime();
	this->struct_w = struct_w;
	this->shear_w = shear_w;
	this->bend_w = bend_w;
	this->height = height;
	this->width = width;
	this->distancew = distancew;
	this->distanceh = distanceh;
	this->arraydate = new GLfloat*[height-1];
	vertexbuffer = new GLuint[height - 1];
	glGenBuffers(height - 1, vertexbuffer);
	for (int i = 0; i < height - 1; i++) {
		arraydate[i] = new GLfloat[width*2*3];
	}
	gravityenable = gravity;
	glm::mat4 Model = glm::mat4(1.0);
	Model = glm::translate(Model, center);
	Model = glm::rotate(Model,glm::radians(rotate.x),vec3(1,0,0));
	Model = glm::rotate(Model, glm::radians(rotate.y), vec3(0, 1, 0));
	vec3 posinit = -(((width - 1) * distancew / 2) * vec3(1, 0, 0) + ((height - 1) * distancew / 2) * vec3(0, 1, 0));
	float inittime = glfwGetTime();
	for (int i = 0; i < height; i++) {
		node.push_back(vector<Particle>());
		for (int j = 0; j < width; j++) {
			vec3 before = posinit+j*distancew*vec3(1,0,0)+i*distanceh * vec3(0, 1, 0);
			vec4 after = Model * vec4(before, 1);
			node[i].push_back(Particle(vec3(after),inittime));
		}
	}


	for (int i = 0; i < height - 1; i++) {
		for (int j = 0; j < width - 1; j++) {
			MassStringVector.push_back(MassString(&(node[i][j]), &(node[i + 1][j]), distanceh, struct_w, resist_w));
			MassStringVector.push_back(MassString(&(node[i][j]), &(node[i][j + 1]), distancew, struct_w, resist_w));
		}
	}
	for (int i = 0; i < height - 1; i++) {
		int j = width - 1;
		MassStringVector.push_back(MassString(&(node[i][j]), &(node[i + 1][j]), distanceh, struct_w, resist_w));
	}
	for (int j = 0; j < width - 1; j++) {
		int i = height - 1;
		MassStringVector.push_back(MassString(&(node[i][j]), &(node[i][j + 1]), distancew, struct_w, resist_w));
	}
	for (int i = 0; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			float dis = sqrt(distanceh * distanceh + distancew * distancew);
			MassStringVector.push_back(MassString(&(node[i][j]), &(node[i+1][j + 1]), dis, shear_w, resist_w));
			MassStringVector.push_back(MassString(&(node[i][j]), &(node[i+1][j - 1]), dis, shear_w, resist_w));
		}
	}
	for (int i = 0; i < height - 1; i++) {
		int j = 0;
		float dis = sqrt(distanceh * distanceh + distancew * distancew);
		MassStringVector.push_back(MassString(&(node[i][j]), &(node[i + 1][j + 1]), dis, shear_w, resist_w));
	}
	for (int i = 1; i < height - 1; i++) {
		int j = width - 1;
		float dis = sqrt(distanceh * distanceh + distancew * distancew);
		MassStringVector.push_back(MassString(&(node[i][j]), &(node[i + 1][j -1]), dis, shear_w, resist_w));
	}
	for (int i = 0; i < height - 2; i++) {
		for (int j = 0; j < width - 2; j++) {
			MassStringVector.push_back(MassString(&(node[i][j]), &(node[i + 2][j]),2* distanceh, bend_w, resist_w));
			MassStringVector.push_back(MassString(&(node[i][j]), &(node[i ][j+2]),2* distancew, bend_w, resist_w));
		}
	}
	for (int i = 0; i < height - 2; i++) {
		int j1 = width - 1;
		int j2 = width - 2;
		MassStringVector.push_back(MassString(&(node[i][j1]), &(node[i + 2][j1]), 2 * distanceh, bend_w, resist_w));
		MassStringVector.push_back(MassString(&(node[i][j2]), &(node[i + 2][j2]), 2 * distanceh, bend_w, resist_w));
	}
	for (int j = 0; j < height - 2; j++) {
		int i1 = height - 1;
		int i2 = height - 2;
		MassStringVector.push_back(MassString(&(node[i1][j]), &(node[i1][j + 2]), 2 * distancew, bend_w, resist_w));
		MassStringVector.push_back(MassString(&(node[i2][j]), &(node[i2][j + 2]), 2 * distancew, bend_w, resist_w));
	}
	/*for (int i = 0; i < MassStringVector.size(); i++) {
		deltacache.push_back(MassString::deltaxv());
		deltacache.push_back(MassString::deltaxv());
	}*/
	for (int i = 0; i < height - 1; i++) {
		for (int j = 0; j < width; j++) {
			arraydate[i][6 * j + 0] = node[i][j].pos.x;
			arraydate[i][6 * j + 1] = node[i][j].pos.y;
			arraydate[i][6 * j + 2] = node[i][j].pos.z;
			arraydate[i][6 * j + 3] = node[i + 1][j].pos.x;
			arraydate[i][6 * j + 4] = node[i + 1][j].pos.y;
			arraydate[i][6 * j + 5] = node[i + 1][j].pos.z;
		}
	}
}

Cloth::Cloth()
{
}


void Cloth::Update()
{
	vec3 initial;
	if (!gravityenable)
		initial = vec3(0, 0, 0);
	else
		initial = vec3(0, gravitya,0);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			node[i][j].accelerate = initial;
		}
	}
	for (int i = 0; i < MassStringVector.size(); i++) {
		MassStringVector[i].update();
	}
	
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			bool en[3][3];
			vec3 deltaX[3][3];
			for (int p = 0; p < 3; p++) {
				for (int q = 0; q < 3; q++) {
					if ((i + p - 1 >= 0 && i + p - 1 < height) && (j + q - 1 >= 0 && j + q - 1 < width)) 
						en[p][q] = true;
					else en[p][q] = false;
				}
			}
			
			for (int p = 0; p < 3; p++) {
				for (int q = 0; q < 3; q++) {
					if ((en[p][q]))
						deltaX[p][q] = node[i + p - 1][j + q - 1].pos - node[i][j].pos;
				}
			}
			vec3 sum = vec3(0);
			if (en[2][2]) {
				sum += glm::normalize(glm::cross(deltaX[1][2], deltaX[2][1]));
			}
			if (en[2][0]) {
				sum += glm::normalize(glm::cross(deltaX[2][1], deltaX[2][0]));
				sum += glm::normalize(glm::cross(deltaX[2][0], deltaX[1][0]));
			}
			if (en[0][0]) {
				sum+= glm::normalize(glm::cross( deltaX[1][0],deltaX[0][1]));
			}
			if (en[0][2]) {
				sum+= glm::normalize(glm::cross( deltaX[0][1],deltaX[0][2]));
				sum+= glm::normalize(glm::cross( deltaX[0][2],deltaX[1][2]));
			}
			node[i][j].normal = sum;
		}
	}
	
	float area = distanceh * distancew ;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			vec3 Vrelational = wind(node[i][j].pos) - node[i][j].velocity;
			float V_value = glm::dot(Vrelational, node[i][j].normal);
			node[i][j].accelerate += airfrictionK * area * V_value * node[i][j].normal;
		}
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			node[i][j].accelerate += area * pressure * node[i][j].normal;
		}
	}
	for (collisionobject* cp : collisionSet) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				cp->collision(&node[i][j]);
			}
		}
	}


	float time = glfwGetTime();
	float delta = time - pretime;
	pretime = time;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (!node[i][j].fixed) {
				node[i][j].update(delta);
			}
		}
	}
	
	for (int i = 0; i < height - 1; i++) {
		for (int j = 0; j < width; j++) {
			arraydate[i][6 * j + 0] = node[i][j].pos.x;
			arraydate[i][6 * j + 1] = node[i][j].pos.y;
			arraydate[i][6 * j + 2] = node[i][j].pos.z;
			arraydate[i][6 * j + 3] = node[i + 1][j].pos.x;
			arraydate[i][6 * j + 4] = node[i + 1][j].pos.y;
			arraydate[i][6 * j + 5] = node[i + 1][j].pos.z;
		}
	}

}

void Cloth::display()
{
	//std::cout << line_mode << std::endl;
	glUseProgram(ShaderID);
	if (Cloth::line_mode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform1i(glGetUniformLocation(ShaderID, "enable"), 0);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1i(glGetUniformLocation(ShaderID, "text"), textureID);
		glUniform1i(glGetUniformLocation(ShaderID, "enable"), 1);
	}
	//glUniform1i(glGetUniformLocation(ShaderID, "enable"), 1);

	Update();
	GLuint uniform_model = glGetUniformLocation(ShaderID, "model");
	glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	for (int i = 0; i <height - 1; i++) {
	
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, 2 * 3 * width * sizeof(GLfloat),arraydate[i], GL_DYNAMIC_DRAW);
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
			GLfloat* tep = new GLfloat[2 * 2 * width];
			for (int j = 0; j < width; j++) {
				tep[4 * j] = (float)i / (height - 1);
				tep[4 * j + 1] = (float)j / (width - 1);
				tep[4*j+2]= (float)(i+1) / (height - 1);
				tep[4*j+3]= (float)j / (width - 1);
			}
			glBindBuffer(GL_ARRAY_BUFFER, vBtep);
			glBufferData(GL_ARRAY_BUFFER, 2 * 2 * width * sizeof(GLfloat), tep, GL_DYNAMIC_DRAW);
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
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * width);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


 Cloth::MassString::MassString(Particle* particleA, Particle* particleB, float l0,float elasticK, float resistenceK) {
	 this->particleA = particleA;
	 this->particleB = particleB;
	 this->elasticK = elasticK;
	 this->resistenceK = resistenceK;
	 this->l0 = l0;
}

 void Cloth::MassString::update()
 {
	 vec3 pos_delta = particleA->pos - particleB->pos;
	 vec3 F_elastic=(glm::length(pos_delta) - l0) * elasticK * pos_delta;
	 particleA->accelerate -= F_elastic;
	 particleB->accelerate += F_elastic;
	 vec3 relational_velocity= particleA->velocity - particleB->velocity;
	 vec3 relational_velocity_pro =glm::dot<3,float,glm::qualifier::highp>(relational_velocity , pos_delta) * (glm::normalize(pos_delta));
	 vec3 F_resistence = resistenceK * relational_velocity_pro;
	 particleA->accelerate -= F_resistence;
	 particleB->accelerate += F_resistence;
 }
