//
// Unpacking directive function
//

#include <corenv.h>

#if COR_I960
	#pragma align
#endif

#if COR_WIN32 || COR_NTKERNEL
	#ifdef _MSC_VER
		#pragma pack()
	#endif

	#ifdef __BORLANDC__
		#pragma option -a.
	#endif
#endif
