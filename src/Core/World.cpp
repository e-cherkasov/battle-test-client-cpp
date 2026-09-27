#include "World.hpp"

#include "EventBus.hpp"
#include "Health.hpp"
#include "IO/Events/MapCreated.hpp"
#include "IO/Events/UnitAttacked.hpp"
#include "IO/Events/UnitDied.hpp"
#include "IO/Events/UnitMoved.hpp"
#include "IO/Events/UnitSpawned.hpp"

#include <algorithm>
#include <cassert>

namespace sw::core
{
	World::World(uint32_t width, uint32_t height, EventBus& events) :
			_map(width, height),
			_events(events)
	{
		_events.emit(io::MapCreated{width, height});
	}

	void World::addChild(std::unique_ptr<Node> node)
	{
		assert(node != nullptr);
		assert(node->_parent == nullptr);
		assert(getChildById(node->getId()) == nullptr);

		const Position position = node->getPosition();
		if (node->blocksCell())
		{
			_map.placeUnit(position, node->getId());
		}
		_events.emit(io::UnitSpawned{node->getId(), node->getType(), position.x, position.y});

		node->_parent = this;
		_childrenById[node->getId()] = node.get();
		_children.push_back(std::move(node));
	}

	Node* World::getChildById(UnitId id)
	{
		auto it = _childrenById.find(id);
		return it != _childrenById.end() ? it->second : nullptr;
	}

	const std::vector<std::unique_ptr<Node>>& World::getChildren() const
	{
		return _children;
	}

	std::vector<Node*> World::findInRange(
		Position center, uint32_t minRange, uint32_t maxRange, const std::function<bool(const Node&)>& filter) const
	{
		std::vector<Node*> result;
		for (const auto& child : _children)
		{
			const uint32_t steps = distance(center, child->getPosition());
			if (steps >= minRange && steps <= maxRange && filter(*child))
			{
				result.push_back(child.get());
			}
		}
		return result;
	}

	bool World::contains(Position position) const
	{
		return _map.contains(position);
	}

	bool World::isCellFree(Position position) const
	{
		return _map.isFree(position);
	}

	void World::move(Node& node, Position to)
	{
		assert(node._parent == this);
		assert(_map.contains(to));

		if (node.blocksCell())
		{
			_map.removeUnit(node.getPosition());
			_map.placeUnit(to, node.getId());
		}
		node.setPosition(to);
		_events.emit(io::UnitMoved{node.getId(), to.x, to.y});
	}

	void World::dealDamage(const Node& attacker, Node& target, uint32_t damage)
	{
		auto* health = target.getComponent<Health>();
		assert(health != nullptr);

		const bool wasAlive = health->isAlive();
		health->takeDamage(damage);

		const auto targetHp = static_cast<uint32_t>(std::max<int64_t>(health->getHp(), 0));
		_events.emit(io::UnitAttacked{attacker.getId(), target.getId(), damage, targetHp});

		// The dead unit keeps its cell until the end of the round, since the task says it disappears next turn
		if (wasAlive && !health->isAlive())
		{
			_events.emit(io::UnitDied{target.getId()});
		}
	}

	void World::removeDeadChildren()
	{
		// Collect first because removeFromParent erases from the list we iterate
		std::vector<Node*> dead;
		for (const auto& child : _children)
		{
			if (!child->isAlive())
			{
				dead.push_back(child.get());
			}
		}
		for (Node* node : dead)
		{
			node->removeFromParent();
		}
	}

	size_t World::getAliveCount() const
	{
		return static_cast<size_t>(std::count_if(
			_children.begin(), _children.end(), [](const std::unique_ptr<Node>& child) { return child->isAlive(); }));
	}

	void World::removeChild(Node& node)
	{
		assert(node._parent == this);

		if (node.blocksCell())
		{
			_map.removeUnit(node.getPosition());
		}
		_childrenById.erase(node.getId());

		auto it = std::find_if(
			_children.begin(),
			_children.end(),
			[&node](const std::unique_ptr<Node>& child) { return child.get() == &node; });
		assert(it != _children.end());
		_children.erase(it);
	}
}
