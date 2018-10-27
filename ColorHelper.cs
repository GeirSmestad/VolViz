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
        // Hue-saturation picker dimensions
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
        public static Bitmap DrawValueBar(double hue, double saturation)
        {
            var result = new Bitmap(HueSaturationSize, ValuePickerHeight);
            
            for (int x = 0; x < HueSaturationSize; x++)
            {
                double value = x / (double)HueSaturationSize * 100;
                var color = ColorHSVConverter.ColorFromHSV(hue, saturation, value);

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
        public static Color? GetRgbValueOfHsvCirclePixelCoords(int x, int y, int value)
        {
            double hue, saturation;
            bool colorExistsForThesePixelCoords = GetHueAndSaturationOfPixelCoords(x, y, out hue, out saturation);

            if (colorExistsForThesePixelCoords)
            {
                return ColorHSVConverter.ColorFromHSV(hue, saturation, value);
            }

            return null;
        }

        // Returns true if the point specified is inside the Hue and Saturation circle
        public static bool GetHueAndSaturationOfPixelCoords(int x, int y, 
            out double hue, out double saturation)
        {
            double radius = HueSaturationSize / 2;
            bool pointIsInsideCircle = true;

            Vector2 center = new Vector2(HueSaturationSize / 2.0f, HueSaturationSize / 2.0f);
            double distanceFromCenter = Vector2.Distance(center, new Vector2(x, y));
            
            double angleInRadians = Math.Atan2(y - center.Y, x - center.X);
            if (angleInRadians < 0)
            {
                angleInRadians = 2 * Math.PI + angleInRadians;
            }

            if (distanceFromCenter > radius)
            {
                pointIsInsideCircle = false;
                distanceFromCenter = radius;
            }
            
            saturation = (1 - (radius - distanceFromCenter) / radius) * 100;
            hue = angleInRadians * (180 / Math.PI);

            return pointIsInsideCircle;
        }
    }
}
