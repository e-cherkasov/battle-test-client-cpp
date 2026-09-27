#include "MeleeAttack.hpp"

#include "Core/Node.hpp"
#include "Core/Random.hpp"
#include "Core/TurnContext.hpp"
#include "Core/World.hpp"
#include "TargetFilters.hpp"

namespace sw::features
{
	namespace
	{
		constexpr uint32_t kMeleeRange = 1;
	}

	MeleeAttack::MeleeAttack(uint32_t damage) :
			_damage(damage)
	{}

	bool MeleeAttack::tryAct(core::TurnContext& context)
	{
		core::Node& self = *getOwner();
		const auto targets = context.world.findInRange(
			self.getPosition(),
			kMeleeRange,
			kMeleeRange,
			[&self](const core::Node& target) { return isAttackable(self, target); });
		if (targets.empty())
		{
			return false;
		}

		context.world.dealDamage(self, *context.random.pickOne(targets), _damage);
		return true;
	}
}
