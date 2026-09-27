#pragma once

#include "details/CommandParserVisitor.hpp"

#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace sw::io
{
	class CommandParser
	{
	private:
		// Returns why the command was skipped, or an empty string if it was handled
		std::unordered_map<std::string, std::function<std::string(std::istream&)>> _commands;

	public:
		template <class TCommandData>
		CommandParser& add(std::function<void(TCommandData)> handler)
		{
			std::string commandName = TCommandData::Name;
			auto [it, inserted] = _commands.emplace(
				commandName,
				[handler = std::move(handler)](std::istream& stream) -> std::string
				{
					TCommandData data;
					CommandParserVisitor visitor(stream);
					data.visit(visitor);
					if (visitor.getBadField() != nullptr)
					{
						return std::string("missing or invalid ") + visitor.getBadField();
					}
					std::string extra;
					if (stream >> extra)
					{
						return "unexpected argument " + extra;
					}
					handler(std::move(data));
					return {};
				});
			if (!inserted)
			{
				throw std::runtime_error("Command already exists: " + commandName);
			}

			return *this;
		}

		void parse(std::istream& stream);
	};
}
