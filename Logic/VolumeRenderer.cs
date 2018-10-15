using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using VolViz.Data;

namespace VolViz.Logic
{
    public class VolumeRenderer
    {
        public Volume Volume;
        public ViewPlane ViewPlane;

        private int xSize = 64;
        private int ySize = 64;

        private float stepSize = 0.01f;

        public VolumeRenderer(Volume volume)
        {
            ViewPlane = new ViewPlane();
            this.Volume = volume;
        }

        public Bitmap Render()
        {
            var result = new Bitmap(xSize, ySize);

            Color[,] buffer = new Color[xSize, ySize];

            for (int x = 0; x < xSize; x++)
            {
                Parallel.For(0, ySize, y =>
                {
                    var currentColor = CastRayFirstHit(x / (float)xSize, y / (float)ySize);
                    //var currentColor = CastRayMip(x / (float)xSize, y / (float)ySize);
                    buffer[x, y] = currentColor;
                });
            }

            for (int x = 0; x < xSize; x++)
            {
                for (int y = 0; y < ySize; y++)
                {
                    result.SetPixel(x, y, buffer[x, y]);
                }
            }

            return result;
        }

        /// <summary>
        /// Cast a ray from the viewport, through the volume. Return the color of the ray
        /// after volume traversal. Rendering method is first-hit.
        /// 
        /// Ray is cast from a chosen point on the viewport, where 0 is bottom and left, and
        /// 1 is top and right.
        /// </summary>
        private Color CastRayFirstHit(float viewportX, float viewportY)
        {
            Vector4 result = new Vector4(0, 0, 0, 0);

            var rayPosition = ViewPlane.BottomLeft +
                ViewPlane.RightSpan * viewportX +
                ViewPlane.UpSpan * viewportY;

            float rayLength = 0;
            float cutoffDistance = 3f;
            float threshold = 0f;

            while (rayLength < cutoffDistance)
            {
                // TODO: Inefficient to translate to model space on every step. This can be handled better.
                float voxelValue = Volume.GetCenteredVoxelClosest(
                    rayPosition.X,
                    rayPosition.Y,
                    rayPosition.Z);

                if (voxelValue > threshold)
                {
                    return Color.FromArgb(
                        (int)(voxelValue * 255),
                        (int)(voxelValue * 255),
                        (int)(voxelValue * 255));
                }

                rayPosition += ViewPlane.ProjectionDirection * stepSize;
                rayLength += stepSize;
            }

            return Color.FromArgb(0, 0, 0);
        }

        /// <summary>
        /// Cast a ray from the viewport, through the volume.
        /// 
        /// Rendering method is maximum intensity projection.
        /// </summary>
        private Color CastRayMip(float viewportX, float viewportY)
        {
            Vector4 result = new Vector4(0, 0, 0, 0);

            // TODO: Viewport size varies when zooming. Need to consider viewport size when calculating
            // ray position if scaling/zooming is implemented.
            var rayPosition = ViewPlane.BottomLeft +
                ViewPlane.RightSpan * viewportX +
                ViewPlane.UpSpan * viewportY;

            float rayLength = 0;
            float cutoffDistance = 2;

            float maximumIntensity = 0;

            while (rayLength < cutoffDistance)
            {
                // TODO: Inefficient to translate to model space on every step. This can be handled better.
                float voxelValue = Volume.GetVoxelClosest(
                    rayPosition.X * Volume.SizeOfLargestDimension,
                    rayPosition.Y * Volume.SizeOfLargestDimension,
                    rayPosition.Z * Volume.SizeOfLargestDimension);

                if (voxelValue > maximumIntensity)
                {
                    maximumIntensity = voxelValue;
                }

                rayPosition += ViewPlane.ProjectionDirection * stepSize;
                rayLength += stepSize;
            }

            return Color.FromArgb(
                (int)(maximumIntensity * 255),
                (int)(maximumIntensity * 255),
                (int)(maximumIntensity * 255));
        }
    }
}
