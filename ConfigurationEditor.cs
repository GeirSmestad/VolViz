﻿using System;
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
        }
    }
}
