using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace VolViz
{
    public static class Utils
    {
        public static string GetShaderPath()
        {
            var executablePath = $"{Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)}";

            var releasePath = $"{executablePath}\\shaders.hlsl";
            var devPath = $"{executablePath}\\..\\..\\DirectX\\shaders.hlsl";

            if (File.Exists(releasePath)) {
                return releasePath;
            }

            if (File.Exists(devPath))
            {
                return devPath;
            }

            throw new FileNotFoundException($"Could not find shaders. Looked in '{releasePath}' and '{devPath}'");
        }
    }
}
