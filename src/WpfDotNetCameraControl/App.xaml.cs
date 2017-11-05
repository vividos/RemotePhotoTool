using RemotePhotoTool.CameraControl;
using System.Windows;
using WpfDotNetCameraControl.Helper;

namespace WpfDotNetCameraControl
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        /// <summary>
        /// Navigation service for application
        /// </summary>
        public NavigationService NavigationService { get; private set; }

        /// <summary>
        /// Camera control instance used in the application
        /// </summary>
        public Instance Instance { get; private set; }

        /// <summary>
        /// Constructs a new app object
        /// </summary>
        public App()
        {
            this.NavigationService = new NavigationService();

            this.Instance = Instance.Get();
        }
    }
}
