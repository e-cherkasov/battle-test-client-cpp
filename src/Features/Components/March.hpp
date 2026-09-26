#pragma once

#include "Core/Component.hpp"
#include "Core/Geometry.hpp"

namespace sw::features
{
	struct March : core::Component
	{
		explicit March(core::Position target) :
				target(target)
		{}

		core::Position target;
	};
}
