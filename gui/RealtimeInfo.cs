using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CameraCoolerGUI
{
    struct RealtimeInfo
    {
        public short chipTemp;
        public short caseTemp;
        public ushort caseHumidity;
        public short targetTemp;

        public static RealtimeInfo FromByteArray(byte[] array, int startIndex)
        {
            int index = startIndex;
            RealtimeInfo ri;
            ri.chipTemp = BitConverter.ToInt16(array, index);
            index += 2;
            ri.caseTemp = BitConverter.ToInt16(array, index);
            index += 2;
            ri.caseHumidity = BitConverter.ToUInt16(array, index);
            index += 2;
            ri.targetTemp = BitConverter.ToInt16(array, index);
            index += 2;
            return ri;
        }
    }
}
