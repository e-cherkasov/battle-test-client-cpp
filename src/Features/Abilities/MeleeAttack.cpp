#include "MeleeAttack.hpp"

#include "Attack.hpp"

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
		return attackRandomUnit(context, *getOwner(), kMeleeRange, kMeleeRange, _damage);
	}
}
