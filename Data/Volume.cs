using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Numerics;

namespace VolViz.Data
{
    public class Volume
    {
        public int XSize;
        public int YSize;
        public int ZSize;

        public int centerOfX;
        public int centerOfY;
        public int centerOfZ;

        public int SliceSize;
        public int NumberOfVoxels;

        public int SizeOfLargestDimension;

        public float[,,] Contents;
        public float[,,] GradientMagnitudes;

        /// <summary>
        /// Initialize an empty volume
        /// </summary>
        public Volume(int xSize, int ySize, int zSize)
        {
            this.XSize = xSize;
            this.YSize = ySize;
            this.ZSize = zSize;

            this.centerOfX = xSize / 2;
            this.centerOfY = ySize / 2;
            this.centerOfZ = zSize / 2;

            this.NumberOfVoxels = SliceSize * zSize;
            this.SizeOfLargestDimension = Math.Max(Math.Max(XSize, YSize), ZSize);

            this.Contents = new float[xSize, ySize, zSize];
        }

        public float GetVoxelClosest(float x, float y, float z)
        {
            return GetClosestElement(x, y, z, Contents);
        }
        
        public float GetVoxelTrilinear(float x, float y, float z)
        {
            return InterpolateTrilinear(x, y, z, Contents);
        }

        public float GetGradientClosest(float x, float y, float z)
        {
            return GetClosestElement(x, y, z, GradientMagnitudes);
        }

        public float GetGradientTrilinear(float x, float y, float z)
        {
            return InterpolateTrilinear(x, y, z, GradientMagnitudes);
        }
        
        public static Volume LoadFromDatFile(string filename)
        {
            // Note: These .dat files consist of little-endian 16-bit integers. Consider your system if it's exotic.

            Volume result;
            
            using (var stream = new FileStream(filename, FileMode.Open))
            {
                using (var reader = new BinaryReader(stream))
                {
                    var xSize = reader.ReadInt16();
                    var ySize = reader.ReadInt16();
                    var zSize = reader.ReadInt16();

                    result = new Volume(xSize, ySize, zSize);

                    var sliceSize = xSize * ySize;

                    for (int z = 0; z < zSize; z++)
                    {
                        for (int y = 0; y < ySize; y++)
                        {
                            for (int x = 0; x < xSize; x++)
                            {
                                int thisVoxel = reader.ReadInt16();

                                // Add voxel data to array, scaling to [0,1].
                                result.Contents[x, y, z] = thisVoxel / 4095.0f; 
                            }
                        }
                    }
                }
            }

            result.CalculateGradients();

            return result;
        }

        public static Volume GetTestWireframeVolume()
        {
            var result = new Volume(128, 128, 128);

            for (int x = 0; x < 128; x++)
            {
                for (int y = 0; y < 128; y++)
                {
                    for (int z = 0; z < 128; z++)
                    {
                        if (x < 5 && y < 5 ||
                            y < 5 && z < 5 ||
                            x < 5 && z < 5 ||

                            x < 5 && y > 123 ||
                            y < 5 && z > 123 ||
                            x < 5 && z > 123 ||

                            x > 123 && y < 5 ||
                            y > 123 && z < 5 ||
                            x > 123 && z < 5 ||

                            x > 123 && y > 123 ||
                            y > 123 && z > 123 ||
                            x > 123 && z > 123)
                        {
                            result.Contents[x, y, z] = 0.7f;
                        }
                        else
                        {
                            result.Contents[x, y, z] = 0.0f;
                        }
                    }
                }
            }

            result.CalculateGradients();

            return result;
        }

        private void CalculateGradients()
        {
            Vector3[,,] gradients = new Vector3[XSize, YSize, ZSize];
            GradientMagnitudes = new float[XSize, YSize, ZSize];

            // Calculate gradients and gradient magnitudes.

            for (int z = 0; z < ZSize; z++)
            {
                for (int y = 0; y < YSize; y++)
                {
                    for (int x = 0; x < XSize; x++)
                    {
                        // Edge or corner voxel, gradient is not defined. Set to null vector
                        if (x < 0 || y < 0 || z < 0 ||
                            x >= XSize || y >= YSize || z >= ZSize)
                        {
                            gradients[x, y, z] = Vector3.Zero;
                        }
                        else
                        {
                            // Calculate gradient using central differences approximation. Can use Sobel 3D
                            // or Zucker-Hummel if this is not sufficient.

                            float xDifference = (GetVoxel(x + 1, y, z) - GetVoxel(x - 1, y, z)) * 0.5f;
                            float yDifference = (GetVoxel(x, y + 1, z) - GetVoxel(x, y - 1, z)) * 0.5f;
                            float zDifference = (GetVoxel(x, y, z + 1) - GetVoxel(x, y, z - 1)) * 0.5f;

                            gradients[x, y, z] = new Vector3(xDifference, yDifference, zDifference);
                        }

                        GradientMagnitudes[x, y, z] = Vector3.Distance(Vector3.Zero, gradients[x, y, z]);
                    }
                }
            }
        }

        private float GetClosestElement(float x, float y, float z, float[,,] volume)
        {
            int xVal = (int)Math.Floor(x + 0.5);
            int yVal = (int)Math.Floor(y + 0.5);
            int zVal = (int)Math.Floor(z + 0.5);

            // Anything outside the volume has zero intensity
            if (xVal < 0 || yVal < 0 || zVal < 0 ||
                xVal >= XSize || yVal >= YSize || zVal >= ZSize)
            {
                return 0;
            }

            return volume[xVal, yVal, zVal];
        }

        private float InterpolateTrilinear(float x, float y, float z, float[,,] volume)
        {
            // Define corner values for interpolation.
            float p000 = GetClosestElement((int)Floor(x), (int)Floor(y), (int)Floor(z), volume);
            float p100 = GetClosestElement((int)Ceil(x), (int)Floor(y), (int)Floor(z), volume);
            float p101 = GetClosestElement((int)Ceil(x), (int)Floor(y), (int)Ceil(z), volume);
            float p001 = GetClosestElement((int)Floor(x), (int)Floor(y), (int)Ceil(z), volume);

            float p010 = GetClosestElement((int)Floor(x), (int)Ceil(y), (int)Floor(z), volume);
            float p110 = GetClosestElement((int)Ceil(x), (int)Ceil(y), (int)Floor(z), volume);
            float p111 = GetClosestElement((int)Ceil(x), (int)Ceil(y), (int)Ceil(z), volume);
            float p011 = GetClosestElement((int)Floor(x), (int)Ceil(y), (int)Ceil(z), volume);

            // See http://en.wikipedia.org/wiki/Trilinear_interpolation#Method///////
            float xd = x - (int)Floor(x);
            float yd = y - (int)Floor(y);
            float zd = z - (int)Floor(z);

            float c00 = p000 * (1 - xd) + p100 * xd;
            float c10 = p010 * (1 - xd) + p110 * xd;
            float c01 = p001 * (1 - xd) + p101 * xd;
            float c11 = p011 * (1 - xd) + p111 * xd;

            float c0 = c00 * (1 - yd) + c10 * yd;
            float c1 = c01 * (1 - yd) + c11 * yd;

            float c = c0 * (1 - zd) + c1 * zd;

            return c;
        }

        private double Floor(float x) => Math.Floor(x);
        private double Ceil(float x) => Math.Ceiling(x);
        private float GetVoxel(float x, float y, float z) => GetVoxelClosest(x, y, z);
    }
}
