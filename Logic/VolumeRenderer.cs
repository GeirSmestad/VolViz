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
        public Viewport Viewport;

        private int xSize = 128;
        private int ySize = 128;

        private float stepSize = 1f;

        public VolumeRenderer(Volume volume)
        {
            Viewport = new Viewport();
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
                    //var currentColor = CastRayFirstHit(x / (float)xSize, y / (float)ySize);
                    var currentColor = CastRayMip(x / (float)xSize, y / (float)ySize);
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
            const float threshold = 0f;

            Vector3 entryPoint;
            Vector3 exitPoint;
            
            bool rayIntersectsVolume = Viewport.GetRayBoxIntersectionPointsInModelSpace(
                Volume, viewportX, viewportY, out entryPoint, out exitPoint);
            
            if (!rayIntersectsVolume)
            {
                return Color.FromArgb(64, 0, 0);
            }

            float cutoffDistance = Vector3.Distance(entryPoint, exitPoint);

            // Projection direction is identical in intermediate space and model space
            var projectionDirection = Viewport.ProjectionDirection;

            float rayLength = 0;
            var rayPosition = entryPoint;

            while (rayLength < cutoffDistance)
            {
                float voxelValue = Volume.GetVoxelClosest(
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

                rayPosition += projectionDirection * stepSize;
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
            Vector3 entryPoint;
            Vector3 exitPoint;

            var rayIntersectsVolume = Viewport.GetRayBoxIntersectionPointsInModelSpace(
                Volume, viewportX, viewportY, out entryPoint, out exitPoint);

            if (!rayIntersectsVolume)
            {
                return Color.FromArgb(64, 0, 0);
            }

            float cutoffDistance = Vector3.Distance(entryPoint, exitPoint);

            // Projection direction is identical in intermediate space and model space
            var projectionDirection = Viewport.ProjectionDirection;

            float rayLength = 0;
            float maximumIntensity = 0;
            var rayPosition = entryPoint;

            while (rayLength < cutoffDistance)
            {
                float voxelValue = Volume.GetVoxelClosest(
                    rayPosition.X,
                    rayPosition.Y,
                    rayPosition.Z);

                if (voxelValue > maximumIntensity)
                {
                    maximumIntensity = voxelValue;
                }

                rayPosition += Viewport.ProjectionDirection * stepSize;
                rayLength += stepSize;
            }

            return Color.FromArgb(
                (int)(maximumIntensity * 255),
                (int)(maximumIntensity * 255),
                (int)(maximumIntensity * 255));
        }
    }
}
