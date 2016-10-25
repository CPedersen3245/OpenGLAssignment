//
// Created by Campbell Pedersen on 24/10/16.
//

#include <stdio.h>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#define CUBE_SIZE 1.0f

using namespace std;

//Strings for instructions
string instructions[10] = {"Z/z: Zoom in/out" ,
                     "X/x: Rotation around x-axis start/stop",
                     "Y/y: Rotation around y-axis start/stop",
                     "A/a: Animation begin",
                     "F/f: Make animation faster",
                     "S/s: Make animation slower",
                     "T/t: Pause animation",
                     "C/c: Resume animation",
                     "p: Flat shading",
                     "P: Smooth shading"};

//Variables for controlling camera
float _cameraAngleX = 0.0f;
float _cameraAngleY = 0.0f;
bool _rotatingX = false;
bool _rotatingY = false;

//Forward declarations of methods.
void init();
void render();
void resize(int w, int h);
void keyboard(unsigned char c, int x, int y);
void renderText();

//Main to initialize variables, and start GLUT's main loop
int main(int argc, char** argv)
{
    //Create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(300, 300);
    glutInitWindowSize(640, 480);
    glutCreateWindow("CG Assignment 2");

    //Initialization of... things.
    init();

    //Input callbacks
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(render);
    glutReshapeFunc(resize);

    glutMainLoop();
    return 0;
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_NORMALIZE);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Camera angle + zoom
    if(_rotatingX)
    {
        _cameraAngleX += 0.5f;
    }
    if(_rotatingY)
    {
        _cameraAngleY += 0.5f;
    }
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(_cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(_cameraAngleY, 0.0f, 1.0f, 0.0f);
    glutPostRedisplay();
    //Ambient Light
    GLfloat ambientColour[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColour);

    glPushMatrix();
    //DRAWING CUBE
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);

    //Front face
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);

    //Back face
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);
        glVertex3f(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

    //Right face
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE);
        glVertex3f(CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

    //Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

    //Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

    //Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE);
    glEnd();
    glPopMatrix();

    /*Text Rendering*/
    renderText();

    glutSwapBuffers();
}

void renderText()
{
    glMatrixMode(GL_PROJECTION);
    double *matrix = new double[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glOrtho(0, 640, 0, 480, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

    for(int i = 0; i < 10; i++) //For each line
    {
        std::string line = instructions[i]; //Get line from string array
        glRasterPos2f(10.0f, (460.0f - (float)i*15)); //Position of raster goes down each

        for (int j = 0; j < line.length(); j++) //For each character
        {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)line[j]); //Give 'em hell, GLUT!
        }
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w/(double)h, 1.0, 200.0);
}

void keyboard(unsigned char c, int x, int y)
{
    switch(c)
    {
        case 27 :
            exit(0);
            break;

        case 'X': case 'x' :
            _rotatingX = !_rotatingX;
            break;

        case 'Y': case 'y' :
            _rotatingY = !_rotatingY;

    }
}