#ifndef GLWIDGETSLICER_H
#define GLWIDGETSLICER_H

#include <QtOpenGL>
#include <QGLWidget>

#include <cmath>
#include "Volume.h"
#include "Vector3d.h"
#include "Matrix4d.h"

#include "gl_tf_editor.h"

class GLWidgetSlicer : public QGLWidget
{
	Q_OBJECT

    // ************************************************************************************************************
    // *** Basic methods ******************************************************************************************
public:
    /// Default constructor
	GLWidgetSlicer(QWidget *parent=0) : QGLWidget(parent),
        m_texture(0)
	{
        volumeIsSet = false;
        currentAxis = 0;
        zoomFactor = 1;

        pan_x = 0;
        pan_y = 0;
        mouse_down = false;

        prev_mouse_x = 0;
        prev_mouse_y = 0;
    }

    /// Destructor
	~GLWidgetSlicer() 
	{
		//Delete texture
        glDeleteTextures(1, &m_texture);
    }

    // ************************************************************************************************************
    // *** Overloaded methods *************************************************************************************
	QSize sizeHint() const {
		return QSize(600, 600);
	}

	QSize minimumSizeHint() const {
		return QSize(200, 200);
	}

    // ************************************************************************************************************
    // *** Slots **************************************************************************************************
public slots:
    /// Set the volume to be visualized
    void setVolume(Volume *v)
    {
        m_volume = v;
        volumeIsSet = true;

        std::cout << "Debug: Volume was set (slice renderer)." << std::endl;

        int maxSliceNum = m_volume->getDepth()-1;
        currentSlice = 0;

        zoomFactor = 1;
        pan_x = 0;
        pan_y = 0;

        emit maxSlicesNumberChanged(maxSliceNum);

        if (this->isVisible())
        {
            updateGL();
        }
    }

    /// Set the slice number
    void setSlice(int s)
    {
        std::cout << "Debug: SetSlice s " << s <<std::endl;

        // Recompute and set texture

        currentSlice = s;
        updateSlicer();
    }

    // Called when user changes the axis selection.
    void changeAxis(int a)
    {
        currentAxis = a;
        currentSlice = 0;

        zoomFactor = 1;
        pan_x = 0;
        pan_y = 0;

        if (!volumeIsSet) {
            return;
        }

        std::cout << "Debug: Changed axis to " << a << std::endl;

        int maxSliceNum;

        if (a == 0) { // Z
            maxSliceNum = m_volume->getDepth()-1;
        } else if (a == 1) { // X
            maxSliceNum = m_volume->getWidth()-1;
        } else if (a == 2) { // Y
            maxSliceNum = m_volume->getHeight()-1;
        }

        emit maxSlicesNumberChanged(maxSliceNum);

        // Redraw display buffer, if visible.
        if (this->isVisible())
        {
            updateGL();
        }
    }

    void updateSlicer() {
        updateGL();
    }

    // ************************************************************************************************************
    // *** Signals ************************************************************************************************
signals:
    /// Notify that the number of slices is changes (e.g. whan changing slice axis)
    void maxSlicesNumberChanged(int slicesNumber);


    // ************************************************************************************************************
    // *** OpenGL handlers ****************************************************************************************
protected:
    /// Initialize the OpenGL context
	void initializeGL() {
		glClearColor(0, 0, 0, 1);
        glEnable(GL_DEPTH_TEST);

        // Initialize the texture object
        glGenTextures(1,&m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        // Add standard background 2x2 texture
        unsigned char buf[4];

        buf[0] = buf[2] = 0;
        buf[1] = buf[3] = 255 ;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 2, 2, 0 ,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, buf);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

    /// Render a scene in the OpenGL context
	void paintGL() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Dimensions of our rendered texture; only set if texture present
        int texture_x = 0;
        int texture_y = 0;

        if (volumeIsSet) {
            int dim_x = m_volume->getWidth();
            int dim_y = m_volume->getHeight();
            int dim_z = m_volume->getDepth();

            float* textureBuffer;

            std::cout << "Debug: Generating texture. Current axis is " << currentAxis << std::endl;

            // Generate texture based on our dimension and slice selection
            if (currentAxis == 0) { // Z selected
                textureBuffer = new float[dim_x*dim_y];

                texture_x = dim_x; // Dataset X and dataset Y correspond directly to the texture coordinate axes
                texture_y = dim_y;

                for (int x = 0 ; x < dim_x ; x++) {
                    for (int y = 0 ; y < dim_y ; y++) {

                        float pixelLuminosity = m_volume->getVoxel(x, y, currentSlice);
                        textureBuffer[y * dim_x + x] = pixelLuminosity;
                    }
                }

                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, dim_x, dim_y, 0 ,
                    GL_LUMINANCE, GL_FLOAT, textureBuffer);

            } else if (currentAxis == 1) { // X selected
                textureBuffer = new float[dim_y*dim_z];

                texture_x = dim_z; // Dataset Z corresponds to texture X, dataset Y corresponds to texture Y
                texture_y = dim_y;

                for (int z = 0 ; z < dim_z ; z++) {
                    for (int y = 0 ; y < dim_y ; y++) {

                        float pixelLuminosity = m_volume->getVoxel(currentSlice, y, z);
                        textureBuffer[y * dim_z + z] = pixelLuminosity;
                    }
                }

                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, dim_z, dim_y, 0 ,
                    GL_LUMINANCE, GL_FLOAT, textureBuffer);

            } else if (currentAxis == 2) { // Y selected
                textureBuffer = new float[dim_x*dim_z];

                texture_x = dim_x; // Dataset X corresponds to texture X, dataset Z corresponds to texture Y
                texture_y = dim_z;

                for (int z = 0 ; z < dim_z ; z++) {
                    for (int x = 0 ; x < dim_x ; x++) {
                        float pixelLuminosity = m_volume->getVoxel(x, currentSlice, z);
                        textureBuffer[z * dim_x + x] = pixelLuminosity;
                    }
                }

                std::cout << "Finished filling texture buffer." << std::endl;

                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, dim_x, dim_z, 0 ,
                    GL_LUMINANCE, GL_FLOAT, textureBuffer);
            }

            delete [] textureBuffer;

            std::cout << "Debug: Finished generating texture" << std::endl;
        }

        // Do the rendering
		glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);

        // Scale texture to correct aspect ratio depending on the largest axis,
        if (texture_x != 0 && texture_x > texture_y) {
            glScalef(1,((float)(texture_y))/texture_x,1);
        } else if (texture_x != 0 && texture_y > texture_x){
            glScalef(((float)(texture_x))/texture_y,1,1);
        }

        // Add mouse zoom and panning factors
        glScalef(zoomFactor, zoomFactor, zoomFactor);
        glTranslatef((pan_x/300.0)*(1.0/zoomFactor), (pan_y/300.0)*(1.0/zoomFactor), 0);

		glBegin(GL_QUADS);
		glColor3f(1.0f,1.0f,1.0f);

        glTexCoord2f(0,0); glVertex2f(-1,-1);
        glTexCoord2f(1,0); glVertex2f(1,-1);
        glTexCoord2f(1,1); glVertex2f(1,1);
        glTexCoord2f(0,1); glVertex2f(-1,1);

		glEnd();
	}

    /// Called when the window is resized
	void resizeGL(int width, int height) {
        m_uWidth = width;
        m_uHeight =  height;

        // Ensure correct aspect ratio of texture after window resizing has occurred

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

        mouse_down = true;

        prev_mouse_x = event->x();
        prev_mouse_y = event->y();

    }

    /// Called when the mouse moves
    void mouseMoveEvent(QMouseEvent *event)
    {
        if (mouse_down && volumeIsSet) {
            int dX = event->x() - prev_mouse_x;
            int dY = event->y() - prev_mouse_y;

            pan_x += dX;
            pan_y -= dY; // Axis needed to be flipped; maybe openGL and QT use different coordinate systems.

            prev_mouse_x = event->x();
            prev_mouse_y = event->y();

            updateGL();
        }
    }

    /// Called when a mouse button is released
    void mouseReleaseEvent(QMouseEvent *event)
    {
        mouse_down = false;
    }

    /// Called when the mouse wheel moves
    void wheelEvent(QWheelEvent * event)
    {
        int wheelMovement = event -> delta();

        /* This is a very rudamentary zoom function just implemented for demonstration. To do
           this properly, we should have exponential scaling and also zoom towards the current
           center of the panning operation. */

        if (wheelMovement > 0) {

            std::cout << "Debug: Zooming in. Zoom factor is " << zoomFactor << std::endl;

            zoomFactor += 0.1;

        } else if (wheelMovement < 0) {

            std::cout << "Debug: Zooming out. Zoom factor is " << zoomFactor << std::endl;

            if (zoomFactor >= 0.3) {

                zoomFactor -= 0.1;
            }
        }

        updateGL();
    }

    // ************************************************************************************************************
    // *** Class members ******************************************************************************************
private:
    const Volume *m_volume; ///< pointer to the volume to be visualized
    bool volumeIsSet;

    int currentAxis; // Current axis: 0 is z, 1 is x, 2 is y
    int currentSlice;

    float zoomFactor; // Zoom factor, used when scrolling with mouse wheel

    int pan_x; // Mouse panning of texture
    int pan_y;

    int prev_mouse_x; // Data about mouse movements, used when panning.
    int prev_mouse_y;
    bool mouse_down;

    int m_uWidth;           ///< the width of the OpenGL context
    int m_uHeight;          ///< the height of the OpenGL context

    GLuint m_texture;       ///< the texture used to show the current slice


    // Add anything else you need here
    // ...

}; /* GLWidgetSlicer */

#endif
