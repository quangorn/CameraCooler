using HidLibrary;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CameraCoolerGUI
{
    class Device
    {
        private const int VendorId = 1155;
        private const int ProductId = 22356;
        private static HidDevice device;
        private static bool attached;
        private static byte[] outData;

        public void Disconnect()
        {
            if (attached)
            {
                device.CloseDevice();
                attached = false;
            }
        }

        public Result<bool> TryConnect()
        {
            if (!attached)
            {
                device = HidDevices.Enumerate(VendorId, ProductId).FirstOrDefault();
                if (device == null)
                    return Result<bool>.Error("Could not find a device");

                device.OpenDevice();
                outData = new byte[device.Capabilities.FeatureReportByteLength - 1];
                attached = true;
            }
            return Result<bool>.Ok(true);
        }

        public Result<bool> WriteSettings(Settings settings)
        {
            if (!attached)
                return Result<bool>.Error("Device is not connected");

            settings.ToByteArray(outData);
            if (!device.WriteFeatureData(outData))
                return Result<bool>.Error("Write failed");

            return Result<bool>.Ok(true);
        }
    }
}
