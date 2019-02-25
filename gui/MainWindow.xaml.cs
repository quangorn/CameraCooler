using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace CameraCoolerGUI
{
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        private const double TEMP_FACTOR = 100.0;

        public event PropertyChangedEventHandler PropertyChanged;
        private Device device = new Device();
        private DispatcherTimer updateTimer;

        private Settings settings;
        public double SettingsTargetTemp { get => settings.targetTemp / TEMP_FACTOR; set => settings.targetTemp = (short)(value * TEMP_FACTOR); }
        public double SettingsDewPointUnsafeZone { get => settings.dewPointUnsafeZone / TEMP_FACTOR; set => settings.dewPointUnsafeZone = (short)(value * TEMP_FACTOR); }
        public ushort SettingsBalanceResistor { get => settings.balanceResistor; set => settings.balanceResistor = value; }
        public short SettingsPFactor { get => settings.pFactor; set => settings.pFactor = value; }
        public short SettingsIFactor { get => settings.iFactor; set => settings.iFactor = value; }
        public short SettingsDFactor { get => settings.dFactor; set => settings.dFactor = value; }

        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;

            ConnectIfNeeded();
            InitializeTimer();
        }

        private bool ReadSettings()
        {
            Result<Settings> readSettingsResult = device.ReadSettings();
            if (readSettingsResult.IsNotOk())
            {
                StatusText.Text = readSettingsResult.GetErrorMessage();
                return false;
            }
            settings = readSettingsResult.GetResultObject();
            OnPropertyChanged(new PropertyChangedEventArgs(null)); //all properties changed
            StatusText.Text = "Read settings complete";
            return true;
        }

        private bool ReadCoolerState()
        {
            Result<bool> result = device.ReadCoolerState();
            if (result.IsNotOk())
            {
                StatusText.Text = result.GetErrorMessage();
                return false;
            }
            CoolerStateCB.IsChecked = result.GetResultObject();
            if (CoolerStateCB.IsChecked.Value)
            {
                CoolerStateCB.Content = "On";
            }
            else
            {
                CoolerStateCB.Content = "Off";
            }
            StatusText.Text = "Read cooler state complete";
            return true;
        }

        private bool ConnectIfNeeded()
        {
            if (device.IsConnected())
                return true;

            Result<bool> connectResult = device.TryConnect();
            if (connectResult.IsNotOk())
            {
                StatusText.Text = connectResult.GetErrorMessage();
                return false;
            }
            return ReadSettings() && ReadCoolerState();
        }

        private bool WriteSettings()
        {
            Result<bool> result = device.WriteSettings(settings);
            if (result.IsNotOk())
            {
                StatusText.Text = result.GetErrorMessage();
                return false;
            }
            StatusText.Text = "Write settings complete";
            return ReadSettings();
        }

        private void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, e);
            }
        }

        private void InitializeTimer()
        {
            updateTimer = new DispatcherTimer();
            updateTimer.Tick += new EventHandler(UpdateTimer_Tick);
            updateTimer.Interval = new TimeSpan(0, 0, 1);
            updateTimer.Start();
        }

        private void UpdateTimer_Tick(object sender, EventArgs e)
        {
            if (!ConnectIfNeeded())
                return;

            Result<RealtimeInfo> riResult = device.ReadRealtimeInfo();
            if (riResult.IsNotOk())
            {
                StatusText.Text = riResult.GetErrorMessage();
                return;
            }
            RealtimeInfo ri = riResult.GetResultObject();
            StatusText.Text = "OK";
            ChipTempText.Text = FormatTemp(ri.chipTemp);
            CaseTempText.Text = FormatTemp(ri.caseTemp);
            CaseHumidityText.Text = FormatTemp(ri.caseHumidity);
            DewPointText.Text = FormatTemp(ri.dewPoint);
            TargetTempText.Text = FormatTemp(ri.targetTemp);
            CoolerPowerText.Text = String.Format("{0:0.0}", ri.coolerPower / 2.55);
        }

        private static string FormatTemp(int temp)
        {
            double value = temp / TEMP_FACTOR;
            return String.Format("{0}", value);
        }

        private void SettingsTargetTempWriteButton_Click(object sender, RoutedEventArgs e)
        {
            OnWriteButton();
        }

        private void SettingsWriteButton_Click(object sender, RoutedEventArgs e)
        {
            OnWriteButton();
        }

        private void OnWriteButton()
        {
            if (WriteSettings())
            {
                ReadSettings();
            }
        }

        private void CoolerStateCB_Click(object sender, RoutedEventArgs e)
        {
            CheckBox cb = (CheckBox)sender;
            Result<bool> result = device.WriteCoolerState(cb.IsChecked.Value);
            if (result.IsNotOk())
            {
                StatusText.Text = result.GetErrorMessage();
                return;
            }
            StatusText.Text = "Write cooler state complete";
            ReadCoolerState();
        }
    }
}
