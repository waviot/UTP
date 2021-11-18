using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UTP.Gui
{
    public class SerialConnector
    {
        private SerialPort _bus;
        public SerialConnector(string port, int baud)
        {
            _bus = new SerialPort(port, baud);
            _bus.Open();
        }

        static UInt32 DecodeUInt32(byte[] array, int offset)
        {
            return (UInt32)(array[offset] << 24 | array[offset+1] << 16 | array[offset+2] << 8 | array[offset+3] << 0);
        }

        static UInt16 DecodeUInt16(byte[] array, int offset)
        {
            return (UInt16)(array[offset + 0] << 8 | array[offset + 1] << 0);
        }
        public List<byte> Request(byte[] buffer)
        {
            var watch = System.Diagnostics.Stopwatch.StartNew();
            byte[] seq = { 0xAA, 1 };
            _bus.Write(seq, 0, 2);
            for(int i=0;i< buffer.Length;i++)
            {
                _bus.Write(buffer, i, 1);
                if (buffer[i] == 0xAA)
                {
                    seq[0] = 0;
                    _bus.Write(seq, 0, 1);
                }
            }
            seq[0] = 0xAA; seq[1] = 0x2;
            _bus.Write(seq, 0, 2);

            List<byte> response = new List<byte>();
            byte lastByte = 0;
            while (watch.ElapsedMilliseconds < 3000)
            {
                if (_bus.BytesToRead > 0)
                {
                    byte b = (byte)_bus.ReadByte();
                    if (lastByte == 0xAA)
                    {
                        if (b == 0) response.Add(0xAA);
                        if (b == 1) response.Clear();
                        if (b == 2) return response;
                    }
                    else
                    {
                        if (b != 0xAA) response.Add(b);
                    }
                    lastByte = b;
                }
                else
                {
                    System.Threading.Thread.Sleep(1);
                }              
            }
            throw new Exception("Request timeout");
        }

        public static UInt16 GetCrc16(byte[] v)
        {
            UInt16 crc = 0;
            foreach (var item in v)
            {
                crc = (UInt16)((crc >> 8) | (crc << 8));
                crc ^= item;
                crc ^= (UInt16)((crc & 0xff) >> 4);
                crc ^= (UInt16)(crc << 12);
                crc ^= (UInt16)((crc & 0xff) << 5);
            }
            return crc;
        }

        public CreateSpaceReponse CreateSpace(UInt16 uid, UInt16 crc, UInt32 size )
        {
            CreateSpaceReponse result = new CreateSpaceReponse();
            var list = new List<byte>();
            list.Add(0);
            list.Add((byte)(size >> 24));
            list.Add((byte)(size >> 16));
            list.Add((byte)(size >> 8));
            list.Add((byte)(size >> 0));
            list.Add((byte)(crc >> 8));
            list.Add((byte)(crc >> 0));
            list.Add((byte)(uid >> 8));
            list.Add((byte)(uid >> 0));
            var response = Request(list.ToArray());
            if(response.Count == 5)
            {
                var address = DecodeUInt32(response.ToArray(), 1);
                result.Address = address;
                result.Size = size;
                result.CRC16 = crc;
                return result;
            }
            else
            {
                if(response.Count == 2)
                {
                    if(response[1] == 1) throw new Exception("Недостатчное количество параметров в запросе");
                    if(response[1] == 2) throw new Exception("Недостаточно памяти на конечном устройстве");
                    if(response[1] == 3) throw new Exception("UID уже есть на конечном устройстве");
                    if(response[1] == 4) throw new Exception("Неизвестная ошибка");
                }
            }
            return result;
        }

        public void AppendData(byte[] data)
        {
            var list = new List<byte>();
            list.Add(1);
            list.AddRange(data);
            var response = Request(list.ToArray());
            if(response.Count == 2)
            {
                if (response[1] == 1) throw new Exception("Недостатчное количество параметров в запросе");
                if (response[1] == 2) throw new Exception("Недостаточно памяти на конечном устройстве");
                throw new Exception("Неизвестная ошибка");
            }
        }
       
        public void AppendData(byte[] data, int partSize)
        {
            var list = new List<byte>();
            foreach(var b in data)
            {
                list.Add(b);
                if(list.Count % partSize == 0)
                {
                    AppendData(list.ToArray());
                    list.Clear();
                }
            }
            if(list.Count != 0)
            {
                AppendData(list.ToArray());
            }
        }
        public UInt16 SaveData()
        {
            var list = new List<byte>();
            list.Add(2);
            var response = Request(list.ToArray());

            if (response.Count == 2)
            {
                if (response[1] == 1) throw new Exception("CRC16 сохраненнных данных не совпадает с ожидаемым значением");
                throw new Exception("Неизвестная ошибка"); 
            }

            UInt16 crc = 0;
            crc = (UInt16)(response.ElementAt(1) << 8 | response.ElementAt(2));
            return crc;
        }
      

       public FreeSpaceResponse RequestInfo()
        {
            FreeSpaceResponse result = new FreeSpaceResponse();
            var list = new List<byte>();
            list.Add(3);
            var response = Request(list.ToArray());
            if(response.Count != 9)
            {
                if(response.Count == 2)
                {
                    throw new Exception("Неизвестная ошибка запроса"); 
                }
            }
            result.SegmentsCount = DecodeUInt32(response.ToArray(), 1);
            result.AvailableMemory = DecodeUInt32(response.ToArray(), 5);
            return result;
        }

        private HeaderResponse GetHeader(byte type, UInt16 id) {

            var list = new List<byte>();
            list.Add(type);
            list.Add((byte)(id >> 8));
            list.Add((byte)(id >> 0));
            var response = Request(list.ToArray());

            HeaderResponse result = new HeaderResponse();
            if(response.Count == 2)
            {
                if(response[1] == 1) throw new Exception("Ошибка входных параметров");
                if(response[1] == 2) throw new Exception("Не найден заголовок данных");
            }
            result.UID = DecodeUInt16(response.ToArray(), 1);
            result.Address = DecodeUInt32(response.ToArray(), 3);
            result.Size = DecodeUInt32(response.ToArray(), 7);
            result.CRC16 = DecodeUInt16(response.ToArray(), 11);
            result.HeaderCRC16 = DecodeUInt16(response.ToArray(), 13);
            return result;
        }

        public HeaderResponse GetHeaderBySerial(UInt16 serialNumber)
        {
            return GetHeader(4, serialNumber);
        }
        public HeaderResponse GetHeaderByUid(UInt16 id)
        {
            return GetHeader(5, id);
        }

        public List<byte> ReadSegment(UInt16 uid, UInt32 length)
        {
            var list = new List<byte>();
            while(list.Count< length)
            {
                UInt32 size = (UInt32)(length - list.Count);
                if (size > 64) size = 64;
                list.AddRange(ReadSegment(uid, (UInt32)list.Count, size));
            }
            return list;
        }
        public List<byte> ReadSegment(UInt16 uid, UInt32 offset, UInt32 length)
        {
            var list = new List<byte>();
            list.Add(6);
            list.Add((byte)(uid >> 8));
            list.Add((byte)(uid >> 0));

            list.Add((byte)(offset >> 24));
            list.Add((byte)(offset >> 16));
            list.Add((byte)(offset >> 8));
            list.Add((byte)(offset >> 0));

            list.Add((byte)(length >> 24));
            list.Add((byte)(length >> 16));
            list.Add((byte)(length >> 8));
            list.Add((byte)(length >> 0));

            var response = Request(list.ToArray());

            if (response.Count == 2)
            {
                if (response[0] == 6)
                {
                    if (response[1] == 0x01U)
                    {
                        throw new Exception("Check if passed parameters are correct");
                    }
                    else if (response[1] == 0x02U)
                    {
                        throw new Exception("No headers found for this UID  ");
                    }
                    else if (response[1] == 0x03U)
                    {
                        throw new Exception("Data of specified size cannot be read at the specified address at the specified offset");
                    }
                    else if (response[1] == 0x04U)
                    {
                        throw new Exception("There was a problem reading at the HAL level ");
                    }
                }
            }
            response.RemoveAt(0);
            return response;
        }

        public void EraseSegmentByUid(UInt16 uid)
        {
            List<byte> request = new List<byte>();
            request.Add(7);
            request.Add((byte)(uid >> 8));
            request.Add((byte)(uid & 0xFF));
            var response = Request(request.ToArray());
            if (response.Count != 2)
            {
                throw new Exception("Can't erase segment");
            }
            else
            {
                if (response[1] == 1)
                {
                    throw new Exception("Invalid request parameters");
                }
                else if (response[1] == 2)
                {
                    throw new Exception("The requested memory segment was not found by UID ");
                }
            }
        }

        public void EraseAll()
        {
            List<byte> request = new List<byte>();
            request.Add(8);
            var response = Request(request.ToArray());
            if (response.Count != 2)
            {
                throw new Exception("Can't erase");
            }
            else
            {
                if (response[1] != 0)
                {
                    throw new Exception("Can't erase");
                }
            }
        }
    }
}
