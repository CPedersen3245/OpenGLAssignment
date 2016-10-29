//
// Created by Campbell Pedersen on 24/10/16.
//

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "glm/ext.hpp"


#define CUBE_SIZE 2.0
#define GROUND_SIZE 40.0

using namespace std;

//Strings for instructions
string instructions[10] = {"Z/z: Zoom in/out" , //Done
                           "X/x: Rotation around x-axis start/stop", //Done
                           "Y/y: Rotation around y-axis start/stop", //Done
                           "A/a: Animation begin",
                           "F/f: Make animation faster",
                           "S/s: Make animation slower",
                           "T/t: Pause animation",
                           "C/c: Resume animation",
                           "p: Flat shading", //Done
                           "P: Smooth shading"}; //Done

//Variables for controlling camera
float _cameraAngleX = 0.0f;
float _cameraAngleY = 0.0f;
float _zoom = 0.0f;
bool _rotatingX = false;
bool _rotatingY = false;

//Variables for controlling shading
bool _smooth = false;

//Forward declarations of methods.
void init();
void render();
void resize(int w, int h);
void keyboard(unsigned char c, int x, int y);

bool loadOBJ(
        const char * path,
        vector < glm::vec3 > &out_vertices,
        vector < glm::vec2 > &out_uvs,
        vector < glm::vec3 > &out_normals
);
void renderText();

//Main to initialize variables, and start GLUT's main loop
int main(int argc, char** argv)
{
    //Create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(300, 300);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Under the sea");

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
    glClearColor(0.2f, 0.2f, 0.8f, 1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_CULL_FACE);

    glFrustum(-1.0, 1.0, -1.0, 1.0, 0.0, 40.0);
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
    glScalef(1+_zoom, 1+_zoom, 1.0f);

    //Shading
    if(_smooth)
    {
        glShadeModel(GL_SMOOTH);
    }
    else
    {
        glShadeModel(GL_FLAT);
    }

    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(_cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(_cameraAngleY, 0.0f, 1.0f, 0.0f);
    glutPostRedisplay();

    //Ambient Light
    GLfloat ambientColour[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColour);

    glPushMatrix();

    glColor3f(0.62f, 0.53f, 0.32f);

    //Ocean Floor Rendering
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(GROUND_SIZE, -1.0f, GROUND_SIZE);
        glVertex3f(GROUND_SIZE, -1.0f, -GROUND_SIZE);
        glVertex3f(-GROUND_SIZE, -1.0f, -GROUND_SIZE);
        glVertex3f(-GROUND_SIZE, -1.0f, GROUND_SIZE);
    glEnd();

    //Load object
    vector< glm::vec3 > vertices;
    vector< glm::vec2 > uvs;
    vector< glm::vec3 > normals;
    bool res = loadOBJ("objects/cube.obj", vertices, uvs, normals);

    glColor3f(0.13f, 0.24f, 0.19f);
    glPushMatrix();
    glBegin(GL_TRIANGLES);
        for(int i=0; i<vertices.size(); i += 3)
        {
            glNormal3f(normals.at(i).x, normals.at(i).y, normals.at(i).z);
            glTexCoord2f(uvs.at(i).x, uvs.at(i).y);
            glVertex3f(vertices.at(i).x, vertices.at(i).y, vertices.at(i).z);
            glTexCoord2f(uvs.at(i+1).x, uvs.at(i+1).y);
            glVertex3f(vertices.at(i+1).x, vertices.at(i+1).y, vertices.at(i+1).z);
            glTexCoord2f(uvs.at(i+2).x, uvs.at(i+2).y);
            glVertex3f(vertices.at(i+2).x, vertices.at(i+2).y, vertices.at(i+2).z);
        }
    glEnd();
    glPopMatrix();

    /*
    //SPHERE
    GLUquadricObj* quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricDrawStyle( quadratic, GLU_FILL);
    gluQuadricTexture(quadratic, GL_TRUE);

    glTranslatef(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);
    gluSphere(quadratic, 0.5f, 10, 10);
    */

    glPopMatrix();

    /*Text Rendering*/
    renderText();

    glutSwapBuffers();
}

bool loadOBJ(
        const char* path,
        vector < glm::vec3 > &out_vertices,
        vector < glm::vec2 > &out_uvs,
        vector < glm::vec3 > &out_normals
)
{
    vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    vector< glm::vec3 > temp_vertices;
    vector< glm::vec2 > temp_uvs;
    vector< glm::vec3 > temp_normals;


    FILE* file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }
    while( 1 )
    {
        int i = 0;
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        if ( strcmp( lineHeader, "v" ) == 0 )
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
            i++;
        }
        else if ( strcmp( lineHeader, "vt" ) == 0 )
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if ( strcmp( lineHeader, "vn" ) == 0 )
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if ( strcmp( lineHeader, "f" ) == 0 )
        {
            string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2],
                                 &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser. (Try exporting with other options)\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        for( unsigned int i=0; i<vertexIndices.size(); i++ )
        {
            unsigned int vertexIndex = vertexIndices[i];
            glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
            out_vertices.push_back(vertex);
        }
        for( unsigned int i=0; i<uvIndices.size(); i++ )
        {
            unsigned int uvIndex = uvIndices[i];
            glm::vec2 uv = temp_uvs[ uvIndex-1 ];
            out_uvs.push_back(uv);
        }
        for( unsigned int i=0; i<vertexIndices.size(); i++ )
        {
            unsigned int normalIndex = normalIndices[i];
            glm::vec3 normal = temp_normals[ normalIndex-1 ];
            out_normals.push_back(normal);
        }
    }
    fclose(file);
    return true;
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
            break;

        case 'P':
            _smooth = true;
            break;

        case 'p':
            _smooth = false;
            break;

        case 'Z':
            _zoom += 0.1f;
            break;

        case 'z':
            if(_zoom>-0.9)
            {
                _zoom -= 0.1f;
            }
            break;
    }
}