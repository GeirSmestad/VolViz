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
        public Vector3 UpDirection;
        public Vector3 RightDirection;

        public Vector3 UpperLeft;
        public Vector3 UpperRight;
        public Vector3 BottomRight;
        public Vector3 BottomLeft;

        // TODO: These matrixes must be initialized to their correct values.
        public Matrix4x4 TranslationMatrix;
        public Matrix4x4 RotationMatrix;
        public Matrix4x4 ScalingMatrix;


        //public ViewPlane(float volumeHeight, float volumeDepth, float scalingFactor)
        public ViewPlane()
        {
            // Define a fixed view plane that faces the volume
            ProjectionDirection = new Vector3(0, 0, 1);
            UpDirection = new Vector3(0, 1, 0);
            RightDirection = new Vector3(1, 0, 0);

            UpperLeft = new Vector3(-0.2f, 1.2f, -0.3f);
            UpperRight = new Vector3(1.2f, 1.2f, -0.3f);
            BottomRight = new Vector3(1.2f, -1.2f, -0.3f);
            BottomLeft = new Vector3(-0.2f, -0.2f, -0.3f);
        }
    }
}
