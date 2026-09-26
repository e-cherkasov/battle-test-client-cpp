#pragma once

#include "Component.hpp"
#include "Geometry.hpp"
#include "Types.hpp"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace sw::core
{
	class Node
	{
	public:
		Node(UnitId id, std::string type, Position position);

		UnitId getId() const;
		const std::string& getType() const;
		Position getPosition() const;

		// Lets a unit type that shares cells (e.g. a flying unit or a mine) opt out without Core changes.
		// Must be set before the node is added to a World: addChild places blocking units on the map.
		bool blocksCell() const;
		void setBlocksCell(bool blocks);

		template <class T, class... Args>
		T& addComponent(Args&&... args)
		{
			static_assert(std::is_base_of_v<Component, T>);
			auto component = std::make_unique<T>(std::forward<Args>(args)...);
			T& result = *component;
			_components.push_back(std::move(component));
			return result;
		}

		template <class T>
		T* getComponent()
		{
			for (auto& component : _components)
			{
				if (auto* result = dynamic_cast<T*>(component.get()))
				{
					return result;
				}
			}
			return nullptr;
		}

	private:
		UnitId _id;
		std::string _type;
		Position _position;
		bool _blocksCell = true;
		std::vector<std::unique_ptr<Component>> _components;
	};
}
