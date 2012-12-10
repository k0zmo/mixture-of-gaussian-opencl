//
// Coreco standard definitions.
//

#ifndef _CORDEF_H_
#define _CORDEF_H_

#include <corenv.h>

#if !COR_95KERNEL && !COR_LINUX
	#include <stdlib.h>
#endif

#define CORPROCESS_MAX_NUMBER			16	
#define CORECO_VENDORID					0x11ec

// Default definitions
#define CCONV
#define CORKEVENT								UINT32
#define CorIsBadReadPtr(ptr, size)		0
#define CorIsBadWritePtr(ptr, size)		0
#define CorIsBadStringPtr(ptr, size)	0

///////////////////////////////////////////////////////////////////////////////////////
// Windows 32 user level definitions
#if COR_WIN32
   #if _MSC_VER && !defined(_LIB)
      #if (_DEBUG && !_MT)
         #error The project option "C/C++ | Code Generation | Use run-time library" must either be "Debug Multithreaded" or "Debug Multithreaded DLL"
      #elif (!_DEBUG && !_MT)
         #error The project option "C/C++ | Code Generation | Use run-time library" must either be "Multithreaded" or "Multithreaded DLL"
      #endif
   #endif

   // Allow access to features from Windows NT and up
   #ifndef _WIN32_WINNT
   #define _WIN32_WINNT 0x0400
   #endif

	#include <windows.h>
	#include <Limits.h>


   #ifndef IS_HANDLE_VALID
   #define IS_HANDLE_VALID( handle) ( ( handle != NULL) && ( handle != INVALID_HANDLE_VALUE))
   #endif

	#undef  CORKEVENT
	#define CORKEVENT       HANDLE

	#undef  CCONV
	#define CCONV           __stdcall

	#undef  CorIsBadReadPtr
	#define CorIsBadReadPtr(ptr, size)		IsBadReadPtr(ptr, size)
	#undef  CorIsBadWritePtr
	#define CorIsBadWritePtr(ptr, size)		IsBadWritePtr(ptr, size)
	#undef  CorIsBadStringPtr
	#define CorIsBadStringPtr(ptr, size)	IsBadStringPtr(ptr, size)

	typedef signed char     INT8;
	typedef unsigned char   UINT8;
	typedef short           INT16;
	typedef unsigned short  UINT16;
	typedef float           FLOAT;
	typedef int             BOOL;
	typedef double				DOUBLE;

	#ifndef _BASETSD_H_     // New in Visual C++ 6.0
		typedef long            INT32;
		typedef unsigned long   UINT32;

		// needed for C++ Builder
		typedef __int64		   UINT64;
	#endif

   #define INT8_MAX             CHAR_MAX
   #define INT8_MIN             CHAR_MIN
   #define UINT8_MAX            UCHAR_MAX
   #define INT16_MAX            SHRT_MAX
   #define INT16_MIN            SHRT_MIN
   #define UINT16_MAX           USHRT_MAX
   #define INT32_MAX            LONG_MAX
   #define INT32_MIN            LONG_MIN
   #define UINT32_MAX           ULONG_MAX

///////////////////////////////////////////////////////////////////////////////////////
// Windows 95 kernel level definitions
#elif COR_95KERNEL

	#include <vtoolsc.h>

	typedef long   			*LPLONG;
	typedef unsigned int	UINT;
	typedef signed char     INT8;
	typedef unsigned char   UINT8;
	typedef short           INT16;
	typedef unsigned short  UINT16;
	typedef long            INT32;
	typedef unsigned long   UINT32;
	typedef float           FLOAT;
	typedef int             BOOL;
	typedef double			   DOUBLE;

///////////////////////////////////////////////////////////////////////////////////////
// Windows NT kernel level definitions
#elif COR_NTKERNEL
	#include <ntddk.h>


	#undef  CORKEVENT
	#define CORKEVENT       PKEVENT

	typedef long   			*LPLONG;
	typedef unsigned int	UINT;
	typedef signed char     INT8;
	typedef unsigned char   UINT8;
	typedef short           INT16;
	typedef unsigned short  UINT16;
	#ifndef _BASETSD_H_     // DDK 2000
		typedef long            INT32;
		typedef unsigned long   UINT32;
		typedef __int64         UINT64;
	#endif
	typedef float           FLOAT;
	typedef int             BOOL;


///////////////////////////////////////////////////////////////////////////////////////
// C6x firmware definitions
#elif COR_C60
	#include <Limits.h>
   
	#define INVALID_HANDLE_VALUE    0
	#ifndef TRUE
		#define TRUE                    1
		#define FALSE                   0
	#endif

	typedef char           CHAR;
	typedef char           INT8;
	typedef unsigned char  UINT8;
	typedef short          INT16;
	typedef unsigned short UINT16;
	typedef int            INT32;
	typedef unsigned int   UINT32;

#include "cpackd.h"
	typedef struct
	{
		UINT32 LowDWord;
		UINT32 HiDWord;
	}       UINT64;
#include "cunpackd.h"

	typedef long				INT40;
	typedef unsigned long	UINT40;
	typedef float				FLOAT;
	typedef unsigned int		HANDLE;
	typedef int					BOOL;
	typedef unsigned int		UINT;
	typedef int					INT;
	typedef char            *PSTR;
	typedef double				DOUBLE;
//	typedef unsigned int *LPLONG;

   #define INT8_MAX		CHAR_MAX
   #define INT8_MIN		CHAR_MIN
   #define UINT8_MAX		UCHAR_MAX
   #define INT16_MAX		SHRT_MAX
   #define INT16_MIN		SHRT_MIN
   #define UINT16_MAX	USHRT_MAX
   #define INT32_MAX		INT_MAX
   #define INT32_MIN		INT_MIN
   #define UINT32_MAX	UINT_MAX

///////////////////////////////////////////////////////////////////////////////////////
// i960 firmware definitions
#elif COR_I960
	#include <Limits.h>
   
	#define INVALID_HANDLE_VALUE    0
	#define TRUE                    1
	#define FALSE                   0

	typedef char            CHAR;
   typedef unsigned char   *PUCHAR;
	typedef char            INT8;
	typedef unsigned char   UINT8;
	typedef short           INT16;
	typedef unsigned short  UINT16;
	typedef long            INT32;
	typedef unsigned long   UINT32;

#include "cpackd.h"
	typedef struct
	{
		UINT32  LowDWord;
		UINT32  HiDWord;
	}       UINT64;
#include "cunpackd.h"

	typedef float           FLOAT;
	typedef int             BOOL;
	typedef unsigned int    HANDLE;
	typedef unsigned int    UINT;
	typedef int             INT;
	typedef long   			*LPLONG;
	typedef char            *PSTR;
	typedef double          DOUBLE;
   
   #define INT8_MAX             CHAR_MAX
   #define INT8_MIN             CHAR_MIN
   #define UINT8_MAX            UCHAR_MAX
   #define INT16_MAX            SHRT_MAX
   #define INT16_MIN            SHRT_MIN
   #define UINT16_MAX           USHRT_MAX
   #define INT32_MAX            LONG_MAX
   #define INT32_MIN            LONG_MIN
   #define UINT32_MAX           ULONG_MAX
////////////////////////////////////////////////////////
#elif COR_IOP321


	#define INVALID_HANDLE_VALUE    0
	#define TRUE                    1
	#define FALSE                   0

	typedef char            CHAR;
   typedef unsigned char   *PUCHAR;
	typedef char            INT8;
	typedef unsigned char   UINT8;
	typedef short           INT16;
	typedef unsigned short  UINT16;
	typedef long            INT32;
	typedef unsigned long   UINT32;

#include "cpackd.h"
	typedef struct
	{
		UINT32  LowDWord;
		UINT32  HiDWord;
	}       UINT64;
#include "cunpackd.h"

	typedef float           FLOAT;
	typedef int             BOOL;
	typedef unsigned int    HANDLE;
	typedef unsigned int    UINT;
	typedef int             INT;
	typedef long   			*LPLONG;
	typedef char            *PSTR;
	typedef double          DOUBLE;
   
   #define INT8_MAX             CHAR_MAX
   #define INT8_MIN             CHAR_MIN
   #define UINT8_MAX            UCHAR_MAX
   #define INT16_MAX            SHRT_MAX
   #define INT16_MIN            SHRT_MIN
   #define UINT16_MAX           USHRT_MAX
   #define INT32_MAX            LONG_MAX
   #define INT32_MIN            LONG_MIN
   #define UINT32_MAX           ULONG_MAX


///////////////////////////////////////////////////////////////////////////////////////
// C165 firmware definitions
#elif COR_C165
	#define TRUE            1
	#define FALSE           0

	typedef char            CHAR;
	typedef char            INT8;
	typedef unsigned char   UINT8;
	typedef int             INT16;
	typedef unsigned int    UINT16;
	typedef long            INT32;
	typedef unsigned long   UINT32;

#include "cpackd.h"
	typedef struct
	{
		UINT32 LowDWord;
		UINT32 HiDWord;
	}       UINT64;
#include "cunpackd.h"

	typedef int             BOOL;
	typedef unsigned int    UINT;
	typedef int             INT;
	typedef char            *PSTR;
	typedef unsigned int		HANDLE;
	typedef float           FLOAT;
	
///////////////////////////////////////////////////////////////////////////////////////
// PowerPC definitions
#elif COR_POWERPC

	#define INVALID_HANDLE_VALUE    0
	#ifndef TRUE					
		#define TRUE                    1
		#define FALSE                   0
	#endif

	typedef char           CHAR;
	typedef char           INT8;
	typedef unsigned char  UINT8;
	typedef short          INT16;
	typedef unsigned short UINT16;
	typedef int            INT32;
	typedef unsigned int   UINT32;

#include "cpackd.h"
	typedef struct
	{
		UINT32 LowDWord;
		UINT32 HiDWord;
	}       UINT64;

typedef union _LARGE_INTEGER {
    struct {
        UINT32 LowPart;
        UINT32 HighPart;
    };
    struct {
        UINT32 LowPart;
        UINT32 HighPart;
    } u;
    UINT64 QuadPart;
} LARGE_INTEGER;

typedef LARGE_INTEGER *PLARGE_INTEGER;

#include "cunpackd.h"

	typedef long				INT40;
	typedef unsigned long	UINT40;
	typedef float				FLOAT;
	typedef unsigned int		HANDLE;
	typedef int					BOOL;
	typedef unsigned int		UINT;
	typedef int					INT;
	typedef char            *PSTR;
	typedef double				DOUBLE;
//	typedef unsigned int *LPLONG;

   #define INT8_MAX		CHAR_MAX
   #define INT8_MIN		CHAR_MIN
   #define UINT8_MAX		UCHAR_MAX
   #define INT16_MAX		SHRT_MAX
   #define INT16_MIN		SHRT_MIN
   #define UINT16_MAX	USHRT_MAX
   #define INT32_MAX		INT_MAX
   #define INT32_MIN		INT_MIN
   #define UINT32_MAX	UINT_MAX

///////////////////////////////////////////////////////////////////////////////////////
// TI's DSP DM642
#elif COR_TI_DM642
	#include <Limits.h>
   
	#define INVALID_HANDLE_VALUE    0
	#ifndef TRUE
		#define TRUE                    1
		#define FALSE                   0
	#endif

	typedef char           CHAR;
	
#ifndef _C_USERTYPE_INC		// defined in DM642 NDK header file
	typedef char           INT8;
	typedef unsigned char  UINT8;
	typedef short          INT16;
	typedef unsigned short UINT16;
	typedef int            INT32;
	typedef unsigned int   UINT32;
	typedef unsigned int		HANDLE;	
#endif	

   typedef union
   {
      struct
      {
         UINT8 byte0;
         UINT8 byte1;
         UINT8 byte2;
         UINT8 byte3;
         UINT8 byte4;
         UINT8 byte5;
         UINT8 byte6;
         UINT8 byte7;
      } uint8;

      struct
      {
         UINT16 word0;
         UINT16 word1;
         UINT16 word2;
         UINT16 word3;
      } uint16;

   	struct
   	{
		   UINT32 lo;
		   UINT32 hi;	
	   } uint32;
	
	   double uint64;
	   
   } UINT64;

	typedef float				FLOAT;	
	typedef int					BOOL;
	typedef unsigned int		UINT;
	typedef int					INT;
	typedef char            *PSTR;
	typedef double				DOUBLE;

   #define INT8_MAX		CHAR_MAX
   #define INT8_MIN		CHAR_MIN
   #define UINT8_MAX		UCHAR_MAX
   #define INT16_MAX		SHRT_MAX
   #define INT16_MIN		SHRT_MIN
   #define UINT16_MAX	USHRT_MAX
   #define INT32_MAX		INT_MAX
   #define INT32_MIN		INT_MIN
   #define UINT32_MAX	UINT_MAX

///////////////////////////////////////////////////////////////////////////////////////
// Linux definitions
#elif COR_LINUX


#ifdef __KERNEL__
#include <linux/types.h>
	typedef s8      CHAR;
	typedef s8      INT8;
	typedef u8      UINT8;
	typedef s16     INT16;
	typedef u16     UINT16;
	typedef s32     INT32;
	typedef u32     UINT32;
	typedef s64     INT64;
	typedef u64     UINT64;
	typedef void *  HANDLE;
#include "posixcmn.h"

#else

#include <stddef.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
	typedef int8_t    CHAR;
	typedef int8_t    INT8;
	typedef u_int8_t  UINT8;
	typedef int16_t   INT16;
	typedef u_int16_t   UINT16;
	typedef int32_t   INT32;
	typedef u_int32_t   UINT32;
	typedef long long   INT64;
	typedef u_int64_t   UINT64;
#include "corposix.h"

   #ifndef IS_HANDLE_VALID
   #define IS_HANDLE_VALID( handle) ( ( handle != NULL) && ( handle != INVALID_HANDLE_VALUE))
   #endif

#endif

	typedef float			FLOAT;
	typedef unsigned int	UINT;
	typedef int				INT;
	typedef char			*PSTR, *LPSTR, *LPCSTR, *LPCTSTR;
	typedef double		DOUBLE;
	typedef UINT8		UCHAR;
	typedef UINT8		*PUCHAR;
	typedef INT16		SHORT;
	typedef UINT16		USHORT;
//	typedef INT32		LONG;
//	typedef UINT32		ULONG;
	typedef UINT32		NTSTATUS;
	typedef void		VOID;

   #define INT8_MAX		CHAR_MAX
   #define INT8_MIN		CHAR_MIN
   #define UINT8_MAX		UCHAR_MAX
   #define INT16_MAX		SHRT_MAX
   #define INT16_MIN		SHRT_MIN
   #define UINT16_MAX		USHRT_MAX
   #define INT32_MAX		INT_MAX
   #define INT32_MIN		INT_MIN
   #define UINT32_MAX		UINT_MAX

#endif

///////////////////////////////////////////////////////////////////////////////////////
// General definitions

#ifndef NULL
	#define NULL            0
#endif

#ifndef far
	#define far
#endif

// Maximum length of full pathname 
#ifndef _MAX_PATH
	#define _MAX_PATH     260                 
#endif
#ifndef _MAX_DRIVE
	#define _MAX_DRIVE      3
#endif
#ifndef _MAX_DIR
	#define _MAX_DIR        256
#endif
#ifndef _MAX_FNAME
	#define _MAX_FNAME      256
#endif
#ifndef _MAX_EXT
	#define _MAX_EXT        256
#endif

typedef INT8            *PINT8;
typedef UINT8           *PUINT8;
typedef INT16           *PINT16;
typedef UINT16          *PUINT16;
typedef INT32           *PINT32;
typedef UINT32          *PUINT32;

typedef INT8            BOOL8;
typedef BOOL8           *PBOOL8;
typedef INT16           BOOL16;
typedef BOOL16          *PBOOL16;
typedef INT32           BOOL32;
typedef BOOL32          *PBOOL32;

#if !COR_95KERNEL
	typedef UINT64          *PUINT64;
#endif

#if !COR_C165
	typedef FLOAT           *PFLOAT;
#endif

#if !COR_LINUX
typedef BOOL            *PBOOL;
#endif
typedef unsigned long   DWORD;

#if !COR_C60
	#if (COR_I960 || ((_MSC_VER > 0 && _MSC_VER < 1200)) ||  COR_NTKERNEL  || COR_IOP321 ||  COR_POWERPC || COR_LINUX)
		#ifndef _BASETSD_H_     // DDK 2000
			typedef INT32           LONG;
			typedef UINT32          ULONG;
			typedef UINT32          *PULONG;
		#endif
		typedef UINT32          *PDWORD;
		typedef INT32           LRESULT;
	#endif
#endif

typedef const char      *PCSTR;
typedef UINT8           *PBYTE;
typedef UINT8           *LPBYTE;

#if !COR_95KERNEL
	typedef UINT8           *PBOOLEAN;
#endif

typedef UINT8           *LPBOOLEAN;
typedef UINT16          *PWORD;

#if !COR_95KERNEL
	typedef UINT8           BOOLEAN;
#endif
typedef UINT8           BYTE;
typedef UINT16          WORD;
typedef void            *LPVOID, *PVOID;

typedef char            CORSTR[ 256];

#include "cpackd.h"
	// Scatter/Gather memory structure shared between user, kernel, and firmware drivers.
	typedef struct _CORSGENTRY
	{
		UINT32		address;	// Lower 32-bits of Physical Address for segment.(*)
		UINT32		length;	// Length (in bytes) of the segment.
	} CORSGENTRY, *PCORSGENTRY;
#include "cunpackd.h"

#if COR_I960
	// Already defined for i960 (in intel types.h)
	#include <types.h>
#elif COR_C165
#elif COR_TI_DM642
#else
#if !COR_LINUX
	typedef unsigned int		uint;
	typedef unsigned short	ushort;
	typedef unsigned char	uchar;
	typedef unsigned long	ulong;
#endif
#endif

// volatile definitions
typedef UINT8 volatile  VUINT8;
typedef UINT8 volatile  *PVUINT8;

typedef UINT8 volatile  VBYTE;
typedef UINT8 volatile  *PVBYTE;

typedef UINT16 volatile VUINT16;
typedef UINT16 volatile *PVUINT16;

typedef UINT16 volatile VWORD;
typedef UINT16 volatile *PVWORD;

typedef UINT32 volatile VUINT32;
typedef UINT32 volatile *PVUINT32;

typedef UINT32 volatile VDWORD;
typedef UINT32 volatile *PVDWORD;

#if !COR_95KERNEL
	typedef UINT64 volatile VUINT64;
	typedef UINT64 volatile *PVUINT64;
#endif


// Ethernet-related definitions
typedef struct 
{
   UINT8 i1;
   UINT8 i2;
   UINT8 i3;
   UINT8 i4;
} CORIP_ADDRESS;



#endif   // _CORDEF_H_
