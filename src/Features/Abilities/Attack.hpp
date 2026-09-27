#pragma once

#include <cstdint>

namespace sw::core
{
	class Node;
	struct TurnContext;
}

namespace sw::features
{
	// Hits one random attackable unit at a distance from minRange to maxRange
	bool attackRandomUnit(
		core::TurnContext& context, core::Node& attacker, uint32_t minRange, uint32_t maxRange, uint32_t damage);
}
