#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <ctime>

//  Constantes de la simulacion
// ============================================================
const int    ANCHO     = 1000;
const int    ALTO      = 1000;
const int    NUM_BOIDS = 50;
const float  VELOCIDAD = 2.0f;

// Punto 4 - Parametros de las reglas
const float RADIO_VISION      = 80.0f; 
const float RADIO_SEPARACION  = 25.0f; 
const float PESO_SEPARACION   = 0.05f;  
const float PESO_ALINEAMIENTO = 0.01f;  
const float PESO_COHESION     = 0.005f; 
const float VEL_MAX           = 4.0f;  


//  Punto 1 - Estructura del Boid
// ============================================================
struct Boid {
    float x;
    float y;
    float vx;
    float vy;
    float angulo;
    float color_r, color_g, color_b;
};


//  Punto 2 - Inicializacion de la poblacion
// ============================================================
void initBoids(std::vector<Boid>& boids) {
    boids.resize(NUM_BOIDS);

    for (int i = 0; i < NUM_BOIDS; i++) {
        boids[i].x      = (float)(rand() % ANCHO);
        boids[i].y      = (float)(rand() % ALTO);
        boids[i].angulo = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        boids[i].vx     = VELOCIDAD * cos(boids[i].angulo);
        boids[i].vy     = VELOCIDAD * sin(boids[i].angulo);
        boids[i].color_r = (float)(rand() % 255) / 255.0f;
        boids[i].color_g = (float)(rand() % 255) / 255.0f;
        boids[i].color_b = (float)(rand() % 255) / 255.0f;
    }
}


//  Punto 3 - Representacion visual
// ============================================================

void drawBoid(const Boid& b) {
    glPushMatrix();                              

      glTranslatef(b.x, b.y, 0.0f);              

      float grados = b.angulo * (180.0f / M_PI);
      glRotatef(grados, 0.0f, 0.0f, 1.0f);       


      glColor3f(b.color_r, b.color_g, b.color_b);             
      glBegin(GL_TRIANGLES);
          glVertex2f( 8.0f,  0.0f);   
          glVertex2f(-5.0f,  4.0f);   
          glVertex2f(-5.0f, -4.0f); 
      glEnd();

    glPopMatrix();                        
}


//  Punto 4 - reglas de comportamiento
// ============================================================

struct Vec2 { float x, y; };

Vec2 separacion(const std::vector<Boid>& boids, int i) {
    Vec2 fuerza = {0, 0};
    int  n = (int)boids.size();
    for (int j = 0; j < n; j++) {
        if (i == j) continue;
        float dx   = boids[j].x - boids[i].x;
        float dy   = boids[j].y - boids[i].y;
        float dist = sqrt(dx*dx + dy*dy);
        if (dist < RADIO_SEPARACION && dist > 0) {
            fuerza.x -= dx / (dist * dist);
            fuerza.y -= dy / (dist * dist);
        }
    }
    return fuerza;
}


Vec2 alineamiento(const std::vector<Boid>& boids, int i) {
    Vec2  suma   = {0, 0};
    int   vecinos = 0;
    for (int j = 0; j < (int)boids.size(); j++) {
        if (i == j) continue;
        float dx   = boids[j].x - boids[i].x;
        float dy   = boids[j].y - boids[i].y;
        float dist = sqrt(dx*dx + dy*dy);
        if (dist < RADIO_VISION) {
            suma.x += boids[j].vx;
            suma.y += boids[j].vy;
            vecinos++;
        }
    }
    if (vecinos > 0) { 
        suma.x /= vecinos; 
        suma.y /= vecinos; 
    }

    suma.x -= boids[i].vx;
    suma.y -= boids[i].vy;

    return suma;
}


Vec2 cohesion(const std::vector<Boid>& boids, int i) {
    Vec2 centro  = {0, 0};
    int  vecinos = 0;
    for (int j = 0; j < (int)boids.size(); j++) {
        if (i == j) continue;
        float dx   = boids[j].x - boids[i].x;
        float dy   = boids[j].y - boids[i].y;
        float dist = sqrt(dx*dx + dy*dy);
        if (dist < RADIO_VISION) {
            centro.x += boids[j].x;
            centro.y += boids[j].y;
            vecinos++;
        }
    }
    if (vecinos > 0) {
        centro.x = (centro.x / vecinos) - boids[i].x;  
        centro.y = (centro.y / vecinos) - boids[i].y;
    }
    return centro;
}


// aplicar reglas y mover
void updateBoids(std::vector<Boid>& boids) {
    for (int i = 0; i < (int)boids.size(); i++) {

        Vec2 s = separacion  (boids, i);
        Vec2 a = alineamiento(boids, i);
        Vec2 c = cohesion    (boids, i);


        boids[i].vx += PESO_SEPARACION   * s.x
                     + PESO_ALINEAMIENTO * a.x
                     + PESO_COHESION     * c.x;
        boids[i].vy += PESO_SEPARACION   * s.y
                     + PESO_ALINEAMIENTO * a.y
                     + PESO_COHESION     * c.y;

        float rapidez = sqrt(boids[i].vx*boids[i].vx + boids[i].vy*boids[i].vy);
        if (rapidez > VEL_MAX) {
            boids[i].vx = (boids[i].vx / rapidez) * VEL_MAX;
            boids[i].vy = (boids[i].vy / rapidez) * VEL_MAX;
        }

        boids[i].angulo = atan2(boids[i].vy, boids[i].vx);
        boids[i].x += boids[i].vx;
        boids[i].y += boids[i].vy;
    }
}

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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, ANCHO, 0, ALTO, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

//  Main
// ============================================================
int main() {
    srand((unsigned int)time(nullptr));

    initGLUT();

    GLFWwindow* window = initWindow(ANCHO, ALTO, "Boids");
    if (!window) {
        return -1;
    }

    initOpenGL();

    std::vector<Boid> boids;
    initBoids(boids);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        updateBoids(boids);

        for (int i = 0; i < (int)boids.size(); i++) {
            drawBoid(boids[i]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
