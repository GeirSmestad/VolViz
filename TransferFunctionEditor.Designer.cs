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
            this.alphaDesigner = new System.Windows.Forms.PictureBox();
            this.label1 = new System.Windows.Forms.Label();
            this.buttonSave = new System.Windows.Forms.Button();
            this.buttonLoad = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.colorDesigner)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.alphaDesigner)).BeginInit();
            this.SuspendLayout();
            // 
            // colorDesigner
            // 
            this.colorDesigner.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.colorDesigner.Location = new System.Drawing.Point(12, 12);
            this.colorDesigner.Name = "colorDesigner";
            this.colorDesigner.Size = new System.Drawing.Size(680, 225);
            this.colorDesigner.TabIndex = 0;
            this.colorDesigner.TabStop = false;
            // 
            // alphaDesigner
            // 
            this.alphaDesigner.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.alphaDesigner.Location = new System.Drawing.Point(12, 243);
            this.alphaDesigner.Name = "alphaDesigner";
            this.alphaDesigner.Size = new System.Drawing.Size(680, 225);
            this.alphaDesigner.TabIndex = 1;
            this.alphaDesigner.TabStop = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(760, 101);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(165, 20);
            this.label1.TabIndex = 2;
            this.label1.Text = "todo: color picker here";
            // 
            // buttonSave
            // 
            this.buttonSave.Location = new System.Drawing.Point(12, 485);
            this.buttonSave.Name = "buttonSave";
            this.buttonSave.Size = new System.Drawing.Size(196, 49);
            this.buttonSave.TabIndex = 3;
            this.buttonSave.Text = "Save transfer function";
            this.buttonSave.UseVisualStyleBackColor = true;
            this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
            // 
            // buttonLoad
            // 
            this.buttonLoad.Location = new System.Drawing.Point(214, 485);
            this.buttonLoad.Name = "buttonLoad";
            this.buttonLoad.Size = new System.Drawing.Size(196, 49);
            this.buttonLoad.TabIndex = 4;
            this.buttonLoad.Text = "Load transfer function";
            this.buttonLoad.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(740, 345);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(170, 20);
            this.label2.TabIndex = 5;
            this.label2.Text = "todo: alpha picker here";
            // 
            // TransferFunctionEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(964, 546);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.buttonLoad);
            this.Controls.Add(this.buttonSave);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.alphaDesigner);
            this.Controls.Add(this.colorDesigner);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "TransferFunctionEditor";
            this.Text = "TransferFunctionEditor";
            ((System.ComponentModel.ISupportInitialize)(this.colorDesigner)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.alphaDesigner)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox colorDesigner;
        private System.Windows.Forms.ColorDialog colorDialog;
        private System.Windows.Forms.PictureBox alphaDesigner;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button buttonSave;
        private System.Windows.Forms.Button buttonLoad;
        private System.Windows.Forms.Label label2;
    }
}