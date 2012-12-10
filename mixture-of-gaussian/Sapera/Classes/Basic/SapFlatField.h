#ifndef _SAPFLATFIELD_H_
#define _SAPFLATFIELD_H_

// SapFlatField.h : header file
//

#include "SapClassBasicDef.h"

// Default ratios
const float SapDefFlatFieldPixelRatio				= 99.0f;    // 99 percent of all pixels
const float SapDefFlatFieldDefectRatio				= 1.0f;     // Up to 1 percent of defective pixels
const float SapDefFlatFieldAvgFactorBlack       = 0.08f;		// 0.08 * maximum pixel value
const float SapDefFlatFieldAvgFactorWhite       = 0.75f;		// 0.75f * maximum pixel value

// Forward declarations
class SAPCLASSBASIC_CLASS SapFlatFieldInfo;
class SAPCLASSBASIC_CLASS SapFlatFieldStats;
class SAPCLASSBASIC_CLASS SapFlatFieldDefects;

//
// SapFlatField class declaration
//
class SAPCLASSBASIC_CLASS SapFlatField : public SapManager
{
public:
   // Various constants
   enum MiscValues
   {
      DefNumLinesAverage = 128,

		DefGainMin= 0,
		DefGainMax= 255,
      DefGainDivisor = 128,

		DefOffsetMin= 0,
		DefOffsetMax= 255,

      DefDeviationFactorMax = 4,				// maximum pixel value/4

      InvalidFlatFieldNumber = -1
   };

	enum CorrectionType
	{
		CorrectionTypeField,
		CorrectionTypeLine,
		CorrectionTypeInvalid= -1
	};

public:
	// Constructor/Destructor
   SapFlatField(SapAcquisition *pAcq);
   SapFlatField(SapBuffer *pBuffer);
   SapFlatField(const SapFlatField &flatField);
   virtual ~SapFlatField();

	SapFlatField &operator= (const SapFlatField &flatField);

	// Creation/destruction
   virtual BOOL Create();
   virtual BOOL Destroy();

public:
	// Access to implementation
   SapAcquisition *GetAcquisition() const { return m_pAcq; }
   SapBuffer *GetBuffer()           const { return m_pBuffer; }
   SapBuffer *GetBufferOffset()     const { return m_pBufferOffset; }
   SapBuffer *GetBufferGain()       const { return m_pBufferGain; }

   BOOL IsEnabled()         const { return m_IsEnabled; }
   BOOL IsSoftware()        const { return !m_IsHardware || !m_IsHardwareEnabled; }
   int GetGainDivisor()     const { return m_GainDivisor; }
   int GetNumLinesAverage() const { return m_NumLinesAverage; }
	int GetVerticalOffset()  const { return m_VerticalOffset; }
   CorrectionType GetCorrectionType()   const { return m_CorrectionType; }

   virtual BOOL SetAcquisition(SapAcquisition *pAcq);
   virtual BOOL SetBuffer(SapBuffer *pBuffer);
   virtual BOOL SetCorrectionType( CorrectionType correctionType);
   virtual BOOL SetGainDivisor(int gainDivisor);
   virtual BOOL SetGainMinMax(int gainMin, int gainMax);
   virtual BOOL SetOffsetMinMax(int offsetMin, int offsetMax);
   virtual BOOL SetNumLinesAverage(int numLinesAverage);
	virtual BOOL SetVerticalOffset( int verticalOffset);

   // Load/save gain and offset buffers from/to disk files
	virtual BOOL Load( const char *fileName);
	virtual BOOL Save( const char *fileName);

   // Load gain and offset buffers from buffer objects
	virtual BOOL Load( SapBuffer* pBufferGain, SapBuffer* pBufferOffset);

   // Clear/compute values for gain and offset buffers
   virtual BOOL Clear();
   virtual BOOL ComputeOffset(SapBuffer *pBuffer);
   virtual BOOL ComputeGain(SapBuffer *pBuffer, int deviationMax, SapFlatFieldDefects *pDefects);
   virtual BOOL ComputeGain(SapBuffer *pBuffer, int blackDeviationMax, int whiteDeviationMax, SapFlatFieldDefects *pDefects);

   // Execution control
   virtual BOOL Enable(BOOL enable = TRUE, BOOL useHardware= TRUE);
   virtual BOOL Execute(SapBuffer *pBuffer);
	virtual BOOL Execute(SapBuffer *pBuffer, int index);

   // Buffer statictics
   virtual BOOL GetAverage(SapBuffer *pBuffer, int *pAverage, int *pStdDeviation = NULL);
   virtual BOOL GetStats(SapBuffer *pBuffer, SapFlatFieldStats *pStats);
   
   // Obsolete methods
   BOOL IsHardware() const { return m_IsHardware; }
   virtual BOOL Load( const char *gainFile, const char *offsetFile);
   virtual BOOL Save( const char *gainFile, const char *offsetFile);

protected:
	// Utility methods
	virtual BOOL CreateRemaining();
   virtual void Construct(SapAcquisition *pAcq, SapBuffer *pBuffer);

   virtual BOOL IsFileCompatible(const char *filename, SapBuffer *pBuffer) const;
	virtual BOOL IsBufferCompatible( SapBuffer *pBufferSrc, SapBuffer *pBufferDst) const;
	virtual BOOL IsFormatCompatible(SapFormat format) const;
	virtual BOOL GetPixelDepth( int* pixelDepth) const;

   virtual BOOL SetAcqFlatField(SapBuffer *pBufferGain, SapBuffer *pBufferOffset);

   virtual BOOL FFCAverageLines(void *pBuffer, int verticalOffset, int width, int height, SapFormat format, void *pBufferAverage);
   virtual BOOL FFCComputeOffset(void *pBuffer, int width, int height, SapFormat format, void *pBufferOffset);
   virtual BOOL FFCComputeGain(void *pBuffer, int width, int height, SapFormat format, void *pBufferOffset, float pixelRatio, int offsetDeviationMax, int whiteDeviationMax, void *pBufferGain, SapFlatFieldDefects *pDefects);
   virtual BOOL FFCExecute(void *pBuffer, int width, int height, SapFormat format, void *pBufferOffset, void *pBufferGain, int isLineScan);
   virtual BOOL FFCGetAverage(void *pBuffer, int width, int height, SapFormat format, int *pAverage, int *pStdDeviation);
   virtual BOOL FFCGetStats(void *pBuffer, int width, int height, SapFormat format, void *pBufferOffset, float pixelRatio, SapFlatFieldStats *pStats);

protected:
   SapAcquisition *m_pAcq;
   SapBuffer      *m_pBuffer;
   SapBuffer      *m_pBufferOffset;
   SapBuffer      *m_pBufferGain;

   BOOL m_IsEnabled;
   BOOL m_FlatFieldEnable;       // Current value for 'enable' parameter
   BOOL m_IsHardware;
	BOOL m_IsHardwareEnabled;
   int  m_FlatFieldNumber;
   int  m_GainDivisor;
	int  m_GainMin;
	int  m_GainMax;
	int  m_OffsetMin;
	int  m_OffsetMax;
   int  m_NumLinesAverage;
	int  m_VerticalOffset;

   CorrectionType m_CorrectionType;

   SapXferParams m_XferParams;
};

//
// SapFlatFieldDefects class declaration
//
class SAPCLASSBASIC_CLASS SapFlatFieldDefects
{
public:
   SapFlatFieldDefects()
   {
      m_NumDefects = 0;
      m_NumClusters = 0;
      m_DefectRatio = 0.0f;
   }
   ~SapFlatFieldDefects() {}

   int GetNumDefects()    const { return m_NumDefects; }
   int GetNumClusters()   const { return m_NumClusters; }
   float GetDefectRatio() const { return m_DefectRatio; }

protected:
   int m_NumDefects;
   int m_NumClusters;
   float m_DefectRatio;

   // Classes which can directly access protected methods and members
   friend class SapFlatField;
};

//
// SapFlatFieldStats class declaration
//
class SAPCLASSBASIC_CLASS SapFlatFieldStats
{
public:
   SapFlatFieldStats()
   {
      m_Average = 0;
      m_StdDeviation = 0;
      m_Low = 0;
      m_High = 0;
      m_NumPixels = 0;
      m_PixelRatio = 0.0f;
   }
   ~SapFlatFieldStats() {}

   int GetAverage()      const { return m_Average; }
   int GetStdDeviation() const { return m_StdDeviation; }
   int GetLow()          const { return m_Low; }
   int GetHigh()         const { return m_High; }
   int GetNumPixels()    const { return m_NumPixels; }
   float GetPixelRatio() const { return m_PixelRatio; }

protected:
   int m_Average;
   int m_StdDeviation;
   int m_Low;
   int m_High;
   int m_NumPixels;
   float m_PixelRatio;

   // Classes which can directly access protected methods and members
   friend class SapFlatField;
};

#endif
