using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using VolViz.Data;
using VolViz.Logic;

namespace VolViz
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private Volume volume = null;

        private void button1_Click(object sender, EventArgs e)
        {
            volume = Data.Volume.LoadFromDatFile("..\\..\\Datasets\\lobster.dat");

            trackBar1.Maximum = volume.ZSize-1;


        }

        private void DrawSliceToCanvas(int sliceIndex, Volume dataset)
        {
            canvas.Image = new Bitmap(dataset.XSize, dataset.YSize);

            for (int x = 0; x < dataset.XSize; x++)
            {
                for (int y = 0; y < dataset.YSize; y++)
                {
                    var currentPixel = dataset.Contents[x, y, sliceIndex];
                    var currentIntensity = (int)(currentPixel* 255);

                    var currentColor = Color.FromArgb(currentIntensity, currentIntensity, currentIntensity);

                    ((Bitmap)canvas.Image).SetPixel(x, y, currentColor);
                }
            }

            canvas.Refresh();
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            DrawSliceToCanvas(trackBar1.Value, volume);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            volume = Data.Volume.LoadFromDatFile("..\\..\\Datasets\\lobster.dat");

            VolumeRenderer renderer = new VolumeRenderer(volume);

            var render = renderer.Render();
            var output = ResizeImage(render, canvas.Width, canvas.Height);

            canvas.Image = output;
            canvas.Refresh();
        }

        public static Bitmap ResizeImage(Image image, int width, int height)
        {
            var destRect = new Rectangle(0, 0, width, height);
            var destImage = new Bitmap(width, height);

            destImage.SetResolution(image.HorizontalResolution, image.VerticalResolution);

            using (var graphics = Graphics.FromImage(destImage))
            {
                graphics.CompositingMode = CompositingMode.SourceCopy;
                graphics.CompositingQuality = CompositingQuality.HighQuality;
                graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                graphics.SmoothingMode = SmoothingMode.HighQuality;
                graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;

                using (var wrapMode = new ImageAttributes())
                {
                    wrapMode.SetWrapMode(WrapMode.TileFlipXY);
                    graphics.DrawImage(image, destRect, 0, 0, image.Width, image.Height, GraphicsUnit.Pixel, wrapMode);
                }
            }

            return destImage;
        }
    }
}
