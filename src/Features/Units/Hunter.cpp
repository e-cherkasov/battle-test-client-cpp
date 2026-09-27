#include "Hunter.hpp"

#include "Core/Health.hpp"
#include "Features/Abilities/MeleeAttack.hpp"
#include "Features/Abilities/Move.hpp"

namespace sw::features
{
	std::unique_ptr<core::Node> makeHunter(const io::SpawnHunter& command)
	{
		auto unit = std::make_unique<core::Node>(command.unitId, "Hunter", core::Position{command.x, command.y});
		unit->addComponent<core::Health>(command.hp);
		unit->addComponent<MeleeAttack>(command.strength);
		unit->addComponent<Move>();
		return unit;
	}
}
