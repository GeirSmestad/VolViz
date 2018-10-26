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
            this.SuspendLayout();
            // 
            // comboBoxRenderMode
            // 
            this.comboBoxRenderMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxRenderMode.FormattingEnabled = true;
            this.comboBoxRenderMode.Location = new System.Drawing.Point(45, 143);
            this.comboBoxRenderMode.Name = "comboBoxRenderMode";
            this.comboBoxRenderMode.Size = new System.Drawing.Size(193, 21);
            this.comboBoxRenderMode.TabIndex = 0;
            this.comboBoxRenderMode.SelectedIndexChanged += new System.EventHandler(this.comboBoxRenderMode_SelectedIndexChanged);
            // 
            // ConfigurationEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(402, 265);
            this.Controls.Add(this.comboBoxRenderMode);
            this.Name = "ConfigurationEditor";
            this.Text = "Edit render configuration";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox comboBoxRenderMode;
    }
}