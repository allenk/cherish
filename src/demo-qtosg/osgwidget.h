#ifndef OSGWIDGET
#define OSGWIDGET

/* OSGWidget
 * A widget that inherits from QOpenGLWidget for encapsulating the rendering.
 * The render is done only on demand, so no QTimer is used.
 * Does not require any threaded redraw functionalities.
 * Relies on builtin event system by Qt.
 * The widget contains an instance osgViewer::GraphicsWindowEmbedded,
 * whose graphics context could be attached to any number of cameras.
 * In turn, the cameras are attached to some
 * osgViewer::CompositeViewer instance.
 *
 * The class requires minimum of: Qt 5.4, CMake 2.8.11, OpenSceneGraph 3.2
 *
 * References
 * The class is built based on the official OSG and QT tutorials,
 * as well as other articles:
 * OSG example: OSGWindows
 * OSG example: OSGKeyboardMouse
 * OSG example: OSGCompositeViewer
 * OSG example: OSGPick
 * Qt example: TabletApplication
 * Article "Making Qt and OpenSceneGraph play nice":
 * <http://bastian.rieck.ru/blog/posts/2014/qt_and_openscenegraph/>
 * Article: "To extend the OSGWidget for threaded OpenGL":
 * <http://blog.qt.io/blog/2011/06/03/threaded-opengl-in-4-8/>.
 *
 * Victoria Rudakova, Yale Graphics, 2015
 * <victoria.rudakova@yale.edu>
*/
#include <iostream>
#include <string>

#include <QOpenGLWidget>
#include <QTabletEvent>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

class OSGWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    OSGWidget(QWidget* parent = 0, const int nview = 2, const std::string& fname = "");
    virtual ~OSGWidget();
public slots:
    void setTabletDevice(QTabletEvent::TabletDevice device) { _tabletDevice = device; }
    void getTabletDevice(bool active);
protected:
    void paintEvent(QPaintEvent* pev) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    virtual bool event(QEvent* event);

    void tabletEvent(QTabletEvent* event) Q_DECL_OVERRIDE;

private:
    virtual void onHome();
    virtual void onResize(int w, int h);

    osgGA::EventQueue* getEventQueue() const;
    void processSelection();

    /// everything will be drawn on _graphicsWindow
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _graphicsWindow;

    /// there could be default composite viewers which would consist of 3 most common views
    /// read Viewer vs CompositeViewer to better understand which to use
    osg::ref_ptr<osgViewer::CompositeViewer> _viewer;
    //osg::ref_ptr<osg::Camera> _hudCamera; // for coordiante axes

    //QTabletEvent::PointerType _pointer;
    QTabletEvent::TabletDevice _tabletDevice;

    bool _selectionActive, _selectionFinished;
    QPoint _selectionStart, _selectionEnd;

    int _nview; // number of views per CompositeViewer
    bool _deviceDown; // pen touches the device?
    bool _deviceActive;
}; // class OSGWidget


#endif // OSGWIDGET

