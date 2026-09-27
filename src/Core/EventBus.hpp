#pragma once

#include "IO/System/EventLog.hpp"

#include <cstdint>
#include <utility>

namespace sw::core
{
	class EventBus
	{
	public:
		explicit EventBus(EventLog& log) :
				_log(log)
		{}

		void nextTick()
		{
			++_tick;
		}

		template <class TEvent>
		void emit(TEvent&& event)
		{
			_log.log(_tick, std::forward<TEvent>(event));
		}

	private:
		EventLog& _log;
		// Scenario setup commands are logged at tick 1, simulation rounds start from tick 2.
		uint64_t _tick = 1;
	};
}
