#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 0.0f;
	muevey = 0.0f;
	muevez = 0.0f;
    cambioCamara = false;
    prendeInador=false;
    debugPosicion=false;
    iniciaMoscas = false;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

int Window::Initialise()
{
	// Inicializaci�n de GLFW
	if (!glfwInit())
	{
		printf("Fallo inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	// Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	// para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Proyecto Final Computacion Grafica", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	// Obtener tama�o de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	// MANEJAR TECLADO y MOUSE
	createCallbacks();

	// Se oculta el mouse
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fallo inicializacion de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); // HABILITAR BUFFER DE PROFUNDIDAD
							 //  Asignar valores de la ventana y coordenadas

	// Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	// Callback para detectar que se est� usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow *window, int key, int code, int action, int mode)
{
	Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	
    // Cambio de camara al presionar v
    if (key == GLFW_KEY_V && action == GLFW_RELEASE){
        theWindow->toogleCambioCamara();
    }
    // Boton para debug y obtener la posicion en la que se encuentra la camara
    if (key == GLFW_KEY_Q && action == GLFW_RELEASE){
        theWindow->toogleDebugPosicion();
    }
    
    /** INICIO DE ANIMACIONES **/
    // Inicio/Stop de animacion moscas
    if (key == GLFW_KEY_M && action == GLFW_RELEASE){
        theWindow->toogleIniciaMoscas();
    }
    
    // Inicio animacion de Globito por KeyFrames
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE){
        theWindow->toogleIniciaKeyFramesGlobito();
    }
    
    /** LUCES **/
    // Prende/Apaga luz Innador
    if (key == GLFW_KEY_I && action == GLFW_RELEASE){
        theWindow->tooglePrendeInador();
    }
    
    
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			// printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			// printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow *window, double xPos, double yPos)
{
	Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
