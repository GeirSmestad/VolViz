using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VolViz.Configuration
{
    public enum RenderingMode
    {
        Dvr,
        FirstHit,
        Mip,
        Average,
    }

    public static class RenderingModeExtensions
    {
        public static string ToFriendlyString(this RenderingMode mode)
        {
            switch (mode)
            {
                case RenderingMode.Dvr:
                    return "Direct Volume Rendering";
                case RenderingMode.FirstHit:
                    return "First-hit";
                case RenderingMode.Mip:
                    return "Maximum Intensity Projection";
                case RenderingMode.Average:
                    return "Average Projection";
                default:
                    return mode.ToString();
            }
        }
    }
}
