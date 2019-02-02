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
        public event PropertyChangedEventHandler PropertyChanged;
        private Device device = new Device();
        private DispatcherTimer updateTimer;

        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;

            Result<bool> connectResult = device.TryConnect();
            if (connectResult.IsNotOk())
            {
                StatusText.Text = connectResult.GetErrorMessage();
                return;
            }

            InitializeTimer();
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
            TargetTempText.Text = FormatTemp(ri.targetTemp);
        }

        private static string FormatTemp(int temp)
        {
            double value = temp / 100.0;
            return String.Format("{0}", value);
        }
    }
}
