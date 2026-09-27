#include "Attack.hpp"

#include "Core/Node.hpp"
#include "Core/Random.hpp"
#include "Core/TurnContext.hpp"
#include "Core/World.hpp"
#include "TargetFilters.hpp"

namespace sw::features
{
	bool attackRandomUnit(
		core::TurnContext& context, core::Node& attacker, uint32_t minRange, uint32_t maxRange, uint32_t damage)
	{
		if (damage == 0)
		{
			return false;
		}

		const auto targets = context.world.findInRange(
			attacker.getPosition(),
			minRange,
			maxRange,
			[&attacker](const core::Node& target) { return isAttackable(attacker, target); });
		if (targets.empty())
		{
			return false;
		}

		context.world.dealDamage(attacker, *context.random.pickOne(targets), damage);
		return true;
	}
}
