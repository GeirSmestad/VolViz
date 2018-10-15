using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace VolViz.Data
{
    public class Volume
    {
        public int XSize;
        public int YSize;
        public int ZSize;

        private int centerOfX;
        private int centerOfY;
        private int centerOfZ;

        public int SliceSize;
        public int NumberOfVoxels;

        public int SizeOfLargestDimension;

        public float[,,] Contents;

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

        /// <summary>
        /// In this transformation, 0 yields the center point in the dataset for each dimension.
        /// The largest dimension spans a range from -0.5 to +0.5
        /// </summary>
        public float GetCenteredVoxelClosest(float x, float y, float z)
        {
            float transformedX = centerOfX + (x * SizeOfLargestDimension);
            float transformedY = centerOfY + (y * SizeOfLargestDimension);
            float transformedZ = centerOfZ + (z * SizeOfLargestDimension);

            return GetVoxelClosest(
                transformedX,
                transformedY,
                transformedZ);
        }

        public float GetVoxelClosest(float x, float y, float z)
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

            return Contents[xVal, yVal, zVal];
        }

        public float GetVoxelTrilinear(int x, int y, int z)
        {
            throw new NotImplementedException();
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

            return result;
        }
    }
}
