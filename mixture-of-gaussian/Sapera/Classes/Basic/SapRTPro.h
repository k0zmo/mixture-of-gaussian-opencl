#ifndef _SAPRTPRO_H_
#define _SAPRTPRO_H_

//	SapRTPro.h : header file
//

#include "SapClassBasicDef.h"

//labels assigned to type of rtpro device.  
#define BASE_RTPRO_LABEL      "Base RtPro"      // base onboard RtPro device
#define EXPANSION_RTPRO_LABEL "Expansion RtPro" // expansion rtpro device   
#define CPU_RTPRO_LABEL       "Cpu RtPro"       // cpu based rtpro device

#define SAPRTPRO_MAX_NAME_SIZE 128

//
// SapCommCallbackInfo class declaration
//
class SAPCLASSBASIC_CLASS SapRTProMsgCallbackInfo
{
public:
   SapRTProMsgCallbackInfo(int command, void *inData= NULL, int inDataSize= 0, void *outData= NULL, int outDataSize= 0, void *context= NULL)
   {
      m_command     = command;
      m_inData      = inData;
      m_inDataSize  = inDataSize;
      m_outData     = outData;
      m_outDataSize = outDataSize;
		m_context     = context;
   }

   virtual ~SapRTProMsgCallbackInfo() {}

   int GetCommand()     const { return m_command; }
   void * GetInData()   const { return m_inData; }
   int GetInDataSize()  const { return m_inDataSize; }
   void * GetOutData()  const { return m_outData; }
   int GetOutDataSize() const { return m_outDataSize; }
	void * GetContext()  const { return m_context; }

protected:
   int    m_command;
   void * m_inData;
   int    m_inDataSize;
   void * m_outData;
   int    m_outDataSize;
	void * m_context;
};

typedef SAPSTATUS (CCONV *SapRTProMsgCallback)(SapRTProMsgCallbackInfo *);


//
// SapRTPro class declaration
//
class SAPCLASSBASIC_CLASS SapRTPro : public SapXferNode
{

public:

   // Buffer types
   enum RegAccessMode
   {
      RegAccessModeAutoIncrement    = CORPIXPRO_REGISTER_AUTO_INCREMENT,
      RegAccessModeNoIncrement      = CORPIXPRO_REGISTER_NO_INCREMENT,
   };

   enum RegAccessType
   {
      RegAccessTypeUint8     = CORPIXPRO_REGISTER_ACCESS_TYPE_UINT8,
      RegAccessTypeUint16    = CORPIXPRO_REGISTER_ACCESS_TYPE_UINT16,
      RegAccessTypeUint32    = CORPIXPRO_REGISTER_ACCESS_TYPE_UINT32,
      RegAccessTypeUint32_8  = CORPIXPRO_REGISTER_ACCESS_TYPE_UINT32_UINT8,
   };

   enum MsgPriority
   {
      MsgPriorityLow       = CORPIXPRO_VAL_MESSAGE_PRIORITY_LOW,
      MsgPriorityNormal    = CORPIXPRO_VAL_MESSAGE_PRIORITY_NORMAL,
      MsgPriorityHigh      = CORPIXPRO_VAL_MESSAGE_PRIORITY_HIGH,
   };

   enum MsgFlag
   {
      MsgFlagSynchronous      = CORPIXPRO_VAL_MESSAGE_FLAG_SYNCHRONOUS,
      MsgFlagAsynchornous     = CORPIXPRO_VAL_MESSAGE_FLAG_ASYNCHRONOUS,
   };

	// Constructor/Destructor
	SapRTPro(SapLocation loc = SapLocation::ServerSystem,
             SapRTProMsgCallback pCallback = NULL,
             void *pContext = NULL);

	SapRTPro(const SapRTPro &pixpro);
	virtual ~SapRTPro();

	SapRTPro &operator=(const SapRTPro &pixpro);

   //messaging functions
   virtual BOOL SendMessage( int cmd,
                             void *inData,
                             int inDataSize,
                             void *outData,
                             int outDataSize,
                             MsgPriority priority = MsgPriorityNormal ,
                             MsgFlag flag = MsgFlagSynchronous );

   virtual BOOL SetMsgCallbackInfo( SapRTProMsgCallback pCallback, void *context);
   
   //register read/write access.
   virtual BOOL WriteRegisterBlock( int fpgaId,
                                    int nbReg,
                                    int regOffset,
                                    void *pValues,
                                    RegAccessMode mode = RegAccessModeNoIncrement,
                                    RegAccessType accessType = RegAccessTypeUint32);

   virtual BOOL WriteRegisters( int fpgaId, int nbReg, CORPIXPRO_REG_SET *pRegs);

   //register read/write access.
   virtual BOOL ReadRegisterBlock( int fpgaId,
                                    int nbReg,
                                    int regOffset,
                                    void *pValues,
                                    RegAccessMode mode = RegAccessModeNoIncrement,
                                    RegAccessType accessType = RegAccessTypeUint32);

   virtual BOOL ReadRegisters( int fpgaId, int nbReg, CORPIXPRO_REG_SET *pRegs);


   virtual BOOL LoadFirmwareFromFile( int fpgaId, char *filename);

   virtual BOOL GetLabel( char *label, UINT32 maxSize);
   virtual BOOL GetDesignName( char *pName, UINT32 size);


   //override base implmentation.
   //removed init check, because rtpro paramaters can be changed if object has been created.
	virtual BOOL SetSrcNode(SapXferNode *pSrcNode, int srcPort = 0)				{ m_pSrcNode = pSrcNode; m_SrcPort = srcPort; return TRUE;}
	virtual BOOL SetXferParams(SapXferParams xferParams, int portIndex = 0)		{  m_XferParams[portIndex] = xferParams; return TRUE;}

   // Direct capability/parameter access
   virtual BOOL GetCapability(int cap, void *pValue);
   virtual BOOL GetParameter(int param, void *pValue);
   virtual BOOL SetParameter(int param, int value);
   virtual BOOL SetParameter(int param, void *pValue);
   
	// Module create/destroy
	virtual BOOL Create();
	virtual BOOL Destroy();

protected:

   static SAPSTATUS CCONV OnRTProMsgCallback(UINT32 command, void *inData, UINT32 inDataSize, void *outData, UINT32 outDataSize, void *context);

   int GetBytesPerReg( RegAccessType accessType);

   
//   static SapRTPro *m_pRTPro;

   SapRTProMsgCallback m_pRTProMsgCallback;
   void *m_pMsgContext;

}; 




#endif	// _SAPRTPRO_H_

