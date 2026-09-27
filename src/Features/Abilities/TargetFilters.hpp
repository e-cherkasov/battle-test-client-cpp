#pragma once

#include "Core/Health.hpp"
#include "Core/Node.hpp"

namespace sw::features
{
	// Units without health can not be attacked.
	inline bool isAttackable(const core::Node& self, const core::Node& target)
	{
		return &target != &self && target.isAlive() && target.getComponent<core::Health>() != nullptr;
	}
}
