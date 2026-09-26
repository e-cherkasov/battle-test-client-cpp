#pragma once

#include "EventBus.hpp"
#include "Map.hpp"
#include "Node.hpp"
#include "Types.hpp"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace sw::core
{
	// Owns all units and is the only place that changes world state
	class World
	{
	public:
		World(uint32_t width, uint32_t height, EventBus& events);

		void addChild(std::unique_ptr<Node> node);
		Node* getChildById(UnitId id);

	private:
		Map _map;
		EventBus& _events;
		// Creation order, which is also the turn order.
		std::vector<std::unique_ptr<Node>> _children;
		std::unordered_map<UnitId, Node*> _childrenById;
	};
}
