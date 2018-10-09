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

        public int SliceSize;
        public int NumberOfVoxels;

        public double[,,] Contents;

        /// <summary>
        /// Initialize an empty volume
        /// </summary>
        public Volume(int xSize, int ySize, int zSize)
        {
            this.XSize = xSize;
            this.YSize = ySize;
            this.ZSize = zSize;

            //this.SliceSize = xSize * ySize;
            this.NumberOfVoxels = SliceSize * zSize;

            this.Contents = new double[xSize, ySize, zSize];
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
                                result.Contents[x, y, z] = thisVoxel / 4095.0; 
                            }
                        }
                    }
                }
            }

            return result;
        }
    }
}
