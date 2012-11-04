#include "Event.h"
#include "StlUtils.h"

namespace clw
{
	namespace detail
	{
		template<typename Value>
		Value eventInfo(cl_event id, cl_event_info info)
		{
			Value value;
			cl_int error = CL_SUCCESS;			
			if(!id || (error = clGetEventInfo(id, info, 
			        sizeof(Value), &value, nullptr)) != CL_SUCCESS)
			{
				reportError("eventInfo(): ", error);
				return Value(-1);
			}
			return value;
		}

		cl_ulong eventProfilingInfo(cl_event id, cl_profiling_info info)
		{
			cl_ulong value;
			cl_int error = CL_SUCCESS;
			if(!id || (error = clGetEventProfilingInfo(id, info, 
			        sizeof(cl_ulong), &value, nullptr)) != CL_SUCCESS)
			{
				reportError("eventInfo(): ", error);
				return cl_ulong(0);
			}
			return value;
		}
	}

	Event::~Event()
	{
		if(id)
			clReleaseEvent(id);
	}

	Event::Event(const Event& other)
		: id(other.id)
	{
		if(id)
			clRetainEvent(id);
	}

	Event& Event::operator=(const Event& other)
	{
		if(other.id)
			clRetainEvent(other.id);
		if(id)
			clReleaseEvent(id);
		id = other.id;
		return *this;
	}

	EEventStatus Event::status() const
	{
		return EEventStatus(detail::eventInfo<cl_int>
			(id, CL_EVENT_COMMAND_EXECUTION_STATUS));
	}

	ECommandType Event::commandType() const
	{
		return ECommandType(detail::eventInfo<cl_command_type>
			(id, CL_EVENT_COMMAND_TYPE));
	}

	void Event::waitForFinished()
	{
		if(id)
		{
			cl_int error;
			if((error = clWaitForEvents(1, &id)) != CL_SUCCESS)
				detail::reportError("Event::waitForFinished(): ", error);
		}
	}

	uint64_t Event::queueTime() const
	{
		return uint64_t(detail::eventProfilingInfo
			(id, CL_PROFILING_COMMAND_QUEUED));
	}

	uint64_t Event::submitTime() const
	{
		return uint64_t(detail::eventProfilingInfo
			(id, CL_PROFILING_COMMAND_SUBMIT));
	}

	uint64_t Event::startTime() const
	{
		return uint64_t(detail::eventProfilingInfo
			(id, CL_PROFILING_COMMAND_START));
	}

	uint64_t Event::finishTime() const
	{
		return uint64_t(detail::eventProfilingInfo
			(id, CL_PROFILING_COMMAND_END));
	}

	EventList::EventList(const Event& event)
	{
		append(event);
	}

	EventList::~EventList()
	{
		for(size_t i = 0; i < events.size(); ++i)
			clReleaseEvent(events[i]);
	}

	EventList::EventList(const EventList& other)
		: events(other.events)
	{
		for(size_t i = 0; i < events.size(); ++i)
			clRetainEvent(events[i]);
	}

	EventList& EventList::operator=(const EventList& other)
	{
		for(size_t i = 0; i < events.size(); ++i)
			clReleaseEvent(events[i]);
		events = other.events;
		for(size_t i = 0; i < events.size(); ++i)
			clRetainEvent(events[i]);
		return *this;
	}

	void EventList::waitForFinished()
	{
		if(events.empty())
			return;
		cl_int error;
		if((error = clWaitForEvents(events.size(),
		        operator const cl_event*())) != CL_SUCCESS)
			detail::reportError("EventList::waitForFinished() ", error);
	}

	void EventList::append(const Event& event)
	{
		cl_event id = event.eventId();
		if(id)
		{
			clRetainEvent(id);
			events.push_back(id);
		}
	}

	void EventList::append(const EventList& other)
	{
		for(size_t i = 0; i < other.size(); ++i)
		{
			cl_event e = other.events[i];
			clRetainEvent(e);
			events.push_back(e);
		}
	}

	void EventList::remove(const Event& event)
	{
		clw::remove_value(&events, event.eventId());
	}

	Event EventList::at(size_t index) const
	{
		if(index < events.size())
		{
			cl_event e = events[index];
			clRetainEvent(e);
			return Event(e);
		}
		return Event();
	}

	bool EventList::contains(const Event& event) const
	{
		auto it = std::find_if(events.begin(), events.end(),
			[&event](const cl_event& id)
			{
				return id == event.eventId();
			});
		return it != events.end();
	}
}