//
// Coreco standard library.
//

#ifndef _CORLIB_H_
#define _CORLIB_H_

#include <cordef.h>
#include <corstat.h>

#if COR_NTKERNEL
	#define CorMalloc( nBytes)				ExAllocatePool( PagedPool, nBytes)
	#define CorFree( p)						ExFreePool(p)
	#define CorCalloc( nItem, nBytes)   CorMalloc( (nItem) * (nBytes))
	#define CorRealloc( p, nBytes)      Not implemented
   #define CorMemcpy(d,s,c)            CorManMemcpy( d, s, c, FALSE)
   #define CorMemcpyAlignSrc(d,s,c)    CorManMemcpy( d, s, c, TRUE)
#elif COR_95KERNEL
	#define CorMalloc( nBytes)				malloc( nBytes)
	#define CorFree(p)						free( p)
	#define CorCalloc( nItem, nBytes)   calloc( nItem, nBytes)
	#define CorRealloc( p, nBytes)      realloc( p, nBytes)
   #define CorMemcpy(d,s,c)            CorManMemcpy( d, s, c, FALSE)
   #define CorMemcpyAlignSrc(d,s,c)    CorManMemcpy( d, s, c, TRUE)
#elif COR_LINUX && defined(__KERNEL__)
	#include <GDrv_kernel.h>
	#define CorMalloc( nBytes)				vmalloc( nBytes)
	#define CorFree(p)						vfree( p)
	#define CorCalloc( nItem, nBytes)   CorMalloc( (nItem) * (nBytes))
	#define CorRealloc( p, nBytes)		Not implemented
   #define CorMemcpy(d,s,c)            CorManMemcpy( d, s, c, FALSE)
   #define CorMemcpyAlignSrc(d,s,c)    CorManMemcpy( d, s, c, TRUE)
#else
	#include <assert.h>

	#ifndef ASSERT
		#define ASSERT(a) assert((int)a)
	#endif

	#ifdef NOSTANDARD_ALLOC
		void *CorMalloc(UINT32 size);
		void CorFree(void *memblock);
	#else
		#if COR_WIN32
			#define CorMalloc( nBytes)				malloc( nBytes) // VirtualAlloc( NULL, (nBytes), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)
			#define CorFree(p)						free( p) // VirtualFree( (p), 0, MEM_RELEASE)
			#define CorCalloc( nItem, nBytes)	calloc( nItem, nBytes) // CorMalloc( (nItem) * (nBytes))
			#define CorRealloc( p, nBytes)		realloc( p, nBytes) // Not implemented
		#else
			#define CorMalloc( nBytes)				malloc( nBytes)
			#define CorFree(p)						free( p)
			#define CorCalloc( nItem, nBytes)   calloc( nItem, nBytes)
			#define CorRealloc( p, nBytes)		realloc( p, nBytes)
		#endif
	#endif

	#if COR_WIN32
		#define CorSignalEvent(event)		SetEvent(event)
      #define CorMemcpy(d,s,c)         CorManMemcpy( d, s, c, FALSE)
      #define CorMemcpyAlignSrc(d,s,c) CorManMemcpy( d, s, c, TRUE)
	#endif
	#if COR_C60
		#define CorSignalEvent(event)		CorManSetEvent(event)//_event_set((pointer)event,1)
      #define CorMemcpy(d,s,c)         memcpy( d, s, c, FALSE)
      #define CorMemcpyAlignSrc(d,s,c) memcpy( d, s, c)
	#endif
	#if COR_I960
		#define EVENT_MAGIC_NUMBER 0xCAFECAFE
		#define CorSignalEvent(event)    *(PUINT32)(event)= EVENT_MAGIC_NUMBER
      #define CorMemcpy(d,s,c)         memcpy( d, s, c)
      #define CorMemcpyAlignSrc(d,s,c) memcpy( d, s, c)
	#endif
	#if COR_IOP321
		#define EVENT_MAGIC_NUMBER 0xA55AA55A
		#define CorSignalEvent(event)    *(PUINT32)(event)= EVENT_MAGIC_NUMBER
      #define CorMemcpy(d,s,c)         memcpy( d, s, c)
      #define CorMemcpyAlignSrc(d,s,c) memcpy( d, s, c)
	#endif

#endif

#endif   // _CORLIB_H_
