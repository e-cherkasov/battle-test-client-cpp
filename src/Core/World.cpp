#include "World.hpp"

#include "IO/Events/MapCreated.hpp"
#include "IO/Events/UnitSpawned.hpp"

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
		assert(getChildById(node->getId()) == nullptr);

		const Position position = node->getPosition();
		if (node->blocksCell())
		{
			_map.placeUnit(position, node->getId());
		}
		_events.emit(io::UnitSpawned{node->getId(), node->getType(), position.x, position.y});

		_childrenById[node->getId()] = node.get();
		_children.push_back(std::move(node));
	}

	Node* World::getChildById(UnitId id)
	{
		auto it = _childrenById.find(id);
		return it != _childrenById.end() ? it->second : nullptr;
	}
}
