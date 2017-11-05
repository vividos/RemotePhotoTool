using RemotePhotoTool.CameraControl;
using System.Windows;
using WpfDotNetCameraControl.ViewModels;

namespace WpfDotNetCameraControl.Views
{
    /// <summary>
    /// Remote control window
    /// </summary>
    public partial class RemoteControlWindow : Window
    {
        /// <summary>
        /// The view model for the remote control window
        /// </summary>
        public RemoteControlViewModel ViewModel { get; set; }

        /// <summary>
        /// Constructs a new remote control window
        /// </summary>
        /// <param name="sourceDevice">source device object</param>
        public RemoteControlWindow(SourceDevice sourceDevice)
        {
            RemoteReleaseControl control = sourceDevice.EnterReleaseControl();

            this.ViewModel = new RemoteControlViewModel(sourceDevice, control);
            this.DataContext = this.ViewModel;

            this.InitializeComponent();
        }
    }
}
