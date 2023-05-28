#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed, int startCameraMode);

	void keyControl(bool* keys, GLfloat deltaTime, int newCameraMode);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	//Camara isometrica
	void pan(float dx, float dy);
	void zoom(float delta);
    
    GLfloat getLastYaw(){return lastYaw;};

    glm::vec3 getCameraPosition();
    glm::vec3 getLastCameraPosition(){return lastPosition;};
    glm::vec3 getCameraDirection();
    glm::mat4 calculateViewMatrix();
    
    bool getMueveCuerpo(){return mueveCuerpo;}

	~Camera();

private:
	glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    GLfloat yaw;
    GLfloat pitch;

    GLfloat moveSpeed;
    GLfloat turnSpeed;
    
    // Movimiento del personaje
    bool mueveCuerpo;
    
    // Tipo de camara
    int cameraMode;
    
    // Camara XY
    glm::vec3 lastPosition; // Almacena la ultima posicion de la camara en XY.
    GLfloat lastYaw; // Valor para almacenar el ultimo movimiento de la camara.
    glm::vec3 lastFront;
    
    //Camara isometrica
    float rotation;     // Ángulo de vista
	float scale;        // Escala de la cámara
	glm::vec3 frontIsometrica;
	glm::vec3 upIsometrica;
	glm::vec3 positionIsometrica;

	float zoomFactor;  // Factor de zoom inicial
	float zoomSpeed;   // Velocidad de zoom
	int habilitaZoom;

	void update();
};

