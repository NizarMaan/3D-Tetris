#include "renderer.h"
#include <QTextStream>
#include <QOpenGLBuffer>
#include <cmath>

// constructor
//mode initialized as mode 1 (a.k.a. face mode), Pause as False, standardSpeed at 500,
//currentSpeed set to standardSpeed and speedIncrementor set to 100, readyToScale to false
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent), mode(1), stayRotating(false), isPaused(false), standardSpeed(500), speedAdjustor(100),
      currentSpeed(standardSpeed), readyToScale(false), currentScale(0), maxScale(108), minScale(-108)
//Scaling arbitrarily chosen to be stopped when currentScale reaches max/min scale
//scaling will occur 108/9 times (i.e.12 times) before capping the scaling

{
    //set up view Matrix
    view_matrix.translate(0.0f, 0.0f, -40.0f);

    //setting game ticks
    this->m_pGameTimer = new QTimer(this);
    //starts game ticks
    connect(this->m_pGameTimer, SIGNAL(timeout()), this, SLOT(timer_Tick()));
    //tick every 500 milliseconds
    this->m_pGameTimer->start(standardSpeed);

    //timer hooked up to autoIncrease()
    m_speedUpTimer = new QTimer();
    connect(this->m_speedUpTimer, SIGNAL(timeout()), this, SLOT(autoIncrease()));

    //timer hooked up to keepRotating()
    m_rotationTimer = new QTimer();
    connect(this->m_rotationTimer, SIGNAL(timeout()), this, SLOT(keepRotating()));

    //create new game object to access game functions, pass in game dimensions (10x20)
    game = new Game(10, 20);
}

// constructor
Renderer::~Renderer()
{

}

// called once by Qt GUI system, to allow initialization for OpenGL requirements
// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // Qt support for inline GL function calls
    initializeOpenGLFunctions();

    // sets the background clour
    glClearColor(0.7f, 0.7f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // links to and compiles the shaders, used for drawing simple objects
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "per-fragment-phong.vs.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "per-fragment-phong.fs.glsl");
    m_program->link();
    m_posAttr = m_program->attributeLocation("position_attr");
    m_colAttr = m_program->attributeLocation("colour_attr");
    m_norAttr = m_program->attributeLocation("normal_attr");
    m_PMatrixUniform = m_program->uniformLocation("proj_matrix");
    m_VMatrixUniform = m_program->uniformLocation("view_matrix");
    m_MMatrixUniform = m_program->uniformLocation("model_matrix");
    m_programID = m_program->programId();
    generateBorderTriangles();
    generateCube();

}

// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
    // Clear the screen buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the current shader program

    glUseProgram(m_programID);

    // Modify the current projection matrix so that we move the
    // camera away from the origin.  We'll draw the game at the
    // origin, and we need to back up to see it.

    /*if(rotateVertical){
        view_matrix.rotate(deltaX, 1.0f, 0.0f, 0.0f);
    }*/
    glUniformMatrix4fv(m_VMatrixUniform, 1, false, view_matrix.data());

    //determining which mode to draw, 0-wireframe 1-face mode
    if (mode == 0){
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

    // Not implemented: set up lighting (if necessary)

    // Not implemented: scale and rotate the scene

    QMatrix4x4 model_matrix;

    // You'll be drawing unit cubes, so the game will have width
    // 10 and height 24 (game = 20, stripe = 4).  Let's translate
    // the game so that we can draw it starting at (0,0) but have
    // it appear centered in the window.

    model_matrix.translate(-5.0f, -12.0f, 0.0f);
    glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());

    // Not implemented: actually draw the current game state.
    // Here's some test code that draws red triangles at the
    // corners of the game board.


    // draw border
    if (triVertices.size() > 0)
    {
        // pass in the list of vertices and their associated colours
        // 3 coordinates per vertex, or per colour
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, &triVertices[0]);
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, &triColours[0]);
        glVertexAttribPointer(m_norAttr, 3, GL_FLOAT, GL_FALSE, 0, &triNormals[0]);

        glEnableVertexAttribArray(m_posAttr);
        glEnableVertexAttribArray(m_colAttr);
        glEnableVertexAttribArray(m_norAttr);

        // draw triangles
        glDrawArrays(GL_TRIANGLES, 0, triVertices.size()/3); // 3 coordinates per vertex

        glDisableVertexAttribArray(m_norAttr);
        glDisableVertexAttribArray(m_colAttr);
        glDisableVertexAttribArray(m_posAttr);
    }

    //Draw Well
    //draw bottom border
    for(int i=0; i < 10; i++)
    {
        QMatrix4x4 cube_matrix(model_matrix);
        glEnableVertexAttribArray(m_posAttr);
        glEnableVertexAttribArray(m_norAttr);
        glEnableVertexAttribArray(m_colAttr);
        glVertexAttribPointer(m_posAttr,3,GL_FLOAT,GL_FALSE,0,&cubeVertices[0]);
        glVertexAttribPointer(m_norAttr,3,GL_FLOAT,GL_FALSE,0,&cubeVertices[0]);
        cube_matrix.translate( (float) i, -1, 0.0f);
        glUniformMatrix4fv(m_MMatrixUniform,1,false,cube_matrix.data());
        glDrawArrays(GL_TRIANGLES,0,cubeVertices.size()/3);
        glFinish();
    }

    //draw left border
    for(int i=0; i < 24; i++)
    {
        QMatrix4x4 cube_matrix(model_matrix);
        glEnableVertexAttribArray(m_posAttr);
        glEnableVertexAttribArray(m_norAttr);
        glEnableVertexAttribArray(m_colAttr);
        glVertexAttribPointer(m_posAttr,3,GL_FLOAT,GL_FALSE,0,&cubeVertices[0]);
        glVertexAttribPointer(m_norAttr,3,GL_FLOAT,GL_FALSE,0,&cubeVertices[0]);
        cube_matrix.translate( -1.0, (float) i-1, 0.0f);
        glUniformMatrix4fv(m_MMatrixUniform,1,false,cube_matrix.data());
        glDrawArrays(GL_TRIANGLES,0,cubeVertices.size()/3);
        glFinish();
    }

    //draw right border
    for(int i=0; i < 24; i++){
        QMatrix4x4 cube_matrix(model_matrix);
        glEnableVertexAttribArray(m_posAttr);
        glEnableVertexAttribArray(m_norAttr);
        glEnableVertexAttribArray(m_colAttr);
        glVertexAttribPointer(m_posAttr,3,GL_FLOAT,GL_FALSE,0,&cubeVertices[0]);
        glVertexAttribPointer(m_norAttr,3,GL_FLOAT,GL_FALSE,0,&cubeVertices[0]);
        cube_matrix.translate( 10.0, (float) i-1, 0.0f);
        glUniformMatrix4fv(m_MMatrixUniform,1,false,cube_matrix.data());
        glDrawArrays(GL_TRIANGLES,0,cubeVertices.size()/3);
        glFinish();
    }

    //draw falling pieces
    for (int r = 0; r < 20; r++){
        for(int c = 0; c < 10; c++){
            if(game -> get(r, c) >-1){
                QMatrix4x4 cube_matrix(model_matrix);
                glEnableVertexAttribArray(m_posAttr);
                glEnableVertexAttribArray(m_norAttr);
                glVertexAttribPointer(m_posAttr,3,GL_FLOAT,GL_FALSE,0,&cubeVertices[0]);
                glVertexAttribPointer(m_norAttr,3,GL_FLOAT,GL_FALSE,0,&cubeVertices[0]);
                cube_matrix.translate( (float) c, (float) r, 0.0f);
                glUniformMatrix4fv(m_MMatrixUniform,1,false,cube_matrix.data());
                glDrawArrays(GL_TRIANGLES,0,cubeVertices.size()/3);
                glFinish();
                }
            }
        }

    update();
    // deactivate the program
    m_program->release();

}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int w, int h)
{
    // width and height are better variables to use
    Q_UNUSED(w); Q_UNUSED(h);

    // update viewing projections
    glUseProgram(m_programID);

    // Set up perspective projection, using current size and aspect
    // ratio of display
    QMatrix4x4 projection_matrix;
    projection_matrix.perspective(40.0f, (GLfloat)width() / (GLfloat)height(),
                                  0.1f, 1000.0f);
    glUniformMatrix4fv(m_PMatrixUniform, 1, false, projection_matrix.data());

    glViewport(0, 0, width(), height());

}

// computes the vertices and corresponding colours-per-vertex for a quadrilateral
// drawn from (x1, y1) to (x2, y2)
// Note: the magic numbers in the vector insert commands should be better documented
void Renderer::generateBorderTriangles()
{
    // make sure array lists are clear to start with
    triVertices.clear();
    triColours.clear();

    // add vertices to rectangle list
    float vectList [] = {
        0.0, 0.0, 0.0,  // bottom left triangle
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,

        9.0, 0.0, 0.0,  // bottom right triangle
        10.0, 0.0, 0.0,
        10.0, 1.0, 0.0,

        0.0, 19.0, 0.0, // top left triangle
        1.0, 20.0, 0.0,
        0.0, 20.0, 0.0,

        10.0, 19.0, 0.0,    // top right triangle
        10.0, 20.0, 0.0,
        9.0, 20.0, 0.0 };
    triVertices.insert(triVertices.end(), vectList, vectList + 3*4*3); // 36 items in array

    // shader supports per-vertex colour; add colour for each vertex add colours to colour list - use current colour
    QColor borderColour = Qt::red;
    float colourList [] = { (float)borderColour.redF(), (float)borderColour.greenF(), (float)borderColour.blueF() };
    float normalList [] = { 0.0f, 0.0f, 1.0f }; // facing viewer
    for (int v = 0; v < 4 * 3; v++)
    {
        triColours.insert(triColours.end(), colourList, colourList + 3); // 3 coordinates per vertex
        triNormals.insert(triNormals.end(), normalList, normalList + 3); // 3 coordinates per vertex
    }

}

void Renderer::generateCube(){
    float vectList[] = {
        ////////Front////////
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 1.0, 0.0,

        0.0, 0.0, 0.0,
        1.0, 1.0, 0.0,
        0.0, 1.0, 0.0,

        ////////Right////////
        1.0, 0.0, 0.0,
        1.0, 0.0, -1.0,
        1.0, 1.0, -1.0,

        1.0, 1.0, -1.0,
        1.0, 1.0, 0.0,
        1.0, 0.0, 0.0,

        ////////Back////////
        1.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        1.0, 1.0, -1.0,

        1.0, 1.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 1.0, -1.0,

        ////////Left////////
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, -1.0,

        0.0, 0.0, -1.0,
        0.0, 1.0, -1.0,
        0.0, 1.0, 0.0,

        ////////Top////////
        0.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
        1.0, 1.0, -1.0,

        1.0, 1.0, -1.0,
        0.0, 1.0, -1.0,
        0.0, 1.0, 0.0,

        ////////Bottom////////
        0.0, 0.0, 0.0,
        0.0, 0.0, -1.0,
        1.0, 0.0, -1.0,

        1.0, 0.0, -1.0,
        1.0, 0.0, 0.0,
        0.0, 0.0, 0.0
    };

    cubeVertices.insert(cubeVertices.end(), vectList, vectList + 12*3*3);

    //normals obtained from TA Amirhessam Moltaji http://moltaji.cpsc.ucalgary.ca/Courses/pmwiki.php?n=Site.CPSC453F15
    float normals[] = {
        0.0, 0.0, 0.5,//Front
        0.5, 0.0, 0.0,//Right
        0.0, 0.0, -0.5,//Back
        -1.0, 0.0, 0.0,//Left
        0.0, -1.0, 0.0,//Top
        0.0, 0.0, -1.0//Bottom
    };

    for(unsigned int i = 0; i<6; ++i){
        //for each face
        for(unsigned int j = 0; j < 6; ++j)
        {
            //for each vertex
            cubeNormals.insert(cubeNormals.end(), normals + 3*i, normals+ 3*i);
        }
    }
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{ 
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";

    //bools to determine whether a new Scaling command is undergoing
    if(readyToScale == false && wasScaling == true){
        wasScaling = false;
    }
    //Scale if readyToScale (i.e. shift is pressed)
    if(readyToScale){
        wasScaling = true;      //set this to true to know scaling is/was occuring
        initX = event->x();
    }

    //different  scnearios to determine to keep rotating or not
    //if stayRotating is true and a new button has been pressed then we know
    //to set it back to false for a new rotation to begin
    if(event->button() == 1 && stayRotating == false){
        stayRotating = false;
        rotateVertical = true;
        initX =  event->x();
    }

    if(event->button() == 4 && stayRotating == false){
        stayRotating = false;
        rotateHorizontal = true;
        initX =  event->x();
    }

    if(event->button() == 2 && stayRotating == false){
        stayRotating = false;
        rotateZ = true;
        initX =  event->x();
    }

    if(event->button() == 1 && stayRotating == true){
        stayRotating = false;
        rotateVertical = true;
        initX =  event->x();
    }

    if(event->button() == 4 && stayRotating == true){
        stayRotating = false;
        rotateHorizontal = true;
        initX =  event->x();
    }

    if(event->button() == 2 && stayRotating == true){
        stayRotating = false;
        rotateZ = true;
        initX =  event->x();
    }
}

// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";

    //this check is to know whether scaling did occur
    //shift button might be released before mouse button
    //or vice versa, which requires boolean checks to know whether scaling
    //was occuring
    if(readyToScale == false && wasScaling == true){
        readyToScale = true;
    }
    //if mouse was moving upon release (i.e. theres a difference in x coordinates > or < 0)
    //and if no scaling was occuring
    if((deltaX > 0 || deltaX < 0) && readyToScale == false){
        stayRotating = true;
        keepRotating();
    }
}

// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{ 
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";

    //rotate and update
    if (rotateVertical == true){
        deltaX = (event->x() - initX)* 0.01f;
        view_matrix.rotate(deltaX, 1.0f, 0.0f, 0.0f);
    }
    if (rotateHorizontal == true){
        deltaX = (event->x() - initX)* 0.01f;
        view_matrix.rotate(deltaX, 0.0f, 1.0f, 0.0f);
    }
    if (rotateZ == true){
        deltaX = (event->x() - initX)* 0.01f;
        view_matrix.rotate(deltaX, 0.0f, 0.0f, 1.0f);
    }

    if(readyToScale){
        deltaX = (event->x() - initX);
        //if moving mouse to the right
        if(deltaX > 0){
            if(currentScale < maxScale){
                currentScale+= 9;
                //scale values arbitrarily chosen and hard coded
                view_matrix.scale(1.1111111111f, 1.1111111111f, 1.1111111111f);
            }
        }
        //if moving mouse to the left
        if(deltaX < 0){
            if(currentScale > minScale){
                currentScale -= 9;
                //scale values arbitrarily chosen and hard coded
                view_matrix.scale(0.90f, 0.90f, 0.90f);
            }
        }
    }
    update();
}

//mode 1 is face mode, mode 0 is wireframe mode
void Renderer::faceMode(){
    mode = 1;
    repaint();
}

//mode 1 is face mode, mode 0 is wireframe mode
void Renderer::wireFrameMode(){
    mode = 0;
    repaint();
}

//this mode was not implemented
void Renderer::multiColorMode(){
    //nothing
}

//start new game
void Renderer::newGame(){
    //reset status variables
    this->m_pGameTimer->start(standardSpeed);
    currentSpeed = standardSpeed;
    mode = 1;
    stayRotating = false;
    game->reset();
}

//reset game view
void Renderer::resetView(){
    //reset scaling and rotation statuses
    currentScale = 0;
    stayRotating = false;
    view_matrix.setToIdentity();
    view_matrix.translate(0.0f, 0.0f, -40.0f);
}

//function to pause/unpause the game
void Renderer::pauseGame(){
    if (isPaused){
        this->m_pGameTimer->start(currentSpeed);
        isPaused = !isPaused;
    }
    else{
        this->m_pGameTimer->stop();
        isPaused = !isPaused;
    }
}

//speed up the game by reducing time between game ticks by subtracting speedAdjustor
void Renderer::speedUpGame(){
    //cap currentSpeed to 1
    if (currentSpeed - speedAdjustor >= 1){
        currentSpeed = currentSpeed - speedAdjustor;
        this->m_pGameTimer->start(currentSpeed);
    }
}

//slow down the game by increasing time between game ticksby adding speedAdjustor
void Renderer::slowDownGame(){
    //cap currentSpeed to 1000
    if (currentSpeed - speedAdjustor <= 1000){
        currentSpeed = currentSpeed + speedAdjustor;
        this->m_pGameTimer->start(currentSpeed);
    }
}

//Auto-increase function
//similar to speedUpGame but recursive with a scaling speed up
void Renderer::autoIncrease(){
    //decrease tick interval time by 50milliseconds every 2 seconds
    if (currentSpeed - speedAdjustor >= 1){
        currentSpeed = currentSpeed - 50;
        this->m_pGameTimer->start(currentSpeed);

        //call this function every 2 seconds
        m_speedUpTimer-> start(2000);
    }
}

//function to keep rotating upon mouse release
void Renderer::keepRotating(){
    //rotate and update
    if(stayRotating){
        if (rotateVertical == true){
            view_matrix.rotate(deltaX, 1.0f, 0.0f, 0.0f);
        }
        if (rotateHorizontal == true){
            view_matrix.rotate(deltaX, 0.0f, 1.0f, 0.0f);
        }
        if (rotateZ == true){
            view_matrix.rotate(deltaX, 0.0f, 0.0f, 1.0f);
        }
        update();

        //repeat this function every 10 milliseconds
        m_rotationTimer-> start(10);
    }
}

//starts game ticks
void Renderer:: timer_Tick(){
    game -> tick();
}
