using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using VolViz.Configuration;

namespace VolViz
{
    public static class Utils
    {
        private const string VertexShaderFilename = "VertexShader.hlsl";

        private const string FirstHitShaderFilename = "FirstHitPixelShader.hlsl";
        private const string MipShaderFilename = "MipPixelShader.hlsl";
        private const string AverageShaderFilename = "AveragePixelShader.hlsl";
        private const string DvrShaderFilename = "DvrPixelShader.hlsl";

        public static string GetVertexShaderPath()
        {
            return GetShaderPath(VertexShaderFilename);
        }

        public static string GetPixelShaderForRenderingMode(RenderingMode renderingMode)
        {
            switch (renderingMode)
            {
                case RenderingMode.FirstHit:
                    return GetShaderPath(FirstHitShaderFilename);
                case RenderingMode.Mip:
                    return GetShaderPath(MipShaderFilename);
                case RenderingMode.Average:
                case RenderingMode.Dvr:
                default:
                    throw new NotImplementedException("Rendering mode not supported");
            }
        }

        private static string GetShaderPath(string shaderFilename)
        {
            var releasePath = $"{GetShaderDirectoryForRelease()}\\{shaderFilename}";
            var devPath = $"{GetShaderDirectoryForDev()}\\{shaderFilename}";

            if (File.Exists(releasePath)) {
                return releasePath;
            }

            if (File.Exists(devPath))
            {
                return devPath;
            }

            throw new FileNotFoundException($"Could not find shader. Looked in '{releasePath}' and '{devPath}'");
        }

        private static string GetShaderDirectoryForDev()
        {
            var executablePath = $"{Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)}";

            return $"{executablePath}\\Shaders";
        }

        private static string GetShaderDirectoryForRelease()
        {
            var executablePath = $"{Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)}";

            return $"{executablePath}\\..\\..\\DirectX";
        }
    }
}
