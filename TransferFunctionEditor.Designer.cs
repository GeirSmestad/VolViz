namespace VolViz
{
    partial class TransferFunctionEditor
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.colorDesigner = new System.Windows.Forms.PictureBox();
            this.colorDialog = new System.Windows.Forms.ColorDialog();
            this.label1 = new System.Windows.Forms.Label();
            this.buttonSave = new System.Windows.Forms.Button();
            this.buttonLoad = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.colorDesigner)).BeginInit();
            this.SuspendLayout();
            // 
            // colorDesigner
            // 
            this.colorDesigner.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.colorDesigner.Location = new System.Drawing.Point(8, 8);
            this.colorDesigner.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.colorDesigner.Name = "colorDesigner";
            this.colorDesigner.Size = new System.Drawing.Size(453, 303);
            this.colorDesigner.TabIndex = 0;
            this.colorDesigner.TabStop = false;
            this.colorDesigner.MouseDown += new System.Windows.Forms.MouseEventHandler(this.colorDesigner_MouseDown);
            this.colorDesigner.MouseLeave += new System.EventHandler(this.colorDesigner_MouseLeave);
            this.colorDesigner.MouseMove += new System.Windows.Forms.MouseEventHandler(this.colorDesigner_MouseMove);
            this.colorDesigner.MouseUp += new System.Windows.Forms.MouseEventHandler(this.colorDesigner_MouseUp);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(507, 66);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(113, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "todo: color picker here";
            // 
            // buttonSave
            // 
            this.buttonSave.Location = new System.Drawing.Point(8, 315);
            this.buttonSave.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.buttonSave.Name = "buttonSave";
            this.buttonSave.Size = new System.Drawing.Size(131, 32);
            this.buttonSave.TabIndex = 3;
            this.buttonSave.Text = "Save transfer function";
            this.buttonSave.UseVisualStyleBackColor = true;
            this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
            // 
            // buttonLoad
            // 
            this.buttonLoad.Location = new System.Drawing.Point(143, 315);
            this.buttonLoad.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.buttonLoad.Name = "buttonLoad";
            this.buttonLoad.Size = new System.Drawing.Size(131, 32);
            this.buttonLoad.TabIndex = 4;
            this.buttonLoad.Text = "Load transfer function";
            this.buttonLoad.UseVisualStyleBackColor = true;
            // 
            // TransferFunctionEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(643, 355);
            this.Controls.Add(this.buttonLoad);
            this.Controls.Add(this.buttonSave);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.colorDesigner);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.Name = "TransferFunctionEditor";
            this.Text = "Edit transfer function";
            ((System.ComponentModel.ISupportInitialize)(this.colorDesigner)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox colorDesigner;
        private System.Windows.Forms.ColorDialog colorDialog;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button buttonSave;
        private System.Windows.Forms.Button buttonLoad;
    }
}