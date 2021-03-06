﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using VolViz.Data;
using VolViz.Logic;

namespace VolViz
{
    public partial class TransferFunctionEditor : Form
    {
        private VolumeRenderer _volumeRenderer;
        private TransferFunction _transferFunction;
        private Action _transferFunctionUpdated;

        private double _hue;
        private double _saturation;
        private double _value;

        private Color _selectedColor;

        public TransferFunctionEditor(VolumeRenderer volumeRenderer, Action transferFunctionUpdated)
        {
            _volumeRenderer = volumeRenderer;
            _transferFunction = volumeRenderer.TransferFunction;
            _transferFunctionUpdated = transferFunctionUpdated;

            _hue = 180;
            _saturation = _value = 75;

            InitializeComponent();

            RedrawColorDesigner();
            RedrawValuePicker();
            DrawHueSaturationPicker();
            UpdateSelectedColor();
        }

        private void buttonSave_Click(object sender, EventArgs e)
        {
        }
        
        // TODO: Drawing & mouse detection logic might be extracted to helper class

        private bool _colorDesigner_lmbDown = false;

        private int _indexOfSelectedNode = -1;
        private int _indexOfHilightedNode = -1;

        private const int nodeSize = 15; // Draw size of node in pixels

        private void colorDesigner_MouseDown(object sender, MouseEventArgs e)
        {
            int width = colorDesigner.Width;
            int height = colorDesigner.Height;

            if (e.Button == MouseButtons.Left)
            {
                _colorDesigner_lmbDown = true;

                if (_indexOfHilightedNode != -1)
                {
                    _indexOfSelectedNode = _indexOfHilightedNode;
                    _transferFunction.Nodes[_indexOfSelectedNode].OutputColor =
                        ColorToVector(_selectedColor);
                }
                else
                {
                    // TODO: Restricting to [0,1] might be accomplished with helper functions...
                    float inputValue = e.X / (float)width;
                    if (inputValue < 0) { inputValue = 0; }
                    if (inputValue > 1) { inputValue = 1; }

                    float outputOpacity = invWinFormsY(e.Y, height) / height;
                    if (outputOpacity > 1) { outputOpacity = 1; }
                    if (outputOpacity < 0 ) { outputOpacity = 0; }
                    
                    _transferFunction.AddNode(
                        inputValue,
                        outputOpacity, 
                        ColorToVector(_selectedColor));
                }
            }

            if (e.Button == MouseButtons.Right)
            {
                if (_indexOfHilightedNode != -1 && 
                    _indexOfHilightedNode != 0 && 
                    _indexOfHilightedNode != _transferFunction.Nodes.Count-1)
                {
                    _transferFunction.Nodes.RemoveAt(_indexOfHilightedNode);
                    _indexOfHilightedNode = -1;
                }
            }

            RedrawColorDesigner();
            _transferFunction.RecalculateLookupTables();
            _transferFunctionUpdated();
        }

        private void colorDesigner_MouseUp(object sender, MouseEventArgs e)
        {
            _colorDesigner_lmbDown = false;
            _indexOfSelectedNode = -1;
        }

        private void colorDesigner_MouseLeave(object sender, EventArgs e)
        {
            _colorDesigner_lmbDown = false;
            _indexOfSelectedNode = -1;

            RedrawColorDesigner();
            _transferFunction.RecalculateLookupTables();
            _transferFunctionUpdated();
        }

        private void colorDesigner_MouseMove(object sender, MouseEventArgs e)
        {
            // TODO: Switch selection when hilight has changed and mouse is down
            if (_colorDesigner_lmbDown && _indexOfSelectedNode != -1)
            {
                var height = colorDesigner.Height;
                
                float outputOpacity = invWinFormsY(e.Y, height) / height;
                if (outputOpacity > 1) { outputOpacity = 1; }
                if (outputOpacity < 0) { outputOpacity = 0; }

                // Move currently selected node to next coordinates
                var nodeToMove = _transferFunction.Nodes[_indexOfHilightedNode];
                nodeToMove.OutputOpacity = outputOpacity;
            }
            else
            {
                _indexOfHilightedNode = GetIndexOfNodeAtPixelCoordinate(e.X);
            }

            RedrawColorDesigner();
            _transferFunction.RecalculateLookupTables();
            _transferFunctionUpdated();
        }

        /// <summary>
        /// For an X coordinate in the color designer, return the index in the TF node
        /// list that corresponds to this coordinate, or -1 if this coordinate does not correspond
        /// to a particular node of the transfer function being edited.
        /// 
        /// We only care about the X coordinate counts when selecting TF nodes
        /// </summary>
        private int GetIndexOfNodeAtPixelCoordinate(int x)
        {
            // TODO: Also take Y coordinate into account; this is relevant when there are nodes close in X but not Y
            float width = colorDesigner.Width;

            float min = x - nodeSize / 2f;
            float max = x + nodeSize / 2f;

            float minCoordinateInTfSpace = min / width;
            float maxCoordinateInTfSpace = max / width;

            // Can be more efficient with binary search, but likely not a problem
            for (int i = 0; i < _transferFunction.Nodes.Count; i++)
            {
                var node = _transferFunction.Nodes[i];
                if (node.InputValue > minCoordinateInTfSpace &&
                    node.InputValue < maxCoordinateInTfSpace)
                {
                    return i;
                }
            }

            return -1;
        }

        private Bitmap DrawColorDesigner()
        {
            int width = colorDesigner.Width;
            int height = colorDesigner.Height;

            Bitmap result = new Bitmap(width, height);

            using (Graphics g = Graphics.FromImage(result))
            {
                g.FillRectangle(new SolidBrush(Color.Gray), 0, 0, width, height);
                
                for (int i = 1; i < _transferFunction.Nodes.Count; i++)
                {
                    var previousNode = _transferFunction.Nodes[i - 1];
                    var currentNode = _transferFunction.Nodes[i];

                    g.DrawLine(Pens.Black,
                        width * previousNode.InputValue, invCartesianY(height * previousNode.OutputOpacity, height),
                        width * currentNode.InputValue, invCartesianY(height * currentNode.OutputOpacity, height));
                }

                for (int i = 0; i < _transferFunction.Nodes.Count; i++)
                {
                    var currentNode = _transferFunction.Nodes[i];
                    var brush = new SolidBrush(VectorToColor(currentNode.OutputColor));
                    
                    g.FillEllipse(brush,
                        width * currentNode.InputValue - nodeSize / 2f,
                        invCartesianY(height * currentNode.OutputOpacity, height) - nodeSize / 2f,
                        nodeSize,
                        nodeSize);

                    if (_indexOfHilightedNode == i)
                    {
                        g.DrawEllipse(new Pen(Color.Yellow, 2),
                            width * currentNode.InputValue - nodeSize / 2f,
                            invCartesianY(height * currentNode.OutputOpacity, height) - nodeSize / 2f,
                            nodeSize,
                            nodeSize);
                    }
                }
            }

            return result;
        }

        /// <summary>
        /// In WinForms, (0,0) is the top-left, not the bottom-left. Translate a Y coordinate from the
        /// (cartesian) coordinate system where (0, height) is the top-left, to the coordinate system of
        /// WinForms.
        /// </summary>
        private float invCartesianY(float cartesianYCoordinate, int viewHeight)
        {
            return (viewHeight - cartesianYCoordinate);
        }

        /// <summary>
        /// In WinForms, (0,0) is the top-left, not the bottom-left. Translate a Y coordinate from the
        /// coordinate system of WinForms, to the (cartesian) coordinate system where (0, height) is the
        /// top-left.
        /// </summary>
        private float invWinFormsY(float winFormsYCoordinate, int viewHeight)
        {
            return (viewHeight - winFormsYCoordinate);
        }
        
        private void RedrawColorDesigner()
        {
            var oldImage = colorDesigner.Image;
            colorDesigner.Image = DrawColorDesigner();

            if (oldImage != null)
            {
                oldImage.Dispose();
            }
        }

        private void DrawHueSaturationPicker()
        {
            var oldImage = hueSaturationPicker.Image;

            hueSaturationPicker.Image = ColorHelper.DrawHueSaturationCircle();
            if (oldImage != null)
            {
                oldImage.Dispose();
            }
        }

        private void RedrawValuePicker()
        {
            var oldImage = valuePicker.Image;

            valuePicker.Image = ColorHelper.DrawValueBar(_hue, _saturation);
            if (oldImage != null)
            {
                oldImage.Dispose();
            }
        }

        private void UpdateSelectedColor()
        {
            _selectedColor = ColorHSVConverter.ColorFromHSV(_hue, _saturation, _value);
            panelSelectedColor.BackColor = _selectedColor;
        }

        private bool _hueSaturationPicker_lmbDown = false;

        private void hueSaturationPicker_MouseDown(object sender, MouseEventArgs e)
        {
            _hueSaturationPicker_lmbDown = true;
        }

        private void hueSaturationPicker_MouseUp(object sender, MouseEventArgs e)
        {
            _hueSaturationPicker_lmbDown = false;
        }

        private void hueSaturationPicker_MouseMove(object sender, MouseEventArgs e)
        {
            if (_hueSaturationPicker_lmbDown)
            {
                double hue, saturation;
                ColorHelper.GetHueAndSaturationOfPixelCoords(e.X, e.Y, out hue, out saturation);

                _hue = hue;
                _saturation = saturation;

                UpdateSelectedColor();
                RedrawValuePicker();
            }
        }

        private bool _valuePicker_lmbDown = false;

        private void valuePicker_MouseDown(object sender, MouseEventArgs e)
        {
            _valuePicker_lmbDown = true;
        }

        private void valuePicker_MouseUp(object sender, MouseEventArgs e)
        {
            _valuePicker_lmbDown = false;
        }

        private void valuePicker_MouseMove(object sender, MouseEventArgs e)
        {
            if (_valuePicker_lmbDown)
            {
                _value = (e.X / (double)valuePicker.Width) * 100;
                
                if (_value < 0) { _value = 0; }
                if (_value > 100) { _value = 100; }

                UpdateSelectedColor();
                RedrawValuePicker();
                RedrawColorDesigner();
            }
        }

        public static Vector3 ColorToVector(Color colorToConvert)
        {
            return new Vector3(
                colorToConvert.R,
                colorToConvert.G,
                colorToConvert.B);
        }

        public static Color VectorToColor(Vector3 vectorToConvert)
        {
            return Color.FromArgb(
                (int)vectorToConvert.X,
                (int)vectorToConvert.Y,
                (int)vectorToConvert.Z);
        }
    }
}
