#ifndef _SAPDISPLAY_H_
#define _SAPDISPLAY_H_

//	SapDisplay.h : header file
//

#include "SapClassBasicDef.h"

//
// SapDisplay class declaration
//

class SAPCLASSBASIC_CLASS SapDisplay : public SapManager
{
public:
   // Display types
   typedef int Type;    // For compatibility with old Sapera++ application code
   enum _Type
   {
      TypeUnknown     = 0,
      TypeSystem      = CORDISPLAY_VAL_TYPE_SYSTEM,
      TypeDuplicate   = CORDISPLAY_VAL_TYPE_DUPLICATE,
      TypeExtended    = CORDISPLAY_VAL_TYPE_EXTENDED,
      TypeIndependent = CORDISPLAY_VAL_TYPE_INDEPENDENT
   };

   // Various constants
   enum MiscValues
   {
      MaxFormats = 32
   };

public:
	// Constructor/Destructor
	SapDisplay(SapLocation loc = SapLocation::ServerSystem);
	SapDisplay(const SapDisplay &disp);
	virtual ~SapDisplay();

	SapDisplay &operator=(const SapDisplay &disp);

	// Module create/destroy
	virtual BOOL Create();
	virtual BOOL Destroy();

	// Access to implementation
	CORHANDLE GetHandle()     const { return m_hDisplay;}
	SapLocation GetLocation() const { return m_Location;}

	int GetWidth()			const { return m_Width; }
	int GetHeight()		const { return m_Height; }
   int GetPixelDepth()	const { return m_PixelDepth; }
   int GetRefreshRate()	const { return m_Refresh; }
   BOOL IsInterlaced()	const { return m_Interlaced ? TRUE : FALSE; }
   Type GetType()       const { return m_Type; }

   virtual BOOL GetCapability(int cap, void *pValue);
   virtual BOOL GetParameter(int param, void *pValue);
   virtual BOOL SetParameter(int param, int value);
   virtual BOOL SetParameter(int param, void *pValue);

   virtual BOOL IsCapabilityValid(int cap);
   virtual BOOL IsParameterValid(int param);

   virtual BOOL IsPrimaryVGABoard();
   virtual BOOL IsOffscreenAvailable(SapFormat format);
   virtual BOOL IsOverlayAvailable(SapFormat format);

   // Access to display context
   virtual BOOL GetDC(HDC *pDC);
   virtual BOOL ReleaseDC();

   // Programmable display support
   virtual BOOL SetMode(int width, int height, SapFormat format, int refresh);

   // Obsolete methods
   BOOL IsSystem()      const { return m_Type == TypeSystem; }
   BOOL IsIndependent() const { return m_Type == TypeIndependent; }

protected:
   // Utility methods
   void Construct(SapLocation loc);
	int IncRef() { m_nRef++; return m_nRef;}
	int DecRef() { if (m_nRef > 0) m_nRef--; return m_nRef;}

   // Classes which can directly access protected methods and members
   friend class SapView;

protected:
   SapLocation m_Location;
   CORDISPLAY m_hDisplay;
   int m_nRef;          // Number of views attached to the object

	// Parameters
   int m_Width;         // Displayable width
   int m_Height;        // Displayable height
   int m_PixelDepth;    // Number of bits per pixel
   int m_Refresh;       // Refresh rate in Hz
   int m_Interlaced;    // Non-zero if current display mode is interlaced
   Type m_Type;         // Display type

   // Available offscreen and overlay display formats
   int m_OffscreenFormats[MaxFormats];
   int m_OverlayFormats[MaxFormats];
}; 

#endif	// _SAPDISPLAY_H_
