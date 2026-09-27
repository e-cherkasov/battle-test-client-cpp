#pragma once

#include "Component.hpp"

#include <cstdint>

namespace sw::core
{
	class Health : public Component
	{
	public:
		explicit Health(uint32_t hp) :
				_hp(hp)
		{}

		int64_t getHp() const
		{
			return _hp;
		}

		bool isAlive() const
		{
			return _hp > 0;
		}

	private:
		friend class World;

		void takeDamage(uint32_t damage)
		{
			_hp -= damage;
		}

		int64_t _hp;
	};
}
