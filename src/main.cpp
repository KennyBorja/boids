#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>


//  Punto 1 - Estructura del Boid
// ============================================================
struct Boid {

    float x;      
    float y;      

    float vx;
    float vy;

    float angle;
};



//  Configuracion de GLUT y ventana
// ============================================================
void initGLUT() {
    int argc = 1;
    char *argv[1] = {(char *)"app"};
    glutInit(&argc, argv);
}

GLFWwindow* initWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return nullptr;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        glfwTerminate();
        return nullptr;
    }

    return window;
}

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.08f, 0.09f, 0.13f, 1.0f);
}


//  Main
// ============================================================
int main() {
    initGLUT();

    GLFWwindow* window = initWindow(1000, 1000, "Boids");
    if (!window) {
        return -1;
    }

    initOpenGL();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Aqui se dibujaran los boids en los siguientes puntos

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
