#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace sw::core
{
	class Random
	{
	public:
		explicit Random(uint32_t seed) :
				_engine(seed)
		{}

		template <class T>
		const T& pickOne(const std::vector<T>& items)
		{
			assert(!items.empty());
			std::uniform_int_distribution<size_t> distribution(0, items.size() - 1);
			return items[distribution(_engine)];
		}

	private:
		std::mt19937 _engine;
	};
}
