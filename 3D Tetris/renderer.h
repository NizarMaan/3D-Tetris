/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Renderer - OpenGL widget for drawing scene
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>
#include "game.h"
#include <QTimer>

using namespace std;

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions
{

    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    
    // constructor
    Renderer(QWidget *parent = 0);

    // destructor
    virtual ~Renderer();

    Game * game;                //public game pointer
    bool readyToScale;          //bool to know whether "shift" key is pressed down
    bool wasScaling;            //bool to know whether we were just scaling

public slots:

    //function to determine which viewing mode to switch to (face or wire-frame)
    void faceMode();
    void wireFrameMode();
    void multiColorMode();  //this mode was not implemented

    //functions to start new game and reset game view, respectively
    void newGame();
    void resetView();

    //function to modify game speed
    void pauseGame();

    //functions to speed-up/slow-down game-play
    void speedUpGame();
    void slowDownGame();

    //function to auto-increase game speed
    void autoIncrease();

    //function too keep rotating on mouse release
    void keepRotating();

private slots:
    //method that starts the game ticks
    void timer_Tick();

protected:

    // override fundamental drawing functions

    // Called when OpenGL is first initialized
    void initializeGL();

    // Called when the window is resized
    void resizeGL(int w, int h);

    // Called when the window needs to be redrawn
    void paintGL();

    // override mouse event functions

    // Called when a mouse button is pressed
    virtual void mousePressEvent(QMouseEvent * event);

    // Called when a mouse button is released
    virtual void mouseReleaseEvent(QMouseEvent * event);

    // Called when the mouse moves
    virtual void mouseMoveEvent(QMouseEvent * event);

private:
    int mode;                   //mode variable to keep track of wireframe and face mode
    int prevMode;               //mode variable to keep track of frame/face modes

    bool rotateVertical;        //variable to determine whether to rotate about x-axis
    bool rotateHorizontal;      //variable to determine whether to rotate about y-axis
    bool rotateZ;               //variable to determine whether to rotate about z-axis

    float initX;                //variable to store location of initial x value upon click
    float deltaX;               //variable to store value to rotate/scale by

    bool isPaused;              //bool variable to keep track of pause state

    int standardSpeed;          //store the standard game speed of (500)
    int speedAdjustor;          //store amount of game tickets to slow down/speed up by
    int currentSpeed;           //store resulting speed after de/in-crement

    bool stayRotating;          //bool to determine whether to keep rotating

    //variables to compare our current scaling to determine to stop scaling or not
    int currentScale;
    int maxScale;
    int minScale;

    //timer for game tickss
    QTimer *m_pGameTimer;     //Qtime pointer

    //timer for autoIncrease()
    QTimer *m_speedUpTimer;   //Qtime pointer

    //time for keepRotating()
    QTimer *m_rotationTimer;
    QMatrix4x4 view_matrix;     //make view_matrix a member variable for easy access

    // member variables for shader manipulation
    GLuint m_programID;
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_norAttr;
    GLuint m_MMatrixUniform; // model matrix
    GLuint m_VMatrixUniform; // view matrix
    GLuint m_PMatrixUniform; // projection matrix


    QOpenGLShaderProgram *m_program;

    // for storing triangle vertices and colours
    vector<GLfloat> triVertices;
    vector<GLfloat> triColours;
    vector<GLfloat> triNormals;

    //for storing cube vertices, normals, and colors
    vector<GLfloat> cubeVertices;
    vector<GLfloat> cubeNormals;

    // helper function for loading shaders
    GLuint loadShader(GLenum type, const char *source);

    // helper function for drawing bordering triangles
    void generateBorderTriangles();  

    //function to draw cubes
    void generateCube();
};

#endif // RENDERER_H
