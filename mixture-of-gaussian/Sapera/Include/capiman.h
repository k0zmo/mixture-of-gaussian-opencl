//
// Coreco API - Manager functions.
//

#ifndef _CAPIMAN_H_
#define _CAPIMAN_H_

#include <capidef.h>
#include <capimsg.h>

#define CORMAN_CORECO_CMD		0x80000000
#define CORMAN_USER_CMD			0xa0000000

// Internal use (file and line are implicit)
#define CorManLogEx( logtype, log) _CorManLogEx( logtype, log, __FILE__, __LINE__)
#define CorManLog( status)         _CorManLog( status, __FILE__, __LINE__)

// External use (allows specifying user file and line)
#define CorManLogMessage   _CorManLogEx
#define CorManLogStatus    _CorManLog

#if COR_WIN32
	#define CORMANLOG_AND_RETURN(status) { CorManLog(status); return status; }
#else
	#define CORMANLOG_AND_RETURN(status) { return status; }
#endif

/* Version Manager Value */
#define CORMAN_VAL_VERSION_TYPE_MASK			0x0000FFFF
#define CORMAN_VAL_VERSION_ONBOARD				0x00010000
#define CORMAN_VAL_VERSION_SAPERA				0x00000001
#define CORMAN_VAL_VERSION_ONBOARD_SAPERA	 	(CORMAN_VAL_VERSION_SAPERA | CORMAN_VAL_VERSION_ONBOARD)
#define CORMAN_VAL_VERSION_DRIVER				0x00000002
#define CORMAN_VAL_VERSION_ONBOARD_DRIVER	 	(CORMAN_VAL_VERSION_DRIVER | CORMAN_VAL_VERSION_ONBOARD)

/* Version Manager macro */
#define CORMAN_VERSION_MAJOR(value)		((value & 0xFFC00000) >> 22)
#define CORMAN_VERSION_MINOR(value)		((value & 0x003FFC00) >> 10)
#define CORMAN_VERSION_BETA(value)		(value & 0x000003FF)

#ifdef __cplusplus
extern "C" {
#endif

/* Application programming interface */
CORSTATUS CORAPIFUNC CorManAllocCallbackQueue( UINT32 size);
CORSTATUS CORAPIFUNC CorManFreeCallbackQueue( void);

CORSTATUS CORAPIFUNC CorManRegisterCallback( CORSERVER hServer, PCORMANCALLBACK manCallback);
CORSTATUS CORAPIFUNC CorManUnregisterCallback( CORSERVER hServer);

CORSTATUS CORAPIFUNC CorManUserCmd( CORSERVER hServer, UINT32 cmd, void *inData, UINT32 inDataSize, void *outData, UINT32 outDataSize);
CORSTATUS CORAPIFUNC CorManControl( CORSERVER hServer, UINT32 cmd, void *inData, UINT32 inDataSize, void *outData, UINT32 outDataSize);
CORSTATUS CORAPIFUNC CorManInternalCmd( CORSERVER hServer, UINT32 cmd, void *inData, UINT32 inDataSize, void *outData, UINT32 outDataSize);

CORSTATUS CORAPIFUNC CorManCallback( CORSERVER hServer, PCORCALLBACK callback, void *context, UINT32 eventType, UINT32 eventCount);
CORSTATUS CORAPIFUNC CorManMsgCallback( CORSERVER hServer, PCORMSGCALLBACK callback, void *context, UINT32 cmd, void *inData, UINT32 inDataSize, void *outData, UINT32 outDataSize);

CORSTATUS CORAPIFUNC CorManQueueCallback( CORSERVER hServer, PCORCALLBACK callback, void *context, UINT32 eventType, UINT32 eventCount);
CORSTATUS CORAPIFUNC CorManExecuteCallback( void);

CORSTATUS CORAPIFUNC _CorManLogEx ( UINT32 logtype, PCSTR log, PCSTR module, UINT32 line);
CORSTATUS CORAPIFUNC _CorManLog   ( CORSTATUS status, PCSTR module, UINT32 line);

CORSTATUS CORAPIFUNC CorManGetHandleByIndex( CORSERVER hServer, UINT32 index, PCORHANDLE pHandle);
CORSTATUS CORAPIFUNC CorManGetHandleByName( CORSERVER hServer, PSTR name, PCORHANDLE pHandle);

CORSTATUS CORAPIFUNC CorManGetVersion(CORSERVER hBoard, UINT32 item,void *value );

CORSTATUS CORAPIFUNC CorManGetInstallationDirectory( CORSERVER hServer, PSTR pInstalDir, UINT32 strSize);


#ifdef __cplusplus
}
#endif

#endif   // _CAPIMAN_H_
