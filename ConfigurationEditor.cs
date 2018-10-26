using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using VolViz.Configuration;
using VolViz.Logic;

namespace VolViz
{
    public partial class ConfigurationEditor : Form
    {
        private VolumeRenderer _volumeRenderer;
        private RenderConfiguration _renderConfiguration;
        private Action _renderConfigUpdated;

        public ConfigurationEditor(VolumeRenderer volumeRenderer, Action renderConfigUpdated)
        {
            _volumeRenderer = volumeRenderer;
            _renderConfiguration = volumeRenderer.RenderConfiguration;
            _renderConfigUpdated = renderConfigUpdated;

            InitializeComponent();

            foreach (RenderingMode value in Enum.GetValues(typeof(RenderingMode)))
            {
                comboBoxRenderMode.Items.Add(value.ToFriendlyString());
            }
            comboBoxRenderMode.SelectedIndex = 0;
        }

        private void comboBoxRenderMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            var index = comboBoxRenderMode.SelectedIndex;
            var newRenderingMode = (RenderingMode)index;
            _renderConfiguration.RenderingMode = newRenderingMode;

            renderModeSpecificControls.SelectTab(index);
            _renderConfigUpdated();
        }

        private void stepSizeSelector_ValueChanged(object sender, EventArgs e)
        {
            float newStepSize = (float)stepSizeSelector.Value;
            _renderConfiguration.StepSize = newStepSize;

            _renderConfigUpdated();
        }

        private void checkBoxTrilinearInterpolation_CheckedChanged(object sender, EventArgs e)
        {
            _renderConfiguration.UseTrilinearInterpolation = checkBoxTrilinearInterpolation.Checked;

            _renderConfigUpdated();
        }

        private void checkBoxUseGradientsInTF_CheckedChanged(object sender, EventArgs e)
        {
            _renderConfiguration.UseGradientsInTransferFunction = checkBoxUseGradientsInTF.Checked;

            _renderConfigUpdated();
        }

        private void checkBoxPhongShading_CheckedChanged(object sender, EventArgs e)
        {
            _renderConfiguration.UsePhongShading = checkBoxPhongShading.Checked;

            _renderConfigUpdated();
        }
    }
}
