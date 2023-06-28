/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 02/03/2022
 *
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cstdio>
 // GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Mesh.h"
#include "Camera.h"
#include "Hermite.h"
#include "Bezier.h"
#include "CatmullRom.h"


// Protótipos das funções
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
int setupGeometry();
int loadTexture(string path);
void loadOBJ(string path);
void loadMTL(string path);
vector<glm::vec3> generateControlPointsSet(const std::string& input);
std::vector<glm::vec3> createCircularCurve(float radius, int numPoints, float speed = 1.0);


// VARIAVEIS
const GLuint WIDTH = 1000, HEIGHT = 1000;
bool rotateX = false, rotateY = false, rotateZ = false;
vector<GLfloat> positions;
vector<GLfloat> textureCoords;
vector<GLfloat> normals;
vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;
string mtlFile = "";
string texturePath = "";

string tresdModelsPaths = "../../3D_Models";
string sunPath = tresdModelsPaths + "/Sun/bola.obj";
string earth = tresdModelsPaths + "/Earth/bola.obj";
Mesh sun;
float sunY = 0.0f;
float sunX = 0.0f;
float moveNumber = 0.1f;
float maxMove = 10.0f;

float sunScale = 1.0f;
float radius = 2.0f;
float earthSpeed = 5.0f;
int numPoints = 100;
bool isSunSelect = false;
 
Camera camera;


// Função MAIN
int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Suzanne Example", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	Shader shader("../shaders/hello.vs", "../shaders/hello.fs");

	// SUN
	loadOBJ(sunPath);
	loadMTL(tresdModelsPaths + "/Sun/" + mtlFile);
	GLuint textureIDSun = loadTexture(tresdModelsPaths + "/Sun/" + texturePath);
	GLuint VAOSun = setupGeometry();

	glUseProgram(shader.ID);

	sun.initialize(VAOSun, positions.size() / 3, &shader, textureIDSun, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(sunScale));

	// EARTH
	loadOBJ(earth);
	loadMTL(tresdModelsPaths + "/Earth/" + mtlFile);
	GLuint textureIDEarth = loadTexture(tresdModelsPaths + "/Earth/" + texturePath);
	GLuint VAOEarth = setupGeometry();
	glUseProgram(shader.ID);
	Mesh earth;
	earth.initialize(VAOEarth, positions.size() / 3, &shader, textureIDEarth, glm::vec3(6.0f, 0.0f, 1.0f), glm::vec3(0.4f, 0.4f, 0.4f),0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	camera.initialize(&shader, width, height, 0.05f,0.05f,-90.0f,glm::vec3(0.0f,0.0f, -1.0f), glm::vec3(1.5f, 0.0f, 15.0f));


	shader.setVec3("ka", ka[0], ka[1], ka[2]);
	shader.setFloat("kd", 0.5);
	shader.setVec3("ks", ks[0], ks[1], ks[2]);
	shader.setFloat("q", ns);

	shader.setVec3("lightPos", -2.0f, 100.0f, 2.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	//std::vector<glm::vec3> controlPoints = generateControlPointsSet(animation);
	std::vector<glm::vec3> controlPoints = createCircularCurve(radius, numPoints, earthSpeed);

	Bezier bezier;
	bezier.setControlPoints(controlPoints);
	bezier.setShader(&shader);
	bezier.generateCurve(100);
	int nbCurvePoints = bezier.getNbCurvePoints();
	int i = 0;


	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		camera.update();

		sun.updatePosition(glm::vec3(sunX, sunY, 0.0f));
		sun.update();
		if (rotateX) {
			sun.rotate(1, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (rotateY) {
			sun.rotate(1, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (rotateZ) {
			sun.rotate(1, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		sun.draw();


		glm::vec3 pointOnCurve = bezier.getPointOnCurve(i);
		earth.updatePosition(pointOnCurve);
		earth.update();
		earth.rotate(1, glm::vec3(0.0f, 1.0f, 0.0f));
		earth.draw();

		i = (i + 1) % nbCurvePoints;



		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAOSun);
	glDeleteVertexArrays(1, &VAOEarth);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	camera.move(window, key, action);

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		isSunSelect = !isSunSelect;

		if (!isSunSelect) {
			rotateX = false;
			rotateY = false;
			rotateZ = false;
			sunY = 0.0f;
			sunX = 0.0f;
		}
	}
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = isSunSelect;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = isSunSelect;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = isSunSelect;
	}

	if (key == GLFW_KEY_I && isSunSelect && action == GLFW_PRESS) {
		sun.increase();
	}

	if (key == GLFW_KEY_L && isSunSelect && action == GLFW_PRESS) {
		sun.decrease();
	}

	if (isSunSelect && action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_UP:
			if (sunY < maxMove) {
				sunY += moveNumber; // Translação para cima
			}
			else {
				sunY = 0.0f;
			}
			break;
		case GLFW_KEY_DOWN:
			if (sunY > -maxMove) {
				sunY -= moveNumber; // Translação para baixo
			}
			else {
				sunY = 0.0f;
			}
			break;
		case GLFW_KEY_LEFT:
			cout << "max" << maxMove << "sunX" << sunX << endl;
			if (sunX > -maxMove) {
				sunX -= moveNumber; // Translação para a esquerda
			}
			else {
				sunX = 0.0f;
			}
			break;
		case GLFW_KEY_RIGHT:
			if (sunX < maxMove) {
				sunX += moveNumber; // Translação para a direita
			}
			else {
				sunX = 0.0f;
			}
			break;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.rotate(window, xpos, ypos);
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	GLuint VAO, VBO[3];

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), textureCoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	return VAO;

}

void loadOBJ(string path)
{
	vector<glm::vec3> vertexIndices;
	vector<glm::vec2> textureIndices;
	vector<glm::vec3> normalIndices;

	ifstream file(path);
	if (!file.is_open())
	{
		cerr << "Failed to open file: " << path << endl;
		return;
	}

	string line;
	while (getline(file, line))
	{
		istringstream iss(line);
		string prefix;
		iss >> prefix;

		if (prefix == "mtllib")
		{
			iss >> mtlFile;
		}
		else if (prefix == "v")
		{
			float x, y, z;
			iss >> x >> y >> z;
			vertexIndices.push_back(glm::vec3(x, y, z));
		}
		else if (prefix == "vt")
		{
			float u, v;
			iss >> u >> v;
			textureIndices.push_back(glm::vec2(u, v));
		}
		else if (prefix == "vn")
		{
			float x, y, z;
			iss >> x >> y >> z;
			normalIndices.push_back(glm::vec3(x, y, z));
		}
		else if (prefix == "f")
		{
			string v1, v2, v3;
			iss >> v1 >> v2 >> v3;

			glm::ivec3 vIndices, tIndices, nIndices;
			istringstream(v1.substr(0, v1.find('/'))) >> vIndices.x;
			istringstream(v1.substr(v1.find('/') + 1, v1.rfind('/') - v1.find('/') - 1)) >> tIndices.x;
			istringstream(v1.substr(v1.rfind('/') + 1)) >> nIndices.x;
			istringstream(v2.substr(0, v2.find('/'))) >> vIndices.y;
			istringstream(v2.substr(v2.find('/') + 1, v2.rfind('/') - v2.find('/') - 1)) >> tIndices.y;
			istringstream(v2.substr(v2.rfind('/') + 1)) >> nIndices.y;
			istringstream(v3.substr(0, v3.find('/'))) >> vIndices.z;
			istringstream(v3.substr(v3.find('/') + 1, v3.rfind('/') - v3.find('/') - 1)) >> tIndices.z;
			istringstream(v3.substr(v3.rfind('/') + 1)) >> nIndices.z;

			for (int i = 0; i < 3; i++)
			{
				const glm::vec3& vertex = vertexIndices[vIndices[i] - 1];
				const glm::vec2& texture = textureIndices[tIndices[i] - 1];
				const glm::vec3& normal = normalIndices[nIndices[i] - 1];

				positions.push_back(vertex.x);
				positions.push_back(vertex.y);
				positions.push_back(vertex.z);
				textureCoords.push_back(texture.x);
				textureCoords.push_back(texture.y);
				normals.push_back(normal.x);
				normals.push_back(normal.y);
				normals.push_back(normal.z);
			}
		}
	}

	file.close();
}


void loadMTL(string path)
{
	string line, readValue;
	ifstream mtlFile(path);

	while (!mtlFile.eof())
	{
		getline(mtlFile, line);

		istringstream iss(line);

		if (line.find("map_Kd") == 0)
		{
			iss >> readValue >> texturePath;
		}
		else if (line.find("Ka") == 0)
		{
			float ka1, ka2, ka3;
			iss >> readValue >> ka1 >> ka2 >> ka3;
			ka.push_back(ka1);
			ka.push_back(ka2);
			ka.push_back(ka3);
		}
		else if (line.find("Ks") == 0)
		{
			float ks1, ks2, ks3;
			iss >> readValue >> ks1 >> ks2 >> ks3;
			ks.push_back(ks1);
			ks.push_back(ks2);
			ks.push_back(ks3);
		}
		else if (line.find("Ns") == 0)
		{
			iss >> readValue >> ns;
		}
	}
	mtlFile.close();
}

int loadTexture(string path)
{
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}

std::vector<glm::vec3> generateControlPointsSet(const std::string& input) {
	std::vector<glm::vec3> controlPoints;
	std::istringstream iss(input);
	float x, y, z;

	while (iss >> x >> y >> z) {
		controlPoints.push_back(glm::vec3(x, y, z));
	}

	return controlPoints;
}


std::vector<glm::vec3> createCircularCurve(float radius, int numPoints, float speed)
{
	std::vector<glm::vec3> curvePoints;

	float angleIncrement = (2.0f * glm::pi<float>() / numPoints) * speed;
	float currentAngle = 0.0f;

	for (int i = 0; i < numPoints; i++)
	{
		float x = radius * std::cos(currentAngle);
		float y = 0.0f;
		float z = radius * std::sin(currentAngle);

		curvePoints.push_back(glm::vec3(x, y, z));

		currentAngle += angleIncrement;
	}

	return curvePoints;
}