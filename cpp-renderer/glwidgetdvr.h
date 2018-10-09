#ifndef GLWIDGETDVR_H
#define GLWIDGETDVR_H


#include <QtGui>
#include <QtOpenGL>
#include <QGLWidget>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Volume.h"
#include "ViewPlane.h"

#include <vector>

#include "tf_dialog.h"

using std::vector;

class GLWidgetDvr : public QGLWidget
{
	Q_OBJECT

    // ************************************************************************************************************
    // *** Basic methods ******************************************************************************************
public:
    /// Default constructor
    GLWidgetDvr(QWidget *parent=0) : QGLWidget(parent),
        m_texture(0)
    {
        volumeIsSet = false;

        renderingResolutionX = 64;
        renderingResolutionY = 64;

        selectedRenderingResolutionX = 64;
        selectedRenderingResolutionY = 64;

        selectedRotationResolutionX = 16;
        selectedRotationResolutionY = 16;

        selectedRenderingMode = 0;
        selectedProjectionMode = 0;
        selectedInterpolationMode = 0;
        selectedTransferFunctionMode = 0;
        selectedShadingMode = 0;
        selectedFirstHitValue = 0;
        selectedGradientInterpolationMode = 0;

        curMouseX = 0;
        curMouseY = 0;

        // TODO: Has the tf_dialog been constructed yet at this point? Only one way to find out.
        m_transferFunction = tf_dialog.GetTransferFunction();
    }

    /// Destructor
    ~GLWidgetDvr()
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
        if (volumeIsSet) {
            // Dispose old volume
            delete m_volume;
        }

        m_volume = v;
        volumeIsSet = true;

        viewPlane = ViewPlane(m_volume->getHeight(), m_volume->getDepth(), m_volume->getScalingFactor());

        // Get dataset histogram
        histogram = v->GetHistogram();
        tf_dialog.SetHistogram(histogram);

        curMouseX = 0;
        curMouseY = 0;

        std::cout << "Debug: Volume was set (volume renderer)." << std::endl;

        if (this->isVisible())
        {
            updateGL();
        }
    }

    /// User has selected a new resolution for volume rendering
    void setRes(int resolution) {
        if (resolution == 0) {
            selectedRenderingResolutionX = 64;
            selectedRenderingResolutionY = 64;
        } else if (resolution == 1) {
            selectedRenderingResolutionX = 128;
            selectedRenderingResolutionY = 128;
        }  else if (resolution == 2) {
            selectedRenderingResolutionX = 256;
            selectedRenderingResolutionY = 256;
        } else if (resolution == 3) {
            selectedRenderingResolutionX = 512;
            selectedRenderingResolutionY = 512;
        } else if (resolution == 4) {
            selectedRenderingResolutionX = 1024;
            selectedRenderingResolutionY = 1024;
        }

        renderingResolutionX = selectedRenderingResolutionX;
        renderingResolutionY = selectedRenderingResolutionY;

        updateGL();
    }

    /// Select transfer function mode
    void setTfMode(int transferFunctionMode) {
        selectedTransferFunctionMode = transferFunctionMode;
        updateGL();
    }

    /// Set step size for raycasting
    void setStepSize(double size) {
        stepSize = size;
        updateGL();
    }

    /// Set hit threshold for first-hit rendering
    void setHitValue(int firstHitValue) {
        selectedFirstHitValue = firstHitValue;
        updateGL();
    }

    /// User has selected a new resolution for rotation rendering
    void setResRotating(int resolution) {
        if (resolution == 0) {
            selectedRotationResolutionX = 16;
            selectedRotationResolutionY = 16;
        } else if (resolution == 1) {
            selectedRotationResolutionX = 32;
            selectedRotationResolutionY = 32;
        }  else if (resolution == 2) {
            selectedRotationResolutionX = 64;
            selectedRotationResolutionY = 64;
        } else if (resolution == 3) {
            selectedRotationResolutionX = 128;
            selectedRotationResolutionY = 128;
        }
    }

    /// Select rendering mode (first-hit, MIP, average, DVR)
    void setRenderingMode(int renderingMode) {
        selectedRenderingMode = renderingMode;
        updateGL();
    }

    /// Select interpolation mode for voxel values
    void setInterpolationMode(int interpolationMode) {
        selectedInterpolationMode = interpolationMode;
        updateGL();
    }

    /// Set gradient interpolation mode for voxel values
    void setGradientInterpolationMode(int interpolationMode) {
        selectedGradientInterpolationMode = interpolationMode;
        updateGL();
    }

    /// Select shading mode
    void setShading(int shadingMode) {
        selectedShadingMode = shadingMode;
        updateGL();
    }

    /// Select projection mode
    void setViewingMode(int projectionMode) {
        selectedProjectionMode = projectionMode;
    }

    void openWindowingDialog() {
        std::cout << "Debug: creating transfer function widget" << std::endl;

        tf_dialog.setWindowTitle("Transfer function editor");
        tf_dialog.exec();

        if (volumeIsSet) {
            tf_dialog.SetHistogram(histogram);
        }
    }

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

        /* CHANGE THIS */
        unsigned char buf[4];
        buf[0] = buf[2] = 0;
        buf[1] = buf[3] = 255 ;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 2, 2, 0 ,
            GL_LUMINANCE, GL_UNSIGNED_BYTE, buf);
        // ...

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /// Perform raycasting and render the scene
    void paintGL() {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (volumeIsSet) {
            // Dimensions of our rendered texture

            int texture_x = renderingResolutionX;
            int texture_y = renderingResolutionY;

            unsigned char* textureBuffer;

            std::cout << "Debug: Redrawing DVR scene." << std::endl;

            // Generate texture based on our dimension and slice selection
            textureBuffer = new unsigned char[texture_x*texture_y*3];

            int i = 0;
            for (int x = 0 ; x < texture_x ; x++) {
                for (int y = 0 ; y < texture_y ; y++) {

                    Vector3d pixelColor = castRay(x, y, selectedProjectionMode, selectedRenderingMode, selectedInterpolationMode);


                    textureBuffer[(y * texture_x + x)*3 + 0] = (unsigned char)(pixelColor.GetX()*255);
                    textureBuffer[(y * texture_x + x)*3 + 1] = (unsigned char)(pixelColor.GetY()*255);
                    textureBuffer[(y * texture_x + x)*3 + 2] = (unsigned char)(pixelColor.GetZ()*255);


                    /*
                     // TODO: Funky addressing. I think mine might be wrong and this might be correct.
                    textureBuffer[i*3 + 0] = (unsigned char)(pixelColor.GetX()*255);
                    textureBuffer[i*3 + 1] = (unsigned char)(pixelColor.GetY()*255);
                    textureBuffer[i*3 + 2] = (unsigned char)(pixelColor.GetZ()*255);
                    */

/*
                    std::cout << "Color: " << (unsigned int)(pixelColor.GetX()*255) << ", " << (unsigned int)(pixelColor.GetY()*255) << ", " << (unsigned int)(pixelColor.GetZ()*255) << std::endl;
                    std::cout << "Texbf: " << textureBuffer[i*3 + 0] << ", " << textureBuffer[i*3 + 1] << ", " << textureBuffer[i*3 + 2] << std::endl;
*/
                    i++;
                }
            }

            // std::cout << "Finished filling texture buffer." << std::endl;

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_x, texture_y, 0 ,
                GL_RGB, GL_UNSIGNED_BYTE, textureBuffer);

            delete [] textureBuffer;

            // std::cout << "Debug: Finished generating texture" << std::endl;
        }

        // Do the rendering
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);

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
        curMouseX = event->x();
        curMouseY = event->y();

        renderingResolutionX = selectedRotationResolutionX;
        renderingResolutionY = selectedRotationResolutionY;
    }

    /// Called when the mouse moves
    void mouseMoveEvent(QMouseEvent *event)
    {

        int dX = event->x() - curMouseX;
        int dY = event->y() - curMouseY;

        if (event->buttons() & Qt::LeftButton) {
            viewPlane.Rotate(dX, dY);

        } else if (event -> buttons() & Qt::RightButton) {

            // TODO: Moving the view plane doesn't quite work, leave it out for now.
            // viewPlane.Move(dX/200.0, dY/200.0);

        }

        curMouseX = event->x();
        curMouseY = event->y();

        updateGL();
    }

    /// Called when a mouse button is released
    void mouseReleaseEvent(QMouseEvent *event)
    {
        renderingResolutionX = selectedRenderingResolutionX;
        renderingResolutionY = selectedRenderingResolutionY;

        updateGL();
    }

    /// Called when the mouse wheel moves
    void wheelEvent(QWheelEvent * event)
    {
        int wheelMovement = event -> delta();

        if (wheelMovement < 0) {
            viewPlane.Scale(1.05);
        } else if (wheelMovement > 0) {
            viewPlane.Scale(0.95);
        }

        updateGL();
    }

    /// Convenience function to perform volumetric interpolation against the selected volume
    /// data using the selected interpolation mode.
    float interpolateVoxel(float x, float y, float z, int interpolationMode) {
        if (interpolationMode == 0) {
            return m_volume->getVoxelClosest(x, y, z);
        } else if (interpolationMode == 1) {
            return m_volume->getVoxelTrilinear(x, y, z);
        } else {
            // Should never happen, crash
            throw -1;
        }
    }

    /// Casts a ray into the volume, returning the pixel color resulting from the operation.
    /// projectionMode: 0 means parallel, 1 means perspective. All other values are undefined.
    /// renderingMode: 0 means first-hit, 1 means M.I.P, 2 means average, 3 means D.V.R.
    /// interpolationMode: 0 means nearest, 1 means trilinear
    /*
       There is plenty of optimization which could be done here, but I'm not doing this until there's a problem.
      */
    Vector3d castRay(int x, int y, int projectionMode, int renderingMode, int interpolationMode) {
        const int RESOLUTION_X = renderingResolutionX;
        const int RESOLUTION_Y = renderingResolutionY;

        // Parallel projection, for perspective we just have to create non-parallel projection vectors.
        if (projectionMode == 0) {
            // Get the ray projection vector; assumed to be normalized
            Vector3d projectionVector = viewPlane.projectionVector();

            //std::cout << "Raycasting projection vector: " << projectionVector.GetX() << "," << projectionVector.GetY() << "," << projectionVector.GetZ() << std::endl;

            // Start position for this ray cast is defined by lower left corner of the view plane and the x and y pixel selected for rendering
            Vector3d startingPosition(*viewPlane.getLowerLeft());
            startingPosition += (viewPlane.upVector()*y/RESOLUTION_Y);
            startingPosition += (viewPlane.rightVector()*x/RESOLUTION_X);

            int increment = 0; // How far the vector has moved

            Vector3d rayPosition; // Current position of the ray

            // Get factor to scale the volume by, this approach simplifies our vector math
            float scalingFactor = m_volume->getScalingFactor();

            // Use ray-box intersection to position the viewing ray at the entry interface to the volume

            float* inX = new float(0.0);
            float* inY = new float(0.0);
            float* inZ = new float(0.0);

            float* outX = new float(0.0);
            float* outY = new float(0.0);
            float* outZ = new float(0.0);

            bool rayIntersectsVolume = findBoxIntersectionPoints(projectionVector, startingPosition, inX, inY, inZ, outX, outY, outZ,
                                                                 m_volume->getWidth()/scalingFactor, m_volume->getHeight()/scalingFactor, m_volume->getDepth()/scalingFactor);

            if (!rayIntersectsVolume) {
                return Vector3d(0.3,0.3,0.3);
            } else {
                rayPosition.SetX((double)*inX);
                rayPosition.SetY((double)*inY);
                rayPosition.SetZ((double)*inZ);
            }

            // Calculate the length that the view ray needs to travel before going through the volume
            Vector3d totalRayDistance(*inX-*outX, *inY-*outY, *inZ-*outZ);
            double rayLength = totalRayDistance.GetMagnitude();

            // Suggestion from Helwig: Introduce jitter to smooth out artifacts due to
            rayLength -= (float)rand()/(float)RAND_MAX * stepSize;

            delete inX;
            delete inY;
            delete inZ;

            if (selectedRenderingMode != 3) { // When doing DVR, we need to keep these values around a while longer
                delete outX;
                delete outY;
                delete outZ;
            }



            if (renderingMode == 0) { // First hit
                float firstHitValue = 0;

                double rayX;
                double rayY;
                double rayZ;

                // Ray moves until it hits a sample brighter than the threshold value
                while (firstHitValue <= selectedFirstHitValue/100.0 && increment * stepSize < rayLength) {

                    rayX = rayPosition.GetX();
                    rayY = rayPosition.GetY();
                    rayZ = rayPosition.GetZ();

                    // Get the voxel color by the chosen interpolation method
                    firstHitValue = interpolateVoxel(rayX*scalingFactor, rayY*scalingFactor, rayZ*scalingFactor, interpolationMode);

                    rayPosition += projectionVector * stepSize;
                    increment++;

                }

                // return m_transferFunction->GetColor(firstHitValue);


                if (selectedShadingMode == 1) { // Phong shading
                    Vector3d L = viewPlane.getLightVector(); // Vector pointing towards light source
                    Vector3d g_n;

                    if (selectedGradientInterpolationMode == 0) {
                        g_n = -m_volume->getGradient((float)rayX*scalingFactor, (float)rayY*scalingFactor, (float)rayZ*scalingFactor);
                    } else if (selectedGradientInterpolationMode == 1) {
                        g_n = -m_volume->getGradientTrilinear((float)rayX*scalingFactor, (float)rayY*scalingFactor, (float)rayZ*scalingFactor);
                    }

                    //Vector3d g_n = -m_volume->getGradient((float)rayX*scalingFactor, (float)rayY*scalingFactor, (float)rayZ*scalingFactor);

                    Vector3d normalizedEyeDirection = -projectionVector;
                    normalizedEyeDirection.normalize();

                    Vector3d colorValue = phongShadeVoxel(Vector3d(1,1,1), L, g_n, normalizedEyeDirection, 1.3, 5, 1.7);

                    /*
                    if (colorValue.GetX() == 0 && colorValue.GetY() == 0 && colorValue.GetZ() == 0) {
                        std::cout << "black - gradient: " << g_n.GetX() << ", " << g_n.GetY() << ", " << g_n.GetZ() << std::endl;
                    }*/

                    /*
                    if (colorValue.GetX() == 1 && colorValue.GetY() == 1 && colorValue.GetZ() == 1) {
                        std::cout << "white - gradient: " << g_n.GetX() << ", " << g_n.GetY() << ", " << g_n.GetZ() << std::endl;
                    }
                    */



                    return colorValue;

                } else { // No shading, return only first value encountered
                    return m_transferFunction->GetColor(firstHitValue);
                }

            } else if (renderingMode == 1) { // M.I.P

                // TODO: M.I.P should really use preshading instead of postshading

                float maxValue = 0;
                while (increment * stepSize < rayLength) {

                    // Get volume intensity at this position

                    double rayX = rayPosition.GetX();
                    double rayY = rayPosition.GetY();
                    double rayZ = rayPosition.GetZ();

                    // Get the voxel color by the chosen interpolation method
                    float voxelValue = interpolateVoxel(rayX*scalingFactor, rayY*scalingFactor, rayZ*scalingFactor, interpolationMode);

                    if (voxelValue > maxValue) {
                        maxValue = voxelValue;
                    }

                    rayPosition += projectionVector * stepSize;
                    increment++;
                }
                return m_transferFunction->GetColor(maxValue);

            } else if (renderingMode == 2) { // Average intensity / X-Ray

                float sumOfIntensityValues = 0;
                int numberOfSamples = 0;

                while (increment * stepSize < rayLength) {

                    // Get volume intensity at this position

                    double rayX = rayPosition.GetX();
                    double rayY = rayPosition.GetY();
                    double rayZ = rayPosition.GetZ();

                    // Get the voxel color by the chosen interpolation method
                    float voxelValue = interpolateVoxel(rayX*scalingFactor, rayY*scalingFactor, rayZ*scalingFactor, interpolationMode);

                    // Sum up all sample values, then average at the end
                    sumOfIntensityValues += voxelValue;
                    numberOfSamples++;

                    rayPosition += projectionVector * stepSize;
                    increment++;
                }

                float luminosity = 0;

                if (numberOfSamples != 0) {
                    luminosity = sumOfIntensityValues/numberOfSamples;
                }

                return m_transferFunction->GetColor(luminosity);

            }  else if (renderingMode == 3) { // Direct Volume Rendering

                // Set ray position to back of volume, since we are compositing back to front

                rayPosition.SetX(*outX);
                rayPosition.SetY(*outY);
                rayPosition.SetZ(*outZ);

                // int numSteps = floor(rayLength/stepSize-0.0005);

                float c_red_out = 0;
                float c_green_out = 0;
                float c_blue_out = 0;

                float e = exp((float)1);

                while (increment * stepSize < rayLength) {

                    // Get volume intensity at this position
                    double rayX = rayPosition.GetX();
                    double rayY = rayPosition.GetY();
                    double rayZ = rayPosition.GetZ();

                    // Get the voxel color by the chosen interpolation method
                    float voxelValue = interpolateVoxel(rayX*scalingFactor, rayY*scalingFactor, rayZ*scalingFactor, interpolationMode);

                    Vector3d c_i = m_transferFunction->GetColor(voxelValue); // Color of this voxel
                    double alpha_i = m_transferFunction->GetAlpha(voxelValue); // Opacity of this voxel

                    if (selectedShadingMode == 1) { // If Phong shading, modify voxel color according to Phong algorithm
                        Vector3d L = viewPlane.getLightVector(); // Vector pointing towards light source
                        Vector3d g_n;

                        if (selectedGradientInterpolationMode == 0) {
                            g_n = -m_volume->getGradient((float)rayX*scalingFactor, (float)rayY*scalingFactor, (float)rayZ*scalingFactor);
                        } else if (selectedGradientInterpolationMode == 1) {
                            g_n = -m_volume->getGradientTrilinear((float)rayX*scalingFactor, (float)rayY*scalingFactor, (float)rayZ*scalingFactor);
                        }


                        Vector3d normalizedEyeDirection = -projectionVector;
                        normalizedEyeDirection.normalize();

                        c_i = phongShadeVoxel(Vector3d(c_i.GetX(), c_i.GetY(), c_i.GetZ()), L, g_n, normalizedEyeDirection,  7, 8, 1.7);
                    }

                    // If gradient-based transfer function, modify alpha value according to gradient at this voxel
                    if (selectedTransferFunctionMode == 1) {
                        double magnitude;
                        if (selectedGradientInterpolationMode == 0) {
                            magnitude = m_volume->getGradientMagnitude(rayX*scalingFactor, rayY*scalingFactor, rayZ*scalingFactor);
                        } else if (selectedGradientInterpolationMode == 1) {
                            magnitude = m_volume->getGradientMagnitudeTrilinear(rayX*scalingFactor, rayY*scalingFactor, rayZ*scalingFactor);
                        }

                        // It turns out that the luminosiry when compositing when using the gradient is highly dependent on the step size.
                        alpha_i = alpha_i * (1 - 1/log(e+magnitude));
                    }

                    c_red_out = c_i.GetX() * alpha_i + (1-alpha_i)*c_red_out;
                    c_green_out = c_i.GetY() * alpha_i + (1-alpha_i)*c_green_out;
                    c_blue_out = c_i.GetZ() * alpha_i + (1-alpha_i)*c_blue_out;

                    rayPosition -= projectionVector * stepSize; // When doing DVR, ray moves back-to-front
                    increment++;
                    }

                // Free memory from ray-box intersection calculations
                delete outX;
                delete outY;
                delete outZ;

                return Vector3d(c_red_out, c_green_out, c_blue_out);

            }
        }
    }

    /// Return the resulting color when phong shading a particular voxel. Input vectors are assumed to be normalized.
    ///
    /// voxelColor - the color of the voxel to be shaded
    /// normal - the normal or gradient to the voxel to be shaded
    /// eyeDirection - direction from the voxel to be shaded to the eye point
    /// k_d - scalar factor for diffuse shading
    /// k_s - scalar factor for specular shading
    /// b - exponent determining sharpness of specular shading
    ///
    /// The light in the shader is for now assumed to be white, (1,1,1).
    ///
    Vector3d phongShadeVoxel(Vector3d voxelColor, Vector3d L, Vector3d normal, Vector3d eyeDirection, float k_d, float k_s, float b) {
        Vector3d H = -L -eyeDirection; // Halfway vector between light direction and eye direction (seen from target voxel)
        H.normalize();

        float s_d = k_d * abs(L.Dot(normal)); // Diffuse component
        float s_s = k_s * pow((float)(abs((-H).Dot(normal))), b); // Specular component

        Vector3d result = (voxelColor * s_d + Vector3d(1,1,1) * s_s);
        // Prevent overflow in the case that the color intensity is saturated
        if (result.GetX() > 1) {
            result.SetX(1.0);
        }
        if (result.GetY() > 1) {
            result.SetY(1.0);
        }
        if (result.GetZ() > 1) {
            result.SetZ(1.0);
        }

        return result;
    }

    /// Check ray-box intersection between ray at rayStart moving in rayDirection. Out parameters are pointers which show the coordinates
    /// where the ray enters the volume. These are modified only if the ray intersects the volume, and the function returns
    /// true only in the cases where it does.
    ///
    /// x_max, y_max and z_max are the boundaries of the volume.
    ///
    /// We could optionally also return the point where the box exits the volume, but this is unnecessary in my implementation.
    bool findBoxIntersectionPoints(Vector3d rayDirection, Vector3d rayStart,
                                   float* inX, float* inY, float* inZ,
                                   float* outX, float* outY, float* outZ,
                                   float x_max, float y_max, float z_max) {
        float Tnear = -500000;
        float Tfar = 500000;

        const float x_min = 0.0;
        const float y_min = 0.0;
        const float z_min = 0.0;

        if (rayDirection.GetX() == 0) {// if the ray is parallel to the X axis
            if(rayStart.GetX() < x_min || rayStart.GetX() > x_max) { // if origin not between the yz planes
                return false; // no intersection
            }
        } else {
            // compute the intersection distance to the yz planes
            float T1 = (x_min - rayStart.GetX()) / rayDirection.GetX();
            float T2 = (x_max - rayStart.GetX()) / rayDirection.GetX();

            if (T1 > T2) {
                std::swap(T1, T2); // T1 is the intersection with near plane
            }
            if (T1 > Tnear) { // Look for the largest Tnear
                Tnear = T1;
            }
            if (T2 < Tfar) {
                Tfar = T2; // Look for the smallest TFar
            }
            if (Tnear > Tfar) {
                return false;
            }
            if (Tfar < 0) {
                return false;
            }
        }

        if (rayDirection.GetY() == 0) {// if the ray is parallel to the Y axis
            if(rayStart.GetY() < y_min || rayStart.GetY() > y_max) { // if origin not between the xz planes
                return false; // no intersection
            }
        } else {
            // compute the intersection distance to the xz planes
            float T1 = (y_min - rayStart.GetY()) / rayDirection.GetY();
            float T2 = (y_max - rayStart.GetY()) / rayDirection.GetY();

            if (T1 > T2) {
                std::swap(T1, T2); // T1 is the intersection with near plane
            }
            if (T1 > Tnear) { // Look for the largest Tnear
                Tnear = T1;
            }
            if (T2 < Tfar) {
                Tfar = T2; // Look for the smallest TFar
            }
            if (Tnear > Tfar) {
                return false;
            }
            if (Tfar < 0) {
                return false;
            }
        }


        if (rayDirection.GetZ() == 0) {// if the ray is parallel to the Z axis
            if(rayStart.GetZ() < z_min || rayStart.GetZ() > z_max) { // if origin not between the xy planes
                return false; // no intersection
            }
        } else {
            // compute the intersection distance to the xy planes
            float T1 = (z_min - rayStart.GetZ()) / rayDirection.GetZ();
            float T2 = (z_max - rayStart.GetZ()) / rayDirection.GetZ();

            if (T1 > T2) {
                std::swap(T1, T2); // T1 is the intersection with near plane
            }
            if (T1 > Tnear) { // Look for the largest Tnear
                Tnear = T1;
            }
            if (T2 < Tfar) {
                Tfar = T2; // Look for the smallest TFar
            }
            if (Tnear > Tfar) {
                return false;
            }
            if (Tfar < 0) {
                return false;
            }
        }

        float entryX = rayStart.GetX() + Tnear * rayDirection.GetX();
        float exitX = rayStart.GetX() + Tfar * rayDirection.GetX();

        float entryY = rayStart.GetY() + Tnear * rayDirection.GetY();
        float exitY = rayStart.GetY() + Tfar * rayDirection.GetY();

        float entryZ = rayStart.GetZ() + Tnear * rayDirection.GetZ();
        float exitZ = rayStart.GetZ() + Tfar * rayDirection.GetZ();

        *inX = entryX;
        *inY = entryY;
        *inZ = entryZ;


        *outX = exitX;
        *outY = exitY;
        *outZ = exitZ;


        return true;
    }


    // ************************************************************************************************************
    // *** Class members ******************************************************************************************
private:
    const Volume *m_volume; ///< pointer to the volume to be visualized

    int m_uWidth;           ///< the width of the OpenGL context
    int m_uHeight;          ///< the height of the OpenGL context

    bool volumeIsSet;

    GLuint m_texture;       ///< the texture used to show the current slice

    float stepSize;

    int renderingResolutionX; // Resolution of rendered texture in each dimension
    int renderingResolutionY;

    int selectedRotationResolutionX; // Resolution of texture during rotation, in each dimension
    int selectedRotationResolutionY;

    int selectedRenderingResolutionX;
    int selectedRenderingResolutionY;

    int selectedRenderingMode;
    int selectedProjectionMode;
    int selectedInterpolationMode; // 0 is nearest, 1 is trilinear
    int selectedTransferFunctionMode;
    int selectedShadingMode;
    int selectedFirstHitValue;
    int selectedGradientInterpolationMode;

    ViewPlane viewPlane;     /// The viewing plane of this volume renderer

    int curMouseX;
    int curMouseY;

    TFDialog tf_dialog;
    TransferFunction* m_transferFunction;

    vector<float> histogram;

};

#endif
