#include "RangedAttack.hpp"

#include "Attack.hpp"
#include "Core/Node.hpp"
#include "Core/TurnContext.hpp"
#include "Core/World.hpp"

namespace sw::features
{
	namespace
	{
		constexpr uint32_t kAdjacentDistance = 1;

		// A unit that died this round still stands on its cell, but it no longer takes part in the fight
		bool hasLivingNeighbour(const core::World& world, const core::Node& self)
		{
			const auto neighbours = world.findInRange(
				self.getPosition(),
				kAdjacentDistance,
				kAdjacentDistance,
				[&self](const core::Node& other) { return &other != &self && other.isAlive(); });
			return !neighbours.empty();
		}
	}

	RangedAttack::RangedAttack(const Params& params) :
			_params(params)
	{}

	bool RangedAttack::tryAct(core::TurnContext& context)
	{
		core::Node& self = *getOwner();
		if (_params.requireNoAdjacentUnits && hasLivingNeighbour(context.world, self))
		{
			return false;
		}
		return attackRandomUnit(context, self, _params.minRange, _params.maxRange, _params.damage);
	}
}
