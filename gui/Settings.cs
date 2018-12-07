using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CameraCoolerGUI
{
    struct Settings
    {
        public int targetTemp;

        public static Settings FromByteArray(byte[] array)
        {
            Settings s;
            s.targetTemp = array[0];
            return s;
        }

        public void ToByteArray(byte[] array)
        {
            array[0] = (byte) targetTemp;
        }
    }
}
