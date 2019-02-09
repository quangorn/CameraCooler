using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CameraCoolerGUI
{
    struct Settings
    {
        public short targetTemp;

        public static Settings FromByteArray(byte[] array, int startIndex)
        {
            int index = startIndex;
            Settings s;
            s.targetTemp = BitConverter.ToInt16(array, index);
            index += 2;
            return s;
        }

        public void ToByteArray(byte[] array, int startIndex)
        {
            int index = startIndex;
            targetTemp.ToByteArray(array, index);
            index += 2;
        }
    }
}
