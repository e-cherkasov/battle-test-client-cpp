#include "Move.hpp"

#include "Core/EventBus.hpp"
#include "Core/Node.hpp"
#include "Core/TurnContext.hpp"
#include "Core/World.hpp"
#include "Features/Components/March.hpp"
#include "IO/Events/MarchEnded.hpp"

#include <cstdint>

namespace sw::features
{
	namespace
	{
		uint32_t stepCloser(uint32_t from, uint32_t to)
		{
			if (from < to)
			{
				return from + 1;
			}
			if (from > to)
			{
				return from - 1;
			}
			return from;
		}

		core::Position stepTowards(core::Position from, core::Position to)
		{
			return {stepCloser(from.x, to.x), stepCloser(from.y, to.y)};
		}

		void endMarch(core::TurnContext& context, const core::Node& unit, March& march)
		{
			const core::Position position = unit.getPosition();
			context.events.emit(io::MarchEnded{unit.getId(), position.x, position.y});
			march.target.reset();
		}
	}

	bool Move::tryAct(core::TurnContext& context)
	{
		core::Node& self = *getOwner();
		auto* march = self.getComponent<March>();
		if (march == nullptr || !march->target.has_value())
		{
			return false;
		}

		const core::Position target = *march->target;
		if (self.getPosition() == target)
		{
			endMarch(context, self, *march);
			return false;
		}

		const core::Position next = stepTowards(self.getPosition(), target);
		// Wait until the cell is free
		if (!context.world.isCellFree(next))
		{
			return false;
		}

		context.world.move(self, next);
		if (next == target)
		{
			endMarch(context, self, *march);
		}
		return true;
	}
}
