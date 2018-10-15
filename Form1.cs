using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using VolViz.Data;
using VolViz.Logic;

namespace VolViz
{
    public partial class Form1 : Form
    {
        bool lmbDown = false;
        bool rmbDown = false;

        Vector2 mousePositionAtLastTick = new Vector2(0,0);

        public Form1()
        {
            InitializeComponent();
        }

        private Volume volume = null;
        private VolumeRenderer renderer = null;

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

            renderer = new VolumeRenderer(volume);

            redrawVolumeRender();
        }
        
        private void canvas_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                lmbDown = true;
            }
            if (e.Button == MouseButtons.Right)
            {
                rmbDown = true;
            }
        }

        private void canvas_MouseUp(object sender, MouseEventArgs e)
        {
            lmbDown = false;
            rmbDown = false;
        }

        private void canvas_MouseLeave(object sender, EventArgs e)
        {
            lmbDown = false;
            rmbDown = false;
        }

        private void canvas_MouseMove(object sender, MouseEventArgs e)
        {
            const float pixelToModelSpaceScalingFactor = 0.001f;
            const float rotationScalingFactor = 0.0035f;

            var motionDuringThisTick = new Vector2(e.X, e.Y) - mousePositionAtLastTick;
            bool projectionHasChanged = false;

            if (lmbDown)
            {
                var translation = new Vector3(motionDuringThisTick.X, motionDuringThisTick.Y, 0) * 
                    pixelToModelSpaceScalingFactor;

                // We are moving the view plane, not the volume, but this causes mouse motion to feel opposite from
                // what's intuitive. So we move the view plane translation in the opposite direciton as the mouse.
                translation = translation * -1;

                renderer.ViewPlane.Move(translation);
                projectionHasChanged = true;
            }

            if (rmbDown)
            {
                // Y motion intuitively corresponds to X axis rotation, and vice versa.
                var rotation = new Vector2(motionDuringThisTick.Y, motionDuringThisTick.X) *
                    rotationScalingFactor;

                renderer.ViewPlane.Rotate(rotation);
                projectionHasChanged = true;
            }

            if (projectionHasChanged)
            {
                redrawVolumeRender();
            }

            mousePositionAtLastTick = new Vector2(e.X, e.Y);

            if (renderer != null)
            { 
                labelViewportState.Text = renderer.ViewPlane.GetStateDescription();
            }
        }

        private void redrawVolumeRender()
        {
            var render = renderer.Render();
            var output = ResizeImage(render, canvas.Width, canvas.Height);

            var previousFrame = canvas.Image;

            canvas.Image = output;

            if (previousFrame != null)
            {
                previousFrame.Dispose();
            }

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

        private void button3_Click(object sender, EventArgs e)
        {
            volume = Data.Volume.GetTestWireframeVolume();

            renderer = new VolumeRenderer(volume);

            redrawVolumeRender();
        }
    }
}
