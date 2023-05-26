/*
Proyecto final de Computación Gráfica e Iteracción Humano-Computadora.
*/
#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader_light.h"
#include "Skybox.h"
#include "Sphere.h"
#include "Texture.h"
#include "Window.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "PointLight.h"
#include "SpotLight.h"
const float toRadians = 3.14159265f / 180.0f;

/********************** Inicio variables animación **************************/
//OVNI
float movUFOX;
float movUFOY;
float movUFOZ;
float rotUFO;
float rotUFOOffset;
float movUFOOffset;
float toffsetu = 0.0f;
float toffsetv = 0.0f;
// Meap
float movMeapX;
float movMeapY;
float movMeapZ;
float rotMeap;
float movMeapOffset;
float rotMeapOffset;
/********************** Fin Variables animación **************************/

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

/********************** Texturas **************************/
Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture CartelConstruccion;
/********************** Fin Texturas **************************/

/********************** Modelos **************************/
Model Camino_M;
Model Edificio;
Model UFO;
Model Meap;
Model Innador;
/********************** Fin Modelos **************************/

Skybox skybox;

/// <summary>
/// Materiales.
/// </summary>
Material Material_brillante;
Material Material_opaco;

// Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

/// <summary>
/// Luz direccional.
/// </summary>
DirectionalLight mainLight;

/// <summary>
/// Máximo de luces puntuales.
/// </summary>
PointLight pointLights[MAX_POINT_LIGHTS];

/// <summary>
/// Máximo de luces direccionales.
/// </summary>
SpotLight spotLights[MAX_SPOT_LIGHTS];

/// <summary>
/// Luces direccionales auxiliares.
/// </summary>
SpotLight spotLights_aux[MAX_SPOT_LIGHTS];

/// <summary>
/// Vertex shader.
/// </summary>
static const char* vShader = "shaders/shader_light.vert";

/// <summary>
/// Fragment shader.
/// </summary>
static const char* fShader = "shaders/shader_light.frag";

/********************** Funciones **************************/
/// <summary>
/// Cálculo del promedio de las normales para sombreado de Phong
/// </summary>
/// <param name="indices"></param>
/// <param name="indiceCount"></param>
/// <param name="vertices"></param>
/// <param name="verticeCount"></param>
/// <param name="vLength"></param>
/// <param name="normalOffset"></param>
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;
		vertices[in0] += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x;
		vertices[nOffset + 1] = vec.y;
		vertices[nOffset + 2] = vec.z;
	}
}

/// <summary>
/// Creador de objetos por vectores.
/// </summary>
void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2 };

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
		-1.0f, -1.0f, -0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f };

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3 };

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f, 0.0f, 10.0f, 0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f, 10.0f, 10.0f, 0.0f, -1.0f, 0.0f };
	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7 };

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	};

	unsigned int flechaIndices[] = {
		0, 1, 2,
		0, 2, 3 };

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.5f,
		0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.5f, 0.0f,
		-0.5f, 1.0f, 1.0f,
		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,
		0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f };

	Mesh* obj1 = new Mesh();

	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);
}

/// <summary>
/// Creador de shaders.
/// </summary>
void CreateShaders()
{
	Shader* shader1 = new Shader();

	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

/// <summary>
/// Crea un cubo.
/// </summary>
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.25f,  0.33f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.5f,	0.33f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.5f,	0.66f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.25f,	0.66f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.5f,  0.33f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.75f,	0.33f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.75f,	0.66f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.5f,	0.66f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	1.0f,  0.33f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.75f,	0.33f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.75f,	0.66f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	1.0f,	0.66f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,  0.33f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.25f,	0.33f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.25f,	0.66f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.66f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.25f,  0.33f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.5f,	0.33f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.5f,	0.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.25f,	0.0f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.25f,  0.66f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.5f,	0.66f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.5f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.25f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}
/********************** Fin funciones **************************/

/// <summary>
/// Función main.
/// </summary>
/// <returns></returns>
int main()
{
	/********************** Ventana **************************/
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	/********************** Fin Ventana **************************/

	/********************** Llamadas funciones **************************/
	CreateObjects();
	CreateShaders();
	CrearCubo();
	/********************** Fin llamadas funciones **************************/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.5f, 0.5f);

	/********************** Cargas de texturas **************************/
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.jpg");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	CartelConstruccion = Texture("Textures/CartelConstruccion.png");
	CartelConstruccion.LoadTextureA();
	/********************** Cargas de texturas **************************/

	/********************** Cargas de Modelos **************************/
	Edificio = Model();
	Edificio.LoadModel("Models/Edificio/Edificio.obj");
	UFO = Model();
	UFO.LoadModel("Models/UFO/UFO.obj");
	Meap = Model();
	Meap.LoadModel("Models/Meap/meap.obj");
	Innador = Model();
	Innador.LoadModel("Models/Innador.obj");
    
	/********************** Fin de cargas de Modelos **************************/
	/********************** Skybox **************************/
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/Dia/rainbow_rt.png");
	skyboxFaces.push_back("Textures/Skybox/Dia/rainbow_lf.png");
	skyboxFaces.push_back("Textures/Skybox/Dia/rainbow_dn.png");
	skyboxFaces.push_back("Textures/Skybox/Dia/rainbow_up.png");
	skyboxFaces.push_back("Textures/Skybox/Dia/rainbow_bk.png");
	skyboxFaces.push_back("Textures/Skybox/Dia/rainbow_ft.png");

	skybox = Skybox(skyboxFaces);
	/********************** Fin Skybox **************************/

	/********************** Luces **************************/
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	// luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	/// <summary>
	/// Contador de luces puntuales.
	/// </summary>
	/// <returns></returns>
	unsigned int pointLightCount = 0;
	// Declaraci�n de primer luz puntual
	//    pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
	//                                0.0f, 1.0f,
	//                                0.0f, 2.5f, 1.5f,
	//                                0.3f, 0.2f, 0.1f);
	//    pointLightCount++;

	unsigned int spotLightCount = 0;

	/// <summary>
	/// Linterna.
	/// </summary>
	/// <returns></returns>
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	/// <summary>
	/// Luz UFO
	/// </summary>
	/// <returns></returns>
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		20.f, 20.0f,
		-230.0f, 180.0f, -190.0f,
		0.0f, -1.0f, 0.0f,
		0.8f, 0.05f, 0.0f, // Alcance, Difusión, 0
		15.0f); // Angulo de apertura

	spotLightCount++;
	/********************** Fin Luces **************************/

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	/******************* Animaciones Inicializacion ****************************/
	// OVNI
	rotUFO = 0.0f;
	rotUFOOffset = 1.0f;
	movUFOOffset = 1.0f;
	int tipoMovUFO = 1;
	bool luzUFO = false;
	// Meap
	movMeapOffset = 1.0f;
	rotMeapOffset = 1.0f;
	float MeapPosX = 0.0f;
	float MeapPosZ = 0.0f;

	/******************* Fin Animaciones Inicializacion ****************************/


	// Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		/*************************** Animaciones *************************************/
		switch (tipoMovUFO)
		{
		case 1:
			movUFOX = -300.0f;
			movUFOZ = -300.0f;
			movMeapX = -300.0f;
			movMeapY = 125.0f;
			movMeapZ = -300.0f;
			MeapPosX = -230.0;
			MeapPosZ = -90.0;
			rotMeap = 0.0f;
			tipoMovUFO = 2;
			break;
		case 2:
			if (movUFOX < 0 && movUFOZ < 0)
			{
				movUFOX += movUFOOffset * deltaTime;
				movUFOZ += movUFOOffset * deltaTime;
				movMeapX += movUFOOffset * deltaTime;
				movMeapZ += movUFOOffset * deltaTime;
			}
			else {
				tipoMovUFO = 3;
				luzUFO = true;
			}
			break;
		case 3:
			if (movMeapY > 0) {
				movMeapY -= movMeapOffset * deltaTime;
				rotMeap += rotMeapOffset * deltaTime * 3;
			}
			else
			{
				luzUFO = false;
				tipoMovUFO = 4;
			}
			break;
		case 4:
			if (movUFOX < 300) {
				movUFOX += movUFOOffset * deltaTime;
			}
			else {
				tipoMovUFO = 5;
			}
			break;
		case 5:
			if (movUFOZ < 300) {
				movUFOZ += movUFOOffset * deltaTime;
			}
			else {
				tipoMovUFO = 6;
			}
			break;
		case 6:
			if (movUFOX > 0 && movUFOZ > 0)
			{
				movUFOX -= movUFOOffset * deltaTime;
				movUFOZ -= movUFOOffset * deltaTime;
			}
			else {
				tipoMovUFO = 7;
			}
			break;
		case 7:
			if (movUFOX > 0 && movUFOZ > 0)
			{
				movUFOX -= movUFOOffset * deltaTime;
				movUFOZ -= movUFOOffset * deltaTime;
			}
			else {
				luzUFO = true;
				tipoMovUFO = 8;
			}
			break;
		case 8:
			if (movMeapY < 125) {
				movMeapY += movMeapOffset * deltaTime;
				rotMeap -= rotMeapOffset * deltaTime * 3;
			}
			else
			{
				luzUFO = false;
				tipoMovUFO = 9;
			}
			break;
		case 9:
			if (movUFOX > -300 && movUFOZ > -300)
			{
				movUFOX -= movUFOOffset * deltaTime;
				movUFOZ -= movUFOOffset * deltaTime;
				movMeapX -= movUFOOffset * deltaTime;
				movMeapZ -= movUFOOffset * deltaTime;
			}
			else {
				tipoMovUFO = 1;
			}
			break;
		default:
			tipoMovUFO = 1;
			break;
		}
		rotUFO += rotUFOOffset * deltaTime;

		/*************************** FIN Animaciones *************************************/


		// Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		// informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		/********************** Renderizado Luces **************************/
				// luz ligada a la camara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;

		// informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);

		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		if (luzUFO) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else
		{
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		}
		/********************** Fin renderizado luces **************************/

		/********************** Renderizado modelos **************************/

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 1.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		/// <summary>
		/// Modelo del edificio
		/// </summary>
		/// <returns></returns>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Edificio.RenderModel();

		/// <summary>
		/// UFO
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-230.0f + movUFOX, 120.0f + movUFOY, -190 + movUFOZ));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, rotUFO * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		UFO.RenderModel();

        /// <summary>
        /// Modelo de Meap.
        /// </summary>
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-230.0f + movMeapX, 0.0f + movMeapY, -190 + movMeapZ));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
        model = glm::rotate(model, rotMeap * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Meap.RenderModel();

        /// <summary>
        /// Modelo del edificio
        /// </summary>
        /// <returns></returns>
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.050f, 0.05f, 0.05f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Innador.RenderModel();

		/// <summary>
		/// Cartel en construccion.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, 25.f, -101.0f));
		model = glm::scale(model, glm::vec3(15.f, 15.f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CartelConstruccion.UseTexture();
		meshList[5]->RenderMesh();

		// Las texturas transparentes, se ponen al final xd
		// Agave �qu� sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -20.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		// blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		// textura con movimiento
		// Importantes porque la variable uniform no podemos modificarla directamente
		toffsetu += 0.001 * deltaTime;
		toffsetv += 0.0 * deltaTime;

		/********************** Texturas animadas. ************************************/
				// para que no se desborde la variable
		if (toffsetu > 1.0)
		{
			toffsetu = 0.0;
		}
		toffset = glm::vec2(toffsetu, toffsetv);

		meshList[4]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
