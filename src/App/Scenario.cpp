#include "Scenario.hpp"

#include "Features/Components/March.hpp"
#include "Features/Units/Hunter.hpp"
#include "Features/Units/Swordsman.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/March.hpp"
#include "IO/Events/MarchStarted.hpp"
#include "IO/System/CommandParser.hpp"

#include <cassert>

namespace sw::app
{
	Scenario::Scenario(core::EventBus& events) :
			_events(events)
	{}

	template <class TSpawnCommand>
	void Scenario::registerUnit(io::CommandParser& parser, std::unique_ptr<core::Node> (*factory)(const TSpawnCommand&))
	{
		parser.add<TSpawnCommand>([this, factory](TSpawnCommand command) { getWorld().addChild(factory(command)); });
	}

	void Scenario::load(std::istream& stream)
	{
		io::CommandParser parser;

		parser.add<io::CreateMap>(
			[this](io::CreateMap command)
			{
				assert(!_world.has_value());
				_world.emplace(command.width, command.height, _events);
			});

		// The list of unit types: should add a new unit here
		registerUnit(parser, &features::makeSwordsman);
		registerUnit(parser, &features::makeHunter);

		parser.add<io::March>(
			[this](io::March command)
			{
				core::Node* unit = getWorld().getChildById(command.unitId);
				assert(unit != nullptr);

				const core::Position target{command.targetX, command.targetY};
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

	core::World& Scenario::getWorld()
	{
		assert(_world.has_value());
		return *_world;
	}
}
