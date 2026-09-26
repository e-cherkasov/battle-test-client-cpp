#pragma once

#include "Core/EventBus.hpp"
#include "Core/Node.hpp"
#include "Core/World.hpp"

#include <iosfwd>
#include <memory>
#include <optional>

namespace sw::io
{
	class CommandParser;
}

namespace sw::app
{
	// Binds scenario commands to the world. The world exists only when map exists
	class Scenario
	{
	public:
		explicit Scenario(core::EventBus& events);

		void load(std::istream& stream);

	private:
		template <class TSpawnCommand>
		void registerUnit(io::CommandParser& parser, std::unique_ptr<core::Node> (*factory)(const TSpawnCommand&));

		core::World& getWorld();

		core::EventBus& _events;
		std::optional<core::World> _world;
	};
}
