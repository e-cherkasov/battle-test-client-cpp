#pragma once

#include "Random.hpp"

namespace sw::core
{
	class EventBus;
	class World;

	class Simulation
	{
	public:
		Simulation(World& world, EventBus& events);

		void run();

	private:
		World& _world;
		EventBus& _events;
		Random _random;
	};
}
