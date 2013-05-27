#ifndef _CD_EVENT_H_
#define _CD_EVENT_H_

/****************************************************************************
 ****************************************************************************
 *             Canon Digital Camera SDK : Event IDs                         *
 ****************************************************************************
 ****************************************************************************/

/*---------------------------------------------------------------------------
               Camera Event Identifiers  : cdEventID
----------------------------------------------------------------------------*/

/*//////////  Event Code Masks  ////////////////////////////////////////////*/
#define cdEVENT_ISSPECIFIC_MASK		0x08000000L
#define cdEVENT_RESERVED_MASK		0x07F00000L
#define cdEVENT_SEVERITY_MASK		0x000F0000L
#define cdEVENT_EVENTID_MASK		0x0000FFFFL

/*//////////  Macros  //////////////////////////////////////////////////////*/
#define cdEVENT_ISSPECIFIC(event)	(0 != ((event)&(cdEVENT_ISSPECIFIC_MASK)))
#define cdEVENT_SEVERITY(event)		((event)&(cdEVENT_SEVERITY_MASK))
#define cdEVENT_RESERVEDID(event) 	((event)&(cdEVENT_RESERVED_MASK))
#define cdEVENT_EVENTID(event)		((event)&(cdEVENT_EVENTID_MASK))

/*//////////  Event Severity levels ////////////////////////////////////////*/
#define cdEVENT_SEVERITY_NONE         0x00000000L 
	/* nothing bad happened: this notification is just informational
	*/
#define cdEVENT_SEVERITY_WARNING	  0x00020000L 
	/* The camera is going to shut down, or some warning event happened.
	   More detail information is given in the event code.
	   If client does not know the event code, it should close device 
	   immediatly.
	*/
#define cdEVENT_SEVERITY_SHUTDOWN	  0x00040000L 
	/* the camera has shut down. The Event ID tells the reason of the shut 
	   down.			
	*/

/*//////////  Event Identifiers  ///////////////////////////////////////////*/
#define cdEVENT_BATTERY_LEVEL_CHANGED		0x0001L 
	/* event data tells current battery level 
	*/
	#define BATTERY_LEVEL_MASK					(0x0000000FL)

	#define BATTERY_LEVEL_NORMAL				(0x00000006L)
	#define BATTERY_LEVEL_WEAK					(0x00000004L)
	#define BATTERY_LEVEL_SAFETY_LOW			(0x00000003L)
	#define BATTERY_LEVEL_LB					(0x00000002L)

	#define BATTERY_STATUS_NORMAL				(0x00000006L)
	#define BATTERY_STATUS_WEAK					(0x00000004L)
	#define BATTERY_STATUS_SAFETY_LOW			(0x00000003L)

	#define BATTERY_KIND_MASK					(0xF0000000L)
	#define BATTERY_KIND_UNKNOWN				(0x00000000L)
	#define BATTERY_KIND_AC						(0x10000000L)
	#define BATTERY_KIND_LI						(0x20000000L)
	#define BATTERY_KIND_NIH					(0x30000000L)
	#define BATTERY_KIND_NICD					(0x40000000L)
	#define BATTERY_KIND_ALMN					(0x50000000L)
 
	/* Combining this with cdEVENT_SEVERITY_NONE indicates the notification is
	   just for information, no action is required.
	   Combining this with cdEVENT_SEVERITY_WARNING indicates that user should
	   close device immediately. 
	   Combining this with cdEVENT_SEVERITY_SHUTDOWN indicates that device has
	   shut down because of Low Battery 
	*/

#define cdEVENT_DIAL_CHANGE 				0x0002L 

#define cdEVENT_CF_GATE_CHANGED  			0x0003L 
	/* event data tells whether gate opened */
	#define cdEVENT_PARAM_CF_GATE_OPEN			(0x00000000L)
	/*#define cdEVENT_PARAM_CF_GATE_CLOSE			(0x00000001L)*/

#define cdEVENT_BATT_COVER_CHANGED 			0x0004L
	/* event data tells whether cover opened */
	#define cdEVENT_PARAM_BATT_COVER_OPEN		(0x00000000L)
	/*#define cdEVENT_PARAM_BATT_COVER_CLOSE		(0x00000001L)*/

#define cdEVENT_CONNECTION_TIME_OUT 		0x0005L 
	/* Combining this with cdEVENT_SEVERITY_SHUTDOWN tells that the camera has
	   already shut down because of the time out.
	   Combining this with cdEVENT_SEVERITY_WARNING tells that the camera is 
	   going to shut down.
	   The event data tells how long (in minutes) camera would remain waking up.
	*/

#define cdEVENT_CONNECTION_DISAPPEARED		0x000DL
	/* USB/IEEE1394 connection is disappeared
	*/

#define cdEVENT_UPDATE_CAMERA_CONDITIONS	0x000EL
	/* Update Camera conditions event
	*/
	#define cdEVENT_PARAM_FILE_CREATED					0x00000001L
	#define cdEVENT_PARAM_FILE_REMOVED					0x00000002L
	#define cdEVENT_PARAM_FILE_ATTRIBUTE_CHANGED		0x00000004L
	#define cdEVENT_PARAM_FORMATED						0x00000100L	
	
	/* PTP Camera event's flag*/
	#define cdEVENT_PARAM_PTP							0x00010000L	

#define cdEVENT_CHANGE_LOCK_STATUS			0x000FL
	/* Used as parameter
	*/
	#define cdEVENT_PARAM_DISPLAY_PLAY_OR_NENU			0x01000000L
	#define cdEVENT_PARAM_PROCESSING_REC_OR_PREPARE_REC	0x02000000L
	/* used as lower 3Byte for parameter
	*/
	#define cdEVENT_PARAM_LOCK_TYPE1					0x00000001L
	#define cdEVENT_PARAM_LOCK_TYPE2					0x00000002L
	#define cdEVENT_PARAM_LOCK_TYPE3					0x00000004L
	#define cdEVENT_PARAM_LOCK_TYPE4					0x00000008L
	#define cdEVENT_PARAM_LOCK_TYPE5					0x00000010L
	#define cdEVENT_PARAM_LOCK_TYPE6					0x00000020L

#define cdEVENT_UNRECOVERABLE_ERROR			0x0006L

#define cdEVENT_UNKNOWN_COMMAND_RECEIVED 	0x0007L

#define cdEVENT_RELEASE_PARAM_CHANGED       0x000BL


/*---------------------------------------------------------------------------
              Release control Event Identifiers  : cdReleaseEventID
----------------------------------------------------------------------------*/

#define cdRELEASE_EVENT_RESET_HW_ERROR		(10L)
		/* Notify hardware error.
		   The second parameter of this event specifies the kind of error as
		   follows.
		*/
		#define HW_ERROR_PARAM_CAPTURE_FAULT		(0x00000001) /* Capture faild */
		#define HW_ERROR_PARAM_LENSE_CLOSE			(0x00000002) /* Lens closed   */
		#define HW_ERROR_PARAM_CAPTURE_MODE_DEPTH	(0x00000003) /* DEPTH mode    */
		#define HW_ERROR_PARAM_SENSOR_CLEANING		(0x00000004) /* Cleaning      */
		#define HW_ERROR_PARAM_SILENT_OPERATION		(0x00000005) /* PF21          */

#define cdRELEASE_EVENT_CHANGED_BY_UI		(27L)
		/* Notify the capture parameter in the camera is changed by user 
		   operation to camera UI.
           The Host PC must get new capture parameter from the camera by 
		   using RELEASE_CONTROL_GET_RELEASE_PARAMS command when this 
		   notification received.
		*/

#define cdRELEASE_EVENT_RELEASE_START		(28L)
		/* Notify the camera starts the capture operation with asynchronous mode.
		*/

#define cdRESEASE_EVENT_RELEASE_COMPLETE	(29L)
		/* Notify the camera stop the capture operation with asynchronous mode.	
		   The second parameter of this event specifies the number of released image data.
		   Data type is cdUInt32.
		   SDK client have to execute CDGetReleasedData() this number of the times.
		*/
		
#define cdRELEASE_EVENT_CAM_RELEASE_ON		(30L)
		/* Notify the release SW is pushed on the camera by User.
		   Currently this notification is supported by new Powershot only.
		   When this notification is happened, the camera does not start 
		   the captre.
		   To capture the image, the host PC must execute the sequence to 
		   capture the image same as PC release operation nomaly.
		   (Do CDRelease() command.)		
		*/

#define cdRELEASE_EVENT_ROTATION_ANGLE_CHANGED (31L)
		/* Notify the rotation angle detected by auto rotation sensor has changed.
		   The second parameter of this event shows the angle.
				0: 0 or 180 degree
				1: 90 degrees
				3: 270 degrees
		*/

#define cdRELEASE_EVENT_ABORT_PC_EVF		(32L)
		/* Notify the viewfinder function became the use cancel. 
		*/

#define cdRELEASE_EVENT_ENABLE_PC_EVF		(33L)
		/* Notify the view finder function became applicable. 
		*/


/*
---------------------------------------------------------------
     Sample : cdEventCallbackFunction
---------------------------------------------------------------

cdUInt32 sampleEventCallbackFunction(
				cdEventID			EventID, 
				const void*			pData,		
				cdUInt32 			DataSize,	
				cdContext			Context )
{
	if(cdEVENT_ISSPECIFIC(EventID)){
		return cdOK; //ignore specific event 
	}

	if(cdEVENT_SEVERITY_NONE == cdEVENT_SEVERITY(EventID)){

		switch (cdEVENT_EVENTID(EventID)){
		case cdEVENT_BATTERY_LEVEL_CHANGED:
			// just inform user the current battery level
			// ((cdUInt32*)pData) would tell the battery level
			break;
		}
		return cdOK;
	}
	else if (cdEVENT_SEVERITY_WARNING == cdEVENT_SEVERITY(EventID)){ 

		switch (cdEVENT_EVENTID(EventID)){
		case cdEVENT_BATTERY_LEVEL_CHANGED:
			// *((cdUInt32*)pData) would tell the battery level
			// Client should tell end user the battery is going down.
			break;
		case cdEVENT_CONNECTION_TIME_OUT:
			// *((cdUInt32*)pData) would tell the remaining seconds(approx) to shutdown
			// Client should bring up alert dialog 
			// returning cdDEVICE_STAY_AWAKE error postpone the time out,
			// returning 0(cdOK) let camera shut down.
			break;
		case cdEVENT_UNKNOWN_COMMAND_RECEIVED:
			// this is for debug purpose. Just generates debug log string
			break;
		default:
			;  // ignore other event
		}
		return cdOK;
	}
	else if (cdEVENT_SEVERITY_SHUTDOWN == cdEVENT_SEVERITY(EventID)){

		// Camera has shut down. Use the event ID to tell end user the reason.
        // The typical behavior of client is to post message itself from here.
        // At the main event loop of the client, upon receipt of the message, client wil close device.
		switch (cdEVENT_EVENTID(EventID)){
		case cdEVENT_BATTERY_LEVEL_CHANGED: // LB happened
			break;
		case cdEVENT_CF_GATE_CHANGED:       // gate is opened
			break;
		case cdEVENT_BATT_COVER_CHANGED:    // battery cover is opened.
			break;
		case cdEVENT_DIAL_CHANGE:           // Dial is changed
			break;
		case cdEVENT_CONNECTION_TIME_OUT:   // timed out since last request from PC
			break;
		default:// unknown reason
			;
		}
		return cdOK;
 	}
	return cdOK;//this should not happen. Just in case.
}
*/



#endif