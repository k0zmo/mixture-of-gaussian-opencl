#include "NDRange.h"

namespace clw
{
	namespace detail
	{
		size_t roundUp(size_t value, size_t multiple)
		{
			if(multiple <= 1)
				return value;
			int v = value % multiple;
			if (v)
				return value + (multiple - v);
			return value;
		}
	}

	NDRange NDRange::roundTo(const NDRange& range) const
	{
		switch(dims)
		{
		case 1:
			return NDRange(detail::roundUp(width(), range.width()));
		case 2:
			return NDRange(detail::roundUp(width(), range.width()),
			               detail::roundUp(height(), range.height()));
		case 3:
			return NDRange(detail::roundUp(width(), range.width()),
			               detail::roundUp(height(), range.height()),
			               detail::roundUp(depth(), range.depth()));
		default:
			return NDRange();
		}
	}
}