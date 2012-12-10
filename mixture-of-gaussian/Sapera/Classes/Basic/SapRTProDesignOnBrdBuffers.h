#ifndef _SAPRTPRO_DESIGN_ONBOARD_BUFFERS_H_
#define _SAPRTPRO_DESIGN_ONBOARD_BUFFERS_H_

//	SapRTProDesignOnBrdBuffers.h : header file
//

#include "SapClassBasicDef.h"

#define PORTNUM_TO_PORTINDEX( portNum) ((portNum) & 0xffff)
#define PORTNUM_TO_PROCESSINGBLOCK( portNum) ( ((portNum) >> 16) & 0xffff)
#define PORTINDEX_PROCESSINGBLOCK_TO_PORTNUM(portIndex, blockIndex)   ((blockIndex<<16)&&0xffff0000) + (portIndex&0xffff)


//
// SapRTProDesignOnBrdBuffers class declaration
//
class SAPCLASSBASIC_CLASS SapRTProDesignOnBrdBuffers : public SapRTProDesign
{

public:
	// Constructor/Destructor
	SapRTProDesignOnBrdBuffers( SapLocation location = SapLocation::ServerSystem,
                               char *pLabel = NULL,
                               SapRTPro *pRTPro = NULL,
                               BOOL bCreateInputOnBrdBuffers = TRUE,
                               BOOL bCreateOutputOnBrdBuffers = TRUE);

	SapRTProDesignOnBrdBuffers(const SapRTProDesignOnBrdBuffers &rtProDesign);
	virtual ~SapRTProDesignOnBrdBuffers();

	SapRTProDesignOnBrdBuffers &operator=(const SapRTProDesignOnBrdBuffers &rtpro);

   BOOL CopyClass( const SapRTProDesignOnBrdBuffers *pSrcClass, SapRTProDesignOnBrdBuffers *pDstClass);

   //setting input/output from application.
   virtual BOOL SetInputSrc(   int inputIdx, 
                            SapXferNode *pSrcNode,
                            int srcPort = 0,
                            int relativeBufferIdx = 0,
                            int nbOnBrdBuffer = 2,
                            SapBuffer *pOnBrdBuffers = NULL);

   virtual BOOL SetInputSrc( int inputIdx, 
                             SapRTProDesignOnBrdBuffers *pRtProDesign, 
                             int outputIdx,
                             int relativeBufferIdx = 0);

   virtual BOOL SetOutputDst( int outputIdx,
                           SapXferNode *pDstNode,
                           int dstPort = 0, 
                           int nbOnBrdBuffers = 2,
                           SapBuffer *pOnBrdBuffers = NULL );

   virtual SapBuffer* GetOutputOnBrdBuffers( int outputIdx);
   virtual SapBuffer* GetInputOnBrdBuffers( int inputIdx);

   //must be implemented by derived class
   virtual BOOL Create( void);
   virtual BOOL Destroy( void);

protected:

   virtual BOOL CreateInputResources( void);
   virtual BOOL CreateInputResources( int inputIdx);

   virtual BOOL CreateOutputResources( void);
   virtual BOOL CreateOutputResources( int outputIdx);

   virtual BOOL DestroyInputResources( void);
   virtual BOOL DestroyInputResources( int inputIdx);

   virtual BOOL DestroyOutputResources( void);
   virtual BOOL DestroyOutputResources( int outputIdx);

   virtual BOOL AddInputXferPath( SapTransfer *pTransfer, int inputIdx);
   virtual BOOL AddOutputXferPath( SapTransfer *pTransfer, int outputIdx);

private:

   BOOL m_bCreateInputOnBrdBuffers;
   BOOL m_bCreateOutputOnBrdBuffers;

}; 

#endif	// _SAPPIXPRO_H_

