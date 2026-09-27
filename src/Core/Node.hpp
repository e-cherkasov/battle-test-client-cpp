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
	class World;
	struct TurnContext;

	class Node
	{
	public:
		Node(UnitId id, std::string type, Position position);

		UnitId getId() const;
		const std::string& getType() const;
		Position getPosition() const;

		// Flying units and mines will share cells with others, so they turn this off
		// Set it before addChild, because that is when a blocking unit is placed on the map
		bool blocksCell() const;
		void setBlocksCell(bool blocks);

		bool isAlive() const;

		template <class T, class... Args>
		T& addComponent(Args&&... args)
		{
			static_assert(std::is_base_of_v<Component, T>);
			auto component = std::make_unique<T>(std::forward<Args>(args)...);
			T& result = *component;
			static_cast<Component&>(result)._owner = this;
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

		template <class T>
		const T* getComponent() const
		{
			for (const auto& component : _components)
			{
				if (const auto* result = dynamic_cast<const T*>(component.get()))
				{
					return result;
				}
			}
			return nullptr;
		}

		// Destroys this node, so do not use it after the call
		void removeFromParent();

		// Plays one turn. Returns true if the node acted
		bool update(TurnContext& context);

	private:
		friend class World;

		void setPosition(Position position);

		UnitId _id;
		std::string _type;
		Position _position;
		bool _blocksCell = true;
		World* _parent = nullptr;
		std::vector<std::unique_ptr<Component>> _components;
	};
}
