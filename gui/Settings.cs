using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CameraCoolerGUI
{
    public struct Settings
    {
        public short targetTemp;
        public short dewPointUnsafeZone;
        public ushort balanceResistor;
        public short pFactor;
        public short iFactor;
        public short dFactor;

        public static Settings FromByteArray(byte[] array, int startIndex)
        {
            int index = startIndex;
            Settings s;
            s.targetTemp = BitConverter.ToInt16(array, index);
            index += 2;
            s.dewPointUnsafeZone = BitConverter.ToInt16(array, index);
            index += 2;
            s.balanceResistor = BitConverter.ToUInt16(array, index);
            index += 2;
            s.pFactor = BitConverter.ToInt16(array, index);
            index += 2;
            s.iFactor = BitConverter.ToInt16(array, index);
            index += 2;
            s.dFactor = BitConverter.ToInt16(array, index);
            index += 2;
            return s;
        }

        public void ToByteArray(byte[] array, int startIndex)
        {
            int index = startIndex;
            targetTemp.ToByteArray(array, index);
            index += 2;
            dewPointUnsafeZone.ToByteArray(array, index);
            index += 2;
            balanceResistor.ToByteArray(array, index);
            index += 2;
            pFactor.ToByteArray(array, index);
            index += 2;
            iFactor.ToByteArray(array, index);
            index += 2;
            dFactor.ToByteArray(array, index);
            index += 2;
        }
    }
}
