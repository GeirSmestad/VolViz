#ifndef GL_TF_EDITOR_H
#define GL_TF_EDITOR_H

#include <QColorDialog>
#include <QtOpenGL>
#include <QGLWidget>

#include <cmath>
#include <iostream>
#include "transfer_function.h"
#include "Vector3d.h"
#include "Matrix4d.h"

class GLTrFunctEditor : public QGLWidget
{
    Q_OBJECT

    // ************************************************************************************************************
    // *** Basic methods ******************************************************************************************
public:
    /// Default constructor
    GLTrFunctEditor(QWidget *parent=0) : QGLWidget(parent),
        m_trFunct(0)
    {
        userIsDraggingInternalSample = false;
        maxHistogramValue = 0;
    }

    /// Destructor
    ~GLTrFunctEditor()
    {
    }

    // ************************************************************************************************************
    // *** Public methods *****************************************************************************************
public:
    /// Set the transfer function to edit
    void setTransferFunction(TransferFunction& tf) {
        if(m_trFunct)
            disconnect(m_trFunct, SIGNAL(tfChanged()), this, SLOT(Update()));
        m_trFunct = &tf;

        bool connectionResult = connect(m_trFunct, SIGNAL(tfChanged()), this, SLOT(Update()));

        std::cout << "Debug: Transfer function was set and result of tfChanged() connection is " << connectionResult << std::endl;
    }

    // ************************************************************************************************************
    // *** Public slots *******************************************************************************************
    /// Update the editor's content
public slots:
    void Update() {
        std::cout << "Transfer function was changed." << std::endl;
        updateGL();

        // TODO: Should really update the DVR window here, so we can see in real-time changes to the TF.
    }

    // ************************************************************************************************************
    // *** Overloaded methods *************************************************************************************
public:
    QSize sizeHint() const {
        return QSize(400, 200);
    }

    QSize minimumSizeHint() const {
        return QSize(50, 50);
    }


    void setHistogram(vector<float> parameterHistogram) {
        maxHistogramValue = 0;

        histogram = parameterHistogram;

        for (int i = 0 ; i < histogram.size() ; i++) {
            if (maxHistogramValue < histogram[i]) {
                maxHistogramValue = histogram[i];
            }
        }
    }

    // ************************************************************************************************************
    // *** OpenGL handlers ****************************************************************************************
protected:
    /// Initialize the OpenGL context
    void initializeGL() {
        // glClearColor(0, 0, 0, 1); // Set background color: Opaque black
        glClearColor(0.7, 0.7, 0.7, 1); // Set background color: Grey
        glEnable(GL_DEPTH_TEST);

        //glEnable(GL_POINT_SMOOTH); // This option produce round points instead of square ones

        std::cout << "Initialized GL" << std::endl;

    }

    /// Render a scene in the OpenGL context
    void paintGL() {


        // std::cout << "Painted GL scene" << std::endl;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the current transfer function
        drawTF();

    }

    /// Called when the window is resized
    void resizeGL(int width, int height) {
        m_uWidth = width;
        m_uHeight = height;

        // Set the projection matrix
        glMatrixMode(GL_PROJECTION);
        Matrix4d prj = Matrix4d::createOrthoPrj(- 1., 1., -1., 1., -10., 10.);
        glLoadMatrixd(prj.Get());

        // Set the modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, m_uWidth, m_uHeight);
    }

    /// Called when a mouse button is pressed
    /// Left mouse button: Move a sample
    /// Right mouse button: Delete a sample
    void mousePressEvent(QMouseEvent *event)
    {
        const double sample = static_cast<double>(event->x()) / m_uWidth;
        const double alpha = 1. - static_cast<double>(event->y()) / m_uHeight;

        /*
        std::cout << "Clicked transfer function editor at " << x << ", " << y << ". Sample= " <<
                  sample << ", alpha=" << alpha << std::endl;
        */

        double closestSample = m_trFunct->FindClosestSample(sample, alpha, 0.03);

        // Right button: Delete closest sample, if one exists. Do nothing else.
        if (event->buttons() & Qt::RightButton) {
            if (closestSample != -1) {
                m_trFunct->RemoveSample(closestSample);
                m_trFunct->DiscretizeSamples(100); // Need to manually create new discretization array
                updateGL();
            }

            return;
        }

        if (closestSample != -1 && closestSample != 0 && closestSample != 1) {
            // User has clicked a sample, pick it up (not allowed to move edge samples)
            userIsDraggingInternalSample = true;
            previousClosestSample = closestSample;
            previousColor = m_trFunct->GetColor(closestSample);

        } else if (closestSample != -1) {
            // User wants to move an edge sample
            userIsDraggingEdgeSample = true;
            previousClosestSample = closestSample;
            previousColor = m_trFunct->GetColor(closestSample);
        }

        m_trFunct->printDiscretizedContents();
    }

    /// Called when the mouse moves
    /// If a sample has been picked up, move it.
    void mouseMoveEvent(QMouseEvent *event)
    {
        const double newSample = static_cast<double>(event->x()) / m_uWidth;
        const double newAlpha = 1. - static_cast<double>(event->y()) / m_uHeight;

        if (newAlpha < 0 || newAlpha > 1) {
            // Not allowed to move across the edge of the editor (but edge sample modification ignores X)
            return;
        }

        if (userIsDraggingEdgeSample) {
                    // Edge samples can only be moved in the alpha direction
                    m_trFunct->UpdateEdgeSample(previousClosestSample,
                                                previousColor.GetX(),
                                                previousColor.GetY(),
                                                previousColor.GetZ(),
                                                newAlpha);
                    updateGL();
                }

        if (newSample <= 0 || newSample >= 1) {
            // Not allowed to move across the edge of the editor
            return;
        }

        if (userIsDraggingInternalSample) {

            // Delete previous oldest sample, insert a new sample at this location, update state and redraw

            /* We get undefined behavior when crossing another sample along the X axis (due to insertion-sorted
               samples vector), but TF consistency is maintained - so this is not a problem big enough to worry about. */

            m_trFunct->RemoveSample(previousClosestSample);

            m_trFunct->AddSample(newSample,
                                 Vector3d(previousColor.GetX(), previousColor.GetY(), previousColor.GetZ()),
                                 newAlpha);

            // Update stored sample location, but not color or alpha (this will introduce interpolation artifacts)
            previousClosestSample = newSample;

            updateGL();

            // std::cout << "Dragging sample";
        }
    }

    /// Called when a mouse button is released
    /// Drop any samples that have been picked up.
    void mouseReleaseEvent(QMouseEvent *) {
        userIsDraggingInternalSample = false;
        userIsDraggingEdgeSample = false;
        previousClosestSample = -1;
    }

    /// Called when the mouse wheel moves
    void wheelEvent(QWheelEvent *) {
    }

    /// Called on double click
    /// If a sample was double-clicked, update its color.
    /// Otherwise, add a new sample at the chosen position.
    void mouseDoubleClickEvent(QMouseEvent *event) {
        QColor color = QColorDialog::getColor();

        const double sample = static_cast<double>(event->x()) / m_uWidth;
        const double alpha = 1. - static_cast<double>(event->y()) / m_uHeight;

        double closestSample = m_trFunct->FindClosestSample(sample, alpha, 0.03);

        if (closestSample != -1) {
            // Update the color of selected sample

            if (closestSample == 0 || closestSample == 1) {
                // Edge samples cannot be deleted and get special treatment
                m_trFunct->UpdateEdgeSample(closestSample,
                                            color.red()/255.0,
                                            color.green()/255.0,
                                            color.blue()/255.0,
                                            alpha);
                updateGL();
            } else {
                // Generic internal sample, update value by deleting and replacing
                m_trFunct->RemoveSample(closestSample);

                m_trFunct->AddSample(closestSample,
                                     Vector3d(color.red()/255.0, color.green()/255.0, color.blue()/255.0),
                                     alpha);
            }

        } else {
            // Add a new sample

            /*
            std::cout << "Adding sample: "
                      << color.red() << ", "
                      << color.green() << ", "
                      << color.blue() << ", alpha="
                      << alpha << ", sample="
                      << sample << std::endl;
            */

             m_trFunct->AddSample(sample, color.red()/255.0, color.green()/255.0, color.blue()/255.0, alpha);
        }

        // m_trFunct->printContents();

        updateGL();
    }



    // ************************************************************************************************************
    // *** Private methods ****************************************************************************************
private:
    /// Draw the transfer function on the widget
    void drawTF() const {
        if(m_trFunct) {

            m_trFunct->printContents();

            // std::cout << "Drawing transfer function. Histogram size is " << histogram.size() << std::endl;




            vector<double> tfSamples = *(m_trFunct->GetSamples());

            // Draw first sample
            glPointSize(7.0);
            glBegin(GL_POINTS);
            glColor3f(tfSamples.at(1), tfSamples.at(2), tfSamples.at(3)); // Sample color
            glVertex2f(tfSamples.at(0)*2-1,tfSamples.at(4)*2-1); // Sample coords (sample, alpha)
            glEnd();

            // Iteratively draw the current line segment and the next sample, until end of vector
            for (int i = 0 ; i <= tfSamples.size()-10 ; i+= 5) {
                //std::cout << "Drawing transfer function: Sample location is " << i << std::endl;
                double thisSample = tfSamples.at(i);
                /*
                double thisR = tfSamples.at(i+1);
                double thisG = tfSamples.at(i+2);
                double thisB = tfSamples.at(i+3);
                */
                double thisAlpha = tfSamples.at(i+4);

                double nextSample = tfSamples.at(i+5);
                double nextR = tfSamples.at(i+6);
                double nextG = tfSamples.at(i+7);
                double nextB = tfSamples.at(i+8);
                double nextAlpha = tfSamples.at(i+9);

                // Draw next sample
                glPointSize(7.0);
                glBegin(GL_POINTS);
                glColor3f(nextR, nextG, nextB);
                glVertex2f(nextSample*2-1, nextAlpha*2-1);
                glEnd();

                // Draw next line segment
                glBegin(GL_LINE_STRIP);
                glColor3f(0, 0, 0);
                glVertex2f(thisSample*2-1, thisAlpha*2-1);
                glVertex2f(nextSample*2-1, nextAlpha*2-1);
                glEnd();
            }

        }

        // Draw a line on top of the TF editor to illustrate the color of the transfer function

        glBegin(GL_QUAD_STRIP);

        for (double x = -1 ; x <= 1 ; x += 0.02) {

            Vector3d color = m_trFunct->GetColor((x+1)/2);
            glColor3f(color.GetX(), color.GetY(), color.GetZ());

            glVertex2f(x, 1);
            glVertex2f(x, 0.9);
        }

        glVertex2f(1, 1);
        glVertex2f(1, 0.9);

        glEnd();


        // Draw histogram
        if (histogram.size() != 0) {
            // Paint histogram in background

            //glBegin(GL_QUADS);
             glBegin(GL_POLYGON);


            float barWidth = 2.0 / histogram.size();


            glColor3f(0.9, 0.9, 0.9);

            glVertex2f(-1, -1);

            for (int x = 0 ; x < histogram.size() ; x++) {

                float xCoord = ((float)x / histogram.size())*2-1;
                float yCoord = ((histogram[x]/maxHistogramValue)*2-1) * 0.89; // Don't touch the top bar

                //std::cout << "Painting bar at " << xCoord << " " << yCoord << ". Value = " << histogram[x] << std::endl;

                 glVertex2f(xCoord, yCoord);

                // std::cout << "X coord: " << xCoord << " " << histogram[x] << std::endl;

            }

            glVertex2f(1, -1);
            glEnd();
        }

    } /* drawTF() */



    // ************************************************************************************************************
    // *** Class members ******************************************************************************************
private:
    TransferFunction* m_trFunct;    ///< the controlled transfer function

    int m_uWidth;           ///< the width of the OpenGL context
    int m_uHeight;          ///< the height of the OpenGL context

    bool userIsDraggingInternalSample;
    bool userIsDraggingEdgeSample;

    double previousClosestSample;
    Vector3d previousColor;
    double previousClosestAlpha;

    vector<float> histogram;
    int maxHistogramValue;


}; /* GLTrFunctEditor */


#endif // GL_TF_EDITOR_H
