using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace VolViz.Data
{
    public class TransferFunction
    {
        public List<TfNode> Nodes { get; set; }

        private Dictionary<float, Vector3> _colorLookupTable;
        private Dictionary<float, float> _opacityLookupTable;

        private const float lookupTablePrecision = 0.01f;

        public TransferFunction()
        {
            Nodes = new List<TfNode>()
            {
                new TfNode(0, 0, new Vector3(0, 0, 0)),
                new TfNode(1, 1, new Vector3(255, 255, 255))
            };

            RecalculateLookupTables();
        }

        public Vector3 GetColorOfIntensity(float voxelValue, bool useLookupTable = true)
        {
            if (useLookupTable)
            {
                return _colorLookupTable[(float)Math.Round(voxelValue, 2)];
            }

            TfNode lowerNode, higherNode;
            GetNeighboringNodes(voxelValue, out lowerNode, out higherNode);

            if (higherNode.InputValue == lowerNode.InputValue)
            {
                return higherNode.OutputColor;
            }

            float red = LinearInterpolate(voxelValue,
                lowerNode.InputValue, lowerNode.OutputColor.X,
                higherNode.InputValue, higherNode.OutputColor.X);

            float green = LinearInterpolate(voxelValue,
                lowerNode.InputValue, lowerNode.OutputColor.Y,
                higherNode.InputValue, higherNode.OutputColor.Y);

            float blue = LinearInterpolate(voxelValue,
                lowerNode.InputValue, lowerNode.OutputColor.Z,
                higherNode.InputValue, higherNode.OutputColor.Z);
            
            return new Vector3(red/255f, green/255f, blue/255f);
        }
        
        public float GetOpacityOfIntensity(float voxelValue, bool useLookupTable = true)
        {
            if (useLookupTable)
            {
                return _opacityLookupTable[(float)Math.Round(voxelValue, 2)];
            }

            TfNode lowerNode, higherNode;
            GetNeighboringNodes(voxelValue, out lowerNode, out higherNode);

            if (higherNode.InputValue == lowerNode.InputValue)
            {
                return higherNode.OutputOpacity;
            }
            
            float opacity = LinearInterpolate(voxelValue,
                lowerNode.InputValue, lowerNode.OutputOpacity,
                higherNode.InputValue, higherNode.OutputOpacity);
            
            return opacity;

        }

        public void AddNode(float inputValue, float outputOpacity, Vector3 outputColor)
        {
            var newNode = new TfNode(inputValue, outputOpacity, outputColor);
            Nodes.Add(newNode);
            SortListOfTfNodes();
        }

        public void RecalculateLookupTables()
        {
            _colorLookupTable = new Dictionary<float, Vector3>();
            _opacityLookupTable = new Dictionary<float, float>();

            for (decimal v = 0; v <= 1; v += (decimal)lookupTablePrecision)
            {
                var floatValue = (float)v;

                _colorLookupTable[floatValue] = GetColorOfIntensity(floatValue, useLookupTable: false);
                _opacityLookupTable[floatValue] = GetOpacityOfIntensity(floatValue, useLookupTable: false);
            }
        }

        public void SortListOfTfNodes()
        {
            Nodes.Sort((a, b) =>
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

        private void GetNeighboringNodes(float voxelValue, out TfNode lowerNode, out TfNode higherNode)
        {
            TfNode previousNode = Nodes[0];

            // Could use binary search or other strategy for efficiency if required
            for (int i = 1; i < Nodes.Count; i++)
            {
                var currentNode = Nodes[i];

                if (previousNode.InputValue <= voxelValue && currentNode.InputValue >= voxelValue)
                {
                    lowerNode = previousNode;
                    higherNode = currentNode;
                    return;
                }

                previousNode = currentNode;
            }

            throw new InvalidOperationException("You have asked for a point that is not between two TF nodes.");
        }

        private float LinearInterpolate(float x, float x0, float y0, float x1, float y1)
        {
            return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
        }
    }
}
