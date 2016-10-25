//
// Created by Campbell Pedersen on 24/10/16.
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <GLUT/glut.h>

#define CUBE_SIZE 2.0f

float _cameraAngle = 0.0f;

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
    glutInitWindowPosition(200, 200);
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


    //Camera
    glTranslatef(0.0f, 0.0f, -20.0f);
    glRotatef(-_cameraAngle, 0.0f, 1.0f, 0.0f);
    glPushMatrix();

    //Ambient Light
    GLfloat ambientColour[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColour);

    //DRAWING CUBE
    glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
    glutPostRedisplay();

    //Front face
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);
        glVertex3f(CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE);

    //Back face
        glNormal3f(0.0f, 0.0f, -1.0f);
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

    glutSwapBuffers();
}

void renderText()
{

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
    if(c == 27)
    {
        exit(0);
    }
    if(c == 'x' || c == 'X')
    {
        _cameraAngle += 5.0f;
        if(_cameraAngle >= 360.0)
        {
            _cameraAngle = 0;
        }
    }
    if(c == 'z' || c == 'Z')
    {
        _cameraAngle -= 5.0f;
        if(_cameraAngle <= 0.0)
        {
            _cameraAngle = 360.0;
        }
    }
}