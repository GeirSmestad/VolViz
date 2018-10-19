using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
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
    }
}
