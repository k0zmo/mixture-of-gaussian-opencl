#ifndef _SAPTRANSFEREX_H_
#define _SAPTRANSFEREX_H_

//	SapTransferEx.h : header file
//

#include "SapClassBasicDef.h"

//
// SapAcqToBuf class declaration
//
class SAPCLASSBASIC_CLASS SapAcqToBuf : public SapTransfer
{
public:
	// Default Constructor
	SapAcqToBuf(SapAcquisition *pAcq, SapBuffer *pBuf, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pBuf));
	}
}; 

//
// SapCabToBuf class declaration
//
class SAPCLASSBASIC_CLASS SapCabToBuf : public SapTransfer
{
public:
	// Default Constructor
	SapCabToBuf(SapCab *pCab, SapBuffer *pBuf, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pCab, pBuf));
	}

	SapCabToBuf(SapCab *pCab, int srcPort, SapBuffer *pBuf, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pCab, srcPort, pBuf, 0));
	}
}; 

//
// SapBufToBuf class declaration
//
class SAPCLASSBASIC_CLASS SapBufToBuf : public SapTransfer
{
public:
	// Default Constructor
	SapBufToBuf(SapBuffer *pSrcBuf, SapBuffer *pDstBuf, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pSrcBuf, pDstBuf));
	}
}; 

//
// SapAcqToCab class declaration
//
class SAPCLASSBASIC_CLASS SapAcqToCab : public SapTransfer
{
public:
	// Default Constructor
	SapAcqToCab(SapAcquisition *pAcq, SapCab *pCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pCab, FALSE));
	}
	SapAcqToCab(SapAcquisition *pAcq, SapCab *pCab, int dstPort, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, 0, pCab, dstPort, FALSE));
	}
}; 

//
// SapCabToCab class declaration
//
class SAPCLASSBASIC_CLASS SapCabToCab : public SapTransfer
{
public:
	// Default Constructor
	SapCabToCab(SapCab *pSrcCab, SapCab *pDstCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pSrcCab, pDstCab, FALSE));
	}
}; 

//
// SapBufToCab class declaration
//
class SAPCLASSBASIC_CLASS SapBufToCab : public SapTransfer
{
public:
	// Default Constructor
	SapBufToCab(SapBuffer *pBuf, SapCab *pCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pBuf, pCab, FALSE));
	}
}; 

//
// SapAcqToBufThruDsp class declaration
//
class SAPCLASSBASIC_CLASS SapAcqToBufThruDsp : public SapTransfer
{
public:
	// Default Constructor
	SapAcqToBufThruDsp(SapAcquisition *pAcq, SapDsp *pDsp, SapBuffer *pBuf, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pDsp, FALSE));
		AddPair(SapXferPair(pDsp, pBuf));
	}
}; 

//
// SapAcqToBufThruPixPro class declaration
//
class SAPCLASSBASIC_CLASS SapAcqToBufThruPixPro : public SapTransfer
{
public:
	// Default Constructor
	SapAcqToBufThruPixPro(SapAcquisition *pAcq, SapPixPro *pPixPro, SapBuffer *pBuf, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pPixPro, FALSE));
		AddPair(SapXferPair(pPixPro, pBuf));
	}
}; 

//
// SapAcqToCabThruDsp class declaration
//
class SAPCLASSBASIC_CLASS SapAcqToCabThruDsp : public SapTransfer
{
public:
	// Default Constructor
	SapAcqToCabThruDsp(SapAcquisition *pAcq, SapDsp *pDsp, SapCab *pCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pDsp, FALSE));
		AddPair(SapXferPair(pDsp, pCab, FALSE));
	}
}; 

//
// SapAcqToCabThruPixPro class declaration
//
class SAPCLASSBASIC_CLASS SapAcqToCabThruPixPro : public SapTransfer
{
public:
	// Default Constructor
	SapAcqToCabThruPixPro(SapAcquisition *pAcq, SapPixPro *pPixPro, SapCab *pCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pPixPro, FALSE));
		AddPair(SapXferPair(pPixPro, pCab, FALSE));
	}
}; 

//
// SapAcqToBufCab class declaration
//
class SAPCLASSBASIC_CLASS SapAcqToBufCab : public SapTransfer
{
public:
	// Default Constructor
	SapAcqToBufCab(SapAcquisition *pAcq, SapBuffer *pBuf, SapCab *pCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pBuf));
		AddPair(SapXferPair(pAcq, pCab, FALSE));
	}
	SapAcqToBufCab(SapAcquisition *pAcq, SapBuffer *pBuf, SapCab *pCab, int dstPort, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pBuf));
		AddPair(SapXferPair(pAcq, 0, pCab, dstPort, FALSE));
	}
}; 

//
// SapCabToBufCab class declaration
//
class SAPCLASSBASIC_CLASS SapCabToBufCab : public SapTransfer
{
public:
	// Default Constructor
	SapCabToBufCab(SapCab *pSrcCab, SapBuffer *pBuf, SapCab *pDstCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pSrcCab, pBuf));
		AddPair(SapXferPair(pSrcCab, pDstCab, FALSE));
	}
	SapCabToBufCab(SapCab *pSrcCab, int srcPort, SapBuffer *pBuf, SapCab *pDstCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pSrcCab, srcPort, pBuf, 0));
		AddPair(SapXferPair(pSrcCab, srcPort, pDstCab, srcPort, FALSE));
	}
}; 

//
// SapBufToBufCab class declaration
//
class SAPCLASSBASIC_CLASS SapBufToBufCab : public SapTransfer
{
public:
	// Default Constructor
	SapBufToBufCab(SapBuffer *pSrcBuf, SapBuffer *pDstBuf, SapCab *pCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pSrcBuf, pDstBuf));
		AddPair(SapXferPair(pSrcBuf, pCab, FALSE));
	}
}; 

//
// SapAcqToBufCabThruDsp class declaration
//
class SAPCLASSBASIC_CLASS SapAcqToBufCabThruDsp : public SapTransfer
{
public:
	// Default Constructor
	SapAcqToBufCabThruDsp(SapAcquisition *pAcq, SapDsp *pDsp, SapBuffer *pBuf, SapCab *pCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pDsp, FALSE));
		AddPair(SapXferPair(pDsp, pBuf));
		AddPair(SapXferPair(pDsp, pCab, FALSE));
	}
}; 

//
// SapAcqToBufCabThruPixPro class declaration
//
class SAPCLASSBASIC_CLASS SapAcqToBufCabThruPixPro : public SapTransfer
{
public:
	// Default Constructor
	SapAcqToBufCabThruPixPro(SapAcquisition *pAcq, SapPixPro *pPixPro, SapBuffer *pBuf, SapCab *pCab, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
		AddPair(SapXferPair(pAcq, pPixPro, FALSE));
		AddPair(SapXferPair(pPixPro, pBuf));
		AddPair(SapXferPair(pPixPro, pCab, FALSE));
	}
}; 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SapMulti classes
//

//
// SapMultiAcqToBuf class declaration
//
class SAPCLASSBASIC_CLASS SapMultiAcqToBuf : public SapTransfer
{
public:
	// Default Constructor
	SapMultiAcqToBuf( SapAcquisition *pAcq[], SapBuffer *pBuf[], int numPairs, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
      AddPair(SapXferPair(pAcq[0], pBuf[0]));
      for (int i = 1; i < numPairs; i++)
         AddPair(SapXferPair(pAcq[i], pBuf[i]));
	}
}; 

//
// SapMultiAcqToCab class declaration
//
class SAPCLASSBASIC_CLASS SapMultiAcqToCab : public SapTransfer
{
public:
	// Default Constructor
	SapMultiAcqToCab( SapAcquisition *pAcq[], SapCab *pCab, int numPairs, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
      AddPair(SapXferPair(pAcq[0], 0, pCab, 0));
      for (int i = 1; i < numPairs; i++)
         AddPair(SapXferPair(pAcq[i], 0, pCab, i, FALSE));
	}
	// Overload Constructor
	SapMultiAcqToCab( SapAcquisition *pAcq[], SapCab *pCab, int dstPort[], int numPairs, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
      AddPair(SapXferPair(pAcq[0], 0, pCab, dstPort[0]));
      for (int i = 1; i < numPairs; i++)
         AddPair(SapXferPair(pAcq[i], 0, pCab, dstPort[i], FALSE));
	}
};

//
// SapMultiCabToBuf class declaration
//
class SAPCLASSBASIC_CLASS SapMultiCabToBuf : public SapTransfer
{
public:
	// Default Constructor
	SapMultiCabToBuf( SapCab *pCab, SapBuffer *pBuf[], int numPairs, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
      AddPair(SapXferPair(pCab, 0, pBuf[0], 0));
      for (int i = 1; i < numPairs; i++)
         AddPair(SapXferPair(pCab, i, pBuf[i], 0));
	}
	// Overload Constructor
	SapMultiCabToBuf( SapCab *pCab, int srcPort[], SapBuffer *pBuf[], int numPairs, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
      AddPair(SapXferPair(pCab, srcPort[0], pBuf[0], 0));
      for (int i = 1; i < numPairs; i++)
         AddPair(SapXferPair(pCab, srcPort[i], pBuf[i], 0));
	}
};


//
// SapMultiAcqToBufCab class declaration
//
class SAPCLASSBASIC_CLASS SapMultiAcqToBufCab : public SapTransfer
{
public:
	// Default Constructor
	SapMultiAcqToBufCab(SapAcquisition *pAcq[], SapBuffer *pBuf[], SapCab *pCab, int numPairs, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
      AddPair(SapXferPair(pAcq[0], pBuf[0]));
      AddPair(SapXferPair(pAcq[0],  0, pCab, 0, FALSE));
      for (int i = 1; i < numPairs; i++)
      {
         AddPair(SapXferPair(pAcq[i], pBuf[i]));
         AddPair(SapXferPair(pAcq[i], i, pCab, 0, FALSE));
		}
	}

	SapMultiAcqToBufCab(SapAcquisition *pAcq[], SapBuffer *pBuf[], SapCab *pCab, int dstPort[], int numPairs, SapXferCallback pCallback = NULL, void *pContext = NULL)
		: SapTransfer(pCallback, pContext)
	{
      AddPair(SapXferPair(pAcq[0], pBuf[0]));
      AddPair(SapXferPair(pAcq[0],  0, pCab, dstPort[0], FALSE));
      for (int i = 1; i < numPairs; i++)
      {
         AddPair(SapXferPair(pAcq[i], pBuf[i]));
         AddPair(SapXferPair(pAcq[i], 0, pCab, dstPort[i], FALSE));
      }
	}
}; 

#endif	// _SAPTRANSFEREX_H_

