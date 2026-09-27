#include <App/Scenario.hpp>
#include <Core/EventBus.hpp>
#include <IO/System/EventLog.hpp>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

static void run(const std::string& path)
{
	using namespace sw;

	std::ifstream file(path);
	if (!file)
	{
		throw std::runtime_error("File not found: " + path);
	}

	EventLog eventLog;
	core::EventBus events(eventLog);
	app::Scenario scenario(events);
	scenario.load(file);
	scenario.run();
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: sw_battle_test <scenario-file>\n";
		return 1;
	}

	try
	{
		run(argv[1]);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		return 1;
	}

	return 0;
}
