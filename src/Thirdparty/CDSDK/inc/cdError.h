#ifndef _CD_ERROR_H_
#define _CD_ERROR_H_


/****************************************************************************
 ****************************************************************************
 *            Canon Digital Camera SDK : Error IDs For Ver 6.0.0            *
 ****************************************************************************
 ****************************************************************************/

/*-----------------------------------------------------------------------
   CD-SDK Error Code Masks
------------------------------------------------------------------------*/
#define cdERROR_ISSPECIFIC_MASK						0x80000000L
#define cdERROR_COMPONENTID_MASK					0x7F000000L
#define cdERROR_RESERVED_MASK						0x00FF0000L
#define cdERROR_ERRORID_MASK						0x0000FFFFL

/*-----------------------------------------------------------------------
   CD-SDK Base Component IDs
------------------------------------------------------------------------*/
#define cdERROR_CLIENT_COMPONENTID				  	0x01000000L
#define cdERROR_LLSDK_COMPONENTID				  	0x02000000L
#define cdERROR_HLSDK_COMPONENTID				 	0x03000000L
#define cdERROR_PROPERTY_PARSING_COMPONENTID	  	0x04000000L
#define cdERROR_VIEW_DEVELOPMENT_COMPONENTID	 	0x05000000L
#define cdERROR_VIEW_DECODING_COMPONENTID			0x06000000L 
#define cdERROR_COLOR_MAPPING_COMPONENTID			0x07000000L
#define cdERROR_PICTURE_COLLECTION_COMPONENTID 		0x08000000L
#define cdERROR_SETUP_COMPONENTID					0x09000000L
#define cdERROR_IWRAP_COMPONENTID					0x0A000000L
#define cdERROR_PSUSD_COMPONENTID					0x0B000000L
#define cdERROR_CDSDK_COMPONENTID					0x0C000000L
#define cdERROR_RDSDK_COMPONENTID					0x0D000000L


/*-----------------------------------------------------------------------
   CD-SDK Functin Success Code
------------------------------------------------------------------------*/
#define cdOK										0x00000000L

/*-----------------------------------------------------------------------
   CD-SDK Generic Error IDs
------------------------------------------------------------------------*/
/* Miscellaneous errors */
#define cdUNIMPLEMENTED               				0x00000001L  
#define cdINTERNAL_ERROR							0x00000002L
#define cdMEM_ALLOC_FAILED					  		0x00000003L
#define cdMEM_FREE_FAILED							0x00000004L
#define cdOPERATION_CANCELLED						0x00000005L 
#define cdINCOMPATIBLE_VERSION						0x00000006L
#define cdNOT_SUPPORTED							  	0x00000007L
#define cdUNEXPECTED_EXCEPTION						0x00000008L
#define cdPROTECTION_VIOLATION						0x00000009L   
#define cdMISSING_SUBCOMPONENT						0x0000000AL
#define cdSELECTION_UNAVAILABLE						0x0000000BL

/* File errors */
#define cdFILE_IO_ERROR								0x00000020L
#define cdFILE_TOO_MANY_OPEN					  	0x00000021L
#define cdFILE_NOT_FOUND						  	0x00000022L
#define cdFILE_OPEN_ERROR							0x00000023L
#define cdFILE_CLOSE_ERROR							0x00000024L
#define cdFILE_SEEK_ERROR							0x00000025L
#define cdFILE_TELL_ERROR							0x00000026L
#define cdFILE_READ_ERROR							0x00000027L
#define cdFILE_WRITE_ERROR							0x00000028L
#define cdFILE_PERMISSION_ERROR						0x00000029L
#define cdFILE_DISK_FULL_ERROR						0x0000002AL
#define cdFILE_ALREADY_EXISTS					  	0x0000002BL
#define cdFILE_FORMAT_UNRECOGNIZED					0x0000002CL
#define cdFILE_DATA_CORRUPT							0x0000002DL
#define cdFILE_NAMING_NA							0x0000002EL

/* Directory errors */			
#define cdDIR_NOT_FOUND								0x00000040L
#define cdDIR_IO_ERROR						  		0x00000041L
#define cdDIR_ENTRY_NOT_FOUND				  		0x00000042L
#define cdDIR_ENTRY_EXISTS					  		0x00000043L
#define cdDIR_NOT_EMPTY						  		0x00000044L

/* Property errors */
#define cdPROPERTIES_UNAVAILABLE					0x00000050L
#define cdPROPERTIES_MISMATCH						0x00000051L
#define cdPROPERTIES_NOT_LOADED						0x00000053L

/* Function Parameter errors */		
#define cdINVALID_PARAMETER							0x00000060L
#define cdINVALID_HANDLE						  	0x00000061L
#define cdINVALID_POINTER						  	0x00000062L
#define cdINVALID_INDEX							  	0x00000063L
#define cdINVALID_LENGTH						  	0x00000064L
#define cdINVALID_FN_POINTER					  	0x00000065L
#define cdINVALID_SORT_FN							0x00000066L

/* Device errors */
#define cdDEVICE_NOT_FOUND            				0x00000080L
#define cdDEVICE_BUSY                 				0x00000081L
#define cdDEVICE_INVALID              				0x00000082L
#define cdDEVICE_EMERGENCY							0x00000083L
#define cdDEVICE_MEMORY_FULL						0x00000084L
#define cdDEVICE_INTERNAL_ERROR						0x00000085L
#define cdDEVICE_INVALID_PARAMETER					0x00000086L
#define cdDEVICE_NO_DISK							0x00000087L
#define cdDEVICE_DISK_ERROR							0x00000088L
#define cdDEVICE_CF_GATE_CHANGED					0x00000089L
#define cdDEVICE_DIAL_CHANGED						0x0000008AL
#define cdDEVICE_NOT_INSTALLED						0x0000008BL
#define cdDEVICE_STAY_AWAKE							0x0000008CL
#define cdDEVICE_NOT_RELEASED						0x0000008DL

/* Stream errors */
#define cdSTREAM_IO_ERROR						 	0x000000A0L
#define cdSTREAM_NOT_OPEN							0x000000A1L
#define cdSTREAM_ALREADY_OPEN						0x000000A2L    
#define cdSTREAM_OPEN_ERROR							0x000000A3L
#define cdSTREAM_CLOSE_ERROR					  	0x000000A4L
#define cdSTREAM_SEEK_ERROR							0x000000A5L
#define cdSTREAM_TELL_ERROR							0x000000A6L
#define cdSTREAM_READ_ERROR							0x000000A7L
#define cdSTREAM_WRITE_ERROR					  	0x000000A8L
#define cdSTREAM_PERMISSION_ERROR					0x000000A9L
#define cdSTREAM_COULDNT_BEGIN_THREAD				0x000000AAL
#define cdSTREAM_BAD_OPTIONS						0x000000ABL
#define cdSTREAM_END_OF_STREAM						0x000000ACL

/* Communications errors */
#define cdCOMM_PORT_IS_IN_USE         				0x000000C0L
#define cdCOMM_DISCONNECTED           				0x000000C1L
#define cdCOMM_DEVICE_INCOMPATIBLE					0x000000C2L
#define cdCOMM_BUFFER_FULL					  		0x000000C3L
#define cdCOMM_USB_BUS_ERR							0x000000C4L

/* Lock/Unlock */
#define cdUSB_DEVICE_LOCK_ERROR						0x000000D0L
#define cdUSB_DEVICE_UNLOCK_ERROR					0x000000D1L        

/* STI/WIA */
#define cdSTI_UNKNOWN_ERROR							0x000000E0L
#define cdSTI_INTERNAL_ERROR						0x000000E1L
#define cdSTI_DEVICE_CREATE_ERROR					0x000000E2L
#define cdSTI_DEVICE_RELEASE_ERROR					0x000000E3L
#define cdDEVICE_NOT_LAUNCHED						0x000000E4L
    
#define cdENUM_NA									0x000000F0L
#define cdINVALID_FN_CALL							0x000000F1L
#define cdHANDLE_NOT_FOUND							0x000000F2L
#define cdINVALID_ID						  		0x000000F3L 
#define cdWAIT_TIMEOUT_ERROR					    0x000000F4L	

/* PTP */
#define cdSESSION_NOT_OPEN                         0x00002003
#define cdINVALID_TRANSACTIONID                    0x00002004
#define cdINCOMPLETE_TRANSFER                      0x00002007
#define cdINVALID_STRAGEID                         0x00002008
#define cdDEVICEPROP_NOT_SUPPORTED                 0x0000200A
#define cdINVALID_OBJECTFORMATCODE                 0x0000200B
#define cdSELF_TEST_FAILED                         0x00002011
#define cdPARTIAL_DELETION                         0x00002012
#define cdSPECIFICATION_BY_FORMAT_UNSUPPORTED      0x00002014
#define cdNO_VALID_OBJECTINFO                      0x00002015
#define cdINVALID_CODE_FORMAT                      0x00002016
#define cdUNKNOWN_VENDER_CODE                      0x00002017
#define cdCAPTURE_ALREADY_TERMINATED               0x00002018
#define cdINVALID_PARENTOBJECT                     0x0000201A
#define cdINVALID_DEVICEPROP_FORMAT                0x0000201B
#define cdINVALID_DEVICEPROP_VALUE                 0x0000201C
#define cdSESSION_ALREADY_OPEN                     0x0000201E
#define cdTRANSACTION_CANCELLED                    0x0000201F
#define cdSPECIFICATION_OF_DESTINATION_UNSUPPORTED 0x00002020
#define cdUNKNOWN_COMMAND                          0x0000A001
#define cdOPERATION_REFUSED                        0x0000A005
#define cdLENS_COVER_CLOSE                         0x0000A006


#define cdLAST_GENERIC_ERROR_PLUS_ONE			   0x000000F5L 

#endif