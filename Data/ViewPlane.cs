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
    /// Assumption is that volume is static, consists of a cube of dimensions 1,
    /// and is positioned between 0 and 1 in all 3 dimensions.
    /// </summary>
    public class ViewPlane
    {
        public Vector3 ProjectionDirection;
        public Vector3 UpSpan;
        public Vector3 RightSpan;

        public Vector3 UpperLeft;
        public Vector3 UpperRight;
        public Vector3 BottomRight;
        public Vector3 BottomLeft;

        private Vector3 InitialUpperLeft = new Vector3(-0.7f, 0.7f, 0f);
        private Vector3 InitialUpperRight = new Vector3(0.7f, 0.7f, 0f);
        private Vector3 InitialBottomRight = new Vector3(0.7f, -0.7f, 0f);
        private Vector3 InitialBottomLeft = new Vector3(-0.7f, -0.7f, 0f);

        public static Matrix4x4 UnityMatrix = new Matrix4x4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);

        public Matrix4x4 TranslationMatrix = UnityMatrix;
        public Matrix4x4 RotationMatrixX = UnityMatrix;
        public Matrix4x4 RotationMatrixY = UnityMatrix;
        public Matrix4x4 ScalingMatrix = UnityMatrix;
        
        private Vector3 currentTranslation;
        private Vector2 currentRotation;
        private float currentScaling = 1;

        public ViewPlane()
        {
            // Define a fixed view plane that faces the volume
            UpperLeft = InitialUpperLeft;
            UpperRight = InitialUpperRight;
            BottomRight = InitialBottomRight;
            BottomLeft = InitialBottomLeft;

            ProjectionDirection = new Vector3(0, 0, 1);

            // Move to initial position, slightly outside the possible volume on the Z axis
            Move(new Vector3(0, 0, -0.8f));

            RecalculateVectors();
        }

        public void Move(Vector3 translation)
        {
            currentTranslation += translation;

            TranslationMatrix = Matrix4x4.CreateTranslation(currentTranslation);

            RecalculateVectors();
        }

        // Zooms the view in or out by changing the size of the view plane
        public void Zoom(float increment)
        {
            currentScaling += increment;

            if (currentScaling <= 0.01)
            {
                currentScaling = 0.01f;
            }

            ScalingMatrix = Matrix4x4.CreateScale(currentScaling);

            RecalculateVectors();
        }

        public void Rotate(Vector2 rotation)
        {
            currentRotation = new Vector2(currentRotation.X + rotation.X, currentRotation.Y + rotation.Y);

            RotationMatrixX = Matrix4x4.CreateRotationX(currentRotation.X);
            RotationMatrixY = Matrix4x4.CreateRotationY(currentRotation.Y);

            RecalculateVectors();
        }

        private void RecalculateVectors()
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
    }
}
