using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VolViz.Data
{
    public class Volume
    {
        public int XSize;
        public int YSize;
        public int ZSize;

        public int[,,] Contents;

        /// <summary>
        /// Initialize an empty volume
        /// </summary>
        public Volume(int xSize, int ySize, int zSize)
        {
            this.XSize = xSize;
            this.YSize = ySize;
            this.ZSize = zSize;

            this.Contents = new int[xSize, ySize, zSize];
        }

        public static Volume LoadFromDatFile(string filename)
        {
            return new Volume(1, 1, 1);
        }
    }
}
