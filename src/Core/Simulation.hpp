#pragma once

#include "Random.hpp"

#include <cstdint>

namespace sw::core
{
	class EventBus;
	class World;

	class Simulation
	{
	public:
		// The same seed replays the same battle
		Simulation(World& world, EventBus& events, uint32_t seed);

		void run();

	private:
		World& _world;
		EventBus& _events;
		Random _random;
	};
}
