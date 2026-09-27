#pragma once

#include "Core/Component.hpp"

#include <cstdint>

namespace sw::features
{
	class MeleeAttack : public core::Component
	{
	public:
		explicit MeleeAttack(uint32_t damage);

		bool tryAct(core::TurnContext& context) override;

	private:
		uint32_t _damage;
	};
}
