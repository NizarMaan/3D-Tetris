#include "window.h"
#include "renderer.h"
#include <QActionGroup>
Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("CPSC453: Tetromino Apocalypse");

    // Create the main drawing object
    renderer = new Renderer();

    // Create the actions to be used by the menus
    createActions();

    // Create the menus
    // Setup the file menu
    mFileMenu = menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(mQuitAction);  // add quitting
    mFileMenu->addAction(mNewGameAction); //reset game
    mFileMenu->addAction(mResetViewAction); //reset game view

    //setup draw menu
    mDrawMenu = menuBar()-> addMenu(tr("&Draw"));
    mDrawMenu->addAction(mWireFrameAction); //add wire frame mode
    mDrawMenu->addAction(mFaceModeAction);  //add face mode
    mDrawMenu->addAction(mMultiColorMode);  //add multi-color mode (not implemented)

    //setup game menu
    mGameMenu = menuBar()-> addMenu(tr("&Game"));
    mGameMenu->addAction(mPauseAction); //pause option
    mGameMenu->addAction(mSpeedUpAction); //speed up gameplay
    mGameMenu->addAction(mSlowDownAction); //slow down gameplay
    mGameMenu->addAction(mAutoIncreaseAction);//automatically speed up gameplay

    //setup actiongroup for radio buttons
    actionGroup = new QActionGroup(this);
    actionGroup->addAction(mWireFrameAction);
    actionGroup->addAction(mMultiColorMode);
    actionGroup->addAction(mFaceModeAction);


    // Setup the application's widget collection
    QVBoxLayout * layout = new QVBoxLayout();

    // Add renderer
    layout->addWidget(renderer);
    renderer->setMinimumSize(300, 600);

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);

}

// helper function for creating actions
void Window::createActions()
{
    // Quits the application
    mQuitAction = new QAction(tr("&Quit"), this);
    mQuitAction->setShortcut(QKeySequence(Qt::Key_Q));
    mQuitAction->setStatusTip(tr("Quits the application"));
    connect(mQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    // Resets the application
    mNewGameAction = new QAction(tr("&New Game"), this);
    mNewGameAction->setShortcut(QKeySequence(Qt::Key_N));
    mNewGameAction->setStatusTip(tr("Starts a new game"));
    connect(mNewGameAction, SIGNAL(triggered()), renderer, SLOT(newGame()));

    // Resets the application's game view
    mResetViewAction = new QAction(tr("&Reset View"), this);
    mResetViewAction->setShortcut(QKeySequence(Qt::Key_R));
    mResetViewAction->setStatusTip(tr("Resets game view"));
    connect(mResetViewAction, SIGNAL(triggered()), renderer, SLOT(resetView()));

//the viewing modes as radio buttons
//---------------------------------Radio Buttons----------------------------------------------------------------------
    //switches to wire frame mode
    mWireFrameAction = new QAction(tr("&Wire Frame Mode"), this);
    mWireFrameAction->setShortcut(QKeySequence(Qt::Key_W));
    mWireFrameAction->setStatusTip(tr("Switches to Wire Frame mode"));
    mWireFrameAction->setCheckable(true);
    connect(mWireFrameAction, SIGNAL(triggered()), renderer, SLOT(wireFrameMode()));

    //switches to multi-color frame mode (not implemented)
    mMultiColorMode = new QAction(tr("&Multi-Color Mode"), this);
    mMultiColorMode->setShortcut(QKeySequence(Qt::Key_M));
    mMultiColorMode->setStatusTip(tr("Switches to Multi-Color mode"));
    mMultiColorMode->setCheckable(true);
    connect(mMultiColorMode, SIGNAL(triggered()), renderer, SLOT(multiColorMode()));

    //switches to face mode
    mFaceModeAction = new QAction(tr("&Face Mode"), this);
    mFaceModeAction->setShortcut(QKeySequence(Qt::Key_F));
    mFaceModeAction->setStatusTip(tr("Switches to Face Mode"));
    mFaceModeAction->setCheckable(true);
    mFaceModeAction->setChecked(true);
    connect(mFaceModeAction, SIGNAL(triggered()), renderer, SLOT(faceMode()));
//-----------------------------------------------------------------------------------------------

    //Pause Game
    mPauseAction = new QAction(tr("&Pause"), this);
    mPauseAction->setShortcut(QKeySequence(Qt::Key_P));
    mPauseAction->setStatusTip(tr("Pauses Game"));
    connect(mPauseAction, SIGNAL(triggered()), renderer, SLOT(pauseGame()));

    //Speed up Game-play
    mSpeedUpAction = new QAction(tr("&Speed Up"), this);
    mSpeedUpAction->setShortcut(QKeySequence(Qt::Key_PageUp));
    mSpeedUpAction->setStatusTip(tr("Increases Game Speed"));
    connect(mSpeedUpAction, SIGNAL(triggered()), renderer, SLOT(speedUpGame()));

    //Slow-Down Game-play
    mSlowDownAction = new QAction(tr("&Slow Down"), this);
    mSlowDownAction->setShortcut(QKeySequence(Qt::Key_PageDown));
    mSlowDownAction->setStatusTip(tr("Decreases Game Speed"));
    connect(mSlowDownAction, SIGNAL(triggered()), renderer, SLOT(slowDownGame()));

    //Auto-speed up Game-play
    mAutoIncreaseAction = new QAction(tr("&Auto-Increase"), this);
    mAutoIncreaseAction->setShortcut(QKeySequence(Qt::Key_A));
    mAutoIncreaseAction->setStatusTip(tr("Auto Increase Game Speed"));
    connect(mAutoIncreaseAction, SIGNAL(triggered()), renderer, SLOT(autoIncrease()));
}

//detect key input to manipulate falling pieces
void Window::keyPressEvent(QKeyEvent * event){
    switch(event-> key()){
        case Qt::Key_Up:
            renderer->game->rotateCCW();
            break;
        case Qt::Key_Down:
            renderer->game->rotateCW();
            break;
        case Qt::Key_Space:
            renderer->game->drop();
            break;
        case Qt::Key_Left:
            renderer->game->moveLeft();
            break;
        case Qt::Key_Right:
            renderer->game->moveRight();
            break;
        case Qt::Key_Shift:
            renderer->readyToScale = true;
            break;
    }
}
void Window::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Shift ){
        renderer->readyToScale = false;
    }
}

// destructor
Window::~Window()
{
    delete renderer;
}
