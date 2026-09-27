#pragma once

#include "Core/Component.hpp"

#include <cstdint>

namespace sw::features
{
	class RangedAttack : public core::Component
	{
	public:
		struct Params
		{
			uint32_t damage;
			uint32_t minRange;
			uint32_t maxRange;
			// Set for the hunter, who can not shoot while someone stands next to it
			bool requireNoAdjacentUnits;
		};

		explicit RangedAttack(const Params& params);

		bool tryAct(core::TurnContext& context) override;

	private:
		Params _params;
	};
}
