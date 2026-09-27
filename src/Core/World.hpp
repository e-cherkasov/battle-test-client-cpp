#pragma once

#include "Geometry.hpp"
#include "Map.hpp"
#include "Node.hpp"
#include "Types.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace sw::core
{
	class EventBus;

	// Owns all units. Units move and hurt each other only through the world, which reports these changes
	class World
	{
	public:
		World(uint32_t width, uint32_t height, EventBus& events);

		void addChild(std::unique_ptr<Node> node);
		Node* getChildById(UnitId id);
		// Units in creation order which is also the turn order
		const std::vector<std::unique_ptr<Node>>& getChildren() const;

		// Looks through units, so units that do not block a cell are found too
		std::vector<Node*> findInRange(
			Position center,
			uint32_t minRange,
			uint32_t maxRange,
			const std::function<bool(const Node&)>& filter) const;

		bool contains(Position position) const;
		bool isCellFree(Position position) const;

		void move(Node& node, Position to);
		void dealDamage(const Node& attacker, Node& target, uint32_t damage);
		void removeDeadChildren();

		size_t getAliveCount() const;

	private:
		void removeChild(Node& node);

		Map _map;
		EventBus& _events;
		// Creation order, which is also the turn order.
		std::vector<std::unique_ptr<Node>> _children;
		std::unordered_map<UnitId, Node*> _childrenById;
	};
}
