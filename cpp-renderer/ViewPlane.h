#ifndef VIEWPLANE_H
#define VIEWPLANE_H

#include <iostream>

#include "Vector3d.h"
#include "Matrix4d.h"

/// Represents the viewing plane
class ViewPlane
{
    // ********************************************************************************************************
    // *** Basic methods **************************************************************************************
public:
    /// Default constructor
    ViewPlane()
    {
        // Do not use, does not initialize properly.
    }

    /// Constructor. Provide volume height, depth and scaling factor (size of largest dimension)
    /// to properly place the viewing plane against the center of the volume.
    ViewPlane(float volumeHeight, float volumeDepth, float scalingFactor)
    {

        // Create basic viewing plane 3 units out on the X axis, in the YZ plane.

        initialLowerLeft = Vector3d(3, -0.5, -0.5);
        initialUpperRight = Vector3d(3, 0.5, 0.5);
        initialUpperLeft = Vector3d(3, 0.5, -0.5);

        initialLightPosition = Vector3d(4, 1, 1);

        // Move the plane so the volume is in the center of the screen
        translationMatrix.translate(Vector3d(0,(volumeHeight/scalingFactor)/2,(volumeDepth/scalingFactor)/2));

        // Size the plane to see slightly more than the full volume.
        scalingMatrix.scale(Vector3d(1, 2, 2));

        updateVectors();
    }

    /*
    /// Destructor
    ~ViewPlane()
    {
    }
*/

    // ********************************************************************************************************
    // *** Public methods *************************************************************************************
public:
    /// Scales the view plane, in effect zooming.
    void Scale(float scalingFactor)
    {   
        // Modify the scaling matrix
        scalingMatrix.scale(Vector3d(1, scalingFactor, scalingFactor));

        updateVectors();

        // May have to do some eye-point update here as well if we are using perspective projection.
    }

    /// Rotates the view plane around the center of the volume in the requested X-Y direction
    void Rotate(float x, float y) {
        /* This is not the rotation about the volume center like we want, but about the origin. */

        Vector3d yAxis(0, 1, 0);
        Vector3d zAxis(0, 0, 1);

        // Modify the rotation matrix to match the desired rotation, about the y and z axes
        rotationMatrix.rotate(x, yAxis);
        rotationMatrix.rotate(y, zAxis);

        updateVectors();
    }

    /// Moves the plane in the X-Y direction
    void Move(float x, float y) {
        // This works, but rotation is screwed up after translating.
        translationMatrix.translate(Vector3d(0, y, -x));

        updateVectors();
    }

    Vector3d* getLowerLeft() {
        return &lowerLeft;
    }

    Vector3d* getUpperRight() {
        return &upperRight;
    }

    /// Return the vector along which this view plane casts rays
    Vector3d projectionVector() {
        return projectionDirection;
    }

    /// Return the vector that spans out the viewing plane, pointing up.
    Vector3d upVector() {
        return upDirection;
    }

    /// Return the vector that spans out the viewing plane, pointing right.
    Vector3d rightVector() {
        return rightDirection;
    }

    Vector3d getLightVector() {
        return lightPosition;
    }

    // ********************************************************************************************************
    // *** Class members **************************************************************************************
private:

    /// Updates the position of the position vectors according to the transformation matrices.
    /// Call this after updating one of the matrices. Also updates the projection vector.
    void updateVectors() {
        Matrix4d transformationMatrix = translationMatrix * rotationMatrix * scalingMatrix;

        /* TODO: I _think_ that this is the order required to get translation to work properly. But
         * it also requires some more setup to do properly wrt. the moving/scaling combo, and ensuring
         * that the setup process is correct. There is also a memory leak when moving, not sure where. */
        // Matrix4d transformationMatrix = rotationMatrix * scalingMatrix * translationMatrix;

        lowerLeft = transformationMatrix * initialLowerLeft;
        upperRight = transformationMatrix * initialUpperRight;
        upperLeft = transformationMatrix * initialUpperLeft;

        lightPosition = transformationMatrix * initialLightPosition; // Rotate light vector with the camera

        upDirection = upperLeft - lowerLeft;
        rightDirection = upperRight - upperLeft;

        // Recalculate projection vector
        projectionDirection = rightDirection.Cross(upDirection);
        projectionDirection.normalize(); // Normalize the result; we want a fixed length for the projection vector.
    }

    Vector3d projectionDirection; // The projection vector, updated whenever the plane moves.
    Vector3d upDirection;
    Vector3d rightDirection;

    // Technically not necessary using all four vectors, but this might make our lives easier.
    Vector3d lowerLeft;
    Vector3d upperRight;
    Vector3d upperLeft;

    // Transformation matrices for the view plane
    Matrix4d translationMatrix;
    Matrix4d rotationMatrix;
    Matrix4d scalingMatrix;

    // Initial vectors, used for calculating transformations
    Vector3d initialLowerLeft;
    Vector3d initialUpperRight;
    Vector3d initialUpperLeft;
    Vector3d initialLightPosition;

    Vector3d lightPosition;

}; /* ViewPlane */



#endif // VIEWPLANE_H
