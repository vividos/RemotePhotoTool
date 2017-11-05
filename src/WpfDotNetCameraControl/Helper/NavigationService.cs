using RemotePhotoTool.CameraControl;
using System.Diagnostics;
using System.Windows;
using WpfDotNetCameraControl.Views;

namespace WpfDotNetCameraControl.Helper
{
    /// <summary>
    /// Navigation service class; supports changing to different pages
    /// </summary>
    public class NavigationService
    {
        /// <summary>
        /// Page key for remote control window
        /// </summary>
        public const string PageRemoteControl = "PageRemoteControl";

        /// <summary>
        /// Contains currently active window
        /// </summary>
        public Window CurrentWindow { get; set; }

        /// <summary>
        /// Navigates to new page, using page key and optional parameter
        /// </summary>
        /// <param name="pageKey">page key of page to show</param>
        /// <param name="parameter">optional parameter for page</param>
        public void NavigateTo(string pageKey, object parameter = null)
        {
            Window newWindow = null;

            switch (pageKey)
            {
                case PageRemoteControl:
                    var sourceDevice = parameter as SourceDevice;
                    newWindow = new RemoteControlWindow(sourceDevice);
                    break;

                default:
                    Debug.Assert(false, "invalid page key");
                    break;
            }

            if (newWindow != null)
            {
                newWindow.Show();

                if (this.CurrentWindow != null)
                {
                    this.CurrentWindow.Close();
                }

                this.CurrentWindow = newWindow;
            }
        }
    }
}
