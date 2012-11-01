#pragma once

#include "Prerequisites.h"

namespace clw
{
	class Context
	{
	public:
		Context() : id(0) {}
		~Context();

		cl_context contextId() const { return id; }
	private:
		cl_context id;
	};
}