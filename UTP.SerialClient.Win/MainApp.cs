using Bwl.Framework;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UTP.Gui
{
    public partial class MainApp : FormAppBase
    {
        SerialConnector _serial;
        private Random _rnd = new Random();
        public MainApp()
        {
            InitializeComponent();
        }

        private void MainApp_Load(object sender, EventArgs e)
        {
            var ports = System.IO.Ports.SerialPort.GetPortNames();
            comboPorts.Items.AddRange(ports);
            groupBox1.Enabled = false;
            groupBox2.Enabled = false;
        }

        private void bReload_Click(object sender, EventArgs e)
        {
            try
            {
                _logger.AddMessage("Попытка запроса данных о памяти");
                var result = _serial.RequestInfo();
                _logger.AddMessage("Segments count: " + result.SegmentsCount + " Free space: 0x" + result.AvailableMemory.ToString("X"));
                gridSegmentsView.Rows.Clear();
                for (UInt16 i = 0; i < result.SegmentsCount; i++)
                {
                    var segment = _serial.GetHeaderBySerial(i);
                    gridSegmentsView.Rows.Add(new string[] {i.ToString(),
                        segment.UID.ToString(),
                        "0x"+segment.Address.ToString("X"),
                        "0x"+segment.Size.ToString("X"),
                        "0x"+segment.CRC16.ToString("X")});

                }
                gridSegmentsView.Refresh();
            }
            catch (Exception ex)
            {
                _logger.AddError(ex.Message);
            }
        }

        private void bUploadText_Click(object sender, EventArgs e)
        {
            try
            {
                var uid = UInt16.Parse(textUid.Text);
                byte[] bytes = Encoding.ASCII.GetBytes(textData.Text);
                var response = _serial.CreateSpace(uid, SerialConnector.GetCrc16(bytes), (UInt16)bytes.Length);
                _logger.AddMessage("Address: 0x" + response.Address.ToString("X") + " " +
                        "0x" + response.Size.ToString("X") + " " +
                        "0x" + response.CRC16.ToString("X"));
                _serial.AppendData(bytes);
                _serial.SaveData();
            }
            catch (Exception ex)
            {
                _logger.AddError(ex.Message);
            }
        }

        private void bDelete_Click(object sender, EventArgs e)
        {
            try
            {
                var index = gridSegmentsView.CurrentCell.RowIndex;
                var uid = UInt16.Parse(gridSegmentsView.Rows[index].Cells[1].Value.ToString());
                _serial.EraseSegmentByUid(uid);
                _logger.AddMessage("Успешно удалено");
            }
            catch(Exception ex)
            {
                _logger.AddError(ex.Message);
            }
        }

        private void bDeleteAll_Click(object sender, EventArgs e)
        {
            try
            {
                _serial.EraseAll();
                _logger.AddMessage("Успешно удалено");
            }
            catch (Exception ex)
            {
                _logger.AddError(ex.Message);
            }
        }

        private void bExport_Click(object sender, EventArgs e)
        {
            try
            {
                var index = gridSegmentsView.CurrentCell.RowIndex;
                var uid = UInt16.Parse(gridSegmentsView.Rows[index].Cells[1].Value.ToString());
                var size = Convert.ToUInt32(gridSegmentsView.Rows[index].Cells[3].Value.ToString(), 16);
                var data = _serial.ReadSegment(uid, size);
                System.IO.Stream fileStream;

                SaveFileDialog saveFileDialog = new SaveFileDialog();
                if (saveFileDialog.ShowDialog() == DialogResult.OK)
                {
                    if ((fileStream = saveFileDialog.OpenFile()) != null)
                    {
                        fileStream.Write(data.ToArray(), 0, data.Count);
                        fileStream.Close();
                    }
                }
            }
            catch(Exception ex)
            {
                _logger.AddError(ex.Message);
            }
           
        }

        private void textFileClick(object sender, EventArgs e)
        {
            try
            {
                using (OpenFileDialog openFileDialog = new OpenFileDialog())
                {
                    openFileDialog.InitialDirectory = "c:\\";
                    openFileDialog.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
                    openFileDialog.FilterIndex = 2;
                    openFileDialog.RestoreDirectory = true;

                    if (openFileDialog.ShowDialog() == DialogResult.OK)
                    {
                        textFilePath.Text = openFileDialog.FileName;
                    }
                }
            }
            catch(Exception ex)
            {
                _logger.AddError(ex.Message);
            }
            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                FileStream fs = File.OpenRead(textFilePath.Text);
                using (MemoryStream ms = new MemoryStream())
                {
                    fs.CopyTo(ms);
                    var data = ms.ToArray();
                    var uid = UInt16.Parse(textFileUid.Text);
                    var response = _serial.CreateSpace(uid, SerialConnector.GetCrc16(data), (UInt16)data.Length);
                    _logger.AddMessage("Address: 0x" + response.Address.ToString("X") + " " +
                            "0x" + response.Size.ToString("X") + " " +
                            "0x" + response.CRC16.ToString("X"));
                    _serial.AppendData(data, 64);
                    _serial.SaveData();
                }
            }
            catch(Exception ex)
            {

            }
        }

        private void textFileUid_ClickHandler(object sender, EventArgs e)
        {
            textFileUid.Text = _rnd.Next(100, 65000).ToString();
        }

        private void bConnect_Click(object sender, EventArgs e)
        {
            try
            {
                var port = comboPorts.SelectedItem.ToString();
                _serial = new SerialConnector(port, 115200);
                groupBox3.Enabled = false;
                groupBox1.Enabled = true;
                groupBox2.Enabled = true;
            }
            catch (Exception ex)
            {
                _logger.AddError(ex.Message);
            }
        }
    }
}
