using System;
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

namespace VolViz
{
    public partial class TransferFunctionEditor : Form
    {
        private TransferFunction _transferFunction;
        private Action _transferFunctionUpdated;

        public TransferFunctionEditor(Action transferFunctionUpdated)
        {
            _transferFunction = new TransferFunction();

            _transferFunctionUpdated = transferFunctionUpdated;

            InitializeComponent();
        }

        public TransferFunction GetTransferFunction()
        {
            return _transferFunction;
        }

        private void buttonSave_Click(object sender, EventArgs e)
        {
            // Debug only, to test window interaction
            _transferFunction.counter += 1;
            _transferFunctionUpdated();
        }


        // TODO: Drawing & mouse detection logic can be extracted to helper class

        private bool _colorDesigner_lmbDown = false;
        private List<TfNode> _nodesOfCurrentTf = new List<TfNode>()
        {
            new TfNode(0, 0, new Vector3(0,0,0)),
            new TfNode(1, 1, new Vector3(1,1,1))
        };

        private int _indexOfSelectedNode = -1;
        private int _indexOfHilightedNode = -1;

        private const int nodeSize = 10; // Draw size of node in pixels

        private void colorDesigner_MouseDown(object sender, MouseEventArgs e)
        {
            int width = colorDesigner.Width;
            int height = colorDesigner.Height;

            if (e.Button == MouseButtons.Left)
            {
                _colorDesigner_lmbDown = true;

                // TODO: Currently adding new node here. Select OR add node, depending on X axis position
                var newNode = new TfNode(
                    e.X / (float)width,
                    invWinFormsY(e.Y, height) / height,
                    new Vector3(1, 1, 1));

                _nodesOfCurrentTf.Add(newNode);
                SortListOfTfNodes();

            }

            if (e.Button == MouseButtons.Right)
            {
                // TODO: If over a node, delete it
            }

            RedrawColorDesigner();
        }

        private void colorDesigner_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                _colorDesigner_lmbDown = false;
            }
        }

        private void colorDesigner_MouseLeave(object sender, EventArgs e)
        {
            _colorDesigner_lmbDown = false;

            RedrawColorDesigner();
        }

        private void colorDesigner_MouseMove(object sender, MouseEventArgs e)
        {
            if (_colorDesigner_lmbDown)
            {
                // Move currently selected node to next coordinates
            }
            else
            {
                _indexOfHilightedNode = GetIndexOfNodeAtPixelCoordinate(e.X);
                // If mouse over node, hilight slightly

                // Else, clear highlight
            }

            RedrawColorDesigner();
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
            float width = colorDesigner.Width;

            float min = x - nodeSize / 2f;
            float max = x + nodeSize / 2f;

            float minCoordinateInTfSpace = min / width;
            float maxCoordinateInTfSpace = max / width;

            // Can be more efficient with binary search, but likely not a problem
            for (int i = 0; i < _nodesOfCurrentTf.Count; i++)
            {
                var node = _nodesOfCurrentTf[i];
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

                // TODO: Draw histogram

                for (int i = 1; i < _nodesOfCurrentTf.Count; i++)
                {
                    var previousNode = _nodesOfCurrentTf[i - 1];
                    var currentNode = _nodesOfCurrentTf[i];

                    g.DrawLine(Pens.Black,
                        width * previousNode.InputValue, invCartesianY(height * previousNode.OutputOpacity, height),
                        width * currentNode.InputValue, invCartesianY(height * currentNode.OutputOpacity, height));
                }
                
                for (int i = 0; i < _nodesOfCurrentTf.Count; i++)
                {
                    var currentNode = _nodesOfCurrentTf[i];
                    var brush = Brushes.LightBlue;

                    if (_indexOfHilightedNode == i)
                    {
                        brush = Brushes.Yellow;
                    }

                    g.FillEllipse(brush,
                        width * currentNode.InputValue - nodeSize / 2f,
                        invCartesianY(height * currentNode.OutputOpacity, height) - nodeSize / 2f,
                        nodeSize,
                        nodeSize
                        );
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

        private void SortListOfTfNodes()
        {
            _nodesOfCurrentTf.Sort((a, b) =>
            {
                if (b.InputValue > a.InputValue)
                {
                    return -1;
                }
                if (a.InputValue > b.InputValue)
                {
                    return 1;
                }
                return 0;
            });
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
    }
}
