#pragma once

#include "Component.hpp"

#include <cstdint>

namespace sw::core
{
	class Health : public Component
	{
	public:
		explicit Health(uint32_t hp) :
				_hp(static_cast<int32_t>(hp))
		{}

		int32_t getHp() const
		{
			return _hp;
		}

		bool isAlive() const
		{
			return _hp > 0;
		}

		void takeDamage(uint32_t damage)
		{
			_hp -= static_cast<int32_t>(damage);
		}

	private:
		int32_t _hp;
	};
}
