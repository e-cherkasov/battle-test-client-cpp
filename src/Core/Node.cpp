#include "Node.hpp"

namespace sw::core
{
	Node::Node(UnitId id, std::string type, Position position) :
			_id(id),
			_type(std::move(type)),
			_position(position)
	{}

	UnitId Node::getId() const
	{
		return _id;
	}

	const std::string& Node::getType() const
	{
		return _type;
	}

	Position Node::getPosition() const
	{
		return _position;
	}

	bool Node::blocksCell() const
	{
		return _blocksCell;
	}

	void Node::setBlocksCell(bool blocks)
	{
		_blocksCell = blocks;
	}
}
