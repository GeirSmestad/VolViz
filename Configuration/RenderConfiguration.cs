using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VolViz.Configuration
{
    public class RenderConfiguration
    {
        public RenderingMode RenderingMode { get; set; }
        public bool UseTrilinearInterpolation { get; set; }
        public bool UsePhongShading { get; set; }
        public bool UseGradientsInTransferFunction { get; set; }
    }
}
