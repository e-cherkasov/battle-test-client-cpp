#pragma once

#include <algorithm>
#include <cstdint>

namespace sw::core
{
	struct Position
	{
		uint32_t x{};
		uint32_t y{};

		bool operator==(const Position&) const = default;
	};

	// Number of steps between two cells for a unit that can move to any of its 8 neighbours
	inline uint32_t distance(Position a, Position b)
	{
		const uint32_t dx = a.x > b.x ? a.x - b.x : b.x - a.x;
		const uint32_t dy = a.y > b.y ? a.y - b.y : b.y - a.y;
		return std::max(dx, dy);
	}
}
