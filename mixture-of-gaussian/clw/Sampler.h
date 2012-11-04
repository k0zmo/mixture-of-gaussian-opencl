#pragma once

#include "Prerequisites.h"

namespace clw
{


	enum EFilterMode
	{
		Filter_Nearest = 0x1140,
		Filter_Linear  = 0x1141
	};

	enum EAddressingMode
	{
		Addressing_None             = 0x1130,
		Addressing_Clamp_to_edge    = 0x1131,
		Addressing_Clamp            = 0x1132,
		Addressing_Repeat           = 0x1133,
		Addressing_Mirrored_repeat  = 0x1134
	};

	class Sampler
	{
	public:
		Sampler() : ctx(nullptr), id(0) {}
		Sampler(Context* ctx, cl_sampler id)
			: ctx(ctx), id(id) {}
		~Sampler();

		Sampler(const Sampler& other);
		Sampler& operator=(const Sampler& other);

		bool isNull() const { return id == 0; }
		
		Context* context() const { return ctx; }
		cl_sampler samplerId() const { return id; }

		EFilterMode filterMode() const;
		EAddressingMode addressingMode() const;
		bool normalizedCoords() const;

	private:
		Context* ctx;
		cl_sampler id;
	};
}