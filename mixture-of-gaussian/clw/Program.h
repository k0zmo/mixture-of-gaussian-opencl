#pragma once

#include "Prerequisites.h"

namespace clw
{
	typedef vector<unsigned char> ByteCode;

	class Program
	{
	public:
		Program() : ctx(nullptr), id(0) {}
		Program(Context* ctx, cl_program id)
			: ctx(ctx), id(id) {}
		~Program();
		
		Program(const Program& other);
		Program& operator=(const Program& other);
		
		bool isNull() const { return id == 0; }
		
		Context* context() const { return ctx; }
		cl_program programId() const { return id; }
		
		bool build(const string& options = string());
		//bool build(const vector<Device> devices, const string& options = string());
		
		string log() const;
		
		vector<Device> devices() const;
		string sourceCode() const;
		vector<ByteCode> binaries() const;

		Kernel createKernel(const char* name) const;
		Kernel createKernel(const string& name) const;
		vector<Kernel> createKernels() const;
		
	private:
		Context* ctx;
		cl_program id;
	};
}