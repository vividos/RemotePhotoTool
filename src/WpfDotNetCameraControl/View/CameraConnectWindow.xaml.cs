using System.Windows;
using WpfDotNetCameraControl.ViewModel;

namespace WpfDotNetCameraControl.View
{
    /// <summary>
    /// Interaction logic for CameraConnectWindow.xaml
    /// </summary>
    public partial class CameraConnectWindow : Window
    {
        /// <summary>
        /// The view model of the camera connect window
        /// </summary>
        public CameraConnectViewModel ViewModel { get; set; }

        /// <summary>
        /// Constructs a new camera connect window
        /// </summary>
        public CameraConnectWindow()
        {
            this.ViewModel = new CameraConnectViewModel();
            this.DataContext = this.ViewModel;

            this.InitializeComponent();
        }

        /// <summary>
        /// Called when user clicked on the open camera button.
        /// </summary>
        /// <param name="sender">sender object</param>
        /// <param name="args">event args</param>
        private void OnClickedButtonOpenCamera(object sender, RoutedEventArgs args)
        {
            // TODO implement
        }

        /// <summary>
        /// Called when user clicked on the exit button.
        /// </summary>
        /// <param name="sender">sender object</param>
        /// <param name="args">event args</param>
        private void OnClickedButtonExit(object sender, RoutedEventArgs args)
        {
            this.Close();
        }
    }
}
