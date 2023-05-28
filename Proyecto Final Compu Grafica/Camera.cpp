#include "Camera.h"
#include <stdio.h>

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed, int startCameraMode)
{
	position = startPosition;
    lastPosition = position;
	worldUp = startUp;
	yaw = startYaw;
    lastYaw = yaw; // Se guarda el ultimo valor de yaw
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);
    lastFront = front;

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	//Camara isometrica
	rotation = 0.0f;   // Ángulo de vista inicial de 45 grados
	scale = 1.0f;       // Escala inicial de 1.0
	positionIsometrica = glm::vec3(174.933762f, 180.697876f, -336.841492f);
	frontIsometrica = glm::vec3(-0.406201f, -0.615662f, 0.675249f);
	upIsometrica = glm::vec3(-0.317359f, 0.788011f, 0.527563f);

	zoomFactor = 1.0f;  // Factor de zoom inicial
	zoomSpeed = 0.001f;   // Velocidad de zoom
	habilitaZoom = 0;
    
    cameraMode = startCameraMode;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime, int newCameraMode)
{
	GLfloat velocity = moveSpeed * deltaTime;
    cameraMode = newCameraMode; // Se actualiza el tipo de camara obtenida del programa principal con la entrada del teclado.

    //Controles WASD para el modo de camara de plano XZ
    if (cameraMode == 0) {
        
        
        if (keys[GLFW_KEY_W])
        {
            lastPosition += front * velocity;
        }

        if (keys[GLFW_KEY_S])
        {
            lastPosition -= front * velocity;
        }

        if (keys[GLFW_KEY_A])
        {
            lastPosition -= right * velocity;
        }

        if (keys[GLFW_KEY_D])
        {
            lastPosition += right * velocity;
        }
        
        position = lastPosition;

    }
    
	//Controles WASD para los modos de camara libre
	if (cameraMode == 1) {
		if (keys[GLFW_KEY_W])
		{
			position += front * velocity;
		}

		if (keys[GLFW_KEY_S])
		{
			position -= front * velocity;
		}

		if (keys[GLFW_KEY_A])
		{
			position -= right * velocity;
		}

		if (keys[GLFW_KEY_D])
		{
			position += right * velocity;
		}
	}
    
    // Camara Isometrica
	else if (cameraMode == 2) {
		if (keys[GLFW_KEY_A]) {
			pan(0.1f, 0.0f);  // Desplazamiento a la izquierda
		}
		else if (keys[GLFW_KEY_D]) {
			pan(-0.1f, 0.0f);   // Desplazamiento a la derecha
		}
		else if (keys[GLFW_KEY_W]) {
			pan(0.0f, 0.1f);   // Desplazamiento hacia arriba
		}
		else if (keys[GLFW_KEY_S]) {
			pan(0.0f, -0.1f);  // Desplazamiento hacia abajo
		}
		else if (keys[GLFW_KEY_X]) {
			if (habilitaZoom < 1)
			{
				habilitaZoom++;
				zoomFactor += zoomSpeed;  // Acercamiento
			}
				habilitaZoom = 0;	
			//zoom(0.1f);        // Acercamiento
			//zoomFactor += zoomSpeed;  // Acercamiento
		}
		else if (keys[GLFW_KEY_Z]) {
			//zoom(-0.1f);       // Alejamiento
			if (habilitaZoom < 1)
			{
				habilitaZoom++;
				zoomFactor -= zoomSpeed;  // Alejamiento
			}
				habilitaZoom = 0;
		}

	}

}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	//Camara fija plano XZ
	if (cameraMode == 0) {
		xChange *= turnSpeed;
		yChange *= turnSpeed;
        
		yaw += xChange;
        lastYaw = yaw; // Se guarda el ultimo valor de yaw
        
        pitch += yChange;
        
        if (pitch > 89.0f)
        {
            pitch = 89.0f;
        }
        
        if (pitch < -89.0f)
        {
            pitch = -89.0f;
        }
	}

	//Camara libre
	else if (cameraMode == 1) {
		xChange *= turnSpeed;
		yChange *= turnSpeed;

		yaw += xChange;

		pitch += yChange;

		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}

		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}
	}

	//Camara isometrica
	else if (cameraMode == 2){

	}


	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
    if (cameraMode == 0) {
        return glm::lookAt(lastPosition, lastPosition + lastFront, up);
    }
    else if (cameraMode == 1) {
		return glm::lookAt(position, position + front, up);
	}
	//Modo de camara isometrica
	else {
		// Ajustar la posición de la cámara isométrica en función del zoom
		positionIsometrica = zoomFactor * positionIsometrica;
		zoomFactor = 1.0f;
		return glm::lookAt(positionIsometrica, positionIsometrica + frontIsometrica, upIsometrica);
	}
	
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}


glm::vec3 Camera::getCameraDirection()
{
    if (cameraMode == 0){
        return glm::normalize(lastFront);
    }else{
        return glm::normalize(front);
    }
}

void Camera::update()
{

	//Camara fija al plano XZ
	if (cameraMode == 0)
	{
        // Front para el movimiento
        front.x = cos(glm::radians(lastYaw));
        front.y = 0.0f;
        front.z = sin(glm::radians(lastYaw));
        
        // front para la camara
        lastFront.x = cos(glm::radians(lastYaw));
        lastFront.y = sin(glm::radians(pitch));
        lastFront.z = sin(glm::radians(lastYaw));
        
        front = glm::normalize(front);
        lastFront = glm::normalize(lastFront);
		right = glm::normalize(glm::cross(lastFront, worldUp));
		up = glm::normalize(glm::cross(right, lastFront));
	}

	//Camara libre
	else if (cameraMode == 1) {
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}

	//Camara isometrica
	else if (cameraMode == 2) {
		// Aplicar transformaciones de vista y proyección en OpenGL
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		// Configurar una proyección ortográfica isométrica
		glOrtho(-scale, scale, -scale, scale, -scale, scale);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		// Aplicar desplazamiento y rotación de la cámara
		glTranslatef(positionIsometrica.x, positionIsometrica.y, positionIsometrica.z);
		glRotatef(rotation, 0.0f, 0.0f, 1.0f);
	}

	
}



//Camara isometrica
void Camera::pan(float dx, float dy) {
	// Desplazamiento de la cámara en los ejes X y Y
	positionIsometrica.x += dx;
	positionIsometrica.y += dy;
}

void Camera::zoom(float delta) {
	// Acercamiento o alejamiento de la camara
	scale += delta;
	// Limitar el valor mínimo de la escala
	if (scale < 0.1f) {
		scale = 0.1f;
	}
}

Camera::~Camera()
{
}
