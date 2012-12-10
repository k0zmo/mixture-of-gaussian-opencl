#ifndef _SAPRTPRO_DESIGN_H_
#define _SAPRTPRO_DESIGN_H_

//	SapRTProDesign.h : header file
//

#include "SapClassBasicDef.h"

#define SAPRTPRO_MAX_INPUTS  32
#define SAPRTPRO_MAX_OUTPUTS 32


class SAPCLASSBASIC_CLASS SapXferContextInfo;
class SAPCLASSBASIC_CLASS SapTransfer;
class SAPCLASSBASIC_CLASS SapXferCallbackInfo;
typedef void (*SapXferCallback)(SapXferCallbackInfo *);

class SAPCLASSBASIC_CLASS SapRTProDesignInOutParam
{
   public:
	// Constructor/destructor
	SapRTProDesignInOutParam( SapXferNode *pSrcNode = NULL,
                             int srcPort = 0,
                             SapXferNode *pDstNode = NULL,
                             int dstPort = 0,
                             UINT32 nbOnBrdBuffers = 1,
                             int relativeBufferIdx = 0, 
                             SapBuffer *pOnBrdbuffers = NULL,
                             BOOL bOwnOnBrdBuffers = FALSE)
   {
      m_pSrcNode = pSrcNode;
      m_SrcPort = srcPort;
      m_pDstNode = pDstNode;
      m_dstPort = dstPort;
      m_nbOnBrdBuffers = nbOnBrdBuffers;
      m_relativeBufferIdx = relativeBufferIdx;
      m_pOnBrdBuffers = pOnBrdbuffers;
      m_bOwnOnBrdBuffers = bOwnOnBrdBuffers;
      m_pXferCallBack = NULL;
      m_pXferContext = NULL;
   }

	SapRTProDesignInOutParam(const SapRTProDesignInOutParam &param)
   {
      //use the = operator.
      *this = param;
   }

   virtual ~SapRTProDesignInOutParam(){  return;}

	SapRTProDesignInOutParam &operator=(const SapRTProDesignInOutParam &param)
   {
      m_pSrcNode = param.m_pSrcNode;
      m_SrcPort = param.m_SrcPort;
      m_pDstNode = param.m_pDstNode;
      m_dstPort = param.m_dstPort;
      m_nbOnBrdBuffers = param.m_nbOnBrdBuffers;
      m_relativeBufferIdx = param.m_relativeBufferIdx;
      m_pOnBrdBuffers = param.m_pOnBrdBuffers;
      m_bOwnOnBrdBuffers = param.m_bOwnOnBrdBuffers;
      m_pXferCallBack = param.m_pXferCallBack;
      m_pXferContext = param.m_pXferContext;

      return *this;
   }

   void SetSrcNode(SapXferNode* pSrcNode)  { m_pSrcNode = pSrcNode;}
   void SetSrcPort(int srcPort)           { m_SrcPort = srcPort;}
   void SetDstNode(SapXferNode *pDstNode)   { m_pDstNode = pDstNode;}
   void SetDstPort(int dstPort)           { m_dstPort = dstPort;}

   void SetNbOnBrdBuffers(int nbOnBrdBuffers)     { m_nbOnBrdBuffers = nbOnBrdBuffers;}
   void SetRelativeBufferIdx(int relativeBufferIdx)       { m_relativeBufferIdx = relativeBufferIdx;}
   void SetOnBrdBuffers(SapBuffer *pOnBrdBuffers)     { m_pOnBrdBuffers = pOnBrdBuffers;}
   void SetOnBrdBufferOwner(BOOL bOwnOnBrdBuffers)   { m_bOwnOnBrdBuffers = bOwnOnBrdBuffers;}

	void SetXferCallbackInfo( SapXferCallback pCallback, void *pContext= NULL){ m_pXferCallBack = pCallback; m_pXferContext = pContext;}

   SapXferNode *GetSrcNode()  { return m_pSrcNode;}
   int GetSrcPort()           { return m_SrcPort;}
   SapXferNode *GetDstNode()   { return m_pDstNode;}
   int GetDstPort()           { return m_dstPort;}

   int GetNbOnBrdBuffers()     { return m_nbOnBrdBuffers;}
   int GetRelativeBufferIdx()       { return m_relativeBufferIdx;}
   SapBuffer *GetOnBrdBuffers()     { return m_pOnBrdBuffers;}
   BOOL GetOnBrdBufferOwner()   { return m_bOwnOnBrdBuffers;}

   
   SapXferCallback GetXferCallback( ){ return m_pXferCallBack;}
   void *GetXferContext( ) { return m_pXferContext;}


   protected:

      //for xfernode connections.
      SapXferNode   *m_pSrcNode;   
      int           m_SrcPort;
      SapXferNode   *m_pDstNode;    
      int           m_dstPort;
   
      
      int           m_nbOnBrdBuffers;     
      int           m_relativeBufferIdx;
      SapBuffer     *m_pOnBrdBuffers;   

      BOOL          m_bOwnOnBrdBuffers;  // True if onboard buffers were created by object to complete connection.

      SapXferCallback m_pXferCallBack;
      void        *m_pXferContext;


};

//
// SapRTProDesign class declaration
//
class SAPCLASSBASIC_CLASS SapRTProDesign : public SapManager
{

public:
	// Constructor/Destructor
	SapRTProDesign( SapLocation location = SapLocation::ServerSystem, char* pLabel = NULL, SapRTPro *pRTPro = NULL);

	SapRTProDesign(const SapRTProDesign &rtProDesign);
	virtual ~SapRTProDesign();

	SapRTProDesign &operator=(const SapRTProDesign &rtpro);

   BOOL CopyClass( const SapRTProDesign *pSrcClass, SapRTProDesign *pDstClass);

   SapRTPro *GetRTPro( void)
   {
      return m_pRTPro;
   }
	SapLocation GetLocation()		const { return m_Location; }

   BOOL GetDefaultFirmwareFilename( char *pName, UINT32 size);
   BOOL SetDefaultFirmwareFilename( char *pName);

   BOOL GetDesignName( char *pName, UINT32 size);
   BOOL SetDesignName( char *pName);

   BOOL LoadDefaultFirmware( void);

   //setting input/output from application.
   virtual BOOL SetInputSrc(   int inputIdx, 
                            SapXferNode *pSrcNode,
                            int srcPort = 0,
                            int relativeBufferIdx = 0);

   virtual BOOL SetInputSrc( int inputIdx, 
                             SapRTProDesign *pRtProDesign, 
                             int outputIdx,
                             int relativeBufferIdx = 0);

   virtual BOOL SetOutputDst( int outputIdx,
                           SapXferNode *pDstNode,
                           int dstPort = 0);

	virtual BOOL SetXferCallbackInfo( SapXferCallback pCallback, void *pContext= NULL){ if( m_bInitOK) return FALSE; m_pXferCallBack = pCallback; m_pXferContext = pContext; return TRUE;}
	virtual BOOL SetOutputXferCallbackInfo( UINT32 ouputIdx,  SapXferCallback pCallback, void *pContext= NULL); 

   virtual SapXferCallback GetXferCallback() const { return m_pXferCallBack; }


   virtual BOOL SetMsgCallbackInfo( SapRTProMsgCallback pCallback, void *context);

   //this function must be implemented by the derived class.
   virtual BOOL SetCustomDesignParameters( void)  = NULL;

   SapXferParams  GetInputXferParam( int inputIdx);
   BOOL           SetOutputXferParam( int outputIdx, SapXferParams xferParam);
   SapXferNode    *GetInputSrcNode( int inputIdx);
   SapXferNode    *GetOutputDstNode( int outputIdx);

   virtual BOOL AddXferPaths( SapTransfer *pTransfer);
   virtual BOOL SetInOutParameters(SapTransfer *pTransfer );


   //must be implemented by derived class
   virtual BOOL Create( void);
   virtual BOOL Destroy( void);

protected:
	SapLocation		m_Location;


   SapRTPro  *m_pRTPro;       //realtime pro object.
   BOOL        m_bOwnRTPro;         //true if realtime pro was created internally by this object.

   //messaging callback info
   SapRTProMsgCallback m_pRTProMsgCallback;
   void *m_pMsgContext;

   SapXferCallback m_pXferCallBack;
   void        *m_pXferContext;

   char        m_DefaultFirmwareFilename[ SAPRTPRO_MAX_NAME_SIZE];
   char        m_DesignName[ SAPRTPRO_MAX_NAME_SIZE];

   SapRTProDesignInOutParam *m_pInputInfo[SAPRTPRO_MAX_INPUTS];
   SapRTProDesignInOutParam *m_pOutputInfo[SAPRTPRO_MAX_OUTPUTS];

   //internal functions to manage connections.
   virtual BOOL DefineInput( int inputIdx, SapXferNode *pDstNode, int dstPort);
   virtual BOOL UndefineInput(  int inputIdx );

   virtual BOOL DefineOutput(  int outputIdx, SapXferNode *pSrcNode, int srcPort);
   virtual BOOL UndefineOutput( int outputIdx );

   virtual BOOL AddAllInputXferPaths( SapTransfer *pTransfer);
   virtual BOOL AddInputXferPath( SapTransfer *pTransfer, int inputIdx);

   virtual BOOL AddAllOutputXferPaths( SapTransfer *pTransfer);
   virtual BOOL AddOutputXferPath( SapTransfer *pTransfer, int outputIdx);

   virtual BOOL SetAllInputParamsRelIndex(SapTransfer *pTransfer);
   virtual BOOL SetInputParamsRelIndex(SapTransfer *pTransfer,int inputIdx);


   virtual SAPSTATUS InternalMsgCommand( SapRTProMsgCallbackInfo *pCallbackInfo);

   // Internal callback management
   static SAPSTATUS CCONV OnRTProMsgCallback( SapRTProMsgCallbackInfo *pCallbackInfo);

}; 

#endif	// _SAPPIXPRO_H_

