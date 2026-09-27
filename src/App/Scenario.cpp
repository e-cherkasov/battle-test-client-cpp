#include "Scenario.hpp"

#include "Core/EventBus.hpp"
#include "Core/Simulation.hpp"
#include "Features/Components/March.hpp"
#include "Features/Units/Hunter.hpp"
#include "Features/Units/Swordsman.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/March.hpp"
#include "IO/Events/MarchStarted.hpp"
#include "IO/System/CommandParser.hpp"

#include <cstdint>
#include <iostream>
#include <string>

namespace sw::app
{
	namespace
	{
		constexpr uint64_t kMaxMapCells = 1000000;

		void skipCommand(const char* command, const std::string& reason)
		{
			std::cerr << "Warning: " << command << " skipped: " << reason << '\n';
		}

		std::string toString(core::Position position)
		{
			return "(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")";
		}
	}

	Scenario::Scenario(core::EventBus& events) :
			_events(events)
	{}

	template <class TSpawnCommand>
	void Scenario::registerUnit(io::CommandParser& parser, std::unique_ptr<core::Node> (*factory)(const TSpawnCommand&))
	{
		parser.add<TSpawnCommand>(
			[this, factory](TSpawnCommand command)
			{
				auto unit = factory(command);
				if (canSpawn(TSpawnCommand::Name, *unit))
				{
					_world->addChild(std::move(unit));
				}
			});
	}

	void Scenario::load(std::istream& stream)
	{
		io::CommandParser parser;

		parser.add<io::CreateMap>(
			[this](io::CreateMap command)
			{
				if (_world.has_value())
				{
					skipCommand(io::CreateMap::Name, "map is already created");
					return;
				}
				if (command.width == 0 || command.height == 0)
				{
					skipCommand(io::CreateMap::Name, "map must have at least one cell");
					return;
				}
				if (uint64_t{command.width} * command.height > kMaxMapCells)
				{
					skipCommand(
						io::CreateMap::Name, "map is too big, at most " + std::to_string(kMaxMapCells) + " cells");
					return;
				}
				_world.emplace(command.width, command.height, _events);
			});

		// Supported unit types
		registerUnit(parser, &features::makeSwordsman);
		registerUnit(parser, &features::makeHunter);

		parser.add<io::March>(
			[this](io::March command)
			{
				if (!hasMap(io::March::Name))
				{
					return;
				}

				core::Node* unit = _world->getChildById(command.unitId);
				if (unit == nullptr)
				{
					skipCommand(io::March::Name, "unknown unit " + std::to_string(command.unitId));
					return;
				}

				const core::Position target{command.targetX, command.targetY};
				if (!_world->contains(target))
				{
					skipCommand(io::March::Name, "target " + toString(target) + " is outside the map");
					return;
				}

				if (auto* march = unit->getComponent<features::March>())
				{
					march->target = target;
				}
				else
				{
					unit->addComponent<features::March>(target);
				}

				const core::Position position = unit->getPosition();
				_events.emit(io::MarchStarted{command.unitId, position.x, position.y, target.x, target.y});
			});

		parser.parse(stream);
	}

	void Scenario::run(uint32_t seed)
	{
		if (!_world.has_value())
		{
			std::cerr << "Warning: scenario has no CREATE_MAP\n";
			return;
		}
		core::Simulation(*_world, _events, seed).run();
	}

	bool Scenario::hasMap(const char* command)
	{
		if (!_world.has_value())
		{
			skipCommand(command, "the scenario must start with CREATE_MAP");
			return false;
		}
		return true;
	}

	bool Scenario::canSpawn(const char* command, const core::Node& unit)
	{
		if (!hasMap(command))
		{
			return false;
		}

		const core::Position position = unit.getPosition();
		if (_world->getChildById(unit.getId()) != nullptr)
		{
			skipCommand(command, "unit " + std::to_string(unit.getId()) + " already exists");
			return false;
		}
		if (!unit.isAlive())
		{
			skipCommand(command, "unit " + std::to_string(unit.getId()) + " has no HP");
			return false;
		}
		if (!_world->contains(position))
		{
			skipCommand(command, toString(position) + " is outside the map");
			return false;
		}
		if (unit.blocksCell() && !_world->isCellFree(position))
		{
			skipCommand(command, toString(position) + " is occupied");
			return false;
		}
		return true;
	}
}
