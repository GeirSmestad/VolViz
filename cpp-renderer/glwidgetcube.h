#ifndef GLWIDGETCUBE_H
#define GLWIDGETCUBE_H

#include <QtOpenGL>
#include <QGLWidget>

#include <cmath>
#include "Volume.h"
#include "Vector3d.h"
#include "Matrix4d.h"

class GLWidgetCube : public QGLWidget
{
    Q_OBJECT

    // ************************************************************************************************************
    // *** Basic methods ******************************************************************************************
public:
    /// Default constructor
    GLWidgetCube(QWidget *parent=0) : QGLWidget(parent)
    {
        // Initialize memorization values

        xPos = 0;
        yPos = 0;

        rotationX = 0;
        rotationY = 0;

        oldRotationX = 0;
        oldRotationY = 0;

        cubePositionX = 0;
        cubePositionY = 0;

        oldPositionX = 0;
        oldPositionY = 0;
    }

    /// Destructor
    ~GLWidgetCube()
    {
    }

    // ************************************************************************************************************
    // *** Overloaded methods *************************************************************************************
public:
    QSize sizeHint() const {
        return QSize(600, 600);
    }

    QSize minimumSizeHint() const {
        return QSize(200, 200);
    }


    // ************************************************************************************************************
    // *** OpenGL handlers ****************************************************************************************
protected:

    int curMouseY;
    int curMouseX;

    // For some reason, cube is rotated relative to itself when storing transformation matrices like this.

    /*


    */
    Matrix4d rotationMatrix;
    Matrix4d translationMatrix;

    // Create variables for memorizing cube rotation and position
    int rotationX;
    int rotationY;

    int oldRotationX;
    int oldRotationY;

    int cubePositionX;
    int cubePositionY;

    int oldPositionX;
    int oldPositionY;

    int xPos;
    int yPos;

    /// Initialize the OpenGL context
    void initializeGL() {
        glClearColor(0, 0, 0, 1);
        glEnable(GL_DEPTH_TEST);
    }

    /// Render a scene in the OpenGL context
    void paintGL() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /// Set the current rotation/translation matrix
        //glLoadMatrixd(m_matMatrixCubeTransformation.Get());
        glLoadMatrixd((translationMatrix * rotationMatrix).Get());

        //do the rendering
        drawCube();
    }

    /// Called when the window is resized
    void resizeGL(int width, int height) {
        m_uWidth = width;
        m_uHeight =  height;

        // Set the projection matrix
        glMatrixMode(GL_PROJECTION);
        Matrix4d prj = Matrix4d::createOrthoPrj(
                    -static_cast<double>(width) / (1e-12 + height),
                    static_cast<double>(width) / (1e-12 + height),
                    - 1., 1., -10., 10.);
        glLoadMatrixd(prj.Get());

        // Set the modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, m_uWidth, m_uHeight);
    }

    /// Called when a mouse button is pressed
    void mousePressEvent(QMouseEvent *event)
    {
        // Get mouse position
        curMouseX = event->x();
        curMouseY = event->y();


        qDebug("Mouse down at (%d, %d)", curMouseX, curMouseY);
    }

    /// Called when the mouse moves
    void mouseMoveEvent(QMouseEvent *event)
    {
        int dX = event->x() - curMouseX;
        int dY = event->y() - curMouseY;

        qDebug("Mouse delta is (%d, %d)", dX, dY);

/*
        rotationMatrix = Matrix4d();
        translationMatrix = Matrix4d();
*/

        Matrix4d temp;

        // Rotate when left button is pressed
        if (event->buttons() & Qt::LeftButton) {
            // Create unit vectors for rotation around each axis

            // This rotates around the center axis
            Vector3d yAxis(0, 1, 0);
            Vector3d xAxis(1, 0, 0);

            /*
            rotationMatrix.rotate( dX, yAxis);
            rotationMatrix.rotate( dY, xAxis);
            */

            temp.rotate( dX, yAxis);
            temp.rotate( dY, xAxis);

            rotationMatrix.premul(temp);
        // Translate when right button is pressed
        } else if (event->buttons() & Qt::RightButton) {
            Vector3d movementVector(0.003 * dX, -0.003 * dY, 0);

            //translationMatrix.translate(movementVector);
            temp.translate(movementVector);

            translationMatrix.premul(temp);
        }

/*
        m_matMatrixCubeTransformation.premul(rotationMatrix);
        m_matMatrixCubeTransformation.premul(translationMatrix);
*/



        // Get new mouse position
        curMouseX = event->x();
        curMouseY = event->y();

        updateGL();
    }

    /// Called when a mouse button is released
    void mouseReleaseEvent(QMouseEvent *event)
    {

    }

    /// Called when the mouse wheel moves
    void wheelEvent(QWheelEvent * event)
    {
        int wheelMovement = event -> delta();

        qDebug("Mouse wheel moved with value %d", wheelMovement);

        Matrix4d scalingMatrix;

        Vector3d scalingVector; // Maybe inefficient, but makes simpler code. Meh.

        if (wheelMovement > 0) {
            scalingVector = Vector3d(1.05, 1.05, 1.05);
        } else if (wheelMovement < 0) {
            scalingVector = Vector3d(0.95, 0.95, 0.95);
        }

        scalingMatrix.scale(scalingVector);

        // Should ideally also store this centrally. This way it will always scale wrt. the origin. But can't be bothered.
        translationMatrix.premul(scalingMatrix);

        updateGL();


    }

    /// Draw a cube
    void drawCube() {

        double volx = 0.5;
        double voly = 0.5;
        double volz = 0.5;

        glBegin(GL_QUADS);

        // First face
        glColor3d(0.0,0.0,1.);
        glVertex3d(-volx, -voly, volz);
        glVertex3d(volx, -voly, volz);
        glVertex3d(volx, voly, volz);
        glVertex3d(-volx, voly, volz);

        // Second face
        glColor3d(1.0,0.0,0.0);
        glVertex3d(volx, -voly, volz);
        glVertex3d(volx, -voly, -volz);
        glVertex3d(volx, voly, -volz);
        glVertex3d(volx, voly, volz);

        // Third face
        glColor3d(1.0,1.0,0.0);
        glVertex3d(volx, -voly, -volz);
        glVertex3d(-volx, -voly, -volz);
        glVertex3d(-volx, voly, -volz);
        glVertex3d(volx, voly, -volz);

        // Fourth face
        glColor3d(0.0,1.0,1.0);
        glVertex3d(-volx, -voly, -volz);
        glVertex3d(-volx, -voly, volz);
        glVertex3d(-volx, voly, volz);
        glVertex3d(-volx, voly, -volz);

        // top face
        glColor3d(0.0,1.0,0.0);
        glVertex3d(-volx, voly, volz);
        glVertex3d(volx, voly, volz);
        glVertex3d(volx, voly, -volz);
        glVertex3d(-volx, voly, -volz);

        // bottom face
        glColor3d(1.0,0.0,1.0);
        glVertex3d(-volx, -voly, -volz);
        glVertex3d(volx, -voly, -volz);
        glVertex3d(volx, -voly, volz);
        glVertex3d(-volx, -voly, volz);
        glEnd();
    }


    // ************************************************************************************************************
    // *** Class members ******************************************************************************************
private:
    const Volume *m_volume; ///< pointer to the volume to be visualized

    int m_uWidth;           ///< the width of the OpenGL context
    int m_uHeight;          ///< the height of the OpenGL context

    Matrix4d m_matMatrixCubeTransformation; ///< the current rotation/translation matrix

}; /* GLWidgetCube */

#endif
