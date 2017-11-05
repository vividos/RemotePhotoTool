using RemotePhotoTool.CameraControl;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Input;
using WpfDotNetCameraControl.Helper;
using WpfDotNetCameraControl.Views;

namespace WpfDotNetCameraControl.ViewModels
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

                this.OnPropertyChanged(nameof(this.CameraList));
                this.OnPropertyChanged(nameof(this.IsEnabledOpenCameraButton));
            }
        }

        /// <summary>
        /// Property that stores the currently selected camera
        /// </summary>
        public SourceInfo SelectedCamera { get; set; }

        /// <summary>
        /// Returns if the camera open button is enabled
        /// </summary>
        public bool IsEnabledOpenCameraButton
        {
            get { return this.cameraList.Count > 0; }
        }

        /// <summary>
        /// Command to execute when open camera button has been pressed
        /// </summary>
        public ICommand OpenCameraCommand { get; private set; }

        #region INotifyPropertyChanged implementation
        /// <summary>
        /// Event that is triggered when a property has changed
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

        /// <summary>
        /// Constructs a new view model object; starts waiting for cameras to connect.
        /// </summary>
        public CameraConnectViewModel()
        {
            this.instance = (App.Current as App).Instance;

            var sourceInfoList = this.instance.EnumerateDevices();
            this.CameraList = new ObservableCollection<SourceInfo>(sourceInfoList);

            this.OpenCameraCommand = new CommandHandler(this.OpenCamera);

            this.instance.AsyncWaitForCamera(this.OnCameraAdded);
        }

        /// <summary>
        /// Called when the Open button is clicked; opens the camera and shows the remote control
        /// window.
        /// </summary>
        private void OpenCamera()
        {
            if (this.SelectedCamera == null)
            {
                return;
            }

            this.instance.AsyncWaitForCamera();

            var sourceDevice = this.SelectedCamera.Open();

            var navigationService = (App.Current as App).NavigationService;
            navigationService.NavigateTo(NavigationService.PageRemoteControl);
        }

        /// <summary>
        /// Handler that is called when camera list has changed.
        /// </summary>
        private void OnCameraAdded()
        {
            var sourceInfoList = this.instance.EnumerateDevices();
            this.CameraList = new ObservableCollection<SourceInfo>(sourceInfoList);
        }

        #region IDisposable implementation
        /// <summary>
        /// Cleans up view model; unregisters camera connect handler.
        /// </summary>
        public void Dispose()
        {
            if (this.instance != null)
            {
                this.instance.AsyncWaitForCamera();

                this.instance.Dispose();
                this.instance = null;
            }
        }
        #endregion
    }
}
