#pragma once

#include "Geometry.hpp"
#include "Types.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace sw::core
{
	class Map
	{
	public:
		Map(uint32_t width, uint32_t height);

		bool contains(Position position) const;
		bool isFree(Position position) const;
		void placeUnit(Position position, UnitId id);

	private:
		size_t toIndex(Position position) const;

		uint32_t _width;
		uint32_t _height;
		std::vector<std::optional<UnitId>> _cells;
	};
}
