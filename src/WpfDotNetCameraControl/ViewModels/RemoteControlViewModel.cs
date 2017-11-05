using RemotePhotoTool.CameraControl;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using WpfDotNetCameraControl.Helper;

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
        /// Backing store for property IsViewfinderChecked
        /// </summary>
        private bool isViewfinderChecked;

        /// <summary>
        /// Viewfinder object
        /// </summary>
        private Viewfinder viewfinder;

        /// <summary>
        /// Property storing the camera name
        /// </summary>
        public string CameraName { get; private set; }

        /// <summary>
        /// Indicates if pressing the Release button is possible
        /// </summary>
        public bool IsReleasePossible { get; private set; }

        /// <summary>
        /// Command to execute when release shutter button has been pressed
        /// </summary>
        public ICommand ReleaseShutterCommand { get; private set; }

        /// <summary>
        /// List of currently available shooting modes
        /// </summary>
        public ObservableCollection<ImageProperty> ShootingModeList { get; private set; }

        /// <summary>
        /// List of currently available aperture values
        /// </summary>
        public ObservableCollection<ImageProperty> ApertureList { get; private set; }

        /// <summary>
        /// List of currently available shutter speed values
        /// </summary>
        public ObservableCollection<ImageProperty> ShutterSpeedList { get; private set; }

        /// <summary>
        /// List of currently available ISO speed values
        /// </summary>
        public ObservableCollection<ImageProperty> IsoSettingsList { get; private set; }

        /// <summary>
        /// Currently selected value for shooting mode
        /// </summary>
        public ImageProperty SelectedShootingMode { get; set; }

        /// <summary>
        /// Currently selected value for aperture
        /// </summary>
        public ImageProperty SelectedAperture { get; set; }

        /// <summary>
        /// Currently selected value for shutter speed
        /// </summary>
        public ImageProperty SelectedShutterSpeed { get; set; }

        /// <summary>
        /// Currently selected value for ISO speed
        /// </summary>
        public ImageProperty SelectedIsoSetting { get; set; }

        /// <summary>
        /// Indicates if shooting mode combobox is enabled
        /// </summary>
        public bool IsEnabledShootingMode { get; private set; }

        /// <summary>
        /// Indicates if aperture combobox is currently enabled
        /// </summary>
        public bool IsEnabledAv { get; private set; }

        /// <summary>
        /// Indicates if shutter speed combobox is currently enabled
        /// </summary>
        public bool IsEnabledTv { get; private set; }

        /// <summary>
        /// Indicates if ISO speed combobox is currently enabled
        /// </summary>
        public bool IsEnabledISO { get; private set; }

        /// <summary>
        /// Indicates if viewfinder is available and should be enabled
        /// </summary>
        public bool IsViewfinderEnabled { get; private set; }

        /// <summary>
        /// Indicates if viewfinder checkbox is currently checked
        /// </summary>
        public bool IsViewfinderChecked
        {
            get
            {
                return this.isViewfinderChecked;
            }

            set
            {
                this.isViewfinderChecked = value;
                this.EnableViewfinder(this.isViewfinderChecked);

                this.OnPropertyChanged(nameof(this.IsViewfinderChecked));
            }
        }

        /// <summary>
        /// Live viewfinder image to show
        /// </summary>
        public BitmapImage ViewfinderImage { get; private set; }

        /// <summary>
        /// Creates a new view model for the remote control view
        /// </summary>
        /// <param name="device">source device object</param>
        /// <param name="control">remote release control object</param>
        public RemoteControlViewModel(SourceDevice device, RemoteReleaseControl control)
        {
            this.device = device;
            this.control = control;

            this.SetupBindings();

            this.control.PropertyChanged += this.OnPropertyChanged;
            this.control.StateChanged += this.OnStateChanged;
            this.control.DownloadChanged += this.OnDownloadChanged;
        }

        /// <summary>
        /// Sets up all initial bindings values
        /// </summary>
        private void SetupBindings()
        {
            this.CameraName = this.device.ModelName;

            this.ReleaseShutterCommand = new CommandHandler(this.ReleaseShutter);

            this.IsEnabledShootingMode =
                this.control.GetCapability(RemoteReleaseControl.RemoteCapability.ChangeShootingMode);

            var currentShootingMode = this.control.GetImagePropertyByType(ImagePropertyType.ShootingMode);

            this.ShootingModeList = new ObservableCollection<ImageProperty>(currentShootingMode.ValidValues);
            this.SelectedShootingMode = currentShootingMode;

            this.IsEnabledAv = false;
            this.IsEnabledTv = false;
            this.IsEnabledISO = true;

            this.IsViewfinderEnabled =
                this.device.GetDeviceCapability(SourceDevice.DeviceCapability.RemoteViewfinder);

            this.OnShootingModeChanged();
        }

        /// <summary>
        /// Enables or disables viewfinder
        /// </summary>
        /// <param name="enableViewfinder">true to enable viewfinder, false to disable</param>
        private void EnableViewfinder(bool enableViewfinder)
        {
            if (enableViewfinder)
            {
                this.viewfinder = this.control.StartViewfinder();

                if (this.viewfinder.GetCapability(Viewfinder.ViewfinderCapability.OutputTypeVideoOut))
                {
                    this.viewfinder.Output = Viewfinder.OutputType.Off;
                }

                this.viewfinder.AvailViewfinderImage += this.OnAvailViewfinderImage;
            }
            else
            {
                if (this.viewfinder != null)
                {
                    this.viewfinder.AvailViewfinderImage -= this.OnAvailViewfinderImage;

                    this.viewfinder.Close();
                    this.viewfinder.Dispose();
                    this.viewfinder = null;
                }
            }
        }

        /// <summary>
        /// Called on worker thread when new viewfinder image is available
        /// </summary>
        /// <param name="jpegImageData">JPEG image data</param>
        private void OnAvailViewfinderImage(byte[] jpegImageData)
        {
            Task.Factory.StartNew(() =>
            {
                MemoryStream stream = new MemoryStream(jpegImageData);
                BitmapImage image = new BitmapImage();
                image.BeginInit();
                image.StreamSource = stream;
                image.EndInit();

                Dispatcher.CurrentDispatcher.Invoke((Action)delegate
                {
                    this.ViewfinderImage = image;
                    this.OnPropertyChanged(nameof(this.ViewfinderImage));
                });
            });
        }

        #region OnPropertyChanged handler
        /// <summary>
        /// Called when a property has changed
        /// </summary>
        /// <param name="propertyEvent">property event</param>
        /// <param name="propertyId">property ID that has changed</param>
        private void OnPropertyChanged(RemoteReleaseControl.PropertyEvent propertyEvent, uint propertyId)
        {
            uint shootingModePropertyId =
                this.control.GetImagePropertyByType(ImagePropertyType.ShootingMode).Id;

            if (propertyEvent == RemoteReleaseControl.PropertyEvent.PropertyDescChanged &&
                propertyId == shootingModePropertyId)
            {
                this.OnShootingModeChanged();
            }
        }

        /// <summary>
        /// Called when shooting mode has changed
        /// </summary>
        private void OnShootingModeChanged()
        {
            this.UpdateComboBoxEnabledState();
            this.UpdateComboBoxListEntries();
            this.UpdateComboBoxSelectedValues();
        }

        /// <summary>
        /// Updates enabled state of settings comboboxes
        /// </summary>
        private void UpdateComboBoxEnabledState()
        {
            ImageProperty shootingMode = this.control.GetImagePropertyByType(ImagePropertyType.ShootingMode);

            bool isManual = shootingMode.Name ==
                this.control.GetShootingModeImageProperty(RemoteReleaseControl.ShootingMode.M).Name;

            bool isAv = shootingMode.Name ==
                this.control.GetShootingModeImageProperty(RemoteReleaseControl.ShootingMode.Av).Name;

            bool isTv = shootingMode.Name ==
                this.control.GetShootingModeImageProperty(RemoteReleaseControl.ShootingMode.Tv).Name;

            bool isProgram = shootingMode.Name ==
                this.control.GetShootingModeImageProperty(RemoteReleaseControl.ShootingMode.P).Name;

            this.IsEnabledAv = !(!isManual && (isTv || isProgram));
            this.IsEnabledTv = !(!isManual && (isAv || isProgram));

            this.OnPropertyChanged(nameof(this.IsEnabledAv));
            this.OnPropertyChanged(nameof(this.IsEnabledTv));
        }

        /// <summary>
        /// Updates combobox lists, e.g. when property description has changed
        /// </summary>
        private void UpdateComboBoxListEntries()
        {
            this.ApertureList = new ObservableCollection<ImageProperty>(
                this.control.GetImagePropertyByType(ImagePropertyType.Av).ValidValues);

            this.ShutterSpeedList = new ObservableCollection<ImageProperty>(
                this.control.GetImagePropertyByType(ImagePropertyType.Tv).ValidValues);

            this.IsoSettingsList = new ObservableCollection<ImageProperty>(
                this.control.GetImagePropertyByType(ImagePropertyType.ISOSpeed).ValidValues);

            this.OnPropertyChanged(nameof(this.ApertureList));
            this.OnPropertyChanged(nameof(this.ShutterSpeedList));
            this.OnPropertyChanged(nameof(this.IsoSettingsList));
        }

        /// <summary>
        /// Updates selected values of settings comboboxes
        /// </summary>
        private void UpdateComboBoxSelectedValues()
        {
            this.SelectedAperture = this.control.GetImagePropertyByType(ImagePropertyType.Av);
            this.SelectedShutterSpeed = this.control.GetImagePropertyByType(ImagePropertyType.Tv);
            this.SelectedIsoSetting = this.control.GetImagePropertyByType(ImagePropertyType.ISOSpeed);

            this.OnPropertyChanged(nameof(this.SelectedAperture));
            this.OnPropertyChanged(nameof(this.SelectedShutterSpeed));
            this.OnPropertyChanged(nameof(this.SelectedIsoSetting));
        }
        #endregion

        /// <summary>
        /// Called when the camera state has changed
        /// </summary>
        /// <param name="stateEvent">state event</param>
        /// <param name="stateId">state ID</param>
        private void OnStateChanged(RemoteReleaseControl.StateEvent stateEvent, uint stateId)
        {
            if (stateEvent == RemoteReleaseControl.StateEvent.CameraShutdown)
            {
                // TODO close window
            }
        }

        /// <summary>
        /// Called when a download changed event occured
        /// </summary>
        /// <param name="downloadEvent">download event</param>
        /// <param name="progressPercent">progress value in percent</param>
        private void OnDownloadChanged(RemoteReleaseControl.DownloadEvent downloadEvent, uint progressPercent)
        {
            // TODO visualize
        }

        /// <summary>
        /// Releases shutter to take a photo
        /// </summary>
        private void ReleaseShutter()
        {
            Task.Factory.StartNew(() =>
            {
                this.control.Release();
            });
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
                this.control.PropertyChanged -= this.OnPropertyChanged;
                this.control.StateChanged -= this.OnStateChanged;
                this.control.DownloadChanged -= this.OnDownloadChanged;

                this.control.Dispose();
                this.control = null;
            }
        }
        #endregion
    }
}
