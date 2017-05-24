using RemotePhotoTool.CameraControl;
using System;
using System.ComponentModel;

namespace WpfDotNetCameraControl.ViewModels
{
    /// <summary>
    /// View model for the Remote control view
    /// </summary>
    public class RemoteControlViewModel : IDisposable, INotifyPropertyChanged
    {
        /// <summary>
        /// Source device object
        /// </summary>
        private SourceDevice device;

        /// <summary>
        /// Remote release control object
        /// </summary>
        private RemoteReleaseControl control;

        /// <summary>
        /// Property storing the camera name
        /// </summary>
        public string CameraName { get; private set; }

        /// <summary>
        /// Creates a new view model for the remote control view
        /// </summary>
        /// <param name="device">source device object</param>
        /// <param name="control">remote release control object</param>
        public RemoteControlViewModel(SourceDevice device, RemoteReleaseControl control)
        {
            this.device = device;
            this.control = control;

            this.CameraName = this.device.ModelName;
        }

        #region INotifyPropertyChanged implementation
        /// <summary>
        /// Event that gets signaled when a property has changed
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Call this method to signal that a property has changed
        /// </summary>
        /// <param name="propertyName">property name; use C# 6 nameof() operator</param>
        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(
                    this,
                    new PropertyChangedEventArgs(propertyName));
            }
        }
        #endregion

        #region IDisposable implementation
        /// <summary>
        /// Cleans up view model; unregisters camera connect handler.
        /// </summary>
        public void Dispose()
        {
            if (this.control != null)
            {
                this.control.Dispose();
                this.control = null;
            }
        }
        #endregion
    }
}
