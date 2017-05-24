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
        /// <param name="device">source device object</param>
        /// <param name="control">remote release control object</param>
        public RemoteControlWindow(SourceDevice device, RemoteReleaseControl control)
        {
            this.ViewModel = new RemoteControlViewModel(device, control);
            this.DataContext = this.ViewModel;

            this.InitializeComponent();
        }
    }
}
