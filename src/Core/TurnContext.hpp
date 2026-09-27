#pragma once

namespace sw::core
{
	class World;
	class Random;
	class EventBus;

	struct TurnContext
	{
		World& world;
		Random& random;
		EventBus& events;
	};
}
