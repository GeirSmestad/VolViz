using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace VolViz.Data
{
    public class TransferFunction
    {
        public int counter = 0; // For testing purposes only, will be removed shortly

        public Vector3 GetColorOfIntensity(float voxelValue)
        {
            return Vector3.Zero;
        }

        public float GetOpacityOfIntensity(float voxelValue)
        {
            return 0;
        }
    }
}
