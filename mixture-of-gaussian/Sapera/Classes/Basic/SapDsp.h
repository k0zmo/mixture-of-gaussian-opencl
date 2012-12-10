#ifndef _SAPDSP_H_
#define _SAPDSP_H_

//	SapDsp.h : header file
//

#include "SapClassBasicDef.h"

//
// SapDsp class declaration
//
class SAPCLASSBASIC_CLASS SapDsp : public SapXferNode
{
public:
	// Constructor/Destructor
	SapDsp(SapLocation loc = SapLocation::ServerSystem, const char *fileName = "", SapXferNode *pSrcNode = NULL);
	SapDsp(SapLocation loc, const char *fileName, SapXferParams xferParams);
	SapDsp(const SapDsp &dsp);
	virtual ~SapDsp();

	SapDsp &operator=(const SapDsp &dsp);

	// Access to implementation
	const char *GetFile() const { return m_FileName; }
	virtual BOOL SetFile( const char *fileName);

	// Module create/destroy
	virtual BOOL Create();
	virtual BOOL Destroy();

protected:
	char m_FileName[MAX_PATH];
	static char m_LastFileName[MAX_PATH];
}; 

#endif	// _SAPDSP_H_

