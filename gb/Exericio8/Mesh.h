#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"


class Mesh
{
public:
	Mesh() {}
	~Mesh() {}
	void initialize(GLuint VAO, int nVertices, Shader* shader, GLuint textureID, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0), float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0));
	void update();
	void draw();
	void updatePosition(glm::vec3 position);
	void rotate(int speed = 1, glm::vec3 axis = glm::vec3(0.0, 0.0, 0.0));
	void increase(float increaseNumber = 0.1f);
	void decrease(float decreaseNumber = 0.1f);

protected:
	GLuint VAO; //Identificador do Vertex Array Object - V�rtices e seus atributos
	int nVertices;

	//Informa��es sobre as transforma��es a serem aplicadas no objeto
	glm::vec3 position;
	glm::vec3 scale;
	float angle;
	glm::vec3 axis;

	//Refer�ncia (endere�o) do shader
	Shader* shader;

	GLuint textureID;
};
