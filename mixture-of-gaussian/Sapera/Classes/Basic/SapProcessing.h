#ifndef _SAPPROCESSING_H_
#define _SAPPROCESSING_H_

//	SapProcessing.h : header file
//

#include "SapClassBasicDef.h"
#include "SapPerformance.h"

// Forward declarations
class SAPCLASSBASIC_CLASS SapProCallbackInfo;
typedef void (*SapProCallback)(SapProCallbackInfo *);


//
// SapProcessing class declaration
//
class SAPCLASSBASIC_CLASS SapProcessing : public SapManager
{
public:
	// Constructor/Destructor
   SapProcessing(SapBuffer *pBuffer, SapProCallback pCallback = NULL, void *pContext = NULL);
	SapProcessing(const SapProcessing &pro);
	virtual ~SapProcessing();

	SapProcessing &operator=(const SapProcessing &pro);

	// Module create/destroy
	virtual BOOL Create();
	virtual BOOL Destroy();

	// Access to implementation
   SapBuffer *GetBuffer()        const { return m_pBuffers; }
	SapProCallback GetCallback()  const { return m_pCallback; }
	void *GetContext()            const { return m_pContext; }
	float GetTime()               const { return m_Time; }
	int GetIndex()                const { return m_Index; }
   int GetThreadPriority()       const { return m_ThreadPriority; }

   virtual BOOL SetBuffer(SapBuffer *pBuffer);
   virtual BOOL SetCallbackInfo(SapProCallback pCallback, void *pContext);

	// Processing Control
	virtual void Init();
	virtual void Execute();
	virtual void Execute(int index);
	virtual void ExecuteNext();

	virtual BOOL IsAutoEmpty()	const              { return m_bAutoEmpty; }
   virtual void SetAutoEmpty(BOOL isAutoEmpty)	 { m_bAutoEmpty = isAutoEmpty; }
	virtual void SetThreadPriority( int priority) { m_ThreadPriority= priority; if( m_hThread != NULL) ::SetThreadPriority( m_hThread, priority);}

   // Obsolete methods, kept for backward compatibility
   SapBuffer *GetBuffers() const                { return GetBuffer(); }
   virtual BOOL SetBuffers(SapBuffer *pBuffers) { return SetBuffer(pBuffers); }

protected:
   void Construct(SapBuffer *pBuffer, SapProCallback pCallback, void *pContext, BOOL bAutoEmpty, int threadPriority);
	virtual BOOL Run() = 0;
	virtual void Next();
	virtual void StartTime();
	virtual void StopTime();
	static UINT WINAPI ThreadProc(LPVOID lpParameter);

protected:
	// Parameters
	SapBuffer *m_pBuffers;			// Reference to a buffer object
	SapProCallback m_pCallback;	// Application callback function
	void *m_pContext;				// Application context

	// Other variables
	SapPerformance	m_Perf;			// Timer object
	float				m_Time;			// Execution time
	BOOL	         m_bAutoEmpty;	// TRUE to automatically empty buffers

	// Thread-related variables
	int		m_Index;					// Index of the last processed buffer
	BOOL		m_bActive;				// TRUE if the thread is processing
	BOOL		m_bEndThread;			// Flag to terminate processing thread
	HANDLE	m_hSema;					// Processing semaphore
	HANDLE	m_hThread;				// Processing thread
	int      m_ThreadPriority;    // Processing thread priority

	int*		m_List;
	int		m_ListHead;
	int		m_ListTail;
	
	CRITICAL_SECTION m_criticalSection;
}; 

//
// SapProCallbackInfo class declaration
//
class SAPCLASSBASIC_CLASS SapProCallbackInfo
{
public:
   SapProCallbackInfo(SapProcessing *pPro, void *context)
      { m_pPro = pPro; m_Context = context; }
	~SapProCallbackInfo() {}

   // Obsolete constructor
   SapProCallbackInfo(void *context)
      { m_pPro = NULL; m_Context = context; }

   SapProcessing *GetProcessing() const { return m_pPro; }
	void *GetContext() const { return m_Context; }

protected:
   SapProcessing *m_pPro;
	void *m_Context;
};

#endif	// _SAPPROCESSING_H_

