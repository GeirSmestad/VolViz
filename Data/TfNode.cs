using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace VolViz.Data
{
    /// <summary>
    /// Transfer function node. Used to define the minima and maxima of the transfer function. All the
    /// function values between these nodes are interpolated.
    /// </summary>
    public class TfNode
    {
        public float InputValue { get; set; }

        public float OutputOpacity { get; set; }
        public Vector3 OutputColor { get; set; }

        public TfNode(float inputValue, float outputOpacity, Vector3 outputColor)
        {
            InputValue = inputValue;
            OutputOpacity = outputOpacity;
            OutputColor = outputColor;
        }
    }
}
