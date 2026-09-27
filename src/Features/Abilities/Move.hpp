#pragma once

#include "Core/Component.hpp"

namespace sw::features
{
	// Walks the owner towards its march target, one cell per turn
	class Move : public core::Component
	{
	public:
		bool tryAct(core::TurnContext& context) override;
	};
}
