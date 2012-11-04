#include "Sampler.h"

namespace clw
{
	namespace detail
	{
		template<typename Value>
		Value samplerInfo(cl_sampler id, cl_sampler_info info)
		{
			Value value;
			cl_int error = CL_SUCCESS;
			if(!id || (error = clGetSamplerInfo(id, info, 
				sizeof(Value), &value, nullptr)) != CL_SUCCESS)
			{
				reportError("samplerInfo(): ", error);
				return Value(0);
			}
			return value;
		}

		template<>
		bool samplerInfo(cl_sampler id, cl_sampler_info info)
		{
			cl_bool value = samplerInfo<cl_bool>(id, info);
			return value != 0;
		}
	}

	Sampler::~Sampler()
	{
		if(id)
			clReleaseSampler(id);
	}

	Sampler::Sampler(const Sampler& other)
		: ctx(other.ctx), id(other.id)
	{
		if(id)
			clRetainSampler(id);
	}

	Sampler& Sampler::operator=(const Sampler& other)
	{
		ctx = other.ctx;
		if(other.id)
			clRetainSampler(other.id);
		if(id)
			clReleaseSampler(id);
		id = other.id;
		return *this;
	}

	EFilterMode Sampler::filterMode() const
	{
		return EFilterMode(detail::samplerInfo<cl_filter_mode>
			(id, CL_SAMPLER_FILTER_MODE));
	}

	EAddressingMode Sampler::addressingMode() const
	{
		return EAddressingMode(detail::samplerInfo<cl_addressing_mode>
			(id, CL_SAMPLER_ADDRESSING_MODE));
	}

	bool Sampler::normalizedCoords() const
	{
		return detail::samplerInfo<bool>(id, CL_SAMPLER_NORMALIZED_COORDS);
	}
}