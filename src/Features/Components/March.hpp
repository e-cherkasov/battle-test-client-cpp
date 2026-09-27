#pragma once

#include "Core/Component.hpp"
#include "Core/Geometry.hpp"

#include <optional>

namespace sw::features
{
	struct March : core::Component
	{
		explicit March(core::Position target) :
				target(target)
		{}

		std::optional<core::Position> target;
	};
}
