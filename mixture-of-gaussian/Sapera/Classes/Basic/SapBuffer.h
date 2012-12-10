#ifndef _SAPBUFFER_H_
#define _SAPBUFFER_H_

//	SapBuffer.h : header file
//

#include "SapClassBasicDef.h"

//
// SapBuffer class declaration
//
class SAPCLASSBASIC_CLASS SapBuffer : public SapXferNode
{
public:
   // Buffer types
   typedef int Type;    // For compatibility with old Sapera++ application code
   enum _Type
   {
      TypeDefault					 = -1,
      TypeVirtual					 = CORBUFFER_VAL_TYPE_VIRTUAL,
      TypeContiguous				 = CORBUFFER_VAL_TYPE_CONTIGUOUS,
      TypeScatterGather			 = CORBUFFER_VAL_TYPE_SCATTER_GATHER,
      TypeOffscreen				 = CORBUFFER_VAL_TYPE_OFFSCREEN,
      TypeOffscreenVideo		 = CORBUFFER_VAL_TYPE_OFFSCREEN | CORBUFFER_VAL_TYPE_VIDEO,
      TypeOverlay					 = CORBUFFER_VAL_TYPE_OVERLAY | CORBUFFER_VAL_TYPE_VIDEO,
      TypeDummy					 = CORBUFFER_VAL_TYPE_DUMMY,
      TypePhysical				 = CORBUFFER_VAL_TYPE_PHYSICAL,
		TypeScatterGatherPhysical= CORBUFFER_VAL_TYPE_SCATTER_GATHER | CORBUFFER_VAL_TYPE_PHYSICAL,
      TypeUnmapped             = CORBUFFER_VAL_TYPE_UNMAPPED,
		TypeScatterGatherUnmapped= CORBUFFER_VAL_TYPE_SCATTER_GATHER | CORBUFFER_VAL_TYPE_UNMAPPED
   };

   // Buffer states
   typedef int State;
   enum _State
   {
      StateEmpty    = CORBUFFER_VAL_STATE_EMPTY,
      StateOverflow = CORBUFFER_VAL_STATE_OVERFLOW,
      StateFull     = CORBUFFER_VAL_STATE_FULL
   };

   // Bayer filter options
   typedef int BayerAlign;
   enum _BayerAlign
   {
      BayerAlignGBRG = CORBUFFER_VAL_BAYER_ALIGN_GB_RG,
      BayerAlignBGGR = CORBUFFER_VAL_BAYER_ALIGN_BG_GR,
      BayerAlignRGGB = CORBUFFER_VAL_BAYER_ALIGN_RG_GB,
      BayerAlignGRBG = CORBUFFER_VAL_BAYER_ALIGN_GR_BG
   };

   typedef int BayerMethod;
   enum _BayerMethod
   {
      BayerMethod1 = CORBUFFER_VAL_BAYER_METHOD_1,
      BayerMethod2 = CORBUFFER_VAL_BAYER_METHOD_2,
      BayerMethod3 = CORBUFFER_VAL_BAYER_METHOD_3,
      BayerMethod4 = CORBUFFER_VAL_BAYER_METHOD_4,

      BayerMethodInterpolate = CORBUFFER_VAL_BAYER_METHOD_1,
      BayerMethodGoldenEagle = CORBUFFER_VAL_BAYER_METHOD_2,
      BayerMethodEagle       = CORBUFFER_VAL_BAYER_METHOD_3
   };

public:
	// Constructor/Destructor
	SapBuffer(int count = SapDefBufferCount, int width = SapDefWidth, int height = SapDefHeight, SapFormat format = SapDefFormat, Type type = SapDefBufferType, SapLocation loc = SapLocation::ServerSystem);
	SapBuffer(int count, int physAddress[], int width = SapDefWidth, int height = SapDefHeight, SapFormat format = SapDefFormat, Type type = TypeContiguous);
	SapBuffer(int count, void *virtAddress[], int width = SapDefWidth, int height = SapDefHeight, SapFormat format = SapDefFormat, Type type = TypeScatterGather);
	SapBuffer(int count, SapXferNode *pSrcNode, Type type = SapDefBufferType, SapLocation loc = SapLocation::ServerSystem);
	SapBuffer(const char *fileName, Type type = SapDefBufferType, SapLocation loc = SapLocation::ServerSystem);
   SapBuffer(int count, const char *bufName, int width = SapDefWidth, int height = SapDefHeight, SapFormat format = SapDefFormat, Type type = TypeScatterGather, SapLocation loc= SapLocation::ServerSystem);
   SapBuffer(int count, const char *bufName, SapXferNode *pSrcNode, Type type = SapDefBufferType, SapLocation loc= SapLocation::ServerSystem);
   SapBuffer(const char *bufName, int startIndex, int count, Type type = TypeVirtual, SapLocation loc= SapLocation::ServerSystem);
	SapBuffer(const SapBuffer &buf);
	virtual ~SapBuffer();

   SapBuffer &operator= (const SapBuffer &buf);

	// Module create/destroy
	virtual BOOL Create();
	virtual BOOL Destroy();

	// Access to implementation
	int GetCount()         const { return m_Count; }
	int GetWidth()         const { return m_XferParams[0].GetWidth(); }
	int GetHeight()        const { return m_XferParams[0].GetHeight(); }
	SapFormat GetFormat()  const { return m_XferParams[0].GetFormat(); }
	Type GetType()         const { return m_Type; }
	int GetPixelDepth()    const { return m_PixelDepth; }
   int GetBytesPerPixel() const;
   int GetPitch()         const;
	void GetParameters(int *count, int *width, int *height, SapFormat *format, Type *type) const;
	const  char* GetBufName() const { return m_BufName;}

	virtual BOOL SetCount(int count);	
	virtual BOOL SetWidth(int width);		
	virtual BOOL SetHeight(int height);	
	virtual BOOL SetFormat(SapFormat format);	
	virtual BOOL SetType(Type type);		
	virtual BOOL SetPixelDepth(int pixelDepth);	
	virtual BOOL SetVirtualAddress( void* virtAddress[]);
	virtual BOOL SetPhysicalAddress( int physAddress[]);
	virtual BOOL SetParameters(int count, int width, int height, SapFormat format, Type type);
	virtual BOOL SetParameters(int count, void* virtAddress[], int width, int height, SapFormat format, Type type);
	virtual BOOL SetParameters(int count, int physAddress[], int width, int height, SapFormat format, Type type);
	virtual BOOL SetParametersFromFile(const char *fileName, Type type);

	float GetFrameRate() const { return m_FrameRate; }
	virtual void SetFrameRate(float frameRate) { m_FrameRate = frameRate; }

   virtual BOOL GetAddress( void **pData);
   virtual BOOL GetAddress( void* virtualBaseAddress, void **pData);

   virtual BOOL GetAddress(int index, void **pData);
   virtual BOOL GetAddress(int index, void* virtualBaseAddress, void **pData);

	virtual BOOL GetAddress(int offset, int size, void **pData);
	virtual BOOL GetAddress(int offset, int size, void* virtualBaseAddress, void **pData);

	virtual BOOL GetAddress(int index, int offset, int size, void **pData);
	virtual BOOL GetAddress(int index, int offset, int size, void* virtualBaseAddress, void **pData);

   virtual BOOL ReleaseAddress(void *pData);
   virtual BOOL ReleaseAddress(int index, void *pData= NULL);

	virtual BOOL IsMapped( int index);
	virtual BOOL IsMapped();

   virtual BOOL GetCounterStamp(int *pCounterStamp);
   virtual BOOL GetCounterStamp(int index, int *pCounterStamp);

   virtual CORHANDLE *GetHandles()          const { return m_bInitOK ? m_pBuffers : NULL; }
	virtual CORHANDLE GetHandle(int index)	  const;
	virtual CORHANDLE GetHandle()	           const { return GetHandle(m_Index); }
	virtual CORHANDLE operator[] (int index) const { return GetHandle(index); }

	virtual int GetIndex() const { return m_Index; }
	virtual BOOL SetIndex(int index);
	virtual void ResetIndex() { m_Index = m_Count - 1; }
	virtual void Next() { m_Index++; if (m_Index == m_Count) m_Index = 0; }

   virtual BOOL GetCapability(int cap, void *pValue);
   virtual BOOL GetCapability(int index, int cap, void *pValue);
   virtual BOOL GetParameter(int param, void *pValue);
   virtual BOOL GetParameter(int index, int param, void *pValue);
   virtual BOOL SetParameter(int param, int value);
   virtual BOOL SetParameter(int index, int param, int value);
   virtual BOOL SetParameter(int param, void *pValue);
   virtual BOOL SetParameter(int index, int param, void *pValue);

   virtual BOOL IsBuffer() { return TRUE; }

	// Access to derived class implementation
   virtual CORBUFFER GetTrash()   const { return CORHANDLE_NULL; }
   virtual SapBuffer *GetParent()       { return this; }
   virtual SapBuffer *GetRoot()         { return this; }

	// State manipulation
	virtual BOOL SetState(State state);
	virtual BOOL SetState(int index, State state);
	virtual BOOL SetAllState(State state);
	virtual BOOL GetState(State *pState);
	virtual BOOL GetState(int index, State *pState);

	// Buffer manipulation
	virtual BOOL Clear();
	virtual BOOL Clear(int index);
	virtual BOOL Clear(SapData value);
	virtual BOOL Clear(int index, SapData value);

	virtual BOOL Load(const char *fileName, int bufIndex, int numBuffers = 0, int frameIndex = 0, const char *options = "-format auto");
	virtual BOOL Save(const char *fileName, const char *options, int bufIndex = -1, int numBuffers = 0);

   virtual BOOL Read(int offset, int numElements, void *pData);
   virtual BOOL Read(int index, int offset, int numElements, void *pData);
   virtual BOOL ReadElement(int x, int y, void *pData);
   virtual BOOL ReadElement(int index, int x, int y, void *pData);
   virtual BOOL ReadElement(int x, int y, SapData *pData);
   virtual BOOL ReadElement(int index, int x, int y, SapData *pData);
   virtual BOOL ReadLine(int x1, int y1, int x2, int y2, void *pData, int *numRead);
   virtual BOOL ReadLine(int index, int x1, int y1, int x2, int y2, void *pData, int *numRead);
   virtual BOOL ReadRect(int x, int y, int width, int height, void *pData);
   virtual BOOL ReadRect(int index, int x, int y, int width, int height, void *pData);

   virtual BOOL Write(int offset, int numElements, const void *pData);
   virtual BOOL Write(int index, int offset, int numElements, const void *pData);
   virtual BOOL WriteElement(int x, int y, const void *pData);
   virtual BOOL WriteElement(int index, int x, int y, const void *pData);
   virtual BOOL WriteElement(int x, int y, SapData data);
   virtual BOOL WriteElement(int index, int x, int y, SapData data);
   virtual BOOL WriteLine(int x1, int y1, int x2, int y2, const void *pData, int *numWritten);
   virtual BOOL WriteLine(int index, int x1, int y1, int x2, int y2, const void *pData, int *numWritten);
   virtual BOOL WriteRect(int x, int y, int width, int height, const void *pData);
   virtual BOOL WriteRect(int index, int x, int y, int width, int height, const void *pData);

	virtual BOOL Copy(SapBuffer *pSrc);
	virtual BOOL Copy(SapBuffer *pSrc, int srcIndex, int dstIndex);
	virtual BOOL CopyAll(SapBuffer *pSrc);

   virtual BOOL BayerConvert(SapBuffer *pSrc, BayerAlign align, BayerMethod method, SapDataFRGB wbCoef, SapLut *pLut = NULL);
   virtual BOOL BayerConvert(SapBuffer *pSrc, int srcIndex, int dstIndex, BayerAlign align, BayerMethod method, SapDataFRGB wbCoef, SapLut *pLut = NULL);
   virtual BOOL BayerWhiteBalance(BayerAlign align, SapDataFRGB *pWbCoef);
   virtual BOOL BayerWhiteBalance(int index, BayerAlign align, SapDataFRGB *pWbCoef);

   virtual BOOL SplitComponents(SapBuffer *pSrc);
   virtual BOOL SplitComponents(SapBuffer *pSrc, int srcIndex);
   virtual BOOL MergeComponents(SapBuffer *pSrc);
   virtual BOOL MergeComponents(SapBuffer *pSrc, int dstIndex);

	// Remote access
	virtual BOOL Register(const char *name);
	virtual BOOL Unregister();

	// Obsolete state manipulation methods
	virtual BOOL Empty()          { return SetState(m_Index, StateEmpty); }
	virtual BOOL Empty(int index) { return SetState(index, StateEmpty); }
	virtual BOOL EmptyAll()       { return SetAllState(StateEmpty); }

   // Obsolete methods, use class SapBufferRoi instead
	virtual int GetNumRoi() const { return 0; }
	virtual const LPRECT GetRoi(int index) const { return NULL; }
	virtual BOOL AddRoi(RECT roi) { return FALSE; }
	virtual BOOL RemoveAllRoi() { return FALSE; }
	virtual CORBUFFER GetChild(int roiIndex, int bufIndex) const { return CORHANDLE_NULL; }

   // Obsolete prototypes for these methods
	virtual BOOL Load(const char *fileName, const char *options = "-format auto", int index = -1, int numFrames = 0);
	virtual BOOL ReadElement(int x, int y, int *red, int *green, int *blue);

protected:
   // Utility methods
   virtual void Construct(const char *bufName);
   void UpdatePixelDepth(int index);
   virtual BOOL IsCapabilityValid(int cap);
	static int GetFileFormatFromOptions(const char *options);

protected:
	// Buffer-related variables
	int m_Count;             // Number of buffers
	Type m_Type;             // Buffer type
	int m_Index;             // Index of last grabbed buffer
   BOOL m_PixelDepthCap;    // Pixel depth capability
   int m_PixelDepth;        // Number of actual data bits per pixel
   int m_DataSize;          // Number of bytes per pixel
   int m_Pitch;             // Byte offset between start addresses of consecutive lines
   int *m_PhysAddress;      // Physical addresses for creating buffers
   void **m_VirtAddress;    // Virtual addresses for creating buffers
   void **m_LockAddress;	 // Addresses of locked buffers ( video, physical)

	// Shared buffer related variables
   char			m_BufName[CORHANDLE_MAX_STRLEN];	// Name of the shared buffer objects
	CORBUFFER*	m_pRemoteBuffers;
	CORBUFFER	m_BufferSharedInfo;					// Buffer handle to shared buffer information
   int			m_StartIndex;

	// File-related variables
	float m_FrameRate;      // Number of frames per second

	// Sapera handles
	CORBUFFER *m_pBuffers;					// Table of buffer handles

   // Classes which can directly access protected methods and members
   friend class SapBufferRoi;
}; 

#endif	// _SAPBUFFER_H_
