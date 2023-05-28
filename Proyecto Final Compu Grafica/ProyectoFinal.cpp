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
// OVNI
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
// Luciernagas
float rotLuciernagas;
float rotLuciernagasOffset;
// Elder Bug
float movElderBugX;
float movElderBugZ;
float movElderBugOffset;
float rotElderBug;
float rotElderBugOffset;
// Piolin
float rotPB;
float posPiolin;
bool alaA;
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
Texture MesaMetal;
Texture Pizarron;
Texture Fuego;
Texture ClaseDoof;
Texture ClaseTocino;
Texture ClaseKirby;
Texture CartelBanio;
/********************** Fin Texturas **************************/

/********************** Modelos **************************/
Model Camino_M;
Model Edificio;
Model UFO;
Model Meap;
Model Innador;
Model Doofenshmirtz;
Model Farol;
Model Luciernagas;
Model BancaHollow;
Model Elderbug;
Model MesaBanco;
Model EscritorioTocino;
Model Tocino;
Model PiolinC;
Model PiolinBD;
Model PiolinBI;
Model Huevo;
Model Sarten;
Model Microondas;
Model CartelCafeteria;
Model Pan;
Model Cafe;
Model Queso;
Model Sandwich;
Model Hornet;
Model Link;
Model Kirby;
Model Phineas;
Model Ferb;

Model WC;
Model Urinal;
Model Lavamanos;

/********************** Fin Modelos **************************/

/********************** Avatar **************************/
Model CuerpoPerry;
Model BrazoDerPerry;
Model BrazoIzqPerry;
Model PiernaDerPerry;
Model PiernaIzqPerry;
/********************** Fin Avatar **************************/

/********************** Skybox **************************/
Skybox skybox;
bool cambioSkybox = false;
bool noche = false;
float horaDia = 0.0f;
/********************** Fin Skybox **************************/

/// <summary>
/// Materiales.
/// </summary>
Material Material_brillante;
Material Material_opaco;

// Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
int cameraMode = 0; // Variable que llevara el tipo de camara que se va a utilizar en el programa.

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
		-0.5f,
		-0.5f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.5f,
		-0.5f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.5f,
		0.5f,
		0.0f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		-0.5f,
		0.5f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,

		0.0f,
		-0.5f,
		-0.5f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		-0.5f,
		0.5f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.5f,
		0.5f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.5f,
		-0.5f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
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
		0,1,2,
		2,3,0,
		// right
		4,5,6,
		6,7,4,
		// back
		8,9,10,
		10,11,8,
		// left
		12,13,14,
		14,15,12,
		// bottom
		16,17,18,
		18,19,16,
		// top
		20,21,22,
		22,23,20,
	};
	// Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
	// average normals
	GLfloat cubo_vertices[] = {
		// front
		// x		y		z		S		T			NX		NY		NZ
		-0.5f,-0.5f,0.5f,0.25f,0.33f,0.0f,0.0f,-1.0f, // 0
		0.5f,-0.5f,0.5f,0.5f,0.33f,	0.0f,0.0f,-1.0f, // 1
		0.5f,0.5f,0.5f,	0.5f,0.66f,0.0f,0.0f,-1.0f, // 2
		-0.5f,0.5f,0.5f,0.25f,0.66f,0.0f,0.0f,-1.0f, // 3
		// right
		// x		y		z		S		T
		0.5f,-0.5f,0.5f,0.5f,0.33f,-1.0f,0.0f,0.0f,
		0.5f,-0.5f,-0.5f,0.75f,0.33f,-1.0f,0.0f,0.0f,
		0.5f,0.5f,-0.5f,0.75f,0.66f,-1.0f,0.0f,0.0f,
		0.5f,0.5f,0.5f,0.5f,0.66f,-1.0f,0.0f,0.0f,
		// back
		-0.5f,-0.5f,-0.5f,1.0f,0.33f,0.0f,0.0f,1.0f,
		0.5f,-0.5f,-0.5f,0.75f,0.33f,0.0f,0.0f,1.0f,
		0.5f,0.5f,-0.5f,0.75f,0.66f,0.0f,0.0f,1.0f,
		-0.5f,0.5f,-0.5f,1.0f,0.66f,0.0f,0.0f,1.0f,
		// left
		// x		y		z		S		T
		-0.5f,-0.5f,-0.5f,0.0f,0.33f,1.0f,0.0f,0.0f,
		-0.5f,-0.5f,0.5f,0.25f,0.33f,1.0f,0.0f,0.0f,
		-0.5f,0.5f,0.5f,0.25f,0.66f,1.0f,0.0f,0.0f,
		-0.5f,0.5f,-0.5f,0.0f,0.66f,1.0f,0.0f,0.0f,
		// bottom
		// x		y		z		S		T
		-0.5f,-0.5f,0.5f,0.25f,0.33f,0.0f,1.0f,0.0f,
		0.5f,-0.5f,0.5f,0.5f,0.33f,0.0f,1.0f,0.0f,
		0.5f,-0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,0.25f,0.0f,0.0f,1.0f,0.0f,
		// UP
		// x		y		z		S		T
		-0.5f,0.5f,0.5f,0.25f,0.66f,0.0f,-1.0f,0.0f,
		0.5f,0.5f,0.5f,0.5f,0.66f,0.0f,-1.0f,0.0f,
		0.5f,0.5f,-0.5f,0.5f,1.0f,0.0f,-1.0f,0.0f,
		-0.5f,0.5f,-0.5f,0.25f,1.0f,0.0f,-1.0f,0.0f,
	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);
}

/// <summary>
/// Crea una mesa.
/// </summary>
void CrearMesa()
{
	unsigned int mesa_indices[] = {
		// front
		0,1,2,
		2,3,0,
		// Cara mesa Y
		4,5,6,
		6,7,4,
		// Cara mesa X
		8,9,10,
		10,11,8,
		// Cara mesa -Y
		12,13,14,
		14,15,12,
		// Cara mesa -X
		16,17,18,
		18,19,16,
		// Pata Y-1
		20,21,22,
		22,23,20,
		// Pata Y-2
		24,25,26,
		26,27,24,
		// Pata X-1
		28,29,30,
		30,31,28,
		// Para X-2
		32,33,34,
		34,35,32,
		// Pata -Y-1
		36,37,38,
		38,39,36,
		// Pata -Y-2
		40,41,42,
		42,43,40,
		// Pata -X-1
		44,45,46,
		46,47,44,
		// Pata -X-2
		48,49,50,
		50,51,48,
	};
	// Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
	// average normals
	GLfloat mesa_vertices[] = {
		// parte superior mesa
		// x		y		z		S		T			NX		NY		NZ
		2.5f, 1.5f, -1.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,  // 0
		-2.5f, 1.5f, -1.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, // 1
		-2.5f, 1.5f, 1.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,  // 2
		2.5f, 1.5f, 1.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,   // 3

		// Cara mesa Y
		2.0f, 1.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,  // 4
		-2.0f, 1.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, // 5
		-2.0f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // 6
		2.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,  // 7

		// Cara mesa X
		2.0f, 1.5f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // 8
		2.0f, 1.5f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,  // 9
		2.0f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // 10
		2.0f, 0.5f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // 11

		// Cara mesa -Y
		-2.0f, 1.5f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 12
		2.0f, 1.5f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // 13
		2.0f, 0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // 14
		-2.0f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 15

		// Cara mesa -X
		-2.0f, 1.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,  // 16
		-2.0f, 1.5f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 17
		-2.0f, 0.5f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 18
		-2.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 19

		// Pata Y-1
		2.0f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // 20
		1.8f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, // 21
		1.8f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // 22
		2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, // 23

		// Para Y-2
		-1.8f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // 24
		-2.0f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, // 25
		-2.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // 26
		-1.8f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, // 27

		// Pata X-1
		2.0f, 0.5f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // 28
		2.0f, 0.5f, -0.8f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // 29
		2.0f, 0.0f, -0.8f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, // 30
		2.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // 31

		// Para X-2
		2.0f, 0.5f, 0.8f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // 32
		2.0f, 0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // 33
		2.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, // 34
		2.0f, 0.0f, 0.8f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // 35

		// Pata -Y-1
		-2.0f, 0.5f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 36
		-1.8f, 0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 37
		-1.8f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 38
		-2.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 39

		// Pata -Y-2
		1.8f, 0.5f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 40
		2.0f, 0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 41
		2.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 42
		1.8f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 43

		// Pata -X-1
		-2.0f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 44
		-2.0f, 0.5f, 0.8f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 45
		-2.0f, 0.0f, 0.8f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 46
		-2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 47

		// Pata -X-2
		-2.0f, 0.5f, -0.8f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 48
		-2.0f, 0.5f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 48
		-2.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 49
		-2.0f, 0.0f, -0.8f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 49
	};

	Mesh* mesa = new Mesh();
	mesa->CreateMesh(mesa_vertices, mesa_indices, 416, 78);
	meshList.push_back(mesa);
}
/********************** Fin funciones **************************/

/// <summary>
/// Función main.
/// </summary>
/// <returns></returns>
int main()
{
	/********************** Ventana **************************/
	mainWindow = Window(1920, 1080); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	/********************** Fin Ventana **************************/

	/********************** Llamadas funciones **************************/
	CreateObjects();
	CreateShaders();
	CrearCubo();
	CrearMesa();
	/********************** Fin llamadas funciones **************************/

	camera = Camera(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.5f, 0.5f, cameraMode);

	/********************** Cargas de texturas **************************/
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.jpg");
	pisoTexture.LoadTextureA();
	CartelConstruccion = Texture("Textures/CartelConstruccion.png");
	CartelConstruccion.LoadTextureA();
	ClaseDoof = Texture("Textures/CartelDoof.png");
	ClaseDoof.LoadTextureA();
	ClaseTocino = Texture("Textures/CartelTocino.png");
	ClaseTocino.LoadTextureA();
	ClaseKirby = Texture("Textures/CartelKirby.png");
	ClaseKirby.LoadTextureA();
	CartelBanio = Texture("Textures/Banio.jpg");
	CartelBanio.LoadTextureA();
	MesaMetal = Texture("Textures/MetalMesa.png");
	MesaMetal.LoadTextureA();
	Pizarron = Texture("Textures/Pizarron.jpg");
	Pizarron.LoadTextureA();
	Fuego = Texture("Textures/Fuego.png");
	Fuego.LoadTextureA();
	/********************** Cargas de texturas **************************/

	/********************** Cargas de Modelos **************************/
	Edificio = Model();
	Edificio.LoadModel("Models/Edificio/Edificio.obj");
	UFO = Model();
	UFO.LoadModel("Models/UFO/UFO.obj");
	Meap = Model();
	Meap.LoadModel("Models/Meap/meap.obj");
	Innador = Model();
	Innador.LoadModel("Models/Innador/Innador.obj");
	Doofenshmirtz = Model();
	Doofenshmirtz.LoadModel("Models/Doofenshmirtz/Doofenshmirtz.obj");
	Farol = Model();
	Farol.LoadModel("Models/Hollow/FarolHollow.obj");
	Luciernagas = Model();
	Luciernagas.LoadModel("Models/Hollow/Luciernagas.obj");
	BancaHollow = Model();
	BancaHollow.LoadModel("Models/Hollow/BancaHollow.obj");
	Elderbug = Model();
	Elderbug.LoadModel("Models/Hollow/ElderBug.obj");
	Hornet = Model();
	Hornet.LoadModel("Models/Hollow/Hornet.obj");
	Link = Model();
	Link.LoadModel("Models/Hollow/Link.obj");
	Kirby = Model();
	Kirby.LoadModel("Models/Hollow/Kirby.obj");

	MesaBanco = Model();
	MesaBanco.LoadModel("Models/Cartoon/MesaBanco.obj");
	EscritorioTocino = Model();
	EscritorioTocino.LoadModel("Models/Cartoon/Escritorio.obj");
	Tocino = Model();
	Tocino.LoadModel("Models/Cartoon/TocinoCharacter.obj");
	Huevo = Model();
	Huevo.LoadModel("Models/Cartoon/HuevoMacho.obj");
	Sarten = Model();
	Sarten.LoadModel("Models/Cartoon/Sarten.obj");
	PiolinC = Model();
	PiolinC.LoadModel("Models/Cartoon/PiolinCuerpo.obj");
	PiolinBD = Model();
	PiolinBD.LoadModel("Models/Cartoon/PiolinBD.obj");
	PiolinBI = Model();
	PiolinBI.LoadModel("Models/Cartoon/PiolinBI.obj");

	Microondas = Model();
	Microondas.LoadModel("Models/Cafeteria/Mircoondas.obj");
	CartelCafeteria = Model();
	CartelCafeteria.LoadModel("Models/Cafeteria/Cartel.obj");
	Pan = Model();
	Pan.LoadModel("Models/Cafeteria/Pan.obj");
	Cafe = Model();
	Cafe.LoadModel("Models/Cafeteria/Cafe.obj");
	Queso = Model();
	Queso.LoadModel("Models/Cafeteria/Queso.obj");
	Sandwich = Model();
	Sandwich.LoadModel("Models/Cafeteria/Sandwich.obj");

	CuerpoPerry = Model();
	CuerpoPerry.LoadModel("Models/Perry/cuerpo.obj");
	BrazoDerPerry = Model();
	BrazoDerPerry.LoadModel("Models/Perry/brazoDerecho.obj");
	BrazoIzqPerry = Model();
	BrazoIzqPerry.LoadModel("Models/Perry/brazoIzquierdo.obj");
	PiernaDerPerry = Model();
	PiernaDerPerry.LoadModel("Models/Perry/piernaDerecha.obj");
	PiernaIzqPerry = Model();
	PiernaIzqPerry.LoadModel("Models/Perry/piernaIzquierda.obj");

	Phineas = Model();
	Phineas.LoadModel("Models/Phineas/phineas.obj");
	Ferb = Model();
	Ferb.LoadModel("Models/ferb.obj");

	WC = Model();
	WC.LoadModel("Models/WC/Wc.obj");
	Urinal = Model();
	Urinal.LoadModel("Models/WC/Urinal.obj");
	Lavamanos = Model();
	Lavamanos.LoadModel("Models/WC/Lavamanos.obj");

	/********************** Fin de cargas de Modelos **************************/
	/********************** Skybox **************************/
	std::vector<std::string> skyboxFaces;
	if (!noche)
	{
		skyboxFaces.push_back("Textures/Skybox/Dia/sh_rt.png");
		skyboxFaces.push_back("Textures/Skybox/Dia/sh_lf.png");
		skyboxFaces.push_back("Textures/Skybox/Dia/sh_dn.png");
		skyboxFaces.push_back("Textures/Skybox/Dia/sh_up.png");
		skyboxFaces.push_back("Textures/Skybox/Dia/sh_bk.png");
		skyboxFaces.push_back("Textures/Skybox/Dia/sh_ft.png");
	}
	else
	{
		skyboxFaces.push_back("Textures/Skybox/Noche/sh_rt.png");
		skyboxFaces.push_back("Textures/Skybox/Noche/sh_lf.png");
		skyboxFaces.push_back("Textures/Skybox/Noche/sh_dn.png");
		skyboxFaces.push_back("Textures/Skybox/Noche/sh_up.png");
		skyboxFaces.push_back("Textures/Skybox/Noche/sh_bk.png");
		skyboxFaces.push_back("Textures/Skybox/Noche/sh_ft.png");
	}

	skybox = Skybox(skyboxFaces);
	/********************** Fin Skybox **************************/

	/********************** Luces **************************/
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	// luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, 0.0f, -1.0f);

	/// <summary>
	/// Contador de luces puntuales.
	/// </summary>
	/// <returns></returns>
	unsigned int pointLightCount = 0;
	// Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(0.0f, 1.0f, 0.0f, // RGB
		1.f, 1.f,
		135.0f, 25.0f, -81.0f, // pos
		0.3f, 0.05f, 0.1f);	   // Alcance, Difusión, 0
	pointLightCount++;

	unsigned int spotLightCount = 0;
	unsigned int spotLightAuxCount = 0;

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
	/// Luz Lampara
	/// </summary>
	/// <returns></returns>
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		20.f, 20.0f,
		-100.0f, 45.0f, -68.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.3f, 0.0f, // Alcance, Difusión, 0
		40.0f);			// Angulo de apertura

	spotLights_aux[0] = SpotLight(1.0f, 1.0f, 1.0f,
		20.f, 20.0f,
		-100.0f, 45.0f, -68.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.3f, 0.0f, // Alcance, Difusión, 0
		40.0f);			// Angulo de apertura

	spotLightCount++;
	spotLightAuxCount++;

	/// <summary>
	/// Luz UFO
	/// </summary>
	/// <returns></returns>
	spotLights[2] = SpotLight(0.0f, 1.0f, 0.0f,
		20.f, 20.0f,
		-230.0f, 180.0f, -190.0f,
		0.0f, -1.0f, 0.0f,
		0.8f, 0.05f, 0.0f, // Alcance, Difusión, 0
		15.0f);			 // Angulo de apertura

	spotLights_aux[1] = SpotLight(0.0f, 1.0f, 0.0f,
		20.f, 20.0f,
		-230.0f, 180.0f, -190.0f,
		0.0f, -1.0f, 0.0f,
		0.8f, 0.05f, 0.0f, // Alcance, Difusión, 0
		15.0f);			 // Angulo de apertura

	spotLightCount++;
	spotLightAuxCount++;
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
	// Luciernagas
	rotLuciernagas = 0.0f;
	rotLuciernagasOffset = 1.0f;
	// ElderBug
	movElderBugX = 0.0f;
	movElderBugZ = 0.0f;
	movElderBugOffset = 1.0f;
	rotElderBug = 0.0f;
	rotElderBugOffset = 1.0f;
	int tipoMovElderBug = 1;
	int pausaElderBug = 0;
	// Piolin
	rotPB = -360.0f * 4;
	posPiolin = 0.0f;
	alaA = false;
	// Avatar
	float rot1Avatar = 0.0f;
	float rot2Avatar = 0.0f;
	int direcRot1Avatar = 1;
	int direcRot2Avatar = -1;
	float rotAvatarOffset = 2.0f;

	bool* keys;

	/******************* Fin Animaciones Inicializacion ****************************/

	// Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Tiempo del mundo para ciclo de día/noche
		horaDia += deltaTime * 0.1;
		// DEBUG de Hora dia:
		// printf("Hora día: %f\n",horaDia);

		if (horaDia >= 400.0f)
		{
			horaDia = 0.0f;
			cambioSkybox = true; // Toggle de cambio de Skybox
		}

		/*************************** Animaciones *************************************/
		// Mov UFO y Meap
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
			else
			{
				tipoMovUFO = 3;
				luzUFO = true;
			}
			break;
		case 3:
			if (movMeapY > 0)
			{
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
			if (movUFOX < 300)
			{
				movUFOX += movUFOOffset * deltaTime;
			}
			else
			{
				tipoMovUFO = 5;
			}
			break;
		case 5:
			if (movUFOZ < 300)
			{
				movUFOZ += movUFOOffset * deltaTime;
			}
			else
			{
				tipoMovUFO = 6;
			}
			break;
		case 6:
			if (movUFOX > 0 && movUFOZ > 0)
			{
				movUFOX -= movUFOOffset * deltaTime;
				movUFOZ -= movUFOOffset * deltaTime;
			}
			else
			{
				tipoMovUFO = 7;
			}
			break;
		case 7:
			if (movUFOX > 0 && movUFOZ > 0)
			{
				movUFOX -= movUFOOffset * deltaTime;
				movUFOZ -= movUFOOffset * deltaTime;
			}
			else
			{
				luzUFO = true;
				tipoMovUFO = 8;
			}
			break;
		case 8:
			if (movMeapY < 125)
			{
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
			else
			{
				tipoMovUFO = 1;
			}
			break;
		default:
			tipoMovUFO = 1;
			break;
		}

		// Mov ElderBug.
		switch (tipoMovElderBug)
		{
		case 1:
			movElderBugX = 0;
			movElderBugZ = 0;
			rotElderBug = 0;
			tipoMovElderBug = 2;
			pausaElderBug = 500;
			break;
		case 2:
			if (pausaElderBug > 0)
			{
				pausaElderBug--;
			}
			else
			{
				tipoMovElderBug = 3;
			}
			break;
		case 3:
			if (movElderBugX > -10)
			{
				movElderBugX -= movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 4;
			}
			break;
		case 4:
			if (rotElderBug < 90)
			{
				rotElderBug += rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 5;
			}
			break;
		case 5:
			if (movElderBugZ < 80)
			{
				movElderBugZ += movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 6;
			}
			break;
		case 6:
			if (rotElderBug < 180)
			{
				rotElderBug += rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 7;
			}
			break;
		case 7:
			if (movElderBugX < 90)
			{
				movElderBugX += movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 8;
			}
			break;
		case 8:
			if (rotElderBug > 90)
			{
				rotElderBug -= rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 9;
			}
			break;
		case 9:
			if (movElderBugZ < 280)
			{
				movElderBugZ += movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				pausaElderBug = 400;
				tipoMovElderBug = 10;
			}
			break;
		case 10:
			if (pausaElderBug > 0)
			{
				pausaElderBug--;
			}
			else
			{
				tipoMovElderBug = 11;
			}
			break;
		case 11:
			if (rotElderBug > 0)
			{
				rotElderBug -= rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 12;
			}
			break;
		case 12:
			if (movElderBugX > 20)
			{
				movElderBugX -= movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 13;
			}
			break;
		case 13:
			if (rotElderBug > -90)
			{
				rotElderBug -= rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 14;
			}
			break;
		case 14:
			if (movElderBugZ > 240)
			{
				movElderBugZ -= movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 15;
			}
			break;
		case 15:
			if (rotElderBug < 0)
			{
				rotElderBug += rotElderBugOffset * deltaTime;
			}
			else
			{
				pausaElderBug = 500;
				tipoMovElderBug = 16;
			}
			break;
		case 16:
			if (pausaElderBug > 0)
			{
				pausaElderBug--;
			}
			else
			{
				tipoMovElderBug = 17;
			}
			break;
		case 17:
			if (rotElderBug > -180)
			{
				rotElderBug -= rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 18;
			}
			break;
		case 18:
			if (movElderBugX < 90)
			{
				movElderBugX += movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 19;
			}
			break;
		case 19:
			if (rotElderBug < -90)
			{
				rotElderBug += rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 20;
			}
			break;
		case 20:
			if (movElderBugZ > 80)
			{
				movElderBugZ -= movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 21;
			}
			break;
		case 21:
			if (rotElderBug < 0)
			{
				rotElderBug += rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 22;
			}
			break;
		case 22:
			if (movElderBugX > -10)
			{
				movElderBugX -= movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 23;
			}
			break;
		case 23:
			if (rotElderBug > -90)
			{
				rotElderBug -= rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 24;
			}
			break;
		case 24:
			if (movElderBugZ > 0)
			{
				movElderBugZ -= movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 25;
			}
			break;
		case 25:
			if (rotElderBug > -180)
			{
				rotElderBug -= rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 26;
			}
			break;
		case 26:
			if (movElderBugX < 0)
			{
				movElderBugX += movElderBugOffset * deltaTime * 0.25;
			}
			else
			{
				tipoMovElderBug = 27;
			}
			break;
		case 27:
			if (rotElderBug < -0)
			{
				rotElderBug += rotElderBugOffset * deltaTime;
			}
			else
			{
				tipoMovElderBug = 1;
			}
			break;
		default:
			tipoMovElderBug = 1;
			break;
		}
		rotUFO += rotUFOOffset * deltaTime;

		if (mainWindow.getInicioMoscas())
		{
			rotLuciernagas += rotLuciernagasOffset * deltaTime;
		}

		// Piolin

		if (alaA)
		{
			if (rotPB >= -30)
			{
				rotPB -= deltaTime;
			}
			else
			{
				alaA = !alaA;
			}
		}
		else
		{
			if (rotPB <= 30)
			{
				rotPB += deltaTime;
			}
			else
			{
				alaA = !alaA;
			}
		}

		posPiolin += deltaTime;

		/*************************** FIN Animaciones *************************************/

		// Recibir eventos del usuario
		glfwPollEvents();
		keys = mainWindow.getsKeys();
		camera.keyControl(keys, deltaTime, cameraMode);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Generación de cambio de hora del día

		if (cambioSkybox)
		{
			noche = !noche;
			std::vector<std::string> newSkyboxFaces;
			if (!noche)
			{
				newSkyboxFaces.push_back("Textures/Skybox/Dia/sh_rt.png");
				newSkyboxFaces.push_back("Textures/Skybox/Dia/sh_lf.png");
				newSkyboxFaces.push_back("Textures/Skybox/Dia/sh_dn.png");
				newSkyboxFaces.push_back("Textures/Skybox/Dia/sh_up.png");
				newSkyboxFaces.push_back("Textures/Skybox/Dia/sh_bk.png");
				newSkyboxFaces.push_back("Textures/Skybox/Dia/sh_ft.png");
			}
			else
			{
				newSkyboxFaces.push_back("Textures/Skybox/Noche/sh_rt.png");
				newSkyboxFaces.push_back("Textures/Skybox/Noche/sh_lf.png");
				newSkyboxFaces.push_back("Textures/Skybox/Noche/sh_dn.png");
				newSkyboxFaces.push_back("Textures/Skybox/Noche/sh_up.png");
				newSkyboxFaces.push_back("Textures/Skybox/Noche/sh_bk.png");
				newSkyboxFaces.push_back("Textures/Skybox/Noche/sh_ft.png");
			}
			skybox = Skybox(newSkyboxFaces);
			cambioSkybox = false;
		}

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

		// Luz principal del mundo (cambia con la hora del día)
		if (!noche)
		{
			mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
				0.5f, 0.5f,
				0.0f, 0.0f, -1.0f);
		}
		else
		{
			mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
				0.2f, 0.2f, // Se hace mas oscuro el ambiente
				0.0f, 0.0f, -1.0f);
		}
		shaderList[0].SetDirectionalLight(&mainLight);

		if (mainWindow.getPrendeInador())
		{
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}
		else
		{
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
		}

		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		if (luzUFO)
		{
			// Enciende lampara en la solo en la noche
			if (!noche)
			{
				spotLights[1] = spotLights_aux[1]; // Se coloca primero la luz del UFO
				shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
			}
			else
			{
				spotLights[1] = spotLights_aux[0]; // Se coloca primero la luz del faro
				shaderList[0].SetSpotLights(spotLights, spotLightCount);
			}
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else
		{
			if (!noche)
			{
				shaderList[0].SetSpotLights(spotLights, spotLightCount - 2);
			}
			else
			{
				spotLights[1] = spotLights_aux[0]; // Se coloca primero la luz del faro
				shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
			}
		}

		// spotLights[1].SetPos(glm::vec3(-100.0f + mainWindow.getmuevex(), 60.0f + mainWindow.getmuevey(), -80.0f + mainWindow.getmuevez()));
		/********************** Fin renderizado luces **************************/

		/********************** Renderizado modelos **************************/

		glm::mat4 model(1.0);
		glm::mat4 modelFarol(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(100.0f, 1.0f, 100.0f));
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
		/// Avatar (Perry)
		/// </summary>

		// Movimiento de las partes del cuerpo al mover avatar
		if (keys[GLFW_KEY_W] || keys[GLFW_KEY_A] || keys[GLFW_KEY_S] || keys[GLFW_KEY_D])
		{
			if (rot1Avatar < 90.0 && rot1Avatar > -90.0)
			{
				rot1Avatar += rotAvatarOffset * direcRot1Avatar;
			}
			else
			{
				direcRot1Avatar = -direcRot1Avatar;
				rot1Avatar += rotAvatarOffset * direcRot1Avatar;
			}
			if (rot2Avatar < 90.0 && rot2Avatar > -90.0)
			{
				rot2Avatar += rotAvatarOffset * direcRot2Avatar;
			}
			else
			{
				direcRot2Avatar = -direcRot2Avatar;
				rot2Avatar += rotAvatarOffset * direcRot2Avatar;
			}
		}

		glm::vec3 escalaPerry = glm::vec3(12.f, 12.f, 12.f);
		glm::vec3 cameraOffset(0.0f, -18.0f, 20.0f); // Posición relativa de la cámara respecto al personaje
		// Cuerpo Perry
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 7.0f, 0.0f));
		if (cameraMode == 0)
		{
			//            model = glm::rotate(model, -camera.getCameraDirection().x, glm::vec3(0.0f, 1.0f, 0.0f));
			//            model = glm::rotate(model, camera.getCameraDirection().z, glm::vec3(0.0f, 1.0f, 0.0f));
			// Actualizar la posición del modelo de Perry según la posición de la cámara
			glm::vec3 perryPosition = camera.getCameraPosition() + cameraOffset;
			model = glm::translate(model, perryPosition);
		}
		else
		{
			glm::vec3 perryPosition = camera.getLastCameraPosition() + cameraOffset;
			model = glm::translate(model, perryPosition);
		}
		modelaux = model;
		model = glm::scale(model, escalaPerry);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoPerry.RenderModel();
		// Brazo Derecho
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, rot2Avatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, escalaPerry);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoDerPerry.RenderModel();
		// Brazo Izquierdo
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, rot1Avatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, escalaPerry);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoIzqPerry.RenderModel();
		// Pierna Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.f, -3.2f, 0.3f));
		model = glm::rotate(model, rot1Avatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, escalaPerry);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaDerPerry.RenderModel();
		// Pierna Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.f, -3.2f, 0.3f));
		model = glm::rotate(model, rot2Avatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, escalaPerry);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaIzqPerry.RenderModel();

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
		/// Modelo del innador
		/// </summary>
		/// <returns></returns>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(115.0f, 5.0f, -80.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Innador.RenderModel();

		/// <summary>
		/// Modelo del Dr. Doofenshmirtz
		/// </summary>
		/// <returns></returns>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(165.0f, 0.0f, -80.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.f, 7.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Doofenshmirtz.RenderModel();

		/// <summary>
		/// Modelo de Phineas
		/// </summary>
		/// <returns></returns>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(133.0f, 10.0f, 1.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.f, 7.f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Phineas.RenderModel();

		/// <summary>
		/// Modelo de Ferb
		/// </summary>
		/// <returns></returns>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(167.0f, 10.0f, 4.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.f, 7.f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ferb.RenderModel();

		/// <summary>
		/// FarolHollow
		/// </summary>
		/// <returns></returns>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, 0.0f, -80.0f));
		model = glm::scale(model, glm::vec3(15.f, 15.f, 15.f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelFarol = model;
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Farol.RenderModel();

		/// <summary>
		/// Luciernagas
		/// </summary>
		model = modelFarol;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0));
		model = glm::scale(model, glm::vec3(2.f, 2.f, 2.f));
		model = glm::rotate(model, rotLuciernagas * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luciernagas.RenderModel();

		/// <summary>
		/// Banca Hollow
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, 0.0f, -67.0f));
		model = glm::scale(model, glm::vec3(25.f, 25.f, 25.f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BancaHollow.RenderModel();

		/// <summary>
		/// ElderBug Hollow
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-110.0f + movElderBugX, 0.0f, -80.0f + movElderBugZ));
		model = glm::scale(model, glm::vec3(15.f, 15.f, 15.f));
		model = glm::rotate(model, -110 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotElderBug * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Elderbug.RenderModel();

		/// <summary>
		/// Microondas
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, 12.0f, 220.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Microondas.RenderModel();

		/// <summary>
		/// Cartel cafetería.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, 80.0f));
		model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		// model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CartelCafeteria.RenderModel();

		/// <summary>
		/// Pan.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-80.0f, 7.0f, 220.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pan.RenderModel();

		/// <summary>
		/// Café.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-110.0f, 7.0f, 170.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cafe.RenderModel();

		/// <summary>
		/// Queso.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 8.0f, 220.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Queso.RenderModel();

		/// <summary>
		/// Sandwich.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, 8.0f, 220.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sandwich.RenderModel();

		/// <summary>
		/// Hornet.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -165.0f));
		model = glm::scale(model, glm::vec3(30.f, 30.f, 30.f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Hornet.RenderModel();

		/// <summary>
		/// Link.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-60.0f, 0.0f, -160.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Link.RenderModel();

		/// <summary>
		/// Escritorio Hollow
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(20.0f, 15.0f, -330.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -60 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(36.0f, 36.0f, 36.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		EscritorioTocino.RenderModel();

		/// <summary>
		/// Cartel clase Doof.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(83, 20.f, 25.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.f, 10.0f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ClaseDoof.UseTexture();
		meshList[5]->RenderMesh();

		/// <summary>
		/// Cartel clase Tocino.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(83, 20.f, -90.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.f, 10.0f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ClaseTocino.UseTexture();
		meshList[5]->RenderMesh();

		/// <summary>
		/// Cartel clase Kirby.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-14, 20.f, -102.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.f, 10.0f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ClaseKirby.UseTexture();
		meshList[5]->RenderMesh();

		/// <summary>
		/// Kirby.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-113.0f, 0.0f, -218.0f));
		model = glm::scale(model, glm::vec3(3.f, 3.f, 3.f));
		model = glm::rotate(model, 60 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kirby.RenderModel();

		/// <summary>
		/// Cartel baño.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(108, 20.f, 166.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.f, 10.0f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CartelBanio.UseTexture();
		meshList[5]->RenderMesh();

		/// <summary>
		/// Escritorio Tocino
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(95.0f, 0.0f, -300.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(36.0f, 36.0f, 36.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		EscritorioTocino.RenderModel();

		/// <summary>
		/// Tocino
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(85.0f, 0.0f, -225.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Tocino.RenderModel();

		/// <summary>
		/// Piolin
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(150.0f, 35.0f, -215.0f));
		model = glm::translate(model, glm::vec3(5 * sin(glm::radians(posPiolin)), 0, 5 * cos(glm::radians(posPiolin))));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiolinC.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(150.0f, 42.5f, -215.0f));
		model = glm::translate(model, glm::vec3(5 * sin(glm::radians(posPiolin)), 0, 5 * cos(glm::radians(posPiolin))));
		model = glm::rotate(model, rotPB * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiolinBD.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(150.0f, 42.5f, -215.0f));
		model = glm::translate(model, glm::vec3(5 * sin(glm::radians(posPiolin)), 0, 5 * cos(glm::radians(posPiolin))));
		model = glm::rotate(model, -rotPB * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiolinBI.RenderModel();

		/// <summary>
		/// Huevo
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(180.0f, 0.0f, -260.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Huevo.RenderModel();

		/// <summary>
		/// Sarten
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(130.0f, 0.0f, -348.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sarten.RenderModel();

		/// <summary>
		/// WC
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(265.0f, 0.0f, 270.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WC.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(265.0f, 0.0f, 241.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WC.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(230.0f, 0.0f, 241.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WC.RenderModel();

		/// <summary>
		/// Urinal
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(235.0f, 7.5f, 257.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Urinal.RenderModel();

		/// <summary>
		/// Lavamanos
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 165.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lavamanos.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 346.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lavamanos.RenderModel();

		/// <summary>
		/// Cartel en construccion.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-140.0f, 0.f, 120.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 35 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(15.f, 15.f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CartelConstruccion.UseTexture();
		meshList[5]->RenderMesh();

		/// <summary>
		/// Mesa de metal 1.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 0.f, 220.0f));
		model = glm::scale(model, glm::vec3(20.f, 5.f, 7.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MesaMetal.UseTexture();
		meshList[6]->RenderMesh();

		/// <summary>
		/// Mesa de metal 2.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, 0.f, 220.0f));
		model = glm::scale(model, glm::vec3(20.f, 5.f, 7.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MesaMetal.UseTexture();
		meshList[6]->RenderMesh();

		/// <summary>
		/// Mesa de metal 3.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-110.0f, 0.f, 170.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(24.f, 5.f, 7.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MesaMetal.UseTexture();
		meshList[6]->RenderMesh();

		/// <summary>
		/// Pizarron Hollow.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(31.0f, 22.0f, -170.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(80.f, 25.f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pizarron.UseTexture();
		meshList[5]->RenderMesh();

		/// <summary>
		/// Pizarron Tocino.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(33.0f, 22.0f, -170.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(80.f, 25.f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pizarron.UseTexture();
		meshList[5]->RenderMesh();

		/// <summary>
		/// Pizarron Perry.
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(135.0f, 22.0f, -101.0f));
		model = glm::scale(model, glm::vec3(80.f, 25.f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pizarron.UseTexture();
		meshList[5]->RenderMesh();

		/// <summary>
		/// Para texturas trasnparentes.
		/// </summary>
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Fuego.UseTexture();

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

		/// <summary>
		/// Fuego
		/// </summary>
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f, 25.0f, -199.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 30 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(38.0f, 30.0f, 50.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Fuego.UseTexture();
		meshList[4]->RenderMesh();

		meshList[4]->RenderMesh();
		glDisable(GL_BLEND);

		/** INTERACCION CON TECLAS **/

		// Tecla de cambio de tipo de camara
		//  0 -> Plano XZ
		//  1 -> Libre
		//  2 -> Isometrica
		if (mainWindow.getCambioCamara())
		{
			cameraMode++;
			if (cameraMode >= 3)
			{
				cameraMode = 0;
			}
			mainWindow.toogleCambioCamara();
		}

		// DEBUG de posición
		if (mainWindow.getDebugPosicion())
		{
			printf("Vector de posicion: x=%f, y=%f, z=%f\n", camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
			mainWindow.toogleDebugPosicion();
		}

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
