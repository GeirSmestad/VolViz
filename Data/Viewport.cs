using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;

namespace VolViz.Data
{
    /// <summary>
    /// Represents the viewport. It is placed in relation to the volume, and is either
    /// the source or the destination of the rays that are cast through the volume.
    /// 
    /// Assumption is that volume is static, centered in the origin, consists of a 
    /// cube of dimensions 1, or smaller, and is positioned between -0.5 and +0.5
    /// in all 3 dimensions.
    /// 
    /// For the purposes of explanations in the code, this particular vector space
    /// that the viewport operates in is termed 'intermediate space'.
    /// </summary>
    public class Viewport
    {
        private readonly Vector3 InitialUpperLeft = new Vector3(-0.7f, 0.7f, 0f);
        private readonly Vector3 InitialUpperRight = new Vector3(0.7f, 0.7f, 0f);
        private readonly Vector3 InitialBottomRight = new Vector3(0.7f, -0.7f, 0f);
        private readonly Vector3 InitialBottomLeft = new Vector3(-0.7f, -0.7f, 0f);

        // Projection direction is always normal to the view plane, facing the volume
        public Vector3 ProjectionDirection;
        public Vector3 UpSpan;
        public Vector3 RightSpan;

        public Vector3 UpperLeft;
        public Vector3 UpperRight;
        public Vector3 BottomRight;
        public Vector3 BottomLeft;

        private Matrix4x4 TranslationMatrix = Matrix4x4.Identity;
        private Matrix4x4 RotationMatrixX = Matrix4x4.Identity;
        private Matrix4x4 RotationMatrixY = Matrix4x4.Identity;
        private Matrix4x4 ScalingMatrix = Matrix4x4.Identity;

        private Vector3 currentTranslation;
        private Vector2 currentRotation;
        private float currentScaling = 1;

        public Viewport()
        {
            // Define a fixed view plane that faces the volume
            UpperLeft = InitialUpperLeft;
            UpperRight = InitialUpperRight;
            BottomRight = InitialBottomRight;
            BottomLeft = InitialBottomLeft;

            ProjectionDirection = new Vector3(0, 0, 1);

            // Move to initial position, slightly outside the possible volume on the Z axis
            Move(new Vector3(0, 0, -0.8f));

            RecalculateViewPlaneVectors();
        }

        /// <summary>
        /// Zooms the view in or out by changing the size of the view plane, and hence
        /// the size of the viewport from which all the viewing rays are projected
        /// </summary>
        public void Zoom(float increment)
        {
            currentScaling += increment;

            if (currentScaling <= 0.01)
            {
                currentScaling = 0.01f;
            }

            ScalingMatrix = Matrix4x4.CreateScale(currentScaling);

            RecalculateViewPlaneVectors();
        }

        /// <summary>
        /// Moves the view plane in the left/right, up/down axes in relation to the volume
        /// </summary>
        public void Move(Vector3 translation)
        {
            currentTranslation += translation;

            TranslationMatrix = Matrix4x4.CreateTranslation(currentTranslation);

            RecalculateViewPlaneVectors();
        }
        
        /// <summary>
        /// Rotates the view plane around the volume
        /// </summary>
        public void Rotate(Vector2 rotation)
        {
            currentRotation = new Vector2(currentRotation.X + rotation.X, currentRotation.Y + rotation.Y);

            RotationMatrixX = Matrix4x4.CreateRotationX(currentRotation.X);
            RotationMatrixY = Matrix4x4.CreateRotationY(currentRotation.Y);

            RecalculateViewPlaneVectors();
        }

        /// <summary>
        /// For a ray cast from a specific point on this viewport, where the viewport has
        /// dimensions [0, 1] in both x and y axes, determine the model space location where
        /// this ray enters and exits the volume. Return false if the ray will not intersect
        /// the volume at all.
        /// 
        /// The model space vector(s) can later be used to look up voxel intensities directly
        /// from the volume dataset.
        /// 
        /// The volume in question must be provided, since this determines the conversion
        /// between intermediate space and the model space.
        /// </summary>
        public bool GetRayBoxIntersectionPointsInModelSpace(Volume model, float x, float y, 
            out Vector3 entryPoint, out Vector3 exitPoint)
        {
            var initialRayPositionInIntermediateSpace = this.BottomLeft +
                this.RightSpan * x +
                this.UpSpan * y;
            
            var rayX = initialRayPositionInIntermediateSpace.X;
            var rayY = initialRayPositionInIntermediateSpace.Y;
            var rayZ = initialRayPositionInIntermediateSpace.Z;
            
            float modelSpaceX = model.centerOfX + (rayX * model.SizeOfLargestDimension);
            float modelSpaceY = model.centerOfY + (rayY * model.SizeOfLargestDimension);
            float modelSpaceZ = model.centerOfZ + (rayZ * model.SizeOfLargestDimension);

            var initialModelSpaceVector = new Vector3(modelSpaceX, modelSpaceY, modelSpaceZ);
            
            var boxIntersectionLocations = GetPointsWhereViewportRaysIntersectVolume(
                model, initialModelSpaceVector);

            if (boxIntersectionLocations == null)
            {
                entryPoint = exitPoint = Vector3.Zero;
                return false; // No intersection.
            }

            entryPoint = boxIntersectionLocations.Item1;
            exitPoint = boxIntersectionLocations.Item2;
            
            return true;
        }

        public string GetStateDescription()
        {
            return
                $"T: {currentTranslation.X:0.00}, {currentTranslation.Y:0.00}, {currentTranslation.Z:0.00}  /  " +
                $"R: {currentRotation.X:0.00}, {currentRotation.Y:0.00}  /  " +
                $"P: {ProjectionDirection.X:0.00}, {ProjectionDirection.Y:0.00}, {ProjectionDirection.Z:0.00}  /  " +
                $"Vul: {UpperLeft.X:0.00}, {UpperLeft.Y:0.00}, {UpperLeft.Z:0.00}  /  " +
                $"Vbr: {BottomRight.X:0.00}, {BottomRight.Y:0.00}, {BottomRight.Z:0.00}  /  " +
                $"Scale: {currentScaling:0.00}";
        }

        private void RecalculateViewPlaneVectors()
        {
            UpperLeft = Vector3.Transform(InitialUpperLeft, ScalingMatrix);
            UpperRight = Vector3.Transform(InitialUpperRight, ScalingMatrix);
            BottomRight = Vector3.Transform(InitialBottomRight, ScalingMatrix);
            BottomLeft = Vector3.Transform(InitialBottomLeft, ScalingMatrix);

            UpperLeft = Vector3.Transform(UpperLeft, TranslationMatrix);
            UpperRight = Vector3.Transform(UpperRight, TranslationMatrix);
            BottomRight = Vector3.Transform(BottomRight, TranslationMatrix);
            BottomLeft = Vector3.Transform(BottomLeft, TranslationMatrix);

            UpperLeft = Vector3.Transform(UpperLeft, RotationMatrixX);
            UpperRight = Vector3.Transform(UpperRight, RotationMatrixX);
            BottomRight = Vector3.Transform(BottomRight, RotationMatrixX);
            BottomLeft = Vector3.Transform(BottomLeft, RotationMatrixX);

            UpperLeft = Vector3.Transform(UpperLeft, RotationMatrixY);
            UpperRight = Vector3.Transform(UpperRight, RotationMatrixY);
            BottomRight = Vector3.Transform(BottomRight, RotationMatrixY);
            BottomLeft = Vector3.Transform(BottomLeft, RotationMatrixY);

            UpSpan = UpperLeft - BottomLeft;
            RightSpan = UpperRight - UpperLeft;

            ProjectionDirection = Vector3.Cross(RightSpan, UpSpan);
            ProjectionDirection = Vector3.Normalize(ProjectionDirection);
        }
        
        /// <summary>
        /// Using a naïve implementation of the ray-box intersection algorithm, determine the 
        /// two points in model space where a particular ray projected along the current projection
        /// vector will enter and leave the model volume, if it hits the volume at all.
        /// 
        /// This algorithm determines the factors by which the ray must be "extended" in order
        /// to hit the entry and exit wall of the volume, respectively.
        /// 
        /// Returns null if the ray will not intersect the volume.
        /// </summary>
        public Tuple<Vector3, Vector3> GetPointsWhereViewportRaysIntersectVolume(Volume model, Vector3 initialRayPosition)
        {
            float t0x = (0 - initialRayPosition.X) / ProjectionDirection.X;
            float t1x = (model.XSize - initialRayPosition.X) / ProjectionDirection.X;

            if (t0x > t1x)
            {
                float temp = t0x;
                t0x = t1x;
                t1x = temp;
            }

            float t0y = (0 - initialRayPosition.Y) / ProjectionDirection.Y;
            float t1y = (model.YSize - initialRayPosition.Y) / ProjectionDirection.Y;

            if (t0y > t1y)
            {
                float temp = t0y;
                t0y = t1y;
                t1y = temp;
            }

            float tmin = Math.Max(t0x, t0y);
            float tmax = Math.Min(t1x, t1y);

            if (t0x > t1y || t0y > t1x)
            {
                return null; // No intersection
            }

            float t0z = (0 - initialRayPosition.Z) / ProjectionDirection.Z;
            float t1z = (model.ZSize - initialRayPosition.Z) / ProjectionDirection.Z;

            if (t0z > t1z)
            {
                float temp = t0z;
                t0z = t1z;
                t1z = temp;
            }

            if (tmin > t1z || t0z > tmax)
            {
                return null; // No intersection
            }

            if (t0z > tmin)
            {
                tmin = t0z;
            }

            if (t1z < tmax)
            {
                tmax = t1z;
            }

            // TODO: There is a risk we will lose the "edge" voxels of the volume when traversing,
            // since the initial and final position of the ray computed here might be slightly
            // inside the volume.
            //
            // Fix this by "stepping back" from or "moving forward" through the volume
            // one increment of tmin and tmax before returning the result vectors.

            return new Tuple<Vector3, Vector3>(
                initialRayPosition + ProjectionDirection * tmin,
                initialRayPosition + ProjectionDirection * tmax);
        }
    }
}
