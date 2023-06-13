#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getmuevey() { return muevey; }
	GLfloat getmuevez() { return muevez; }
	bool getShouldClose()
	{
		return glfwWindowShouldClose(mainWindow);
	}
	bool *getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
    bool getCambioCamara(){return cambioCamara; }
    bool getInicioMoscas(){return iniciaMoscas; }
	bool getInicioCorre() { return iniciaCorre; }
    bool getDebugPosicion(){return debugPosicion; }
    bool getPrendeInador(){return prendeInador; }
    bool getIniciaKeyFramesGlobito(){return iniciaKeyFramesGlobito; }
    void toogleCambioCamara(){cambioCamara = !cambioCamara; }
    void toogleIniciaMoscas(){iniciaMoscas = !iniciaMoscas; }
	void toogleIniciaCorre() { iniciaCorre = !iniciaCorre; }
    void toogleDebugPosicion(){debugPosicion = !debugPosicion; }
    void tooglePrendeInador(){prendeInador = !prendeInador; }
    void toogleIniciaKeyFramesGlobito(){iniciaKeyFramesGlobito = !iniciaKeyFramesGlobito; }

	~Window();

private:
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat muevey;
	GLfloat muevez;
    bool cambioCamara;
    bool iniciaMoscas;
	bool iniciaCorre;
    bool prendeInador;
    bool debugPosicion;
    bool iniciaKeyFramesGlobito;

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow *window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow *window, double xPos, double yPos);
};
