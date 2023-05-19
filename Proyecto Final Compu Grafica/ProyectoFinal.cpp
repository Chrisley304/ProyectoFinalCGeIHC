/*
   Semestre 2023-2
   Animaci�n:
   1.- Simple o basica:Por banderas y condicionales
   2.- Compleja: Por medio de funciones y algoritmos,Textura Animada.
   Adicional.- Texturizado con transparencia usando Blending: Requerimos dibujar lo que esta atras primero
 */
// para cargar imagen
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
// para probar el importer
// #include<assimp/Importer.hpp>

#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader_light.h"
#include "Skybox.h"
#include "Sphere.h"
#include "Texture.h"
#include "Window.h"

// para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "PointLight.h"
#include "SpotLight.h"
const float toRadians = 3.14159265f / 180.0f;

// variables para animaci�n
float movCoche;
float movZHelicoptero = 0.0f;
float movYHelicoptero = 0.0f;
float movHelicopteroOffset;
float giroHelicoptero;
float giroHelicopteroOffset;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanzaDerecho;
bool avanzaHelicoptero;
bool reversaHelicoptero;
float toffsetu = 0.0f;
float toffsetv = 0.0f;
float movYDado;
float movYDadoOffset;
float giroDado;
float giroDadoOffset;

Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Model Camino_M;

// Coche propio (Vocho):
//Model Vocho;
//Model Llantas_Delanteras_Vocho;
//Model Llantas_Traseras_Vocho;

Skybox skybox;

// materiales
Material Material_brillante;
Material Material_opaco;

// Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
// para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLights_aux[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char *vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char *fShader = "shaders/shader_light.frag";

// c�lculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int *indices, unsigned int indiceCount, GLfloat *vertices, unsigned int verticeCount,
                        unsigned int vLength, unsigned int normalOffset)
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


void CreateObjects()
{
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2};

    GLfloat vertices[] = {
        //	x      y      z			u	  v			nx	  ny    nz
        -1.0f, -1.0f, -0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f};

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3};

    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, -10.0f, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f, 0.0f, 10.0f, 0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, 10.0f, 10.0f, 10.0f, 0.0f, -1.0f, 0.0f};
    unsigned int vegetacionIndices[] = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7};

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
        0, 2, 3};

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
        -1.0f, 0.0f};

    Mesh *obj1 = new Mesh();

    obj1->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj1);

    Mesh *obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj2);

    Mesh *obj3 = new Mesh();
    obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(obj3);

    Mesh *obj4 = new Mesh();
    obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
    meshList.push_back(obj4);

    Mesh *obj5 = new Mesh();
    obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
    meshList.push_back(obj5);
}

void CreateShaders()
{
    Shader *shader1 = new Shader();

    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

int main()
{
    mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
    mainWindow.Initialise();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.5f, 0.5f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();
    pisoTexture = Texture("Textures/piso.tga");
    pisoTexture.LoadTextureA();
    AgaveTexture = Texture("Textures/Agave.tga");
    AgaveTexture.LoadTextureA();

    // Coche propio (Vocho)
//    Vocho = Model();
//    Vocho.LoadModel("Models/vocho.obj");
//    Llantas_Traseras_Vocho = Model();
//    Llantas_Traseras_Vocho.LoadModel("Models/llantasvocho.obj");
//    Llantas_Delanteras_Vocho = Model();
//    Llantas_Delanteras_Vocho.LoadModel("Models/llantasvocho.obj");

    std::vector<std::string> skyboxFaces;
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

    skybox = Skybox(skyboxFaces);

    Material_brillante = Material(4.0f, 256);
    Material_opaco = Material(0.3f, 4);

    // luz direccional, s�lo 1 y siempre debe de existir
    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
                                 0.3f, 0.3f,
                                 0.0f, 0.0f, -1.0f);
    // contador de luces puntuales
    unsigned int pointLightCount = 0;
    // Declaraci�n de primer luz puntual
    //    pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
    //                                0.0f, 1.0f,
    //                                0.0f, 2.5f, 1.5f,
    //                                0.3f, 0.2f, 0.1f);
    //    pointLightCount++;

    unsigned int spotLightCount = 0;
    // linterna
    spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
                              0.0f, 2.0f,
                              0.0f, 0.0f, 0.0f,
                              0.0f, -1.0f, 0.0f,
                              1.0f, 0.0f, 0.0f,
                              5.0f);
    spotLightCount++;

    // luz fija
    //    spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
    //                              1.0f, 2.0f,
    //                              5.0f, 10.0f, 0.0f,
    //                              0.0f, -5.0f, 0.0f,
    //                              1.0f, 0.0f, 0.0f,
    //                              15.0f);
    //    spotLightCount++;

//    // luz del coche
//    spotLights[1] = SpotLight(0.5f, 0.f, 0.5f,   // R, G, B
//                              1.0f, 2.0f,        // Light Intensity, Color Intensity
//                              0.f, 1.0f, -1.0f,  // xpos, ypos, zpos
//                              -1.0f, 0.0f, 0.0f, // xdir, ydir, zdir
//                              0.5f, 0.0f, 0.0f,  // con, lin, exp
//                              5.0f);             // edge
//    spotLightCount++;

    // Copia de luz del coche mirando al frente
//    spotLights_aux[0] = SpotLight(0.5f, 0.0f, 0.5f,  // R, G, B
//                                  1.0f, 2.0f,        // Light Intensity, Color Intensity
//                                  0.f, 1.0f, -1.0f,  // xpos, ypos, zpos
//                                  -1.0f, 0.0f, 0.0f, // xdir, ydir, zdir
//                                  0.5f, 0.0f, 0.0f,  // con, lin, exp
//                                  5.0f);
//
//    // Luz del coche mirando de reversa
//    spotLights_aux[1] = SpotLight(0.5f, 0.0f, 0.5f, // R, G, B
//                                  1.0f, 2.0f,       // Light Intensity, Color Intensity
//                                  0.f, 1.0f, -4.0f, // xpos, ypos, zpos
//                                  1.0f, 0.0f, 0.0f, // xdir, ydir, zdir
//                                  0.5f, 0.0f, 0.0f, // con, lin, exp
//                                  5.0f);            // edge

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
           uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
    GLuint uniformColor = 0;
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

    // Loop mientras no se cierra la ventana
    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

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

        // luz ligada a la camara de tipo flash
        glm::vec3 lowerLight = camera.getCameraPosition();
        lowerLight.y -= 0.3f;
        spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

        // informaci�n al shader de fuentes de iluminaci�n
        shaderList[0].SetDirectionalLight(&mainLight);
        shaderList[0].SetPointLights(pointLights, pointLightCount);

        glm::mat4 model(1.0);
        glm::mat4 modelaux(1.0);
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
        pisoTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

        meshList[2]->RenderMesh();
        
        // Poner los modelos aquí
       
        
        
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

        // para que no se desborde la variable
        if (toffsetu > 1.0)
        {
            toffsetu = 0.0;
        }

        // if (toffsetv > 1.0)
        //	toffsetv = 0;
        // printf("\ntfosset %f \n", toffsetu);
        // pasar a la variable uniform el valor actualizado
        toffset = glm::vec2(toffsetu, toffsetv);

//        model = glm::mat4(1.0);
//        model = glm::translate(model, glm::vec3(0.0f, 0.2f, -15.0f));
//        model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
//        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
//        glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset)); // Esta linea es importante porque le "avisa" al shader que se va a actualizar el valor del offset
//        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//
//        FlechaTexture.UseTexture();
        // Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[4]->RenderMesh();
        glDisable(GL_BLEND);

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
