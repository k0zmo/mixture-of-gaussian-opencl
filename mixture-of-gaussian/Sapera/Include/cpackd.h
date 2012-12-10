//
// Packing directive function
//

#include <corenv.h>

#if COR_I960
	#pragma noalign
#endif

#if COR_WIN32 || COR_NTKERNEL
	#ifdef _MSC_VER
		#pragma warning (disable:4103)
		#pragma pack(1)
	#endif

	#ifdef __BORLANDC__
        #pragma option -w-pck
		#pragma option -a1
	#endif
#endif

#ifndef COR_PACK
   #if COR_IOP321
      #define COR_PACK __attribute__((packed))
   #elif COR_LINUX
      #define COR_PACK __attribute__((packed))
	#else
      #define COR_PACK
   #endif
#endif
