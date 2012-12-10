#ifndef _SAPDATA_H_
#define _SAPDATA_H_

// SapData.h : header file
//

#include "SapClassBasicDef.h"


//
// Basic definitions available to all classes
//

// Sapera data formats
typedef int SapFormat;     // For compatibility with old Sapera++ application code
enum _SapFormat
{
   SapFormatUnknown = 0,

   // Monochrome data formats
   SapFormatMono1  = CORDATA_FORMAT_MONO1,
   SapFormatUint1  = CORDATA_FORMAT_UINT1,
   SapFormatBinary = CORDATA_FORMAT_BINARY,  // 1-bit
   SapFormatMono8  = CORDATA_FORMAT_MONO8,
   SapFormatRGBP8  = CORDATA_FORMAT_RGBP8,   // 8-bit planar RGB
   SapFormatInt8   = CORDATA_FORMAT_INT8,
   SapFormatUint8  = CORDATA_FORMAT_UINT8,
   SapFormatMono16 = CORDATA_FORMAT_MONO16,
   SapFormatRGBP16 = CORDATA_FORMAT_RGBP16,  // 16-bit planar RGB
   SapFormatInt16  = CORDATA_FORMAT_INT16,
   SapFormatUint16 = CORDATA_FORMAT_UINT16,
   SapFormatMono24 = CORDATA_FORMAT_MONO24,
   SapFormatInt24  = CORDATA_FORMAT_INT24,
   SapFormatUint24 = CORDATA_FORMAT_UINT24,
   SapFormatMono32 = CORDATA_FORMAT_MONO32,
   SapFormatInt32  = CORDATA_FORMAT_INT32,
   SapFormatUint32 = CORDATA_FORMAT_UINT32,
   SapFormatMono64 = CORDATA_FORMAT_MONO64,
   SapFormatInt64  = CORDATA_FORMAT_INT64,
   SapFormatUint64 = CORDATA_FORMAT_UINT64,

   // Color RGB data formats
   SapFormatRGB5551   = CORDATA_FORMAT_RGB5551,    // 16-bit
   SapFormatRGB565    = CORDATA_FORMAT_RGB565,     // 16-bit
   SapFormatRGB888    = CORDATA_FORMAT_RGB888,     // 24-bit
   SapFormatRGB8888   = CORDATA_FORMAT_RGB8888,    // 32-bit
   SapFormatRGB101010 = CORDATA_FORMAT_RGB101010,  // 32-bit
   SapFormatRGB161616 = CORDATA_FORMAT_RGB161616,  // 48-bit

   // Color HSV data formats
   SapFormatHSV = CORDATA_FORMAT_HSV,

   // Color YUV data formats
   SapFormatUYVY = CORDATA_FORMAT_UYVY,
   SapFormatYUY2 = CORDATA_FORMAT_YUY2,
   SapFormatYVYU = CORDATA_FORMAT_YVYU,
   SapFormatYUYV = CORDATA_FORMAT_YUYV,
   SapFormatY411 = CORDATA_FORMAT_Y411,   // 4:1:1 also known as Y41P
   SapFormatY211 = CORDATA_FORMAT_Y211,   // 2:1:1
   SapFormatYUV  = CORDATA_FORMAT_YUV,

   // Other data formats
   SapFormatFloat   = CORDATA_FORMAT_FLOAT,
   SapFormatComplex = CORDATA_FORMAT_COMPLEX,   // Real and imaginary components
   SapFormatPoint   = CORDATA_FORMAT_POINT,     // X and Y integer components
   SapFormatFPoint  = CORDATA_FORMAT_FPOINT,    // Like Fpoint, but components are float instead of integer

   // Additional monochrome data formats used by look-up tables
   SapFormatMono9  = CORDATA_FORMAT_MONO9,
   SapFormatMono10 = CORDATA_FORMAT_MONO10,
   SapFormatMono11 = CORDATA_FORMAT_MONO11,
   SapFormatMono12 = CORDATA_FORMAT_MONO12,
   SapFormatMono13 = CORDATA_FORMAT_MONO13,
   SapFormatMono14 = CORDATA_FORMAT_MONO14,
   SapFormatMono15 = CORDATA_FORMAT_MONO15,

   SapFormatUint9  = CORDATA_FORMAT_UINT9,
   SapFormatUint10 = CORDATA_FORMAT_UINT10,
   SapFormatUint11 = CORDATA_FORMAT_UINT11,
   SapFormatUint12 = CORDATA_FORMAT_UINT12,
   SapFormatUint13 = CORDATA_FORMAT_UINT13,
   SapFormatUint14 = CORDATA_FORMAT_UINT14,
   SapFormatUint15 = CORDATA_FORMAT_UINT15,

   SapFormatInt9   = CORDATA_FORMAT_INT9,
   SapFormatInt10  = CORDATA_FORMAT_INT10,
   SapFormatInt11  = CORDATA_FORMAT_INT11,
   SapFormatInt12  = CORDATA_FORMAT_INT12,
   SapFormatInt13  = CORDATA_FORMAT_INT13,
   SapFormatInt14  = CORDATA_FORMAT_INT14,
   SapFormatInt15  = CORDATA_FORMAT_INT15,

   // Interlaced color data formats used by look-up tables, data stored as:
   //    RGBRGBRGB ...
   SapFormatColorI8  = CORDATA_FORMAT_COLORI8,
   SapFormatColorI9  = CORDATA_FORMAT_COLORI9,
   SapFormatColorI10 = CORDATA_FORMAT_COLORI10,
   SapFormatColorI11 = CORDATA_FORMAT_COLORI11,
   SapFormatColorI12 = CORDATA_FORMAT_COLORI12,
   SapFormatColorI13 = CORDATA_FORMAT_COLORI13,
   SapFormatColorI14 = CORDATA_FORMAT_COLORI14,
   SapFormatColorI15 = CORDATA_FORMAT_COLORI15,
   SapFormatColorI16 = CORDATA_FORMAT_COLORI16,

   // Non-interlaced color data formats used by look-up tables, data stored as:
   //    RRRRRR ... GGGGGG ... BBBBBB
   SapFormatColorNI8  = CORDATA_FORMAT_COLORNI8,
   SapFormatColorNI9  = CORDATA_FORMAT_COLORNI9,
   SapFormatColorNI10 = CORDATA_FORMAT_COLORNI10,
   SapFormatColorNI11 = CORDATA_FORMAT_COLORNI11,
   SapFormatColorNI12 = CORDATA_FORMAT_COLORNI12,
   SapFormatColorNI13 = CORDATA_FORMAT_COLORNI13,
   SapFormatColorNI14 = CORDATA_FORMAT_COLORNI14,
   SapFormatColorNI15 = CORDATA_FORMAT_COLORNI15,
   SapFormatColorNI16 = CORDATA_FORMAT_COLORNI16 
};

// Format categories
enum SapFormatType
{
   SapFormatTypeUnknown,
   SapFormatTypeMono,
   SapFormatTypeRGB,
   SapFormatTypeHSV,
   SapFormatTypeYUV,
   SapFormatTypeFloat,
   SapFormatTypeComplex,
   SapFormatTypePoint,
   SapFormatTypeFPoint,
   SapFormatTypeColor
};

// Default values for parameters used in various classes
const int SapDefWidth         = 640;
const int SapDefHeight        = 480;
const SapFormat SapDefFormat  = SapFormatMono8;
const int SapDefPixelDepth    = 8;
const int SapDefPixelShift    = 0;
const int SapDefBufferCount   = 1;
const int SapDefLutEntries    = (1 << CORDATA_FORMAT_DATADEPTH(SapFormatUint8));

#define SapDefBufferType   SapBuffer::TypeDefault
#define SapDefFrameType    SapXferParams::FrameInterlaced
#define SapDefFieldOrder   SapXferParams::FieldOrderOddEven

//
// SapData class declaration
//

class SAPCLASSBASIC_CLASS SapData
{
public:
   // Data types for derived classes
   enum Type
   {
      TypeUnknown,
      TypeMono,
      TypeRGB,
      TypeHSV,       // Not used
      TypeYUV,
      TypePoint,
      TypeFloat,
      TypeComplex,   // Not used
      TypeFPoint,
      TypeFRGB
   };

public:
   // Constructor
   SapData()
      { m_Type = TypeUnknown; Clear(); }

   virtual void Clear() { ZeroMemory(&m_Data, sizeof(m_Data)); }

	// Access to implementation
   Type GetType() { return m_Type; }

protected:
   CORDATA m_Data;
   Type m_Type;

   // Classes which can directly access data members
   friend class SapBuffer;
   friend class SapGraphic;
   friend class SapLut;
   friend class SapView;

   // Needed for copy constructor and assignment operator in derived classes
   friend class SapDataMono;
   friend class SapDataRGB;
};


//
// Derived classes declarations
//

class SAPCLASSBASIC_CLASS SapDataMono : public SapData
{
public:
   // Constructors
   SapDataMono()
      { m_Type = TypeMono; }
   SapDataMono(int mono)
      { m_Type = TypeMono; m_Data.mono = mono; }
   SapDataMono(const SapData &data)
      { m_Type = TypeMono; m_Data = data.m_Data; }

   SapDataMono &operator=(const SapData &data) { m_Type = TypeMono; m_Data = data.m_Data; return *this; }

   // Data access methods
   int Mono() { return m_Data.mono; }

   void Set(int mono) { m_Data.mono = mono; }
};

class SAPCLASSBASIC_CLASS SapDataRGB : public SapData
{
public:
   // Constructors
   SapDataRGB()
      { m_Type = TypeRGB; }
   SapDataRGB(int red, int green, int blue)
      { m_Type = TypeRGB; m_Data.rgb.red = red; m_Data.rgb.green = green; m_Data.rgb.blue = blue; }
   SapDataRGB(const SapData &data)
      { m_Type = TypeRGB; m_Data = data.m_Data; }

   SapDataRGB &operator=(const SapData &data) { m_Type = TypeRGB; m_Data = data.m_Data; return *this; }

   // Data access methods
   int Red()   { return m_Data.rgb.red; }
   int Green() { return m_Data.rgb.green; }
   int Blue()  { return m_Data.rgb.blue; }

   void Set(int red, int green, int blue) { m_Data.rgb.red = red; m_Data.rgb.green = green; m_Data.rgb.blue = blue; }
};

class SAPCLASSBASIC_CLASS SapDataYUV : public SapData
{
public:
   // Constructors
   SapDataYUV()
      { m_Type = TypeYUV; }
   SapDataYUV(int y, int u, int v)
      { m_Type = TypeYUV; m_Data.yuv.y = y; m_Data.yuv.u = u; m_Data.yuv.v = v; }

   // Data access methods
   int Y() { return m_Data.yuv.y; }
   int U() { return m_Data.yuv.u; }
   int V() { return m_Data.yuv.v; }

   void Clear() { SapData::Clear(); m_Data.yuv.u = 0x80; m_Data.yuv.v = 0x80; }
   void Set(int y, int u, int v) { m_Data.yuv.y = y; m_Data.yuv.u = u; m_Data.yuv.v = v; }
};

class SAPCLASSBASIC_CLASS SapDataPoint : public SapData
{
public:
   // Constructors
   SapDataPoint()
      { m_Type = TypePoint; }
   SapDataPoint(int x, int y)
      { m_Type = TypePoint; m_Data.point.x = x; m_Data.point.y = y; }

   // Data access methods
   int X() { return m_Data.point.x; }
   int Y() { return m_Data.point.y; }

   void Set(int x, int y) { m_Data.point.x = x; m_Data.point.y = y; }
};

class SAPCLASSBASIC_CLASS SapDataFloat : public SapData
{
public:
   // Constructors
   SapDataFloat()
      { m_Type = TypeFloat; }
   SapDataFloat(float flt)
      { m_Type = TypeFloat; m_Data.flt = flt; }

   // Data access methods
   float Float() { return m_Data.flt; }

   void Clear() { SapData::Clear(); m_Data.flt = 0.0; }
   void Set(float flt) { m_Data.flt = flt; }
};

class SAPCLASSBASIC_CLASS SapDataFPoint : public SapData
{
public:
   // Constructors
   SapDataFPoint()
      { m_Type = TypeFPoint; }
   SapDataFPoint(float x, float y)
      { m_Type = TypeFPoint; m_Data.fpoint.x = x; m_Data.fpoint.y = y; }

   // Data access methods
   float X() { return m_Data.fpoint.x; }
   float Y() { return m_Data.fpoint.y; }

   void Clear() { SapData::Clear(); m_Data.fpoint.x = 0.0; m_Data.fpoint.y = 0.0; }
   void Set(float x, float y) { m_Data.fpoint.x = x; m_Data.fpoint.y = y; }
};

class SAPCLASSBASIC_CLASS SapDataFRGB : public SapData
{
public:
   // Constructors
   SapDataFRGB()
      { m_Type = TypeFRGB; }
   SapDataFRGB(float red, float green, float blue)
      { m_Type = TypeFRGB; m_Data.frgb.red = red; m_Data.frgb.green = green; m_Data.frgb.blue = blue; }

   // Data access methods
   float Red()   { return m_Data.frgb.red; }
   float Green() { return m_Data.frgb.green; }
   float Blue()  { return m_Data.frgb.blue; }

   void Clear() { SapData::Clear(); m_Data.frgb.red = 0.0; m_Data.frgb.green = 0.0; m_Data.frgb.blue = 0.0; }
   void Set(float red, float green, float blue) { m_Data.frgb.red = red; m_Data.frgb.green = green; m_Data.frgb.blue = blue; }
};

#endif   // _SAPDATA_H_
