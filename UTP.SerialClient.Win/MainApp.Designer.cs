
namespace UTP.Gui
{
    partial class MainApp
    {
        /// <summary>
        /// Обязательная переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Требуемый метод для поддержки конструктора — не изменяйте 
        /// содержимое этого метода с помощью редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            this.gridSegmentsView = new System.Windows.Forms.DataGridView();
            this.SID = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.UID = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Address = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Size = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.CRC16 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.bReload = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.bUploadText = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.textData = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.textUid = new System.Windows.Forms.TextBox();
            this.bDelete = new System.Windows.Forms.Button();
            this.bDeleteAll = new System.Windows.Forms.Button();
            this.bExport = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.button1 = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.textFilePath = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.textFileUid = new System.Windows.Forms.TextBox();
            this.comboPorts = new System.Windows.Forms.ComboBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.bConnect = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.gridSegmentsView)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // logWriter
            // 
            this.logWriter.Location = new System.Drawing.Point(0, 309);
            this.logWriter.Size = new System.Drawing.Size(800, 132);
            // 
            // gridSegmentsView
            // 
            this.gridSegmentsView.AllowUserToAddRows = false;
            this.gridSegmentsView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.gridSegmentsView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.SID,
            this.UID,
            this.Address,
            this.Size,
            this.CRC16});
            this.gridSegmentsView.Location = new System.Drawing.Point(12, 27);
            this.gridSegmentsView.Name = "gridSegmentsView";
            this.gridSegmentsView.RowHeadersVisible = false;
            this.gridSegmentsView.Size = new System.Drawing.Size(492, 235);
            this.gridSegmentsView.TabIndex = 2;
            // 
            // SID
            // 
            this.SID.HeaderText = "ID";
            this.SID.Name = "SID";
            this.SID.ReadOnly = true;
            // 
            // UID
            // 
            this.UID.HeaderText = "UID";
            this.UID.Name = "UID";
            this.UID.ReadOnly = true;
            // 
            // Address
            // 
            this.Address.HeaderText = "Адрес";
            this.Address.Name = "Address";
            this.Address.ReadOnly = true;
            // 
            // Size
            // 
            this.Size.HeaderText = "Размер";
            this.Size.Name = "Size";
            this.Size.ReadOnly = true;
            // 
            // CRC16
            // 
            this.CRC16.HeaderText = "CRC16";
            this.CRC16.Name = "CRC16";
            this.CRC16.ReadOnly = true;
            // 
            // bReload
            // 
            this.bReload.Location = new System.Drawing.Point(12, 272);
            this.bReload.Name = "bReload";
            this.bReload.Size = new System.Drawing.Size(105, 23);
            this.bReload.TabIndex = 3;
            this.bReload.Text = "Обновить список";
            this.bReload.UseVisualStyleBackColor = true;
            this.bReload.Click += new System.EventHandler(this.bReload_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.bUploadText);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.textData);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.textUid);
            this.groupBox1.Location = new System.Drawing.Point(510, 90);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(278, 105);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Загрузка данных";
            // 
            // bUploadText
            // 
            this.bUploadText.Location = new System.Drawing.Point(9, 71);
            this.bUploadText.Name = "bUploadText";
            this.bUploadText.Size = new System.Drawing.Size(263, 23);
            this.bUploadText.TabIndex = 4;
            this.bUploadText.Text = "Загрузка";
            this.bUploadText.UseVisualStyleBackColor = true;
            this.bUploadText.Click += new System.EventHandler(this.bUploadText_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 48);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(51, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Данные:";
            // 
            // textData
            // 
            this.textData.Location = new System.Drawing.Point(63, 45);
            this.textData.Name = "textData";
            this.textData.Size = new System.Drawing.Size(209, 20);
            this.textData.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "UID:";
            // 
            // textUid
            // 
            this.textUid.Location = new System.Drawing.Point(63, 19);
            this.textUid.Name = "textUid";
            this.textUid.Size = new System.Drawing.Size(209, 20);
            this.textUid.TabIndex = 0;
            // 
            // bDelete
            // 
            this.bDelete.Location = new System.Drawing.Point(123, 272);
            this.bDelete.Name = "bDelete";
            this.bDelete.Size = new System.Drawing.Size(105, 23);
            this.bDelete.TabIndex = 5;
            this.bDelete.Text = "Удалить";
            this.bDelete.UseVisualStyleBackColor = true;
            this.bDelete.Click += new System.EventHandler(this.bDelete_Click);
            // 
            // bDeleteAll
            // 
            this.bDeleteAll.Location = new System.Drawing.Point(234, 272);
            this.bDeleteAll.Name = "bDeleteAll";
            this.bDeleteAll.Size = new System.Drawing.Size(105, 23);
            this.bDeleteAll.TabIndex = 6;
            this.bDeleteAll.Text = "Удалить всё";
            this.bDeleteAll.UseVisualStyleBackColor = true;
            this.bDeleteAll.Click += new System.EventHandler(this.bDeleteAll_Click);
            // 
            // bExport
            // 
            this.bExport.Location = new System.Drawing.Point(345, 272);
            this.bExport.Name = "bExport";
            this.bExport.Size = new System.Drawing.Size(159, 23);
            this.bExport.TabIndex = 7;
            this.bExport.Text = "Экспорт в файл";
            this.bExport.UseVisualStyleBackColor = true;
            this.bExport.Click += new System.EventHandler(this.bExport_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.button1);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.textFilePath);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.textFileUid);
            this.groupBox2.Location = new System.Drawing.Point(510, 201);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(278, 105);
            this.groupBox2.TabIndex = 5;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Загрузка файла";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(9, 71);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(263, 23);
            this.button1.TabIndex = 4;
            this.button1.Text = "Загрузка";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 48);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(36, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Файл";
            // 
            // textFilePath
            // 
            this.textFilePath.Location = new System.Drawing.Point(63, 45);
            this.textFilePath.Name = "textFilePath";
            this.textFilePath.ReadOnly = true;
            this.textFilePath.Size = new System.Drawing.Size(209, 20);
            this.textFilePath.TabIndex = 2;
            this.textFilePath.Click += new System.EventHandler(this.textFileClick);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 22);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(29, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "UID:";
            // 
            // textFileUid
            // 
            this.textFileUid.Location = new System.Drawing.Point(63, 19);
            this.textFileUid.Name = "textFileUid";
            this.textFileUid.Size = new System.Drawing.Size(209, 20);
            this.textFileUid.TabIndex = 0;
            this.textFileUid.Click += new System.EventHandler(this.textFileUid_ClickHandler);
            // 
            // comboPorts
            // 
            this.comboPorts.FormattingEnabled = true;
            this.comboPorts.Location = new System.Drawing.Point(9, 19);
            this.comboPorts.Name = "comboPorts";
            this.comboPorts.Size = new System.Drawing.Size(111, 21);
            this.comboPorts.TabIndex = 8;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.bConnect);
            this.groupBox3.Controls.Add(this.comboPorts);
            this.groupBox3.Location = new System.Drawing.Point(510, 28);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(278, 56);
            this.groupBox3.TabIndex = 9;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Соединение";
            // 
            // bConnect
            // 
            this.bConnect.Location = new System.Drawing.Point(126, 19);
            this.bConnect.Name = "bConnect";
            this.bConnect.Size = new System.Drawing.Size(146, 21);
            this.bConnect.TabIndex = 5;
            this.bConnect.Text = "Подключить";
            this.bConnect.UseVisualStyleBackColor = true;
            this.bConnect.Click += new System.EventHandler(this.bConnect_Click);
            // 
            // MainApp
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.bExport);
            this.Controls.Add(this.bDeleteAll);
            this.Controls.Add(this.bDelete);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.bReload);
            this.Controls.Add(this.gridSegmentsView);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainApp";
            this.Text = "UTP Client";
            this.Load += new System.EventHandler(this.MainApp_Load);
            this.Controls.SetChildIndex(this.logWriter, 0);
            this.Controls.SetChildIndex(this.gridSegmentsView, 0);
            this.Controls.SetChildIndex(this.bReload, 0);
            this.Controls.SetChildIndex(this.groupBox1, 0);
            this.Controls.SetChildIndex(this.bDelete, 0);
            this.Controls.SetChildIndex(this.bDeleteAll, 0);
            this.Controls.SetChildIndex(this.bExport, 0);
            this.Controls.SetChildIndex(this.groupBox2, 0);
            this.Controls.SetChildIndex(this.groupBox3, 0);
            ((System.ComponentModel.ISupportInitialize)(this.gridSegmentsView)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView gridSegmentsView;
        private System.Windows.Forms.DataGridViewTextBoxColumn SID;
        private System.Windows.Forms.DataGridViewTextBoxColumn UID;
        private System.Windows.Forms.DataGridViewTextBoxColumn Address;
        private System.Windows.Forms.DataGridViewTextBoxColumn Size;
        private System.Windows.Forms.DataGridViewTextBoxColumn CRC16;
        private System.Windows.Forms.Button bReload;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button bUploadText;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textData;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textUid;
        private System.Windows.Forms.Button bDelete;
        private System.Windows.Forms.Button bDeleteAll;
        private System.Windows.Forms.Button bExport;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textFilePath;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textFileUid;
        private System.Windows.Forms.ComboBox comboPorts;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button bConnect;
    }
}

