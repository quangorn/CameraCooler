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
        private const byte REPORT_ID_RUNTIME_INFO = 1;
        private const byte REPORT_ID_SETTINGS = 2;

        private const int VendorId = 1155;
        private const int ProductId = 22356;
        private static HidDevice device;
        private static bool attached;
        private static byte[] outData;

        //TODO: повесить коллбек на подключение устройства

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

            outData[0] = REPORT_ID_SETTINGS;
            settings.ToByteArray(outData, 1);
            if (!device.WriteFeatureData(outData))
                return Result<bool>.Error("Write failed");

            return Result<bool>.Ok(true);
        }

        public Result<Settings> ReadSettings()
        {
            if (!attached)
                return Result<Settings>.Error("Device is not connected");

            if (!device.ReadFeatureData(out outData, REPORT_ID_SETTINGS))
                return Result<Settings>.Error("Read settings failed");

            Settings s = Settings.FromByteArray(outData, 0);
            return Result<Settings>.Ok(s);
        }

        public Result<RealtimeInfo> ReadRealtimeInfo()
        {
            if (!attached)
                return Result<RealtimeInfo>.Error("Device is not connected");

            if (!device.ReadFeatureData(out outData, REPORT_ID_RUNTIME_INFO))
                return Result<RealtimeInfo>.Error("Read failed");
            
            RealtimeInfo ri = RealtimeInfo.FromByteArray(outData, 0);
            return Result<RealtimeInfo>.Ok(ri);
        }
    }
}
