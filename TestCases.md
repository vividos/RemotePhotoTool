## Test Cases ##

This is a list of test cases for the RemotePhotoTool application. If you want to test a particular
camera model with the application, follow the test cases and do everything described. Report back
if something is not working, with detailed infos about camera model (the "info" text) and the
generated log file.

### Test Case 1: General application functions ###

1. Start the application. The main application window must appear, and the "Connect camera" dialog
   in front of it.

2. Click on the "Cancel" button. The "Connect camera" dialog must have been closed. All "Photo Mode"
   buttons must be disabled. The Viewfinder button must be disabled.

3. Click on the "Info" symbol on the top right of the menu band. The "About" dialog must appear,
   with infos about the application and the SDKs used. Close the dialog by clicking the "OK" button.

4. Click on the "Menu band" ribbon button. The application must now show a toolbar with the same
   buttons. Click on the "Menu band" toolbar button to switch back to the ribbon.

5. Click on the "Settings" button. The "Settings" dialog must appear with the settings of the
   application. The settings must be modifiable.

6. Enable the "Logging" checkbox and use the suggested path. At the next start of the application,
   a log file named "Log.txt" must have been created and filled with log entries.

7. Close the dialog with the "OK" button. All modified settings must have been saved and must
   appear when again opening the dialog again.

8. Click on the "Connect" button. The "Connect camera" dialog must appear again. Close the dialog.

9. Click on the "File" menu band entry. Select the "Exit" option. The application must close.

### Test Case 2: Connecting camera functions ###

1. Start the application. The main application window must appear, and the "Connect camera" dialog
   in front of it.

2. Connect your camera to the computer and turn it on. The camera must appear in the list of
   connected cameras after a short time.

3. Hit the refresh button. If you have turned off your camera in the meantime, the camera must
   disappear from the list of connected cameras.

4. Click on the "Info" button. The "Camera Info" dialog must appear, filled with infos about
   the camera.

5. Click the "Copy" button. The camera infos must have been copied onto the clipboard.

6. Close the Info dialog and click on the "OK" button. The "Normal Photo Mode" view must appear.
   All "Photo Mode" buttons must be enabled now. The "Viewfinder" button must be enabled now.

7. Change to other photo modes. The application window must show different elements to view
   camera infos or to control the camera.

8. Switch off the camera. The "Connect camera" dialog must appear again, and the "Photo Mode"
   view must have been closed. Turn back on the camera and reconnect.

9. Close the application. The application must ask for confirmation to close, since you are
   connected to a camera.

### Test Case 3: Viewfinder ###

1. Connect to the camera. The "Normal Photo Mode" view must appear.

2. Click on the "Viewfinder" button. A new resizable window to the right of the elements
   must appear. The live viewfinder image must be shown. The LCD screen on the back of the
   camera must show no live image anymore.

3. Click on the "Viewfinder" button again. The viewfinder window must disappear.
