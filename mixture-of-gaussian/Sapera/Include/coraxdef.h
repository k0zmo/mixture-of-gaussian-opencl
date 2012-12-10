//
// Coreco ActiveX property values
//

#ifndef _CORAXDEF_H_
#define _CORAXDEF_H_

// Common types
typedef enum
{
	ctrlTypeAcq,
	ctrlTypeBuffer,
	ctrlTypeCab,
	ctrlTypeCam,
	ctrlTypeDisplay,
	ctrlTypeGraphic,
	ctrlTypeKernel,
	ctrlTypeLut,
	ctrlTypePro,
	ctrlTypeVic,
	ctrlTypeView,
	ctrlTypeXfer,
	ctrlTypeObject,
	ctrlTypeFile,
} enumCtrlType;

// Acquisition related types
typedef enum
{
	outputFormatMono8,
	outputFormatMono16,
	outputFormatMono32,
	outputFormatRGB5551,
	outputFormatRGB565,
	outputFormatRGB888,
	outputFormatRGB8888,
	outputFormatRGB101010,
	outputFormatRGB161616,
	outputFormatUYVY,
	outputFormatYUY2,
	outputFormatYVYU,
	outputFormatYUYV,
	outputFormatY411,
	outputFormatY211,
	outputFormatFloat,
	outputFormatComplex,
	outputFormatPoint,
	outputFormatRGBP8,
	outputFormatRGBP16,
	outputFormatHSV,
	outputFormatYUV,
	outputFormatFPoint,
} enumOutputFormat;

#include "capiacqd.h"

// Buffer related types
typedef enum
{
	formatMono8,
	formatMono16,
	formatMono32,
	formatRGB5551,
	formatRGB565,
	formatRGB888,
	formatRGB8888,
	formatRGB101010,
	formatRGB161616,
	formatUYVY,
	formatYUY2,
	formatYVYU,
	formatYUYV,
	formatY411,
	formatY211,
	formatFloat,
	formatComplex,
	formatPoint,
	formatRGBP8,
	formatRGBP16,
	formatHSV,
	formatYUV,
	formatFPoint,
} enumFormat;

typedef enum
{
	fileFormatBmp,
	fileFormatTiff,
	fileFormatCrc,
} enumFileFormat;

typedef enum
{
	stateEmpty,
	stateFull,
} enumState;

typedef enum
{
	bayerAlignGbRg = 0x01,
	bayerAlignBgGr = 0x02,
	bayerAlignRgGb = 0x04,
	bayerAlignGrBg = 0x08,
	bayerAlignMask = 0x0F,
} enumBayerAlign;

typedef enum
{
	bayerMethodInterpolate = 0x10,
	bayerMethodAdvanced = 0x20,
	bayerMethodMask = 0xF0,
} enumBayerMethod;

typedef enum
{
	convertRangeClip = 0x01,
	convertRangeRemap = 0x02,
	convertRangeMask = 0x0F,
} enumConvertRange;

typedef enum
{
	convertComplexMagPhase = 0x10,
	convertComplexRealImag = 0x20,
	convertComplexMask = 0xF0,
} enumConvertComplex;

// Cab related types
typedef enum
{
	transModeStandard,
	transModeRoundRobin
} enumTransMode;

// Graphic related types
typedef enum
{
	opmReplace,
	opmXOr,
	opmAnd,
	opmOr
} enumOpm;

typedef enum
{
	angleLeft = 0,
	angleUp = 90,
	angleRight = 180,
	angleDown = 270
} enumAngle;

// Kernel related types
typedef enum
{
	kernelFormatInt,
	kernelFormatFloat
} enumKernelFormat;

typedef enum
{
	typeUser,
	typeNorth,
	typeNortheast,
	typeEast,
	typeSoutheast,
	typeSouth,
	typeSouthwest,
	typeWest,
	typeNorthwest,
	typeLaplacian1,
	typeLaplacian2,
	typeLaplacian3,
	typePrewitthorz,
	typePrewittvert,
	typeSobelhorz,
	typeSobelvert,
	typeLowpass1,
	typeLowpass2,
	typeLowpass3,
	typeHighpass1,
	typeHighpass2,
	typeHighpass3,
	typeVertline,
	typeHorzline,
	typeDiaglineplus,
	typeDiaglineminus,
	typeWeightvertline,
	typeWeighthorzline,
	typeLaws1,
	typeLaws2,
	typeLaws3
} enumKernelType;

// LUT related types
typedef enum
{
	lutFormatMono8,
	lutFormatMono10,
	lutFormatMono16,
	lutFormatColorNI8,
	lutFormatColorNI10,
	lutFormatColorNI16,
	lutFormatMono9,
	lutFormatMono11,
	lutFormatMono12,
	lutFormatMono13,
	lutFormatMono14,
	lutFormatMono15,
	lutFormatColorNI9,
	lutFormatColorNI11,
	lutFormatColorNI12,
	lutFormatColorNI13,
	lutFormatColorNI14,
	lutFormatColorNI15,
	lutFormatColorI8,
	lutFormatColorI9,
	lutFormatColorI10,
	lutFormatColorI11,
	lutFormatColorI12,
	lutFormatColorI13,
	lutFormatColorI14,
	lutFormatColorI15,
	lutFormatColorI16,
} enumLutFormat;

// Processing related types (Color)
typedef enum
{
	colorPacked,
	colorBanded,
} enumColorType;

typedef enum
{
	rgbHsv,
	hsvRgb,
} enumRgbHsvType;

typedef enum
{
	rgbYuv,
	yuvRgb,
} enumRgbYuvType;

// Processing related types (Complex)
typedef enum
{
	complexRect,
	complexPolar,
} enumComplexType;

// Processing related types (Filter)
typedef enum
{
	gauss2x2,
	gauss3x3,
	gauss5x5,
	gauss7x7,
} enumGaussType;

typedef enum
{
	laplace1,
	laplace2,
	laplace3,
} enumLaplaceType;

typedef enum
{
	edgePrewitt,
	edgeSobel,
} enumEdgeType;

typedef enum
{
	magnitudeMax,
	magnitudeAbs,
} enumMagnitudeType;

typedef enum
{
	dirN,
	dirNW,
	dirW,
	dirSW,
	dirS,
	dirSE,
	dirE,
	dirNE,
} enumDir;

// Processing related types (Geometry)
typedef enum  
{
	flipY,
	flipX,
	flipXPose,
	flipR90,
	flipR180,
	flipR270,
} enumFlipType;

typedef enum
{
	polarDirPolarToRect,
	polarDirRectToPolar,
} enumPolarDirType;

typedef enum  
{
	polarNormal,
	polarLog,
} enumPolarType;

typedef enum  
{
	correspondence1_1,
	correspondenceBest,
} enumCorrespondenceType;

// Processing related types (Measurement)
typedef enum 
{
	histNormal,
	histCumul,
} enumHistType;

typedef enum  
{
	peakMinima,
	peakMaxima,
} enumPeakType;

typedef enum  
{
	plotColorRedYellow,
	plotColorYellowGreen,
	plotColorAquaBlue,
	plotColorWhiteYellow,
	plotColorBlackBlue,
} enumPlotColorType;

typedef enum  
{
	fontHelvetica,
	fontTimes,
	fontCourrier,
	fontNoText,
} enumFontType;

typedef enum  
{
	fontStyleNormal,
	fontStyleBold,
	fontStyleItalic,
	fontStyleBoldItalic,
} enumFontStyleType;

typedef enum  
{
	vectorOpDiff,
	vectorOpAbsDiff,
	vectorOpSqrDiff,
	vectorOpEuclidean,
	vectorOpMaximum,
} enumVectorOpType;

// Processing related types (Morpho)
typedef enum  
{
	morphkernelRectangular,
	morphkernelArbitrary,
} enumMorphKernelType;

// Processing related types (Point)
typedef enum
{
	adaptThreshAbsolute,
	adaptThreshMeanMin,
	adaptThreshMaxMean,
	adaptThreshStdDev,
} enumAdaptThresh;

typedef enum
{
	noiseRandom,
	noiseSpike,
	noiseGauss,
} enumNoiseType;

typedef enum
{
	aluAdd,
	aluSub,
	aluMul,
	aluDiv,
	aluOr,
	aluAnd,
	aluXOr,
	aluSrc,
	aluOvl,
	aluExp,
	aluASub,
} enumAluOp;

typedef enum
{
	unaryAdd,
	unarySub,
	unaryMul,
	unaryDiv,
	unaryOr,
	unaryAnd,
	unaryShr,
	unaryShl,
	unaryExp,
	unaryXor,
} enumUnaryOp;

typedef enum
{
	ovfSat,
	ovfWrap,
} enumOverflow;

// Processing related types (Segmentation)
typedef enum
{
	distMapEuclidean3x3,
	distMapEuclidean5x5,
	distMapCityBlock,
	distMapChessBoard,
} enumDistMapType;

typedef enum
{
	connect4,
	connect8,
} enumConnectType;

typedef enum
{
	blobBinary,
	blobLabeled,
} enumBlobType;

typedef enum
{
	matchFindMaxima,
	matchFindMinima,
} enumMatchFindType;

typedef enum
{
	matchSelectFirst,
	matchSelectBest,
} enumMatchSelectType;

typedef enum
{
	pathRasterize,
	pathSample,
} enumPathType;

typedef enum
{
	sampleInterpolate,
	sampleNearest,
} enumSampleType;

typedef enum
{
	grDistmapFillNo = 0,
	grDistmapFillYes = 1,
	grDistmapFillData = 2,
} enumGrDistmapFillType;

typedef enum
{
	xEdgeRising,
	xEdgeFalling,
	xEdgeRisingFalling,
	xEdgePeak,
} enumXEdgeType;

typedef enum
{
	threshRelative,
	threshAbsolute,
} enumThreshType;

typedef enum
{
	kMeansArray,
	kMeansAuto,
} enumKMeansType;

typedef enum
{
	labelGray,
	labelBinary,
	labelOutline,
	labelValues,
} enumLabelType;

typedef enum
{
	featureCentroids,
	featureMajorAxis,
	featureBBox,
	featurePerimeter,
} enumFeatureType;

typedef enum
{
	graphicLines = 1,
	graphicEndPoints = 2,
} enumGraphicType;

typedef enum
{
	rangeActual,
	rangeMax,
	rangeUser,
} enumRangeType;

typedef enum
{
	tMatchSumOfDiff,
	tMatchNormCorr,
	tMatchBinary,
} enumTMatchType;

typedef enum
{
	corrPositive,
	corrNegative,
	corrBoth,
} enumCorrType;

typedef enum
{
	tMatchResultCenter,
	tMatchResultUpperLeft,
	tMatchResultBBox,
} enumTMatchResultType;

typedef enum
{
	deflectionMean,
	deflectionMax,
} enumDeflectionType;

typedef enum
{
	adjustBoth,
	adjustLeft,
	adjustRight,
} enumAdjustType;

typedef enum
{
	zeroXAll,
	zeroXLoHi,
	zeroXHiLo,
} enumZeroXType;

// Processing related types (Transform)
typedef enum
{
	xformForward,
	xformReverse,
} enumXformType;

typedef enum
{
	houghMax,
	houghSum,
	houghIncr,
} enumHoughType;

// Processing related types (Util)
typedef enum  
{
	castResultChar,
	castResultUChar,
	castResultShort,
	castResultUShort,
	castResultInt,
	castResultUInt,
	castResultFloat,
	castResultDouble,
	castResultString,
	castResultFile,
	castResultPoint,
	castResultFpoint,
} enumCastResultType;

typedef enum  
{
	castRound,
	castFloor,
	castCeiling,
} enumCastConversion;

typedef enum  
{
	castClip,
	castBias,
} enumCastType;

// View related types
typedef enum
{
	modeKeyer,
	modeDib,
	modeBlt,
	modeAutoDetect,
} enumMode;

// Transfer related types
typedef enum
{
	eventTypeStartOfField	= 0x00010000,
	eventTypeStartOfOdd		= 0x00020000,
	eventTypeStartOfEven		= 0x00040000,
	eventTypeStartOfFrame	= 0x00080000,
	eventTypeEndOfField		= 0x00100000,
	eventTypeEndOfOdd			= 0x00200000,
	eventTypeEndOfEven		= 0x00400000,
	eventTypeEndOfFrame		= 0x00800000,
	eventTypeEndOfLine		= 0x01000000,
	eventTypeEndOfNLines		= 0x02000000,
} enumEventType;

typedef enum
{
	xferDescFrameDefault,
	xferDescFrameInterlaced,
	xferDescFrameNonInterlaced,
} enumDescFrame;

typedef enum
{
	xferDescFieldOrderDefault,
	xferDescFieldOrderOddEven,
	xferDescFieldOrderEvenOdd,
	xferDescFieldOrderAnyOrder,
	xferDescFieldOrderNextField,
} enumDescFieldOrder;

typedef enum
{
	startModeAsynchronous,
	startModeSynchronous,
	startModeHalfSynchronous,
} enumStartMode;

typedef enum
{
	cycleModeAsynchronous,
	cycleModeSynchronous,
	cycleModeSynchronousWithTrash,
	cycleModeOff,
} enumCycleMode;

// Array related types
typedef enum
{
	arrayTypeUndefined,
	arrayTypeChar,
	arrayTypeShort,
	arrayTypeInt,
	arrayTypeFloat,
	arrayTypePoint,
	arrayTypeFPoint,
	arrayTypeBlob,
	arrayTypeFeature,
	arrayTypeGeom,
	arrayTypeGraph,
	arrayTypeEdge,
	arrayTypeLine,
	arrayTypeString,
} enumArrayType;

typedef enum
{
	arrayFileFormatDisplay,
	arrayFileFormatText,
	arrayFileFormatNone,
	arrayFileFormatHybrid,
	arrayFileFormatBinary,
} enumArrayFileFormatType;

// File related types
typedef enum
{
	fileTypeBmp,
	fileTypeTiff,
	fileTypeCrc,
	fileTypeRaw,
	fileTypeJpeg,
	fileTypeAvi,
	fileTypeJpeg2000,
	fileTypeUnknown = -1,
} enumFileType;

typedef enum
{
	fileDataFormatMono8,
	fileDataFormatMono16,
	fileDataFormatMono32,
	fileDataFormatRGB5551,
	fileDataFormatRGB565,
	fileDataFormatRGB888,
	fileDataFormatRGB8888,
	fileDataFormatRGB101010,
	fileDataFormatRGB161616,
	fileDataFormatUYVY,
	fileDataFormatYUY2,
	fileDataFormatYVYU,
	fileDataFormatYUYV,
	fileDataFormatY411,
	fileDataFormatY211,
	fileDataFormatFloat,
	fileDataFormatComplex,
	fileDataFormatPoint,
	fileDataFormatRGBP8,
	fileDataFormatRGBP16,
	fileDataFormatHSV,
	fileDataFormatYUV,
	fileDataFormatFPoint,
} enumFileDataFormat;

typedef enum
{
	fileSeekBegin,
	fileSeekCurrent,
	fileSeekEnd,
} enumFileSeek;

typedef enum
{
	fileCompressionNone,
	fileCompressionRle,
	fileCompressionLzw,
	fileCompressionJpeg,
	fileCompressionUnknown = -1,
} enumFileCompression;

// Object related types
typedef enum
{
	createModeGeneric,
	createModeConstant,
	createModeGraphic,
} enumCreateMode;

typedef enum
{
	graphicFontHelvetica,
	graphicFontTimes,
	graphicFontCourrier,
} enumGraphicFont;

typedef enum
{
	graphicFontStyleNormal,
	graphicFontStyleBold,
	graphicFontStyleItalic,
	graphicFontStyleBoldItalic,
} enumGraphicFontStyle;

typedef enum
{
	objectTypeGeneric,
	objectTypeSync,
	objectTypeVector,
	objectTypeImage,
	objectTypeVolume,
	objectTypeChar,
	objectTypeByte,
	objectTypeShort,
	objectTypeInt,
	objectTypeLong,
	objectTypeUByte,
	objectTypeUShort,
	objectTypeUInt,
	objectTypeULong,
	objectTypeHex8,
	objectTypeHex16,
	objectTypeHex,
	objectTypeFloat,
	objectTypeDouble,
	objectTypeRGB,
	objectTypeHSV,
	objectTypeYUV,
	objectTypeObsolete3Band,
	objectTypeComplex,
	objectTypePoint,
	objectTypeString,
	objectTypeFile,
	objectTypeRef,
	objectTypeFpoint,
	objectTypePointValue,
	objectTypePerimeter,
	objectTypeBlob,
	objectTypeFeature,
	objectTypeColor,
	objectTypeGeomText,
	objectTypeGeom,
	objectTypeGraphic,
	objectTypeEdge,
	objectTypeLine,
} enumObjectType;

typedef enum
{
	geomTypePoint,
	geomTypeLine,
	geomTypePolyline,
	geomTypePolygon,
	geomTypeRectangle,
	geomTypeCircle,
	geomTypeText,
} enumGeomType;

typedef enum
{
	objectFileFormatDisplay,
	objectFileFormatText,
	objectFileFormatNone,
	objectFileFormatHybrid,
	objectFileFormatBinary,
} enumObjectFileFormatType;

typedef enum
{
	addElemFirst,
	addElemLast,
	addElemByElement,
} enumAddElemType;

#endif   // _CORAXDEF_H_
