#include "Simulation.hpp"

#include "EventBus.hpp"
#include "TurnContext.hpp"
#include "World.hpp"

namespace sw::core
{
	Simulation::Simulation(World& world, EventBus& events, uint32_t seed) :
			_world(world),
			_events(events),
			_random(seed)
	{}

	void Simulation::run()
	{
		TurnContext context{_world, _random, _events};
		bool idle = false;
		while (_world.getAliveCount() > 1 && !idle)
		{
			_events.nextTick();

			bool anyActed = false;
			for (const auto& child : _world.getChildren())
			{
				if (child->isAlive())
				{
					anyActed |= child->update(context);
				}
			}
			_world.removeDeadChildren();

			// If nobody acted the world did not change and the next round would be the same
			idle = !anyActed;
		}
	}
}
