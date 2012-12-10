#ifndef _SAPMANAGER_H_
#define _SAPMANAGER_H_

//	SapManager.h : header file
//

#include "SapClassBasicDef.h"


//
// SapLocation class declaration
//
class SAPCLASSBASIC_CLASS SapLocation
{
public:
   // Common server indices
   enum ServerIndex
   {
      ServerUnknown = -1,
      ServerSystem = 0
   };

   // Common resource indices
   enum ResourceIndex
   {
      ResourceUnknown = -1
   };

public:
	// Constructor/Destructor
	SapLocation() 
		{ m_ServerIndex = ServerSystem; strcpy(m_ServerName, ""); m_ResourceIndex = 0; }

	SapLocation(int serverIndex, int resourceIndex = 0) 
		{ m_ServerIndex = serverIndex; strcpy(m_ServerName, ""); m_ResourceIndex = resourceIndex; }

	SapLocation(const char *serverName, int resourceIndex = 0) 
		{ m_ServerIndex = ServerUnknown; strcpy(m_ServerName, serverName); m_ResourceIndex = resourceIndex; }

	SapLocation(const SapLocation &loc) 
		{ m_ServerIndex = loc.m_ServerIndex; strcpy(m_ServerName, loc.m_ServerName); m_ResourceIndex = loc.m_ResourceIndex; }

	SapLocation(const SapLocation &loc, int resourceIndex) 
		{ m_ServerIndex = loc.m_ServerIndex; strcpy(m_ServerName, loc.m_ServerName); m_ResourceIndex = resourceIndex; }

	virtual ~SapLocation() {}

	int GetServerIndex() const { return m_ServerIndex; }
	const char *GetServerName() const { return m_ServerName; }
	int GetResourceIndex() const { return m_ResourceIndex; }
	BOOL IsUnknown() const { return m_ServerIndex < 0 && !strlen(m_ServerName); }

   // Obsolete method
	BOOL IsNull() const { return IsUnknown(); }

protected:
	int m_ServerIndex;
	char m_ServerName[CORSERVER_MAX_STRLEN];
	int m_ResourceIndex;
};


//
// SapManCallbackInfo class declaration
//
class SAPCLASSBASIC_CLASS SapManCallbackInfo
{
public:
   enum MaxValues
   {
      MaxMessageSize = 255
   };

public:
   SapManCallbackInfo(int serverIndex, void *pContext)
   {
      m_ServerIndex = serverIndex;
      m_pContext = pContext;
   }
   SapManCallbackInfo(SAPSTATUS errorValue, const char *errorMessage, void *pContext)
   {
      m_ErrorValue = errorValue;
      if (errorMessage)
         strncpy(m_ErrorMessage, errorMessage, MaxMessageSize);
      m_pContext = pContext;
   }
   ~SapManCallbackInfo() {}

	int GetServerIndex()          const { return m_ServerIndex; }
   void *GetContext()            const { return m_pContext; }
   SAPSTATUS GetErrorValue()     const { return m_ErrorValue; }
   const char *GetErrorMessage() const { return m_ErrorMessage; }

protected:
	int m_ServerIndex;
   void *m_pContext;
   SAPSTATUS m_ErrorValue;
   char m_ErrorMessage[MaxMessageSize + 1];
};

typedef void (*SapManCallback)(SapManCallbackInfo *);


// These macros that may only be called from SapManager derived classes
// Set current function name for error messages
#define SAP_FUNCNAME(funcName) \
   static char *sapFuncName = funcName;

// Check for an error condition with possible arguments for formatting the error message
#define SAP_CHECK(errorCondition, errorMsg, statement)                  \
   if ((errorCondition))                                                \
   {                                                                    \
      if (strlen(errorMsg) != 0)                                           \
         DisplayMessageAndSave(sapFuncName, errorMsg, __FILE__, __LINE__); \
      statement;                                                        \
   }

// Signal an error condition
#define SAP_ERROR(errorMsg, statement)                                  \
   {                                                                    \
      if (strlen(errorMsg) != 0)                                           \
         DisplayMessageAndSave(sapFuncName, errorMsg, __FILE__, __LINE__); \
      statement;                                                        \
   }

// Valid only for classes that implement the Destroy() method
#define SAP_DESTROY(errorMsg, statement)                                \
   {                                                                    \
      if (strlen(errorMsg) != 0)                                           \
         DisplayMessageAndSave(sapFuncName, errorMsg, __FILE__, __LINE__); \
      Destroy();                                                        \
      statement;                                                        \
   }

// Show a message for the current function with one possible arguments for formatting
#define SAP_MESSAGE(message)                                    \
   {                                                            \
      if (strlen(message) != 0)                                    \
         DisplayMessage(sapFuncName, message, __FILE__, __LINE__); \
   }


// Forward declarations
class SAPCLASSBASIC_CLASS SapInterface;
class SAPCLASSBASIC_CLASS SapResourceInfo;

//
// SapManager class declaration
//

class SAPCLASSBASIC_CLASS SapManager
{
public:
   // Server types
   typedef int ServerType;    // For compatibility with old Sapera++ application code
   enum _ServerType
   {
      ServerNone         = 0,
      ServerSystem       = CORPORT_SYSTEM,
      ServerCobra        = CORPORT_COBRA,
      ServerViperRgb     = CORPORT_VIPERRGB,
      ServerViperDigital = CORPORT_VIPERDIG,
      ServerViperQuad    = CORPORT_VIPERQUAD,
      ServerViperCamLink = CORPORT_VIPERCAMLINK,
      ServerBanditII     = CORPORT_BANDITII,
      ServerX64CL        = CORPORT_X64,
      ServerX64CHL       = CORPORT_X64CHL,
      ServerX64LVDS      = CORPORT_X64LVDS,
      ServerX64NS        = CORPORT_X64NS,
      ServerX64Analog    = CORPORT_X64AN,
      ServerPC2Vision    = CORPORT_PC2V,
      ServerPC2CamLink   = CORPORT_PC2C,
      ServerMamba        = CORPORT_MAMBA,
      ServerAnaconda     = CORPORT_ANACONDA
   };

   // Resource types
   typedef int ResType;    // For compatibility with old Sapera++ application code
   enum _ResType
   {
      ResourceAcq = 0,
      ResourceDisplay,
      ResourceCab,
      ResourcePixPro,
      ResourceRtPro = ResourcePixPro,
      ResourceDsp,
      ResourceGraphic,
      ResourceGio,
      ResourceCounter,
      ResourceLast = ResourceCounter
   };

   // Sapera error return codes
   // To be completed later if necessary
   enum StatusCode
   {
      StatusOk = CORSTATUS_OK,
      StatusInvalidHandle = CORSTATUS_INVALID_HANDLE,
      StatusIncompatibleAcq = CORSTATUS_INCOMPATIBLE_ACQ	,
      StatusIncompatibleBuffer = CORSTATUS_INCOMPATIBLE_BUFFER,
      StatusIncompatibleCab = CORSTATUS_INCOMPATIBLE_CAB	,
      StatusIncompatibleCam = CORSTATUS_INCOMPATIBLE_CAM,
      StatusIncompatibleDisplay = CORSTATUS_INCOMPATIBLE_DISPLAY,
      StatusIncompatibleGraphic = CORSTATUS_INCOMPATIBLE_GRAPHIC,
      StatusIncompatibleKernel = CORSTATUS_INCOMPATIBLE_KERNEL	,
      StatusIncompatibleLut = CORSTATUS_INCOMPATIBLE_LUT	,
      StatusIncompatibleManager = CORSTATUS_INCOMPATIBLE_MANAGER	,
      StatusIncompatiblePro = CORSTATUS_INCOMPATIBLE_PRO	,
      StatusIncompatibleVic = CORSTATUS_INCOMPATIBLE_VIC	,
      StatusIncompatibleView = CORSTATUS_INCOMPATIBLE_VIEW,
      StatusIncompatibleXfer = CORSTATUS_INCOMPATIBLE_XFER,
      StatusIncompatibleString = CORSTATUS_INCOMPATIBLE_STRING	,
      StatusIncompatibleObject = CORSTATUS_INCOMPATIBLE_OBJECT	,
      StatusIncompatibleArray = CORSTATUS_INCOMPATIBLE_ARRAY	,	
      StatusIncompatibleStream = CORSTATUS_INCOMPATIBLE_STREAM	,	
      StatusIncompatibleFile = CORSTATUS_INCOMPATIBLE_FILE		,
      StatusCapInvalid = CORSTATUS_CAP_INVALID				,
      StatusCapNotAvailable = CORSTATUS_CAP_NOT_AVAILABLE,		
      StatusPrmInvalid = CORSTATUS_PRM_INVALID				,
      StatusPrmNotAvailable = CORSTATUS_PRM_NOT_AVAILABLE,		
      StatusPrmOutOfRange = CORSTATUS_PRM_OUT_OF_RANGE	,		
      StatusPrmInvalidValue = CORSTATUS_PRM_INVALID_VALUE,		
      StatusPrmReadOnly = CORSTATUS_PRM_READ_ONLY			,	
      StatusPrmMutuallyExclusive = CORSTATUS_PRM_MUTUALLY_EXCLUSIVE	,
      StatusArgInvalid = CORSTATUS_ARG_INVALID				,
      StatusArgOutOfRange = CORSTATUS_ARG_OUT_OF_RANGE	,	
      StatusArgIncompatible = CORSTATUS_ARG_INCOMPATIBLE	,		
      StatusArgInvalidValue = CORSTATUS_ARG_INVALID_VALUE,		
      StatusArgNull = CORSTATUS_ARG_NULL					    , 
      StatusFileOptionsError = CORSTATUS_FILE_OPTIONS_ERROR,     
      StatusFileOpenModeInvalid = CORSTATUS_FILE_OPEN_MODE_INVALID	,
      StatusFileSeekError = CORSTATUS_FILE_SEEK_ERROR			,
      StatusFileCreateError = CORSTATUS_FILE_CREATE_ERROR	,	
      StatusFileOpenError = CORSTATUS_FILE_OPEN_ERROR			,
      StatusFileReadError = CORSTATUS_FILE_READ_ERROR			,
      StatusFileWriteError = CORSTATUS_FILE_WRITE_ERROR		,	
      StatusFileCloseError = CORSTATUS_FILE_CLOSE_ERROR		,	
      StatusFileFormatUnkown = CORSTATUS_FILE_FORMAT_UNKNOWN ,	
      StatusFileFieldValueNotSupported = CORSTATUS_FILE_FIELD_VALUE_NOT_SUPPORTED ,
      StatusFileGetFieldError = CORSTATUS_FILE_GET_FIELD_ERROR	,
      StatusFileReadOnly = CORSTATUS_FILE_READ_ONLY			,
      StatusFileWriteOnly = CORSTATUS_FILE_WRITE_ONLY			 ,     
      StatusNotImplemented = CORSTATUS_NOT_IMPLEMENTED		,	
      StatusNoMemory = CORSTATUS_NO_MEMORY					,
      StatusClippingOccurred = CORSTATUS_CLIPPING_OCCURED,			
      StatusHardwareError = CORSTATUS_HARDWARE_ERROR		,	
      StatusServiceNotAvailable = CORSTATUS_SERVICE_NOT_AVAILABLE	,
      StatusNotAccessible = CORSTATUS_NOT_ACCESSIBLE			,
      StatusNotAvailable = CORSTATUS_NOT_AVAILABLE				,
      StatusRoutingNotImplemented = CORSTATUS_ROUTING_NOT_IMPLEMENTED,
      StatusRoutingNotAvailable = CORSTATUS_ROUTING_NOT_AVAILABLE	,
      StatusRoutingInUse = CORSTATUS_ROUTING_IN_USE			,
      StatusIncompatibleSize = CORSTATUS_INCOMPATIBLE_SIZE	,	
      StatusIncompatibleFormat = CORSTATUS_INCOMPATIBLE_FORMAT		,
      StatusIncompatibleLocation = CORSTATUS_INCOMPATIBLE_LOCATION,	
      StatusResourceInUse = CORSTATUS_RESOURCE_IN_USE			,
      StatusResourceLinked = CORSTATUS_RESOURCE_LINKED		,	
      StatusSoftwareError = CORSTATUS_SOFTWARE_ERROR			,
      StatusParametersLocked = CORSTATUS_PARAMETERS_LOCKED	,	
      StatusXferNotConnected = CORSTATUS_XFER_NOT_CONNECTED ,	     
      StatusXferEmptyList = CORSTATUS_XFER_EMPTY_LIST		   ,
      StatusXferCantCycle = CORSTATUS_XFER_CANT_CYCLE  		,
      StatusRoutingNotSpecified = CORSTATUS_ROUTING_NOT_SPECIFIED	,
      StatusTransferInProgress = CORSTATUS_TRANSFER_IN_PROGRESS	,
      StatusApiNotLocked = CORSTATUS_API_NOT_LOCKED			,
      StatusServerNotFound = CORSTATUS_SERVER_NOT_FOUND		,	
      StatusCannotSignalEvent = CORSTATUS_CANNOT_SIGNAL_EVENT,		
      StatusNoMessage = CORSTATUS_NO_MESSAGE					,
      StatusTimeOut = CORSTATUS_TIMEOUT						,
      StatusInvalidAlignment = CORSTATUS_INVALID_ALIGNMENT,		
      StatusDdraw256Colors = CORSTATUS_DDRAW_256_COLORS	,		
      StatusPciIoError = CORSTATUS_PCI_IO_ERROR				,
      StatusPciCannotAccessDevice = CORSTATUS_PCI_CANNOT_ACCESS_DEVICE,
      StatusEventCreateError = CORSTATUS_EVENT_CREATE_ERROR       ,
      StatusBoardNotReady = CORSTATUS_BOARD_NOT_READY			,
      StatusXferMaxSize = CORSTATUS_XFER_MAX_SIZE            ,
      StatusProcessingError = CORSTATUS_PROCESSING_ERROR	     ,   
      StatusResourceLocked = CORSTATUS_RESOURCE_LOCKED         , 
      StatusNoMessagingMemory = CORSTATUS_NO_MESSAGING_MEMORY	,	  
      StatusDdrawNotAvailable = CORSTATUS_DDRAW_NOT_AVAILABLE   ,   
      StatusDdrawError = CORSTATUS_DDRAW_ERROR              ,
      StatusResourceNotLocked = CORSTATUS_RESOURCE_NOT_LOCKED,      
      StatusDiskOnChipError = CORSTATUS_DISK_ON_CHIP_ERROR    ,   
      StatusNotCompleted = CORSTATUS_NOT_COMPLETED       	  ,
      StatusAuthentificationFailed = CORSTATUS_AUTHENTIFICATION_FAILED  ,
      StatusInsufficientBandwidth = CORSTATUS_INSUFFICIENT_BANDWIDTH   ,
      StatusFileTellError = CORSTATUS_FILE_TELL_ERROR			,
      StatusMaxProcessExceeded = CORSTATUS_MAX_PROCESS_EXCEEDED     ,
      StatusXferCountMultSrcFrameCount = CORSTATUS_XFER_COUNT_MULT_SRC_FRAME_COUNT  ,
      StatusAcqConnectedToXfer = CORSTATUS_ACQ_CONNECTED_TO_XFER		,
      StatusInsufficientBoardMemory = CORSTATUS_INSUFFICIENT_BOARD_MEMORY ,
      StatusInsufficientResources = CORSTATUS_INSUFFICIENT_RESOURCES    
   };

   // Status reporting modes
   typedef int StatusMode;    // For compatibility with old Sapera++ application code
   enum _StatusMode
   {
      StatusNotify = 0,
      StatusLog,
      StatusDebug,
      StatusCustom,
      StatusCallback
   };

   // Various maximum values
   enum MaxValues
   {
      MaxServers = 40,
      MaxResourceTypes = ResourceLast + 1,
      MaxResources = 8,
      MaxLabelSize = 128,
   };

public:
	// Constructor/Destructor
	SapManager() { m_bInitOK = FALSE; }
	virtual ~SapManager() {}

	// Server-access methods
	static int GetServerCount();
	static int GetServerCount(ResType resourceType);

	static int GetServerIndex(const char *serverName);
	static int GetServerIndex(SapLocation loc);

   // Note: nameSize argument (formerly bufSize) is now obsolete
	static BOOL GetServerName(int serverIndex, char *serverName, int nameSize = CORSERVER_MAX_STRLEN);
	static BOOL GetServerName(SapLocation loc, char *serverName, int nameSize = CORSERVER_MAX_STRLEN);
	static BOOL GetServerName(int serverIndex, ResType resourceType, char *serverName);

	static BOOL GetServerHandle(int serverIndex, PCORSERVER pServer);
	static BOOL GetServerHandle(const char *serverName, PCORSERVER pServer);
	static BOOL GetServerHandle(SapLocation loc, PCORSERVER pServer);

	static ServerType GetServerType(int serverIndex);
	static ServerType GetServerType(const char *serverName);
	static ServerType GetServerType(SapLocation loc);

	static BOOL GetServerSerialNumber(int serverIndex, char *serialNumber);
	static BOOL GetServerSerialNumber(const char *serverName, char *serialNumber);
	static BOOL GetServerSerialNumber(SapLocation loc, char *serialNumber);

   static BOOL ResetServer(int serverIndex, BOOL wait = TRUE, SapManCallback pCallback = NULL, void *pContext = NULL);
   static BOOL ResetServer(const char *serverName, BOOL wait = TRUE, SapManCallback pCallback = NULL, void *pContext = NULL);
   static BOOL ResetServer(SapLocation loc, BOOL wait = TRUE, SapManCallback pCallback = NULL, void *pContext = NULL);

	// Resource-access methods
	static int GetResourceCount(int serverIndex, ResType resourceType);
	static int GetResourceCount(const char *serverName, ResType resourceType);
	static int GetResourceCount(SapLocation loc, ResType resourceType);

   // Note: nameSize argument (formerly bufSize) is now obsolete
	static BOOL GetResourceName(int serverIndex, ResType resourceType, int resourceIndex, char *resourceName, int nameSize = MaxLabelSize);
	static BOOL GetResourceName(const char *serverName, ResType resourceType, int resourceIndex, char *resourceName, int nameSize = MaxLabelSize);
	static BOOL GetResourceName(SapLocation loc, ResType resourceType, char *resourceName, int nameSize = MaxLabelSize);

   static int GetResourceIndex(int serverIndex, ResType resourceType, const char *resourceName);
   static int GetResourceIndex(const char *serverName, ResType resourceType, const char *resourceName);

	static BOOL IsResourceAvailable(int serverIndex, ResType resourceType, int resourceIndex);
	static BOOL IsResourceAvailable(const char *serverName, ResType resourceType, int resourceIndex);
	static BOOL IsResourceAvailable(SapLocation loc, ResType resourceType);

   static BOOL GetInstallDirectory(int serverIndex, char *installDir);
   static BOOL GetInstallDirectory(const char *serverName, char *installDir);
   static BOOL GetInstallDirectory(SapLocation loc, char *installDir);

	// Utility methods
   static BOOL IsSystemLocation() { return CorManIsSystemHandle(CorManGetLocalServer()); }
   static BOOL IsSystemLocation(SapLocation loc);
	static BOOL IsSameServer(SapLocation loc1, SapLocation loc2);
	static BOOL IsSameLocation(SapLocation loc1, SapLocation loc2);
   static int GetPixelDepthMin(SapFormat format);
   static int GetPixelDepthMax(SapFormat format);
   static SapFormatType GetFormatType(SapFormat format);
	static BOOL GetStringFromFormat( SapFormat format, char* txtFormat);
   static int GetResetTimeout() { return m_ResetTimeout; }
   static void SetResetTimeout(int resetTimeout) { m_ResetTimeout = resetTimeout; }

	// Status reporting methods
	static StatusMode GetDisplayStatusMode() { return m_DisplayStatusMode; }
	static BOOL SetDisplayStatusMode(StatusMode mode, SapManCallback pCallback = NULL, void *pContext = NULL);
	static SAPSTATUS DisplayStatus(const char *functionName, SAPSTATUS status);
	static void DisplayMessage(const char *message, const char *fileName = NULL, int lineNumber = 0, ...);
   static void GetLastStatus(SAPSTATUS *pLastStatus);
   static const char *GetLastStatus();

   static BOOL IsStatusOk(const char *funcName, SAPSTATUS status) { return DisplayStatus(funcName, status) == StatusOk; }

   // Check if all handles were created successfully  (derived classes only)
	operator BOOL() const {return m_bInitOK;}

protected:
	BOOL m_bInitOK;            // TRUE if all handles were created successfully (derived classes only)

protected:
	static ServerType GetServerType(CORSERVER hServer);
	static BOOL GetServerSerialNumber(CORSERVER hServer, char *serialNumber);
	static BOOL SetResourceCount(int serverIndex, ResType resourceType);
	static BOOL SetResourceName(int serverIndex, ResType resourceType, int resourceIndex);
   static void DisplayMessage(const char *funcName, const char *message, const char *fileName, int lineNumber);
   static void DisplayMessageAndSave(const char *funcName, char *message, const char *fileName, int lineNumber);
	static void DisplayMessageAndSave(char *message, const char *fileName, int lineNumber);
	static char *FormatMessage(char *formatStr, ...);
	static UINT WINAPI ResetThreadProc(LPVOID lpParameter);
	static BOOL GetInstallDirectory(CORSERVER hServer, char *installDir);

protected:
	static SapInterface m_Interface;
	static SapResourceInfo m_ResourceInfo[MaxResourceTypes];
	static StatusMode m_DisplayStatusMode;

   static int m_ResetIndex;
   static int m_ResetTimeout;
   static SapManCallback m_ResetCallback;
   static void *m_ResetContext;
   static SapManCallback m_ErrorCallback;
   static void *m_ErrorContext;
};


//
// SapInterface class declaration
//
typedef SAPSTATUS (__stdcall *PGETCOUNT)( CORSERVER server, PUINT32 count);
typedef SAPSTATUS (__stdcall *PGETHANDLE)( CORSERVER server, UINT32 instance, CORHANDLE *pHandle);
typedef SAPSTATUS (__stdcall *PRELEASE)( CORHANDLE handle);
typedef SAPSTATUS (__stdcall *PGETPRM)( CORHANDLE handle, UINT32 param, void *value);

class SAPCLASSBASIC_CLASS SapInterface
{
public:
	static PGETCOUNT m_GetCount[SapManager::MaxResourceTypes];
	static PGETHANDLE m_GetHandle[SapManager::MaxResourceTypes];
	static PRELEASE m_Release[SapManager::MaxResourceTypes];
	static PGETPRM m_GetPrm[SapManager::MaxResourceTypes];
	static int m_LabelPrm[SapManager::MaxResourceTypes];
};


//
// SapResourceInfo class declaration
//
class SAPCLASSBASIC_CLASS SapResourceInfo
{
public:
   SapResourceInfo();

   int m_Counts[SapManager::MaxServers];
   char m_Labels[SapManager::MaxServers][SapManager::MaxResources][SapManager::MaxLabelSize];
   BOOL m_IsLabelInitOk[SapManager::MaxServers][SapManager::MaxResources];
};

#endif	// _SAPMANAGER_H_
