#include "Map.hpp"

#include <cassert>

namespace sw::core
{
	Map::Map(uint32_t width, uint32_t height) :
			_width(width),
			_height(height),
			_cells(static_cast<size_t>(width) * height)
	{}

	bool Map::contains(Position position) const
	{
		return position.x < _width && position.y < _height;
	}

	bool Map::isFree(Position position) const
	{
		return contains(position) && !_cells[toIndex(position)].has_value();
	}

	void Map::placeUnit(Position position, UnitId id)
	{
		assert(isFree(position));
		_cells[toIndex(position)] = id;
	}

	void Map::removeUnit(Position position)
	{
		assert(contains(position));
		_cells[toIndex(position)].reset();
	}

	size_t Map::toIndex(Position position) const
	{
		return static_cast<size_t>(position.y) * _width + position.x;
	}
}
