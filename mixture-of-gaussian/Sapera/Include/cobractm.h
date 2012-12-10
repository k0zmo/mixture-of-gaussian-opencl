//
// Cobra board Custom functions.
//

#ifndef _COBRACTM_H_
#define _COBRACTM_H_

#include <cordef.h>
#include <corenv.h>

#include "cpackd.h"

//
// Acquisition Custom Functions
//

//
// 3AD10 (RGB) Only
//
#define COBRA_CUSTOM_CMD_CB_CONTROL	0x00000000
#define COBRA_CUSTOM_CMD_REFERENCE	0x00000001
#define COBRA_CUSTOM_CMD_WRITE_ICS1522	0x00000002
#define COBRA_CUSTOM_CMD_READ_INVCLK_FREQ	0x00000003		// And 3AD10-Mono
#define COBRA_CUSTOM_CMD_WRITE_INVCLK_FREQ	0x00000004		// And 3AD10-Mono

#define COBRA_RED_CHANNEL		0
#define COBRA_GREEN_CHANNEL	1
#define COBRA_BLUE_CHANNEL		2

typedef struct
{
	DWORD Top;
	DWORD Bottom;
	DWORD ClampLevel;
} _COBRA_CUSTOM_REFERENCE;

typedef struct
{
	_COBRA_CUSTOM_REFERENCE	Channel[3];
}	COBRA_CUSTOM_REFERENCE, *PCOBRA_CUSTOM_REFERENCE;

typedef struct
{
	UINT32 regId;
	UINT32 value;
} COBRA_CUSTOM_WRITE_ICS1522;

typedef struct
{
	UINT32 input;	// 0:Red, 1:Green: 2:Blue, 3: All 3 A/D
	UINT32 minFreq;	// Minimum frequency in Hz before inverting pixel clock
	UINT32 maxFreq;	// Maximum frequency in Hz before re-inverting the pixel clock
} COBRA_CUSTOM_INVCLK_FREQ;

//
// 3AD10 (Mono) Only
//
#define COBRA_CUSTOM_CMD_CB_CONTROL_MONO	0x00000000
#define COBRA_CUSTOM_CMD_REFERENCE_MONO	0x00000001
#define COBRA_CUSTOM_CMD_WRITE_ICS1522_MONO	COBRA_CUSTOM_CMD_WRITE_ICS1522 	


typedef _COBRA_CUSTOM_REFERENCE	COBRA_CUSTOM_REFERENCE_MONO, *PCOBRA_CUSTOM_REFERENCE_MONO;

#include "cunpackd.h"

#endif   // _COBRACTM_H_
