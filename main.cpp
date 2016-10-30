//
// Created by Campbell Pedersen on 24/10/16.
// "Under the Sea", CG200 OpenGL Assignment
//

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "imageloader.h"

#define GROUND_SIZE 40.0

using namespace std;

//Strings for instructions
string instructions[10] = {"Z/z: Zoom in/out" , //Done
                           "X/x: Rotation around x-axis start/stop", //Done
                           "Y/y: Rotation around y-axis start/stop", //Done
                           "A/a: Animation begin", //Done
                           "F/f: Make animation faster",
                           "S/s: Make animation slower",
                           "T/t: Pause animation",
                           "C/c: Resume animation",
                           "p: Flat shading", //Done
                           "P: Smooth shading"}; //Done

//Variables for controlling camera
float _cameraAngleX = 15.0f;
float _cameraAngleY = 0.0f;
float _zoom = 0.5f;
bool _rotatingX = false;
bool _rotatingY = false;

//Variables for controlling animation
bool _startedAnimating = false;
bool _animating = false;
float _angle = 0.0;
float _bubbleheight = 0.0;
int _speed = 25;

//Variables for controlling shading
bool _smooth = false;

//Texture IDs
GLuint _textureID[6];

//Object vertices
vector< glm::vec3 > barrelVertices;
vector< glm::vec2 > barrelUvs;
vector< glm::vec3 > barrelNormals;

vector< glm::vec3 > fishVertices;
vector< glm::vec2 > fishUvs;
vector< glm::vec3 > fishNormals;

vector< glm::vec3 > chairVertices;
vector< glm::vec2 > chairUvs;
vector< glm::vec3 > chairNormals;

//OpenGL Method Callbacks
void init();
void render();
void resize(int w, int h);
void keyboard(unsigned char c, int x, int y);

//Methods from loading textures/objects from file
bool loadOBJ(
        const char * path,
        vector < glm::vec3 > &out_vertices,
        vector < glm::vec2 > &out_uvs,
        vector < glm::vec3 > &out_normals
);
GLuint loadTexture(Image* image);

//Initializes gl constants/camera before rendering
void renderSetup();

//Object rendering methods
void renderGround();
void renderBarrel();
void renderRustyBarrel();
void renderFish(float x, float y, float z);
void renderChair();
void renderRock(float x, float y, float z);
void renderBubble(float x, float y, float z);

//Update method; for animation
void update(int value);

//Renders instructions on screen
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

//Initializes GL constants
void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glClearColor(0.0f, 0.1f, 0.3f, 1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_CULL_FACE);

    glFrustum(-1.0, 1.0, -1.0, 1.0, -4.0, 40.0);

    //Loading textures
    Image* image = loadBMP("textures/barrel.bmp");
    _textureID[0] = loadTexture(image);
    image = loadBMP("textures/sand.bmp");
    _textureID[1] = loadTexture(image);
    image = loadBMP("textures/fish.bmp");
    _textureID[2] = loadTexture(image);
    image = loadBMP("textures/chair.bmp");
    _textureID[3] = loadTexture(image);
    image = loadBMP("textures/rock.bmp");
    _textureID[4] = loadTexture(image);
    delete image;

    //Load objects
    loadOBJ("objects/barrel.obj", barrelVertices, barrelUvs, barrelNormals);
    loadOBJ("objects/fish.obj", fishVertices, fishUvs, fishNormals);
    loadOBJ("objects/chair.obj", chairVertices, chairUvs, chairNormals);

    glutTimerFunc(_speed, update, 0);
}

//Main scene rendering function
void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();

    //Set camera, zoom, lighting etc.
    renderSetup();

    //Render objects
    renderGround();
    renderBarrel();
    renderRustyBarrel();
    renderChair();
    renderRock(-4.0f, 0.0f, 4.0f);
    renderRock(-3.5f, 0.0f, 4.1f);
    renderFish(0.0f, 0.0f, 0.0f);
    renderFish(1.0f, 0.2f, 2.0f);
    renderFish(-2.0f, -0.2f, 1.0f);
    renderBubble(-2.5f, -0.5f, -3.0f);

    glPopMatrix();

    /*Text Rendering*/
    renderText();
    glutSwapBuffers();
}

//Sets up camera, constants each frame
void renderSetup()
{
    //Camera angle + zoom
    if(_rotatingX)
    {
        _cameraAngleX += 0.5f;
    }
    if(_rotatingY)
    {
        _cameraAngleY += 0.5f;
    }
    glScalef(+_zoom, +_zoom, 1.0f);

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
    glRotatef(-_cameraAngleY, 0.0f, 1.0f, 0.0f);

    //Ambient Light
    GLfloat ambientColour[] = {0.0f, 0.0f, 0.3f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColour);

    //Directional Light
    GLfloat lightColour1[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat lightPos1[] = {0.0f, 2.0f, 0.0f, 1.0f};
    GLfloat lightDirection1[] = {0.0f, -1.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColour1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDirection1);
}

//Ground rendering
void renderGround()
{

    //Texture
    glColor4f(1,1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureID[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Draw
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(10.0f, 10.0f);
    glVertex3f(GROUND_SIZE, -1.0f, GROUND_SIZE);
    glTexCoord2f(10.0f, 0.0f);
    glVertex3f(GROUND_SIZE, -1.0f, -GROUND_SIZE);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-GROUND_SIZE, -1.0f, -GROUND_SIZE);
    glTexCoord2f(0.0f, 10.0f);
    glVertex3f(-GROUND_SIZE, -1.0f, GROUND_SIZE);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

//Wooden barrel rendering
void renderBarrel()
{
    glPushMatrix();

    //Transform
    glTranslatef(-1.0f, -1.5f, 0.0f);
    glRotatef(-25.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(-25.0f, 1.0f, 0.0f, 0.0f);

    //Texture
    glColor4f(1,1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureID[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Draw
    glBegin(GL_TRIANGLES);
    for(int i=0; i<barrelVertices.size(); i += 3)
    {
        glNormal3f(barrelNormals.at(i).x, barrelNormals.at(i).y, barrelNormals.at(i).z);
        glTexCoord2f(barrelUvs.at(i).x, barrelUvs.at(i).y);
        glVertex3f(barrelVertices.at(i).x, barrelVertices.at(i).y, barrelVertices.at(i).z);
        glTexCoord2f(barrelUvs.at(i+1).x, barrelUvs.at(i+1).y);
        glVertex3f(barrelVertices.at(i+1).x, barrelVertices.at(i+1).y, barrelVertices.at(i+1).z);
        glTexCoord2f(barrelUvs.at(i+2).x, barrelUvs.at(i+2).y);
        glVertex3f(barrelVertices.at(i+2).x, barrelVertices.at(i+2).y, barrelVertices.at(i+2).z);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//Rusty barrel rendering
void renderRustyBarrel()
{
    glPushMatrix();

    //Colour
    glColor4f(0.2f, 0.2f, 0.2f, 1.0f);

    //Transform
    glTranslatef(3.0f, -1.5f, -1.0f);
    glRotatef(-25.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(-75.0f, 1.0f, 0.0f, 0.0f);

    //Draw
    float radius = 1.0f;
    float height = 2.5f;
    float slices = 12.0f;

    GLUquadricObj* quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricDrawStyle(quadratic, GLU_FILL);
    gluQuadricTexture(quadratic, GL_TRUE);

    gluCylinder(quadratic, radius, radius, height, slices+(_zoom*10) , slices+(_zoom*10));
    glRotatef(180, 1,0,0);
    gluDisk(quadratic, 0.0f, radius, slices+(_zoom*10), 1);
    glRotatef(180, 1,0,0);
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quadratic, 0.0f, radius, slices+(_zoom*10), 1);
    glTranslatef(0.0f, 0.0f, height);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//Fish rendering, renders at input argument co-ordinates
void renderFish(float x, float y, float z)
{
    glPushMatrix();

    //Transform
    glTranslatef(x, y, z);
    glRotatef(-_angle, 0.0f, 1.0f, 0.0f);

    //Texture
    glColor4f(1,1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureID[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Draw
    glBegin(GL_TRIANGLES);
    for(int i=0; i<fishVertices.size(); i += 3)
    {
        glNormal3f(fishNormals.at(i).x, fishNormals.at(i).y, fishNormals.at(i).z);
        glTexCoord2f(fishUvs.at(i).x, fishUvs.at(i).y);
        glVertex3f(fishVertices.at(i).x, fishVertices.at(i).y, fishVertices.at(i).z);
        glTexCoord2f(fishUvs.at(i+1).x, fishUvs.at(i+1).y);
        glVertex3f(fishVertices.at(i+1).x, fishVertices.at(i+1).y, fishVertices.at(i+1).z);
        glTexCoord2f(fishUvs.at(i+2).x, fishUvs.at(i+2).y);
        glVertex3f(fishVertices.at(i+2).x, fishVertices.at(i+2).y, fishVertices.at(i+2).z);
    }
    glEnd();
    //glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//Fish + bubble animation variables updating
void update(int value)
{
    if(_animating)
    {
        _angle += 2.0f;
        _bubbleheight = sin(_angle/15);
        if (_angle > 360)
        {
            _angle -= 360;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(_speed, update, 0);
}

//Chair rendering
void renderChair()
{
    glPushMatrix();

    //Transform
    glTranslatef(2.5f, -1.2f, 6.0f);
    glRotatef(-40.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(20.0f, 1.0f, 0.0f, 0.0f);

    //Texture
    glColor4f(1,1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureID[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Draw
    glBegin(GL_TRIANGLES);
    for(int i=0; i<chairVertices.size(); i += 3)
    {
        glNormal3f(chairNormals.at(i).x, chairNormals.at(i).y, chairNormals.at(i).z);
        glTexCoord2f(chairUvs.at(i).x, chairUvs.at(i).y);
        glVertex3f(chairVertices.at(i).x, chairVertices.at(i).y, chairVertices.at(i).z);
        glTexCoord2f(chairUvs.at(i+1).x, chairUvs.at(i+1).y);
        glVertex3f(chairVertices.at(i+1).x, chairVertices.at(i+1).y, chairVertices.at(i+1).z);
        glTexCoord2f(chairUvs.at(i+2).x, chairUvs.at(i+2).y);
        glVertex3f(chairVertices.at(i+2).x, chairVertices.at(i+2).y, chairVertices.at(i+2).z);
    }
    glEnd();
    //glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//Rock rendering, renders at input argument co-ordinates
void renderRock(float x, float y, float z)
{
    glPushMatrix();

    //Texture
    glColor4f(1,1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureID[4]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Transform
    glTranslatef(x, y-1.2, z);

    //ANOTHER ONE
    GLUquadricObj* quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricDrawStyle( quadratic, GLU_FILL);
    gluQuadricTexture(quadratic, GL_TRUE);

    gluSphere(quadratic, 0.5f, 8+(_zoom*10), 8+(_zoom*10));

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//Bubble rendering, renders at input argument co-ordinates
void renderBubble(float x, float y, float z)
{
    glPushMatrix();

    //Transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.2f, 0.2f, 0.2f, 0.4f);

    //Transform
    glTranslatef(x, y+2.0 + _bubbleheight, z);

    //ANOTHER ONE
    GLUquadricObj* quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricDrawStyle( quadratic, GLU_FILL);
    gluQuadricTexture(quadratic, GL_TRUE);
    gluSphere(quadratic, 0.5f, 8+(_zoom*10), 8+(_zoom*10));

    glPopMatrix();
}

//Loads object from .obj file (Faces must be triangulated
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

//Loads texture from .bmp file
GLuint loadTexture(Image* image)
{
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 image->width, image->height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image->pixels);
                return textureId;
}

//Renders text on screen
void renderText()
{
    glMatrixMode(GL_PROJECTION);
    double *matrix = new double[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glOrtho(0, 640, 0, 480, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for(int i = 0; i < 10; i++) //For each line
    {
        string line = instructions[i]; //Get line from string array
        glRasterPos2f(10.0f, (460.0f - (float)i*15)); //Position of raster goes down each
        glColor3f(1.0, 1.0, 1.0);

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

//Handles window resizing
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w/(double)h, 1.0, 200.0);
}

//Handles keyboard input
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
            if(_zoom>0.2)
            {
                _zoom -= 0.1f;
            }
            break;

        case 'A': case 'a':
            _startedAnimating = true;
            _animating = true;
            break;

        case 'F': case 'f':
            if(_speed > 5)
            {
                _speed -= 5;
            }
            break;

        case 'S': case 's':
        {
            _speed += 10;
            break;
        }

        case 'T': case 't':
            _animating = false;
            break;

        case 'C': case 'c':
            if(_startedAnimating)
            {
                _animating = true;
            }
            break;
    }
}