#pragma once

#include "Core/World.hpp"

#include <iosfwd>
#include <memory>
#include <optional>

namespace sw::core
{
	class EventBus;
}

namespace sw::io
{
	class CommandParser;
}

namespace sw::app
{
	// Connects scenario commands to the world. The world exists only after CREATE_MAP.
	class Scenario
	{
	public:
		explicit Scenario(core::EventBus& events);

		void load(std::istream& stream);
		void run();

	private:
		template <class TSpawnCommand>
		void registerUnit(io::CommandParser& parser, std::unique_ptr<core::Node> (*factory)(const TSpawnCommand&));

		core::World& getWorld();

		core::EventBus& _events;
		std::optional<core::World> _world;
	};
}
