using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CameraCoolerGUI
{
    public static class NumberExtension
    {
        public static void ToByteArray(this int source, byte[] destination, int offset)
        {
            if (destination == null)
                throw new ArgumentException("Destination array cannot be null");

            if (destination.Length < offset + 4)
                throw new ArgumentException("Not enough room in the destination array");

            destination[offset + 3] = (byte)(source >> 24);
            destination[offset + 2] = (byte)(source >> 16);
            destination[offset + 1] = (byte)(source >> 8);
            destination[offset] = (byte)source;
        }

        public static void ToByteArray(this short source, byte[] destination, int offset)
        {
            if (destination == null)
                throw new ArgumentException("Destination array cannot be null");

            if (destination.Length < offset + 2)
                throw new ArgumentException("Not enough room in the destination array");

            destination[offset + 1] = (byte)(source >> 8);
            destination[offset] = (byte)source;
        }

        public static void ToByteArray(this ushort source, byte[] destination, int offset)
        {
            if (destination == null)
                throw new ArgumentException("Destination array cannot be null");

            if (destination.Length < offset + 2)
                throw new ArgumentException("Not enough room in the destination array");

            destination[offset + 1] = (byte)(source >> 8);
            destination[offset] = (byte)source;
        }
    }
}
