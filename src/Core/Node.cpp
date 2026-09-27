#include "Node.hpp"

#include "Health.hpp"
#include "World.hpp"

#include <cassert>

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
		assert(_parent == nullptr);
		_blocksCell = blocks;
	}

	bool Node::isAlive() const
	{
		const auto* health = getComponent<Health>();
		return health == nullptr || health->isAlive();
	}

	void Node::removeFromParent()
	{
		assert(_parent != nullptr);
		_parent->removeChild(*this);
	}

	bool Node::update(TurnContext& context)
	{
		// Components are tried in the order they were added, so the unit factory sets the action priority
		for (auto& component : _components)
		{
			if (component->tryAct(context))
			{
				return true;
			}
		}
		return false;
	}

	void Node::setPosition(Position position)
	{
		_position = position;
	}
}
