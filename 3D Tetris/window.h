/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Window - Fundamental GUI for interacting within the scene
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QActionGroup>

class Renderer;

class Window : public QMainWindow
{
    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Window(QWidget *parent = 0);

    // destructor
    ~Window();


private slots:

protected:
    //key press handler
    // Called when a key is pressed or released, respectively
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void keyReleaseEvent(QKeyEvent * event);

private:
    // Main widget for drawing
    Renderer *renderer;

    // Menu items and actions
    QMenu * mFileMenu;
    QMenu * mDrawMenu;
    QMenu * mGameMenu;
    QAction * mWireFrameAction;
    QAction * mFaceModeAction;
    QAction * mQuitAction;
    QAction * mNewGameAction;
    QAction * mResetViewAction;
    QAction * mPauseAction;
    QAction * mSpeedUpAction;
    QAction * mSlowDownAction;
    QAction * mAutoIncreaseAction;
    QAction * mMultiColorMode; // not implemented

    QActionGroup * actionGroup;
    // helper function for creating actions
    void createActions();

};

#endif // WINDOW_H
