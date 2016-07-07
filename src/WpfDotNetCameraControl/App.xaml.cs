using RemotePhotoTool.CameraControl;
using System.Windows;

namespace WpfDotNetCameraControl
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        /// <summary>
        /// Camera control instance used in the application
        /// </summary>
        public Instance Instance { get; private set; }

        /// <summary>
        /// Constructs a new app object
        /// </summary>
        public App()
        {
            this.Instance = Instance.Get();
        }
    }
}
