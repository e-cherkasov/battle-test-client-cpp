#include "CommandParser.hpp"

#include <iostream>
#include <sstream>

namespace sw::io
{
	void CommandParser::parse(std::istream& stream)
	{
		std::string line;
		while (std::getline(stream, line))
		{
			std::istringstream commandStream(line);
			std::string commandName;
			commandStream >> commandName;

			if (commandName.empty() || commandName.rfind("//", 0) == 0)
			{
				continue;
			}

			// A broken line is skipped so the rest of the scenario still runs
			auto command = _commands.find(commandName);
			const std::string error = command == _commands.end() ? "unknown command" : command->second(commandStream);
			if (!error.empty())
			{
				std::cerr << "Warning: " << commandName << " skipped: " << error << '\n';
			}
		}
	}
}
