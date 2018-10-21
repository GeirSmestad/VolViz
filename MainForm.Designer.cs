namespace VolViz
{
    partial class MainForm
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
            this.buttonEditTransferFunction = new System.Windows.Forms.Button();
            this.canvas = new System.Windows.Forms.PictureBox();
            this.buttonLoadVolume = new System.Windows.Forms.Button();
            this.buttonLoadTestVolume = new System.Windows.Forms.Button();
            this.labelViewportState = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.canvas)).BeginInit();
            this.SuspendLayout();
            // 
            // buttonEditTransferFunction
            // 
            this.buttonEditTransferFunction.Location = new System.Drawing.Point(55, 587);
            this.buttonEditTransferFunction.Name = "buttonEditTransferFunction";
            this.buttonEditTransferFunction.Size = new System.Drawing.Size(178, 23);
            this.buttonEditTransferFunction.TabIndex = 0;
            this.buttonEditTransferFunction.Text = "Edit transfer function";
            this.buttonEditTransferFunction.UseVisualStyleBackColor = true;
            this.buttonEditTransferFunction.Click += new System.EventHandler(this.button1_Click);
            // 
            // canvas
            // 
            this.canvas.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.canvas.Location = new System.Drawing.Point(55, 47);
            this.canvas.Name = "canvas";
            this.canvas.Size = new System.Drawing.Size(512, 512);
            this.canvas.TabIndex = 1;
            this.canvas.TabStop = false;
            this.canvas.MouseDown += new System.Windows.Forms.MouseEventHandler(this.canvas_MouseDown);
            this.canvas.MouseLeave += new System.EventHandler(this.canvas_MouseLeave);
            this.canvas.MouseMove += new System.Windows.Forms.MouseEventHandler(this.canvas_MouseMove);
            this.canvas.MouseUp += new System.Windows.Forms.MouseEventHandler(this.canvas_MouseUp);
            // 
            // buttonLoadVolume
            // 
            this.buttonLoadVolume.Location = new System.Drawing.Point(268, 587);
            this.buttonLoadVolume.Name = "buttonLoadVolume";
            this.buttonLoadVolume.Size = new System.Drawing.Size(158, 23);
            this.buttonLoadVolume.TabIndex = 3;
            this.buttonLoadVolume.Text = "Load volume";
            this.buttonLoadVolume.UseVisualStyleBackColor = true;
            this.buttonLoadVolume.Click += new System.EventHandler(this.button2_Click);
            // 
            // buttonLoadTestVolume
            // 
            this.buttonLoadTestVolume.Location = new System.Drawing.Point(268, 624);
            this.buttonLoadTestVolume.Name = "buttonLoadTestVolume";
            this.buttonLoadTestVolume.Size = new System.Drawing.Size(158, 23);
            this.buttonLoadTestVolume.TabIndex = 4;
            this.buttonLoadTestVolume.Text = "Load test cube volume";
            this.buttonLoadTestVolume.UseVisualStyleBackColor = true;
            this.buttonLoadTestVolume.Click += new System.EventHandler(this.button3_Click);
            // 
            // labelViewportState
            // 
            this.labelViewportState.AutoSize = true;
            this.labelViewportState.Location = new System.Drawing.Point(12, 674);
            this.labelViewportState.Name = "labelViewportState";
            this.labelViewportState.Size = new System.Drawing.Size(121, 13);
            this.labelViewportState.TabIndex = 5;
            this.labelViewportState.Text = "Viewport state (pending)";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(623, 699);
            this.Controls.Add(this.labelViewportState);
            this.Controls.Add(this.buttonLoadTestVolume);
            this.Controls.Add(this.buttonLoadVolume);
            this.Controls.Add(this.canvas);
            this.Controls.Add(this.buttonEditTransferFunction);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "MainForm";
            this.Text = "Volume visualization";
            ((System.ComponentModel.ISupportInitialize)(this.canvas)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonEditTransferFunction;
        private System.Windows.Forms.PictureBox canvas;
        private System.Windows.Forms.Button buttonLoadVolume;
        private System.Windows.Forms.Button buttonLoadTestVolume;
        private System.Windows.Forms.Label labelViewportState;
    }
}

