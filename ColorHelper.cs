using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace VolViz
{
    public static class ColorHelper
    {
        // Hue-saturation picker 
        private const int HueSaturationSize = 165;
        private const int ValuePickerHeight = 35;

        public static Bitmap DrawHueSaturationCircle()
        {
            var result = new Bitmap(HueSaturationSize, HueSaturationSize);

            Color bgColor = Color.LightGray;
            
            for (int x = 0; x < HueSaturationSize; x++)
            {
                for (int y = 0; y < HueSaturationSize; y++)
                {
                    Color? color = GetRgbValueOfHsvCirclePixelCoords(x, y, 100);
                    if (color == null)
                    {
                        color = bgColor;
                    }

                    result.SetPixel(x, y, color.Value);
                }
            }

            return result;
        }

        /// <summary>
        /// Hue [0,360], Saturation [0,100]
        /// </summary>
        public static Bitmap DrawValueBar(int hue, int saturation)
        {
            var result = new Bitmap(HueSaturationSize, ValuePickerHeight);

            Color bgColor = Color.LightGray;

            for (int x = 0; x < HueSaturationSize; x++)
            {
                var color = ColorHSVConverter.ColorFromHSV(hue, saturation / 100d, x / (double)HueSaturationSize);

                for (int y = 0; y < ValuePickerHeight; y++)
                {
                    result.SetPixel(x, y, color);
                }
            }

            return result;
        }

        /// <summary>
        /// Value [0,100]
        /// </summary>
        public static Color? GetRgbValueOfHsvCirclePixelCoords(int x, int y, int value, 
            bool allowPointsOutsideCircle = false)
        {
            double radius = HueSaturationSize / 2;

            Vector2 center = new Vector2(HueSaturationSize / 2.0f, HueSaturationSize / 2.0f);
            double distanceFromCenter = Vector2.Distance(center, new Vector2(x, y));

            if (distanceFromCenter > radius)
            {
                if (allowPointsOutsideCircle)
                {
                    distanceFromCenter = radius;
                }
                else
                {
                    return null;
                }
            }

            double angleInRadians = Math.Atan2(y - center.Y, x - center.X);
            if (angleInRadians < 0)
            {
                angleInRadians = 2*Math.PI + angleInRadians;
            }

            double saturation = 1 - (radius - distanceFromCenter) / radius;
            double hue = angleInRadians * (180 / Math.PI);

            return ColorHSVConverter.ColorFromHSV(hue, saturation, value/100f);
        }

        //public static void GetHueAndSaturationOfPixelCoords(int x, int y,
        //    out int hue, out int saturation)
        //{
        //    double angleInRadians = Math.Atan2(y - center.Y, x - center.X);
        //    if (angleInRadians < 0)
        //    {
        //        angleInRadians = 2 * Math.PI + angleInRadians;
        //    }

        //    double saturation = 1 - (radius - distanceFromCenter) / radius;
        //    double hue = angleInRadians * (180 / Math.PI);
        //}
    }
}
