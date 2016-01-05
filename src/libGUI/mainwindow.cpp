#include <iostream>

#include <QMdiSubWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QtWidgets>
#include <QObject>
#include <QRect>
#include <QSize>
#include <QDebug>

#include "mainwindow.h"
#include "glwidget.h"
#include "settings.h"
#include "Data.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_desktop(0)
    , m_mdiArea(new QMdiArea(this))
    , m_undoStack(new QUndoStack(this))
    , m_undoView(new QUndoView(m_undoStack))
    , m_menuBar(new QMenuBar(0)) // http://stackoverflow.com/questions/8108729/qmenu-does-not-work-on-mac-qt-creator
    , m_rootScene(new RootScene(m_undoStack))
{

    this->setMenuBar(m_menuBar);
    this->onCreateViewer();

    m_undoView->setWindowTitle(tr("Command List"));
    m_undoView->show();
    m_undoView->setAttribute(Qt::WA_QuitOnClose, false);

    this->setCentralWidget(m_mdiArea);
    this->initializeActions();
    this->initializeMenus();
}

MainWindow::~MainWindow(){
    if (m_menuBar)
        delete m_menuBar;
}

void MainWindow::SetDesktopWidget(QDesktopWidget *desktop, dureu::APPMODE mode) {
    m_desktop = desktop;
    QRect availS = m_desktop->availableGeometry();
    QRect fullS = m_desktop->geometry();
    //int nscreen = m_desktop->screenCount();
    double scale = 0.9;
    double scale_inv = 1-scale;
    switch (mode) {
    case dureu::SCREEN_MIN:
        this->showNormal();
        break;
    case dureu::SCREEN_MAX:
        this->showMaximized();
        break;
    case dureu::SCREEN_FULL:
        this->showFullScreen();
        break;
    case dureu::SCREEN_VIRTUAL: // needs testing and fixing
        this->resize(QSize(fullS.width(), fullS.height()));
        break;
    case dureu::SCREEN_DETACHED:
        this->resize(QSize(availS.width()*scale, fullS.height()*scale_inv));
        this->move(availS.width()*scale_inv, fullS.height()-availS.height());
        break;
    default:
        std::cerr << "Application mode not recognized, closing application" << std::endl;
        exit(1);
    }
    std::cout << "Widget width and height: " << this->width() << " " << this->height() << std::endl;
}

void MainWindow::getTabletActivity(bool active){
    emit sendTabletActivity(active);
}

/* Create an ordinary single view window on the scene _root
 * To create outside viewer, use:
 * GLWidget* vwid = createViewer(Qt::Window);
*/
void MainWindow::onCreateViewer(){
    GLWidget* vwid = createViewer();
    QMdiSubWindow* subwin = m_mdiArea->addSubWindow(vwid);
    subwin->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    vwid->showMaximized();
    subwin->show();
}

void MainWindow::onFileNew()
{

}

void MainWindow::onFileOpen()
{

}

void MainWindow::onFileSave()
{

}

void MainWindow::onFileSaveAs()
{

}

void MainWindow::onFileImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load an Image File"), QString(),
            tr("Image Files (*.bmp)"));
    if (!fileName.isEmpty()) {
        if (!m_rootScene->loadPhotoFromFile(fileName.toStdString())){
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
    }
}

void MainWindow::onFileClose()
{

}

void MainWindow::onFileExit()
{

}

void MainWindow::onCameraOrbit(){
    emit sendMouseMode(dureu::MOUSE_ROTATE);
}

void MainWindow::onCameraZoom(){
    emit sendMouseMode(dureu::MOUSE_ZOOM);
}

void MainWindow::onCameraPan(){
    emit sendMouseMode(dureu::MOUSE_PAN);
}

void MainWindow::onSelect(){
    emit sendMouseMode(dureu::MOUSE_PICK);
}

void MainWindow::onErase()
{
    emit sendMouseMode(dureu::MOUSE_ERASE);
}

void MainWindow::onDelete()
{
    emit sendMouseMode(dureu::MOUSE_DELETE);
}

void MainWindow::onSketch()
{
    emit sendMouseMode(dureu::MOUSE_SKETCH);
}

void MainWindow::onCanvasOffset()
{
    emit sendMouseMode(dureu::MOUSE_EDIT_OFFSET);
}

void MainWindow::onCanvasRotate()
{
    emit sendMouseMode(dureu::MOUSE_EDIT_ROTATE);
}

void MainWindow::onImageMove()
{
    emit sendMouseMode(dureu::MOUSE_EDIT_MOVE);
}

void MainWindow::onImageRotate()
{

}

void MainWindow::onImageScale()
{

}

void MainWindow::onImageFlip()
{

}

void MainWindow::onImagePush()
{

}

void MainWindow::onStrokePush()
{

}

GLWidget* MainWindow::createViewer(Qt::WindowFlags f, int viewmode)
{
    GLWidget* vwid = new GLWidget(m_rootScene.get(), this, f);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QObject::connect(this, SIGNAL(sendMouseMode(dureu::MOUSE_MODE)),
                     vwid, SLOT(recieveMouseMode(dureu::MOUSE_MODE)));
    return vwid;
}

void MainWindow::initializeActions()
{
    // FILE
    m_actionNewFile = new QAction(Data::fileNewSceneIcon(), tr("&New..."), this);
    this->connect(m_actionNewFile, SIGNAL(triggered(bool)), this, SLOT(onFileNew()));
    m_actionNewFile->setShortcut(tr("Ctrl+N"));

    m_actionClose = new QAction(Data::fileCloseIcon(), tr("&Close"), this);
    this->connect(m_actionClose, SIGNAL(triggered(bool)), this, SLOT(onFileClose()));
    m_actionClose->setShortcut(tr("Ctrl+W"));

    m_actionExit = new QAction(Data::fileExitIcon(), tr("&Exit"), this);
    this->connect(m_actionClose, SIGNAL(triggered(bool)), this, SLOT(onFileExit()));
    m_actionExit->setShortcut(tr("Ctrl+Q"));

    m_actionImportImage = new QAction(Data::fileExitIcon(), tr("Import &Image..."), this);
    this->connect(m_actionImportImage, SIGNAL(triggered(bool)), this, SLOT(onFileImage()));
    m_actionImportImage->setShortcut(tr("Ctrl+I"));

    m_actionOpenFile = new QAction(Data::fileOpenIcon(), tr("&Open..."), this);
    this->connect(m_actionOpenFile, SIGNAL(triggered(bool)), this, SLOT(onFileOpen()));
    m_actionOpenFile->setShortcut(tr("Ctrl+O"));

    m_actionSaveFile = new QAction(Data::fileSaveIcon(), tr("&Save..."), this);
    this->connect(m_actionSaveFile, SIGNAL(triggered(bool)), this, SLOT(onFileSave()));
    m_actionSaveFile->setShortcut(tr("Ctrl+S"));

    // EDIT

    m_actionUndo = m_undoStack->createUndoAction(this, tr("&Undo"));
    m_actionUndo->setIcon(Data::editUndoIcon());
    m_actionUndo->setShortcut(QKeySequence::Undo);

    m_actionRedo = m_undoStack->createRedoAction(this, tr("&Redo"));
    m_actionRedo->setIcon(Data::editRedoIcon());
    m_actionRedo->setShortcut(tr("Ctrl+R"));

    m_actionCut = new QAction(Data::editCutIcon(), tr("&Cut"), this);
    m_actionCut->setShortcut(tr("Ctrl+X"));

    m_actionCopy = new QAction(Data::editCopyIcon(), tr("C&opy"), this);
    m_actionCopy->setShortcut(tr("Ctrl+C"));

    m_actionPaste = new QAction(Data::editPasteIcon(), tr("&Paste"), this);
    m_actionPaste->setShortcut(tr("Ctrl+V"));

    m_actionDelete = new QAction(Data::editDeleteIcon(), tr("&Delete"), this);
    this->connect(m_actionDelete, SIGNAL(triggered(bool)), this, SLOT(onDelete()));
    m_actionDelete->setShortcut(Qt::Key_Delete);

    // SCENE

    m_actionSketch = new QAction(Data::sceneSketchIcon(), tr("&Sketch"), this);
    this->connect(m_actionSketch, SIGNAL(triggered(bool)), this, SLOT(onSketch()));

    m_actionEraser = new QAction(Data::sceneEraserIcon(), tr("&Eraser"), this);
    this->connect(m_actionEraser, SIGNAL(triggered(bool)), this, SLOT(onErase()));

    m_actionSelect = new QAction(tr("S&elect"), this);
    this->connect(m_actionSelect, SIGNAL(triggered(bool)), this, SLOT(onSelect()));

    m_actionOrbit = new QAction(Data::sceneOrbitIcon(), tr("&Orbit"), this);
    this->connect(m_actionOrbit, SIGNAL(triggered(bool)), this, SLOT(onCameraOrbit()));

    m_actionPan = new QAction(Data::scenePanIcon(), tr("&Pan"), this);
    this->connect(m_actionPan, SIGNAL(triggered(bool)), this, SLOT(onCameraPan()));

    m_actionZoom = new QAction(Data::sceneZoomIcon(), tr("&Zoom"), this);
    this->connect(m_actionZoom, SIGNAL(triggered(bool)), this, SLOT(onCameraZoom()));

    m_actionCanvasClone = new QAction(Data::sceneNewCanvasCloneIcon(), tr("Clone Current"), this);

    m_actionCanvasXY = new QAction(Data::sceneNewCanvasXYIcon(), tr("Plane XY"), this);

    m_actionCanvasYZ = new QAction(Data::sceneNewCanvasYZIcon(), tr("Plane YZ"), this);

    m_actionCanvasXZ = new QAction(Data::sceneNewCanvasXZIcon(), tr("Plane XZ"), this);

    m_actionSetStandard = new QAction(Data::sceneNewCanvasSetStandardIcon(), tr("Standard"), this);

    m_actionSetCoaxial = new QAction(Data::sceneNewCanvasSetCoaxialIcon(), tr("Coaxial"), this);

    m_actionSetParallel = new QAction(Data::sceneNewCanvasSetParallelIcon(), tr("Parallel"), this);

    m_actionSetRing = new QAction(Data::sceneNewCanvasSetRingIcon(), tr("Ring"), this);

    m_actionCanvasOffset = new QAction(Data::sceneCanvasOffsetIcon(), tr("Offset Canvas"), this);
    this->connect(m_actionCanvasOffset, SIGNAL(triggered(bool)), this, SLOT(onCanvasOffset()));

    m_actionCanvasRotate = new QAction(Data::sceneCanvasRotateIcon(), tr("Rotate Canvas"), this);
    this->connect(m_actionCanvasRotate, SIGNAL(triggered(bool)), this, SLOT(onCanvasRotate()));

    m_actionImageMove = new QAction(Data::sceneImageMoveIcon(), tr("Move Image"), this);
    this->connect(m_actionImageMove, SIGNAL(triggered(bool)), this, SLOT(onImageMove()));

    m_actionImageRotate = new QAction(Data::sceneImageRotateIcon(), tr("Rotate Image"), this);

    m_actionImageScale = new QAction(Data::sceneImageScaleIcon(), tr("Scale Image"), this);

    m_actionImageFlip = new QAction(Data::sceneImageFlipIcon(), tr("Flip Image"), this);

    m_actionImagePush = new QAction(Data::sceneImagePushIcon(), tr("Push Image"), this);

    m_actionStrokesPush = new QAction(Data::scenePushStrokesIcon(), tr("Push Strokes"), this);
    this->connect(m_actionStrokesPush, SIGNAL(triggered(bool)), this, SLOT(onMousePushStrokes()));
}

void MainWindow::initializeMenus()
{
    // FILE
    QMenu* menuFile = m_menuBar->addMenu(tr("&File"));
    menuFile->addAction(m_actionNewFile);
    menuFile->addAction(m_actionOpenFile);
    menuFile->addSeparator();
    menuFile->addAction(m_actionSaveFile);
    // "Save As" here
    menuFile->addSeparator();
    menuFile->addAction(m_actionImportImage);
    menuFile->addSeparator();
    menuFile->addAction(m_actionClose);
    menuFile->addAction(m_actionExit);

    // EDIT
    QMenu* menuEdit = m_menuBar->addMenu(tr("&Edit"));
    menuEdit->addAction(m_actionUndo);
    menuEdit->addAction(m_actionRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(m_actionCut);
    menuEdit->addAction(m_actionCopy);
    menuEdit->addAction(m_actionPaste);
    menuEdit->addAction(m_actionDelete);

    // SCENE
    QMenu* menuScene = m_menuBar->addMenu(tr("&Scene"));
    menuScene->addAction(m_actionSelect);
    menuScene->addAction(m_actionSketch);
    menuScene->addAction(m_actionEraser);
    menuScene->addSeparator();
    QMenu* submenuCamera = menuScene->addMenu("Camera Navigation");
    submenuCamera->addAction(m_actionOrbit);
    submenuCamera->addAction(m_actionPan);
    submenuCamera->addAction(m_actionZoom);
    menuScene->addSeparator();
    QMenu* submenuCanvas = menuScene->addMenu("New Canvas");
    submenuCanvas->addAction(m_actionCanvasClone);
    submenuCanvas->addAction(m_actionCanvasXY);
    submenuCanvas->addAction(m_actionCanvasYZ);
    submenuCanvas->addAction(m_actionCanvasXZ);
    QMenu* submenuSet = menuScene->addMenu("New Canvas Set");
    submenuSet->addAction(m_actionSetStandard);
    submenuSet->addAction(m_actionSetCoaxial);
    submenuSet->addAction(m_actionSetParallel);
    submenuSet->addAction(m_actionSetRing);
    menuScene->addSeparator();
    QMenu* submenuEC = menuScene->addMenu("Edit Canvas");
    submenuEC->addAction(m_actionCanvasOffset);
    submenuEC->addAction(m_actionCanvasRotate);
    QMenu* submenuEI = menuScene->addMenu("Edit Image");
    submenuEI->addAction(m_actionImageMove);
    submenuEI->addAction(m_actionImageRotate);
    submenuEI->addAction(m_actionImageScale);
    submenuEI->addAction(m_actionImageFlip);
    submenuEI->addAction(m_actionImagePush);
    QMenu* submenuES = menuScene->addMenu("Edit Strokes");
    submenuES->addAction(m_actionStrokesPush);
}
