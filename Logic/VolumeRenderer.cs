using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using VolViz.Data;
using VolViz.Logic;
using VolViz.Configuration;

namespace VolViz.Logic
{
    public class VolumeRenderer
    {
        public Volume Volume;
        public Viewport Viewport;
        public TransferFunction TransferFunction { get; set; }
        public RenderConfiguration RenderConfiguration { get; set; }

        private int xSize = 128;
        private int ySize = 128;

        private Func<float, float, float, float> getVoxelBySelectedMethod;
        private Func<float, float, float, float> getGradientBySelectedMethod;
        private Func<float, float, Color> castRayWithSelectedCompositingAlgorithm;

        public VolumeRenderer(Volume volume)
        {
            Viewport = new Viewport();
            TransferFunction = new TransferFunction();
            RenderConfiguration = new RenderConfiguration();
            this.Volume = volume;

            SetLocalRenderingParameters();
        }

        public Bitmap Render()
        {
            var result = new Bitmap(xSize, ySize);

            Color[,] buffer = new Color[xSize, ySize];
            
            SetLocalRenderingParameters();

            for (int x = 0; x < xSize; x++)
            {
                Parallel.For(0, ySize, y =>
                {
                    var currentColor = castRayWithSelectedCompositingAlgorithm(x / (float)xSize, y / (float)ySize);

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
                float voxelValue = getVoxelBySelectedMethod(
                    rayPosition.X,
                    rayPosition.Y,
                    rayPosition.Z);

                if (voxelValue > threshold)
                {
                    Vector3 color = GetColorOfIntensity(voxelValue);

                    return ZeroOneVectorToColor(color);
                }

                rayPosition += projectionDirection * RenderConfiguration.StepSize;
                rayLength += RenderConfiguration.StepSize;
            }

            Vector3 zeroColor = GetColorOfIntensity(0);

            return ZeroOneVectorToColor(zeroColor);
        }

        /// <summary>
        /// Cast a ray from the viewport, through the volume.
        /// 
        /// Rendering method is Direct Volume Rendering.
        /// </summary>
        private Color CastRayDvr(float viewportX, float viewportY)
        {
            Vector3 entryPoint;
            Vector3 exitPoint;

            bool rayIntersectsVolume = Viewport.GetRayBoxIntersectionPointsInModelSpace(
                Volume, viewportX, viewportY, out entryPoint, out exitPoint);

            if (!rayIntersectsVolume)
            {
                return Color.FromArgb(64, 0, 64);
            }

            float cutoffDistance = Vector3.Distance(entryPoint, exitPoint);

            // In DVR, we project back-to-front
            var rayPosition = exitPoint;
            var projectionDirection = Viewport.ProjectionDirection * -1;

            float rayLength = 0;
            
            Vector3 outputColor = Vector3.Zero;

            while (rayLength < cutoffDistance)
            {
                var voxelValue = getVoxelBySelectedMethod(
                    rayPosition.X,
                    rayPosition.Y,
                    rayPosition.Z);
                
                Vector3 colorAtThisVoxel = GetColorOfIntensity(voxelValue);
                float opacityAtThisVoxel = GetOpacityOfIntensity(voxelValue);

                if (RenderConfiguration.UsePhongShading)
                { // If Phong shading, modify voxel color according to Phong algorithm
                    //Vector3d L = viewPlane.getLightVector(); // Vector pointing towards light source
                    //Vector3d g_n;

                    //if (selectedGradientInterpolationMode == 0)
                    //{
                    //    g_n = -m_volume->getGradient((float)rayX * scalingFactor, (float)rayY * scalingFactor, (float)rayZ * scalingFactor);
                    //}
                    //else if (selectedGradientInterpolationMode == 1)
                    //{
                    //    g_n = -m_volume->getGradientTrilinear((float)rayX * scalingFactor, (float)rayY * scalingFactor, (float)rayZ * scalingFactor);
                    //}
                    
                    //Vector3d normalizedEyeDirection = -projectionVector;
                    //normalizedEyeDirection.normalize();

                    //c_i = phongShadeVoxel(Vector3d(c_i.GetX(), c_i.GetY(), c_i.GetZ()), L, g_n, normalizedEyeDirection, 7, 8, 1.7);
                }

                // If gradient-based transfer function, modify alpha value according to gradient at this voxel
                if (RenderConfiguration.UseGradientsInTransferFunction)
                {
                    var magnitude = getGradientBySelectedMethod(rayPosition.X, rayPosition.Y, rayPosition.Z);

                    // It turns out that the luminosiry when compositing when using the gradient is highly dependent on the step size.
                    opacityAtThisVoxel = opacityAtThisVoxel * (float)(1 - 1 / Math.Log(Math.E + magnitude));
                }
                
                // The following compositing algorithm is the heart of DVR.
                outputColor = colorAtThisVoxel * opacityAtThisVoxel + (1 - opacityAtThisVoxel) * outputColor;
              
                rayPosition += projectionDirection * RenderConfiguration.StepSize;
                rayLength += RenderConfiguration.StepSize;
            }

            return ZeroOneVectorToColor(outputColor);
        }
        
        /// <summary>
        /// This is the transfer function that converts from voxel value to color.
        /// </summary>
        private Vector3 GetColorOfIntensity(float voxelValue)
        {
            return TransferFunction.GetColorOfIntensity(voxelValue);
        }
        
        private float GetOpacityOfIntensity(float voxelValue)
        {
            return TransferFunction.GetOpacityOfIntensity(voxelValue);
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
                return Color.FromArgb(0, 0, 64);
            }

            float cutoffDistance = Vector3.Distance(entryPoint, exitPoint);

            // Projection direction is identical in intermediate space and model space
            var projectionDirection = Viewport.ProjectionDirection;

            float rayLength = 0;
            float maximumIntensity = 0;
            var rayPosition = entryPoint;

            while (rayLength < cutoffDistance)
            {
                float voxelValue = getVoxelBySelectedMethod(
                    rayPosition.X,
                    rayPosition.Y,
                    rayPosition.Z);

                if (voxelValue > maximumIntensity)
                {
                    maximumIntensity = voxelValue;
                }

                rayPosition += Viewport.ProjectionDirection * RenderConfiguration.StepSize;
                rayLength += RenderConfiguration.StepSize;
            }

            Vector3 color = GetColorOfIntensity(maximumIntensity);

            return ZeroOneVectorToColor(color);
        }
        
        /// <summary>
        /// Cast a ray from the viewport, through the volume.
        /// 
        /// Rendering method is average projection, i.e. a close analogy to a X-ray photograph
        /// </summary>
        private Color CastRayAverageProjection(float viewportX, float viewportY)
        {
            Vector3 entryPoint;
            Vector3 exitPoint;

            var rayIntersectsVolume = Viewport.GetRayBoxIntersectionPointsInModelSpace(
                Volume, viewportX, viewportY, out entryPoint, out exitPoint);

            if (!rayIntersectsVolume)
            {
                return Color.FromArgb(0, 64, 0);
            }

            float cutoffDistance = Vector3.Distance(entryPoint, exitPoint);

            // Projection direction is identical in intermediate space and model space
            var projectionDirection = Viewport.ProjectionDirection;

            float rayLength = 0;
            float numberOfSamples = 0;
            float totalAbsorption = 0;
            var rayPosition = entryPoint;

            while (rayLength < cutoffDistance)
            {
                float voxelValue = getVoxelBySelectedMethod(
                    rayPosition.X,
                    rayPosition.Y,
                    rayPosition.Z);

                numberOfSamples += 1;
                totalAbsorption += voxelValue;

                rayPosition += Viewport.ProjectionDirection * RenderConfiguration.StepSize;
                rayLength += RenderConfiguration.StepSize;
            }

            float averageAbsorption = totalAbsorption / numberOfSamples;

            Vector3 color = GetColorOfIntensity(averageAbsorption);

            return ZeroOneVectorToColor(color);
        }

        private void SetLocalRenderingParameters()
        {
            if (RenderConfiguration.UseTrilinearInterpolation)
            {
                getVoxelBySelectedMethod = Volume.GetVoxelTrilinear;
                getGradientBySelectedMethod = Volume.GetGradientTrilinear;
            }
            else
            {
                getVoxelBySelectedMethod = Volume.GetVoxelClosest;
                getGradientBySelectedMethod = Volume.GetGradientTrilinear;
            }

            switch (RenderConfiguration.RenderingMode)
            {
                case RenderingMode.FirstHit:
                    castRayWithSelectedCompositingAlgorithm = CastRayFirstHit;
                    break;
                case RenderingMode.Mip:
                    castRayWithSelectedCompositingAlgorithm = CastRayMip;
                    break;
                case RenderingMode.Average:
                    castRayWithSelectedCompositingAlgorithm = CastRayAverageProjection;
                    break;
                case RenderingMode.Dvr:
                    castRayWithSelectedCompositingAlgorithm = CastRayDvr;
                    break;
                default:
                    throw new InvalidOperationException("No rendering algorithm specified");
            }
        }

        private Color ZeroOneVectorToColor(Vector3 vector)
        {
            return Color.FromArgb(
                (int)(vector.X * 255),
                (int)(vector.Y * 255),
                (int)(vector.Z * 255));
        }
    }
}
