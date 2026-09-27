#pragma once

namespace sw::core
{
	class Node;
	struct TurnContext;

	class Component
	{
	public:
		virtual ~Component() = default;

		// Returns true if the component used the owner's turn.
		virtual bool tryAct(TurnContext&)
		{
			return false;
		}

		Node* getOwner() const
		{
			return _owner;
		}

	private:
		friend class Node;

		Node* _owner = nullptr;
	};
}
