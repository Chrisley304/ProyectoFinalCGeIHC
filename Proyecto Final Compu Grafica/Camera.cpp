#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	//Camara libre
	// 0 -> Plano XZ
	// 1 -> Libre
	// 2 -> Isometrica
	cameraMode = 0;

	habilitaCambioCamara = 0;
	reinicioCambioCamara = 0;


	//Camara isometrica
	rotation = 0.0f;   // Ángulo de vista inicial de 45 grados
	scale = 1.0f;       // Escala inicial de 1.0
	positionIsometrica = glm::vec3(174.933762f, 180.697876f, -336.841492f);
	frontIsometrica = glm::vec3(-0.406201f, -0.615662f, 0.675249f);
	upIsometrica = glm::vec3(-0.317359f, 0.788011f, 0.527563f);

	zoomFactor = 1.0f;  // Factor de zoom inicial
	zoomSpeed = 0.001f;   // Velocidad de zoom
	habilitaZoom = 0;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	//Controles WASD si se está en modo de camara de plano XZ o libre
	if (cameraMode == 0 or cameraMode == 1) {
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

		/*if (keys[GLFW_KEY_Q]) {
			printf("Vector de front: x=%f, y=%f, z=%f", front.x, front.y, front.z);
			printf("Vector de up: x=%f, y=%f, z=%f", up.x, up.y, up.z);
			printf("Vector de posicion: x=%f, y=%f, z=%f", position.x, position.y, position.z);
		}*/

	}
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

	//Tecla de cambio de tipo de camara
	if (keys[GLFW_KEY_V])
	{
		if (habilitaCambioCamara < 1)
		{
			habilitaCambioCamara++;
			reinicioCambioCamara = 0;
			cameraMode++;
			if (cameraMode == 3) {
				cameraMode = 0;
			}
		}
	}
	//Tecla que habilita
	if (keys[GLFW_KEY_C])
	{
		if (reinicioCambioCamara < 1)
		{
			habilitaCambioCamara = 0;
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
	if (cameraMode == 0 or cameraMode == 1) {
		return glm::lookAt(position, position + front, up);
	}
	//Modo de camara isometrica
	else {
		//return glm::lookAt(positionIsometrica, positionIsometrica + frontIsometrica, upIsometrica);
		
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
	return glm::normalize(front);
}

void Camera::update()
{

	//Camara fija al plano XZ
	if (cameraMode == 0)
	{
		glm::vec3 newFront;
		newFront.x = cos(glm::radians(yaw));
		newFront.y = 0.0f;
		newFront.z = sin(glm::radians(yaw));
		front = glm::normalize(newFront);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));

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
	// Acercamiento o alejamiento de la cámara
	scale += delta;
	// Limitar el valor mínimo de la escala
	if (scale < 0.1f) {
		scale = 0.1f;
	}
}

Camera::~Camera()
{
}
