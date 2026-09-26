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

	private:
		int32_t _hp;
	};
}
