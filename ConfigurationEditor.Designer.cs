namespace VolViz
{
    partial class ConfigurationEditor
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
            this.comboBoxRenderMode = new System.Windows.Forms.ComboBox();
            this.stepSizeSelector = new System.Windows.Forms.NumericUpDown();
            this.checkBoxTrilinearInterpolation = new System.Windows.Forms.CheckBox();
            this.checkBoxUseGradientsInTF = new System.Windows.Forms.CheckBox();
            this.checkBoxPhongShading = new System.Windows.Forms.CheckBox();
            this.renderModeSpecificControls = new System.Windows.Forms.TabControl();
            this.tabPageDvr = new System.Windows.Forms.TabPage();
            this.tabPageFirstHit = new System.Windows.Forms.TabPage();
            this.tabPageMip = new System.Windows.Forms.TabPage();
            this.tabPageAverage = new System.Windows.Forms.TabPage();
            ((System.ComponentModel.ISupportInitialize)(this.stepSizeSelector)).BeginInit();
            this.renderModeSpecificControls.SuspendLayout();
            this.SuspendLayout();
            // 
            // comboBoxRenderMode
            // 
            this.comboBoxRenderMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxRenderMode.FormattingEnabled = true;
            this.comboBoxRenderMode.Location = new System.Drawing.Point(42, 50);
            this.comboBoxRenderMode.Name = "comboBoxRenderMode";
            this.comboBoxRenderMode.Size = new System.Drawing.Size(193, 21);
            this.comboBoxRenderMode.TabIndex = 0;
            this.comboBoxRenderMode.SelectedIndexChanged += new System.EventHandler(this.comboBoxRenderMode_SelectedIndexChanged);
            // 
            // stepSizeSelector
            // 
            this.stepSizeSelector.DecimalPlaces = 1;
            this.stepSizeSelector.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.stepSizeSelector.Location = new System.Drawing.Point(97, 119);
            this.stepSizeSelector.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.stepSizeSelector.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.stepSizeSelector.Name = "stepSizeSelector";
            this.stepSizeSelector.Size = new System.Drawing.Size(120, 20);
            this.stepSizeSelector.TabIndex = 1;
            this.stepSizeSelector.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.stepSizeSelector.ValueChanged += new System.EventHandler(this.stepSizeSelector_ValueChanged);
            // 
            // checkBoxTrilinearInterpolation
            // 
            this.checkBoxTrilinearInterpolation.AutoSize = true;
            this.checkBoxTrilinearInterpolation.Location = new System.Drawing.Point(82, 167);
            this.checkBoxTrilinearInterpolation.Name = "checkBoxTrilinearInterpolation";
            this.checkBoxTrilinearInterpolation.Size = new System.Drawing.Size(173, 17);
            this.checkBoxTrilinearInterpolation.TabIndex = 2;
            this.checkBoxTrilinearInterpolation.Text = "Trilinear interpolation of dataset";
            this.checkBoxTrilinearInterpolation.UseVisualStyleBackColor = true;
            this.checkBoxTrilinearInterpolation.CheckedChanged += new System.EventHandler(this.checkBoxTrilinearInterpolation_CheckedChanged);
            // 
            // checkBoxUseGradientsInTF
            // 
            this.checkBoxUseGradientsInTF.AutoSize = true;
            this.checkBoxUseGradientsInTF.Location = new System.Drawing.Point(82, 190);
            this.checkBoxUseGradientsInTF.Name = "checkBoxUseGradientsInTF";
            this.checkBoxUseGradientsInTF.Size = new System.Drawing.Size(205, 17);
            this.checkBoxUseGradientsInTF.TabIndex = 3;
            this.checkBoxUseGradientsInTF.Text = "Use data gradients in transfer function";
            this.checkBoxUseGradientsInTF.UseVisualStyleBackColor = true;
            this.checkBoxUseGradientsInTF.CheckedChanged += new System.EventHandler(this.checkBoxUseGradientsInTF_CheckedChanged);
            // 
            // checkBoxPhongShading
            // 
            this.checkBoxPhongShading.AutoSize = true;
            this.checkBoxPhongShading.Location = new System.Drawing.Point(82, 214);
            this.checkBoxPhongShading.Name = "checkBoxPhongShading";
            this.checkBoxPhongShading.Size = new System.Drawing.Size(97, 17);
            this.checkBoxPhongShading.TabIndex = 4;
            this.checkBoxPhongShading.Text = "Phong shading";
            this.checkBoxPhongShading.UseVisualStyleBackColor = true;
            this.checkBoxPhongShading.CheckedChanged += new System.EventHandler(this.checkBoxPhongShading_CheckedChanged);
            // 
            // renderModeSpecificControls
            // 
            this.renderModeSpecificControls.Controls.Add(this.tabPageDvr);
            this.renderModeSpecificControls.Controls.Add(this.tabPageFirstHit);
            this.renderModeSpecificControls.Controls.Add(this.tabPageMip);
            this.renderModeSpecificControls.Controls.Add(this.tabPageAverage);
            this.renderModeSpecificControls.Location = new System.Drawing.Point(12, 315);
            this.renderModeSpecificControls.Name = "renderModeSpecificControls";
            this.renderModeSpecificControls.SelectedIndex = 0;
            this.renderModeSpecificControls.Size = new System.Drawing.Size(405, 138);
            this.renderModeSpecificControls.TabIndex = 5;
            // 
            // tabPageDvr
            // 
            this.tabPageDvr.Location = new System.Drawing.Point(4, 22);
            this.tabPageDvr.Name = "tabPageDvr";
            this.tabPageDvr.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageDvr.Size = new System.Drawing.Size(397, 112);
            this.tabPageDvr.TabIndex = 0;
            this.tabPageDvr.Text = "Dvr";
            this.tabPageDvr.UseVisualStyleBackColor = true;
            // 
            // tabPageFirstHit
            // 
            this.tabPageFirstHit.Location = new System.Drawing.Point(4, 22);
            this.tabPageFirstHit.Name = "tabPageFirstHit";
            this.tabPageFirstHit.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageFirstHit.Size = new System.Drawing.Size(397, 112);
            this.tabPageFirstHit.TabIndex = 1;
            this.tabPageFirstHit.Text = "FirstHit";
            this.tabPageFirstHit.UseVisualStyleBackColor = true;
            // 
            // tabPageMip
            // 
            this.tabPageMip.Location = new System.Drawing.Point(4, 22);
            this.tabPageMip.Name = "tabPageMip";
            this.tabPageMip.Size = new System.Drawing.Size(397, 112);
            this.tabPageMip.TabIndex = 2;
            this.tabPageMip.Text = "Mip";
            this.tabPageMip.UseVisualStyleBackColor = true;
            // 
            // tabPageAverage
            // 
            this.tabPageAverage.Location = new System.Drawing.Point(4, 22);
            this.tabPageAverage.Name = "tabPageAverage";
            this.tabPageAverage.Size = new System.Drawing.Size(397, 112);
            this.tabPageAverage.TabIndex = 3;
            this.tabPageAverage.Text = "Average";
            this.tabPageAverage.UseVisualStyleBackColor = true;
            // 
            // ConfigurationEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(447, 465);
            this.Controls.Add(this.renderModeSpecificControls);
            this.Controls.Add(this.checkBoxPhongShading);
            this.Controls.Add(this.checkBoxUseGradientsInTF);
            this.Controls.Add(this.checkBoxTrilinearInterpolation);
            this.Controls.Add(this.stepSizeSelector);
            this.Controls.Add(this.comboBoxRenderMode);
            this.Name = "ConfigurationEditor";
            this.Text = "Edit render configuration";
            ((System.ComponentModel.ISupportInitialize)(this.stepSizeSelector)).EndInit();
            this.renderModeSpecificControls.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox comboBoxRenderMode;
        private System.Windows.Forms.NumericUpDown stepSizeSelector;
        private System.Windows.Forms.CheckBox checkBoxTrilinearInterpolation;
        private System.Windows.Forms.CheckBox checkBoxUseGradientsInTF;
        private System.Windows.Forms.CheckBox checkBoxPhongShading;
        private System.Windows.Forms.TabControl renderModeSpecificControls;
        private System.Windows.Forms.TabPage tabPageDvr;
        private System.Windows.Forms.TabPage tabPageFirstHit;
        private System.Windows.Forms.TabPage tabPageMip;
        private System.Windows.Forms.TabPage tabPageAverage;
    }
}