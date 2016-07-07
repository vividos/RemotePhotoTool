using RemotePhotoTool.CameraControl;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;

namespace WpfDotNetCameraControl.ViewModel
{
    /// <summary>
    /// View model for the Camera connect view
    /// </summary>
    public class CameraConnectViewModel : IDisposable, INotifyPropertyChanged
    {
        /// <summary>
        /// Camera control instance object
        /// </summary>
        private Instance instance;

        /// <summary>
        /// List of cameras currently connected
        /// </summary>
        private ObservableCollection<SourceInfo> cameraList;

        /// <summary>
        /// Property to provide list of cameras currently connected
        /// </summary>
        public ObservableCollection<SourceInfo> CameraList
        {
            get
            {
                return this.cameraList;
            }

            set
            {
                this.cameraList = value;

                if (this.PropertyChanged != null)
                {
                    this.PropertyChanged(this, new PropertyChangedEventArgs(nameof(this.CameraList)));
                    this.PropertyChanged(this, new PropertyChangedEventArgs(nameof(this.IsEnabledOpenCameraButton)));
                }
            }
        }

        /// <summary>
        /// Returns if the camera open button is enabled
        /// </summary>
        public bool IsEnabledOpenCameraButton
        {
            get { return this.cameraList.Count > 0; }
        }

        /// <summary>
        /// Event that is triggered when a property has changed
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Constructs a new view model object; starts waiting for cameras to connect.
        /// </summary>
        public CameraConnectViewModel()
        {
            this.instance = (App.Current as App).Instance;

            var sourceInfoList = this.instance.EnumerateDevices();
            this.CameraList = new ObservableCollection<SourceInfo>(sourceInfoList);

            this.instance.AsyncWaitForCamera(this.OnCameraAdded);
        }

        /// <summary>
        /// Cleans up view model; unregisters camera connect handler.
        /// </summary>
        public void Dispose()
        {
            this.instance.AsyncWaitForCamera();
        }

        /// <summary>
        /// Handler that is called when camera list has changed.
        /// </summary>
        private void OnCameraAdded()
        {
            var sourceInfoList = this.instance.EnumerateDevices();
            this.CameraList = new ObservableCollection<SourceInfo>(sourceInfoList);
        }
    }
}
