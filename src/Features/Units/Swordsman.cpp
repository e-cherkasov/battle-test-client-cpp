#include "Swordsman.hpp"

#include "Core/Health.hpp"

namespace sw::features
{
	std::unique_ptr<core::Node> makeSwordsman(const io::SpawnSwordsman& command)
	{
		auto unit = std::make_unique<core::Node>(command.unitId, "Swordsman", core::Position{command.x, command.y});
		unit->addComponent<core::Health>(command.hp);
		return unit;
	}
}
