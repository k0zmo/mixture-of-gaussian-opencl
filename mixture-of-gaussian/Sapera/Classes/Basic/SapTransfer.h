#ifndef _SAPTRANSFER_H_
#define _SAPTRANSFER_H_

//	SapTransfer.h : header file
//

#include "SapClassBasicDef.h"

// Forward declarations
class SAPCLASSBASIC_CLASS SapXferContextInfo;
class SAPCLASSBASIC_CLASS SapTransfer;
class SAPCLASSBASIC_CLASS SapXferCallbackInfo;
typedef void (*SapXferCallback)(SapXferCallbackInfo *);

#define MAX_RTPRODESIGNS_IN_TRANSFER  16

//
// SapXferPair class declaration
//
class SAPCLASSBASIC_CLASS SapXferPair
{
public:
   // Transfer event types
   typedef int EventType;     // For compatibility with old Sapera++ application code
   enum _EventType
   {
      EventNone          = 0,
      EventStartOfField  = CORXFER_VAL_EVENT_TYPE_START_OF_FIELD,
      EventStartOfOdd    = CORXFER_VAL_EVENT_TYPE_START_OF_ODD,
      EventStartOfEven   = CORXFER_VAL_EVENT_TYPE_START_OF_EVEN,
      EventStartOfFrame  = CORXFER_VAL_EVENT_TYPE_START_OF_FRAME,
      EventEndOfField    = CORXFER_VAL_EVENT_TYPE_END_OF_FIELD,
      EventEndOfOdd      = CORXFER_VAL_EVENT_TYPE_END_OF_ODD,
      EventEndOfEven     = CORXFER_VAL_EVENT_TYPE_END_OF_EVEN,
      EventEndOfFrame    = CORXFER_VAL_EVENT_TYPE_END_OF_FRAME,
      EventEndOfLine     = CORXFER_VAL_EVENT_TYPE_END_OF_LINE,
      EventEndOfNLines   = CORXFER_VAL_EVENT_TYPE_END_OF_NLINES,
      EventEndOfTransfer = CORXFER_VAL_EVENT_TYPE_END_OF_TRANSFER,
      EventLineUnderrun  = CORXFER_VAL_EVENT_TYPE_LINE_UNDERRUN,
      EventFieldUnderrun = CORXFER_VAL_EVENT_TYPE_FIELD_UNDERRUN,
      EventMask          = CORXFER_VAL_EVENT_TYPE_MASK
   };

   // Transfer cycle modes
   typedef int CycleMode;
   enum _CycleMode
   {
      CycleUnknown       = -1,
      CycleAsynchronous  = CORXFER_VAL_CYCLE_MODE_ASYNCHRONOUS,
      CycleSynchronous   = CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS,
      CycleWithTrash     = CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_WITH_TRASH,
      CycleOff           = CORXFER_VAL_CYCLE_MODE_OFF,
      CycleNextEmpty     = CORXFER_VAL_CYCLE_MODE_NEXT_EMPTY,
      CycleNextWithTrash = CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_NEXT_EMPTY_WITH_TRASH
   };

   // Flipping modes
   typedef int FlipMode;
   enum _FlipMode
   {
      FlipNone       = CORXFER_VAL_FLIP_OFF,
      FlipHorizontal = CORXFER_VAL_FLIP_HORZ,
      FlipVertical   = CORXFER_VAL_FLIP_VERT
   };

   // Event count source
   enum EventCountSource
   {
      EventCountNone = CORXFER_VAL_EVENT_COUNT_SOURCE_NONE,
      EventCountDst  = CORXFER_VAL_EVENT_COUNT_SOURCE_DST,
      EventCountSrc  = CORXFER_VAL_EVENT_COUNT_SOURCE_SRC
   };

   // Counter stamp time base
   enum CounterStampTimeBase
   {
      CounterStampMicroSec    = CORXFER_VAL_TIME_BASE_US,
      CounterStampMilliSec    = CORXFER_VAL_TIME_BASE_MS,
      CounterStampLine        = CORXFER_VAL_TIME_BASE_LINE,
      CounterStampLineTrigger = CORXFER_VAL_TIME_BASE_LINE_TRIGGER,
      CounterStampFrame       = CORXFER_VAL_TIME_BASE_FRAME,
      CounterStampExtFrameTrigger = CORXFER_VAL_TIME_BASE_EXT_FRAME_TRIGGER
   };

public:
   SapXferPair() 
      { Construct(NULL, 0, NULL, 0, FALSE); }

   SapXferPair(SapXferNode *pSrc, SapXferNode *pDst, BOOL regCallback = TRUE)
      { Construct(pSrc, 0, pDst, 0, regCallback); }

   SapXferPair(SapXferNode *pSrc, int srcPort, SapXferNode *pDst, int dstPort, BOOL regCallback = TRUE)
      { Construct(pSrc, srcPort, pDst, dstPort, regCallback); }

   ~SapXferPair() {}

   SapXferNode *GetSrc() const { return m_pSrc; } 
   int GetSrcPort()      const { return m_SrcPort; }
   int GetSrcIndex()     const { return m_SrcPort; }
   SapXferNode *GetDst() const { return m_pDst; }
   int GetDstPort()      const { return m_DstPort; }

   BOOL IsRegCallback()          const { return m_bRegCallback; }
   SapXferCallback GetCallback() const { return m_pCallback; }
   void *GetContext()            const { return m_pContext; }

	int GetFramesPerCallback()    const { return m_FramesPerCallback;}
	int GetFramesOnBoard()			const { return m_FramesOnBoard;}

   EventType GetEventType() const { return m_EventType; }
   EventCountSource GetEventCountSource() const { return m_EventCountSource; }
   
	CycleMode GetCycleMode() const 
	{ 
			CycleMode cycleMode= m_CycleMode;
		   if( cycleMode == SapXferPair::CycleUnknown)
			{
				// Determine default mode if none was set by the application
				cycleMode = SapXferPair::CycleAsynchronous;
				if ( GetDst()->GetTrash() != CORHANDLE_NULL)
					cycleMode = SapXferPair::CycleWithTrash;
			}

			return cycleMode; 
	}

   FlipMode GetFlipMode()   const 
		{ 	return m_FlipMode; }
   
	CounterStampTimeBase GetCounterStampTimeBase() const 
		{ 	return m_CounterStampTimeBase; }

   BOOL SetCallbackInfo(SapXferCallback pCallback, void *pContext)
		{ 	m_pCallback = pCallback; m_pContext = pContext; return TRUE; }

	BOOL SetEventType(EventType eventType)		
		{ 	m_EventType = eventType; return TRUE; }
   
	BOOL SetEventCountSource(EventCountSource eventCountSource) 
		{ 	m_EventCountSource = eventCountSource; return TRUE; }
	
	BOOL SetCycleMode(CycleMode cycleMode)		
		{ m_CycleMode = cycleMode; return TRUE; }
	
	BOOL SetFlipMode(FlipMode flipMode)			
		{ m_FlipMode = flipMode; return TRUE; }
   
	BOOL SetCounterStampTimeBase(CounterStampTimeBase counterStampTimeBase) 
		{ m_CounterStampTimeBase = counterStampTimeBase; return TRUE; }
	
	BOOL SetFramesPerCallback(int numFrames)  
	{ 
		int nDstCount= GetDst()->GetCount();

		if( numFrames > nDstCount/2)
			numFrames= nDstCount/2 + ((nDstCount&1)? 1: 0);

		if (numFrames > 0) 
			m_FramesPerCallback = numFrames; 
		
		return numFrames > 0; 
	}

	BOOL SetFramesOnBoard(int numFrames)		
	{ 
		if (numFrames > 1) 
			m_FramesOnBoard= numFrames; 

		return numFrames > 1; 
	}

protected:
	// Utility methods
   void Construct(SapXferNode *pSrc, int srcPort, SapXferNode *pDst, int dstPort, BOOL regCallback)
   {
      m_pSrc    = pSrc;
      m_SrcPort = srcPort;
      m_pDst    = pDst;
      m_DstPort = dstPort;

      m_bRegCallback			= regCallback;
      m_pCallback				= NULL;
      m_pContext				= NULL;

		m_FramesPerCallback	= 1;
		m_FramesOnBoard		= 2;

      m_EventType = EventEndOfFrame;
      m_EventCountSource = EventCountDst;
      m_CycleMode = CycleUnknown;
      m_FlipMode  = FlipNone;
      m_CounterStampTimeBase = CounterStampMicroSec;
   }

protected:
   // Source/destination node information
   SapXferNode*	m_pSrc;
   int				m_SrcPort;
   SapXferNode*	m_pDst;
   int				m_DstPort;

   // Callback information
   BOOL            m_bRegCallback;
   SapXferCallback m_pCallback;     // Overrides SapTransfer callback if specified
   void*				 m_pContext;		// Relevant only if overriding SapTransfer callback

   // Transfer parameters
   EventType m_EventType;				// Current transfer event type
   EventCountSource m_EventCountSource;   // Current source of event count
   CycleMode m_CycleMode;				// Current buffer cycle mode
   FlipMode  m_FlipMode;				// Current flipping mode
   CounterStampTimeBase m_CounterStampTimeBase; // Current counter-stamp time base
	int		 m_FramesPerCallback;	// Current number of frames per callback
	int		 m_FramesOnBoard;			// Current number of onboard frames
};

//
// SapXferPairInfo class declaration
//
class SAPCLASSBASIC_CLASS SapXferPairInfo
{
public:
	SapXferPairInfo() 
		{ m_Index = 0; m_pXfer = NULL; m_context = NULL; }
	SapXferPairInfo(SapXferPair &pair, int index, SapTransfer *pXfer) 
		{ m_Pair = pair; m_Index = index; m_pXfer = pXfer; m_context = NULL; }

public:
	SapXferPair	         m_Pair;		// Transfer pair
	int			         m_Index;		// Index in the table
	SapTransfer*         m_pXfer;		// Corresponding transfer object
	SapXferContextInfo*  m_context;
   int                  m_MaxSnapCount;   // Maximum frame count for Snap
};

//
// SapXferContextInfo class declaration
//
class SAPCLASSBASIC_CLASS SapXferContextInfo
{
public:
	SapXferContextInfo()
		{ m_PairInfo= NULL; m_dstIndex= 0; }
	SapXferContextInfo( SapXferPairInfo *pPairInfo, int dstIndex)
		{ m_PairInfo= pPairInfo; m_dstIndex= dstIndex;}

public:
	SapXferPairInfo* m_PairInfo;
	int		        m_dstIndex;
};

//
// SapXferCounterStampInfo class declaration
//
class SAPCLASSBASIC_CLASS SapXferCounterStampInfo
{
public:
   SapXferCounterStampInfo()
   {
      m_IsSupported = FALSE;
      m_IsAvailable = FALSE;
      m_MaxValue = 0;
      m_EventType = 0;
      m_TimeBase = SapXferPair::CounterStampMicroSec;
   }

   BOOL IsSupported() const { return m_IsSupported; }
   BOOL IsAvailable() const { return m_IsAvailable; }
   int GetMaxValue()  const { return m_MaxValue; }
   SapXferPair::EventType GetEventType()           const { return m_EventType; }
   SapXferPair::CounterStampTimeBase GetTimeBase() const { return m_TimeBase; }

protected:
   BOOL m_IsSupported;
   BOOL m_IsAvailable;
   int m_MaxValue;
   SapXferPair::EventType m_EventType;
   SapXferPair::CounterStampTimeBase m_TimeBase;

   // Classes which can directly access protected methods and members
   friend class SapTransfer;
};

//
// SapTransfer class declaration
//

class SAPCLASSBASIC_CLASS SapTransfer : public SapManager
{
public:
   // Transfer start modes
   typedef int StartMode;
   enum _StartMode
   {
      StartAsynchronous     = CORXFER_VAL_START_MODE_ASYNCHRONOUS,
      StartSynchronous      = CORXFER_VAL_START_MODE_SYNCHRONOUS,
      StartHalfAsynchronous = CORXFER_VAL_START_MODE_HALF_ASYNCHRONOUS,
      StartSequential       = CORXFER_VAL_START_MODE_SEQUENTIAL
   };

   // Various constants
   enum MiscValues
   {
      MaxPairs = 64
   };

public:
	// Constructor/Destructor
	SapTransfer(SapXferCallback pCallback = NULL, void *pContext = NULL, SapLocation location = SapLocation::ServerUnknown);
	SapTransfer(SapXferCallback pCallback, SapXferCallback pTrashCallback, void *pContext, SapLocation location = SapLocation::ServerUnknown);
	SapTransfer(const SapTransfer &xfer);
	virtual ~SapTransfer();

	SapTransfer &operator= (const SapTransfer &xfer);

public:
	// Creation/destruction
	virtual BOOL Create();
	virtual BOOL Destroy();

	virtual BOOL Connect();
	virtual BOOL Disconnect();

	// Access to implementation
	BOOL AddPair(SapXferPair &pair);
	void RemoveAllPairs();
	SapXferPair *GetPair(int index);
	int GetNumPairs()							  const { return m_NumPairs;}
	const SapXferPairInfo *GetPairInfo()  const { return m_Pairs;}
   const SapXferCounterStampInfo *GetCounterStampInfo(int pairIndex) const;

   virtual CORHANDLE GetHandle()         const { return m_hXfer; }
   SapLocation GetLocation()             const { return m_Location; }

   virtual SapXferCallback GetCallback() const { return m_pCallback; }
   virtual SapXferCallback GetTrashCallback() const { return m_pTrashCallback; }
   virtual void *GetContext()            const { return m_pContext; }
   virtual BOOL IsGrabbing()             const { return m_bGrab; }
   virtual BOOL IsAutoEmpty()            const { return m_bAutoEmpty; }
	virtual StartMode GetStartMode()      const { return m_StartMode; }

	virtual BOOL SetCallbackInfo( SapXferCallback pCallback, void *pContext= NULL);
	virtual BOOL SetTrashCallbackInfo( SapXferCallback pTrashCallback);

	virtual void SetAutoEmpty(BOOL isAutoEmpty);

   virtual BOOL IsCapabilityValid(int cap);
   virtual BOOL IsParameterValid(int param);

   virtual BOOL GetCapability(int cap, void *pValue);
   virtual BOOL GetParameter(int param, void *pValue);
   virtual BOOL SetParameter(int param, int value);
   virtual BOOL SetParameter(int param, void *pValue);

	virtual BOOL SetStartMode(StartMode startMode);

   BOOL AddRtProDesign( SapRTProDesign *pRtProDesign);
   BOOL RemoveRtProDesigns(SapRTProDesign *pRtProDesign);
   BOOL RemoveAllRtProDesigns();

   // Obsolete methods
	BOOL Add(SapXferPair &pair) { return AddPair(pair); }
	void RemoveAll()            { RemoveAllPairs(); }

public:
	// Transfer Control
	virtual BOOL Snap(int count = 1);
	virtual BOOL Grab();
	virtual BOOL Freeze();
	virtual BOOL Abort();
	virtual BOOL Wait(int timeout);
	virtual BOOL Init(BOOL resetIndex = TRUE);

protected:
	// Utility methods
   void Construct(SapXferCallback pCallback, SapXferCallback pTrashCallback, void *pContext, SapLocation location, int numPairs, const SapXferPairInfo *pPairs, BOOL bAutoEmpty);
	virtual BOOL SetFlipMode(int index, int destIndex= 0);
	virtual BOOL SetStartMode();
	virtual BOOL CreatePair(int index);
	virtual BOOL DestroyPair(int index);

   virtual BOOL GetCapabilities(int pairIndex);
   virtual BOOL SetParameters(int pairIndex, int destIndex);
   virtual BOOL SetParameters(int pairIndex);
   virtual BOOL SetParameters(void);
	virtual BOOL UpdateParameters( void);

	// Callback functions
	static SAPSTATUS CCONV OnNormal(void *context, UINT32 eventType, UINT32 eventCount);
	static SAPSTATUS CCONV OnTrash(void *context, UINT32 eventType, UINT32 eventCount);

protected:
	// Parameters
	SapXferCallback	m_pCallback;						// Application callback function
	SapXferCallback	m_pTrashCallback;				   // Application callback function for trash buffers
	void					*m_pContext;						// Application context
	SapLocation			m_Location;							// Transfer location
	SapXferPairInfo	m_Pairs[MaxPairs];	         // Table of transfer pairs
	int					m_NumPairs;							// Number of transfer pairs
	BOOL	            m_bAutoEmpty;	               // TRUE to automatically empty buffers

   // Counter stamp capabilities
   SapXferCounterStampInfo m_CounterStampInfo[MaxPairs];

   SapRTProDesign    *m_pRtProDesign[ MAX_RTPRODESIGNS_IN_TRANSFER];

   // Sapera transfer module settings
   CORXFER   m_hXfer;         // Transfer handle
   StartMode m_StartMode;     // Current start mode

	// Flags
   BOOL m_bGrab;           // TRUE if transfer is active
   BOOL m_NoConnect;       // TRUE for some special transfer pairs that do not need to be connected
}; 

//
// SapXferCallbackInfo class declaration
//
class SAPCLASSBASIC_CLASS SapXferCallbackInfo
{
public:
   SapXferCallbackInfo(SapTransfer *pXfer, void *context, SapXferPair::EventType eventType, int eventCount, BOOL isTrash, int pairIndex)
      { m_pXfer = pXfer; m_Context = context; m_EventType = eventType; m_EventCount = eventCount; m_bTrash = isTrash; m_PairIndex = pairIndex; }
	~SapXferCallbackInfo() {}

   // Obsolete constructor
   SapXferCallbackInfo(void *context, SapXferPair::EventType eventType, int eventCount, BOOL isTrash, int pairIndex)
      { m_pXfer = NULL; m_Context = context; m_EventType = eventType; m_EventCount = eventCount; m_bTrash = isTrash; m_PairIndex = pairIndex; }

   SapTransfer *GetTransfer() const { return m_pXfer; }
	void *GetContext()   const { return m_Context; }
	SapXferPair::EventType GetEventType() const { return m_EventType; }
	int GetEventCount()  const { return m_EventCount; }
	BOOL IsTrash()       const { return m_bTrash; }
	int GetPairIndex()   const { return m_PairIndex; }

protected:
   SapTransfer *m_pXfer;
	void *m_Context;
	SapXferPair::EventType m_EventType;
	int m_EventCount;
	BOOL m_bTrash;
	int m_PairIndex;
};

#endif   // _SAPTRANSFER_H_
