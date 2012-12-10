//
// Coreco API - Port functions.
//

#ifndef _CAPIPORT_H_
#define _CAPIPORT_H_

#include <capidef.h>

#define CORPORT_SYSTEM	   0x00     // 0
#define CORPORT_COBRA	   0xFF     // 255
#define CORPORT_PYTHON	   0xFE     // 254
#define CORPORT_VIPERRGB   0xFD     // 253
#define CORPORT_VIPERDIG   0xFC     // 252
#define CORPORT_ICESTORM   0xFB     // 251
#define CORPORT_VIPERQUAD  0xFA     // 250
#define CORPORT_REBEL      0xF9     // 249
#define CORPORT_MAMBA      0xF8     // 248
#define CORPORT_BANDIT     0xF7     // 247
#define CORPORT_VIPERCAMLINK  0xF6  // 246
#define CORPORT_BANDITII   0xF5     // 245
#define CORPORT_X64        0xF4     // 244
#define CORPORT_ANACONDA   0xF3     // 243
#define CORPORT_X64AN      0xF2     // 242
#define CORPORT_PC2V			0xF1     // 241
#define CORPORT_PC2C			0xF0     // 240
#define CORPORT_X64LVDS    0xEF     // 239
#define CORPORT_BOARD_NAME 0xEE     // 238: Driver Template, used by example
#define CORPORT_PC2V_NS		0xED     // 237
#define CORPORT_X64CHL     0xEC     // 236
#define CORPORT_NEXUS      0xEB     // 235
#define CORPORT_ARIES      0xEA     // 234
#define CORPORT_X64NS      0xE9     // 233
#define CORPORT_X64IPRO    0xE8     // 232

// NOTE: These definitions are now obsolete. They are only kept for backward
//       compatibility. Use the CORPORT_xxx definitions above instead.
#define CORBOARD_SYSTEM       CORPORT_SYSTEM
#define CORBOARD_COBRA        CORPORT_COBRA
#define CORBOARD_PYTHON       CORPORT_PYTHON
#define CORBOARD_VIPERRGB     CORPORT_VIPERRGB
#define CORBOARD_VIPERDIG     CORPORT_VIPERDIG
#define CORBOARD_ICESTORM     CORPORT_ICESTORM
#define CORBOARD_VIPERQUAD    CORPORT_VIPERQUAD
#define CORBOARD_REBEL        CORPORT_REBEL
#define CORBOARD_MAMBA        CORPORT_MAMBA
#define CORBOARD_VIPERCAMLINK CORPORT_VIPERCAMLINK
#define CORBOARD_BANDITII     CORPORT_BANDITII
#define CORBOARD_X64          CORPORT_X64
#define CORBOARD_ANACONDA     CORPORT_ANACONDA
#define CORBOARD_X64LVDS      CORPORT_X64LVDS
#define CORBOARD_X64CHL       CORPORT_X64CHL
#define CORBOARD_X64IPRO		CORPORT_X64IPRO

#define GET_DRIVER_FILE_NAME_AND_PROC(module,proc)\
		case CORPORT_COBRA:\
			module= "CORCOBRA.CXM";\
			proc= "CorCobraGetControl";\
		break;\
		case CORPORT_PYTHON:\
			module= "CORPYTH.CXM";\
			proc= "CorPythGetControl";\
		break;\
		case CORPORT_VIPERRGB:\
			module= "CORVRGB.CXM";\
			proc= "CorVRgbGetControl";\
		break;\
		case CORPORT_VIPERDIG:\
			module= "CORVDIG.CXM";\
			proc= "CorVDigGetControl";\
		break;\
		case CORPORT_VIPERCAMLINK:\
			module= "CORVCAMLINK.CXM";\
			proc= "CorVCamLinkGetControl";\
		break;\
		case CORPORT_VIPERQUAD:\
			module= "CORVQUAD.CXM";\
			proc= "CorVQuadGetControl";\
		break;\
		case CORPORT_ICESTORM:\
			module= "CORICE.CXM";\
			proc= "CorIceGetControl";\
		break;\
		case CORPORT_MAMBA:\
			module= "CORMAMBA.CXM";\
			proc= "CorMambaGetControl";\
		break;\
		case CORPORT_REBEL:\
			module= "CORREBELDIG.CXM";\
			proc= "CorRebelGetControl";\
		break;\
		case CORPORT_BANDITII:\
			module= "CORBDII.CXM";\
			proc= "CorBdIIGetControl";\
		break;\
		case CORPORT_X64:\
			module= "CORX64x.CXM";\
			proc= "CorX64GetControl";\
		break;\
		case CORPORT_X64AN:\
			module= "CORX64ANx.CXM";\
			proc= "";\
		break;\
		case CORPORT_PC2V:\
			module= "CORPC2Vx.CXM";\
			proc= "";\
		break;\
		case CORPORT_PC2V_NS:\
			module= "CORNSPx.CXM";\
			proc= "";\
		break;\
		case CORPORT_PC2C:\
			module= "CORPC2Cx.CXM";\
			proc= "";\
		break;\
		case CORPORT_ANACONDA:\
			module= "CORANCx.CXM";\
			proc= "";\
		break;\
		case CORPORT_X64LVDS:\
			module= "CORX64LVDSx.CXM";\
			proc= "CorX64LVDSGetControl";\
		break;\
		case CORPORT_X64NS:\
			module= "CORX64NSx.CXM";\
			proc= "";\
		break;\
		case CORPORT_X64CHL:\
			module= "CORX64CHLx.CXM";\
			proc= "CorX64CHLGetControl";\
		break;\
		case CORPORT_NEXUS:\
			module= "CORNexusX.CXM";\
			proc= "";\
		break;\
		case CORPORT_X64IPRO:\
			module= "CORX64IPROx.CXM";\
			proc= "";\
		break;\


#ifdef __cplusplus
extern "C" {
#endif

CORSTATUS CORAPIFUNC CorPortOpen (CORSERVER hServer, CORSERVER hDestServer, UINT32 porttype, PSTR params);
CORSTATUS CORAPIFUNC CorPortClose(CORSERVER hServer, CORSERVER hDestServer);

#ifdef __cplusplus
}
#endif

#endif   // _CAPIPORT_H_
