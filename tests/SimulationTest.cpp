// Each test runs a scenario in the task's command format and checks the printed event log.
// Most scenarios leave a single possible target, so they do not depend on the seed.
// Random choice itself is checked with fixed seeds.

#include "App/Scenario.hpp"
#include "Core/EventBus.hpp"
#include "IO/System/EventLog.hpp"

#include <cstdint>
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	struct Event
	{
		uint64_t tick{};
		std::string name;
		std::map<std::string, std::string> fields;

		uint32_t get(const std::string& key) const
		{
			return static_cast<uint32_t>(std::stoul(fields.at(key)));
		}
	};

	struct ScenarioResult
	{
		std::string log;
		std::string warnings;
		std::vector<Event> events;
	};

	class StreamRedirect
	{
	public:
		StreamRedirect(std::ostream& stream, std::ostream& target) :
				_stream(stream),
				_previous(stream.rdbuf(target.rdbuf()))
		{}

		~StreamRedirect()
		{
			_stream.rdbuf(_previous);
		}

	private:
		std::ostream& _stream;
		std::streambuf* _previous;
	};

	// "[4] UNIT_ATTACKED attackerUnitId=2 targetUnitId=1 damage=5 targetHp=0"
	std::vector<Event> parseLog(const std::string& log)
	{
		std::vector<Event> events;
		std::istringstream lines(log);
		std::string line;
		while (std::getline(lines, line))
		{
			std::istringstream tokens(line);
			Event event;
			char bracket{};
			tokens >> bracket >> event.tick >> bracket >> event.name;
			std::string field;
			while (tokens >> field)
			{
				const auto separator = field.find('=');
				event.fields[field.substr(0, separator)] = field.substr(separator + 1);
			}
			events.push_back(event);
		}
		return events;
	}

	ScenarioResult runScenario(const std::string& commands, uint32_t seed = 0)
	{
		std::ostringstream log;
		std::ostringstream warnings;
		{
			StreamRedirect redirectLog(std::cout, log);
			StreamRedirect redirectWarnings(std::cerr, warnings);
			sw::EventLog eventLog;
			sw::core::EventBus events(eventLog);
			sw::app::Scenario scenario(events);
			std::istringstream input(commands);
			scenario.load(input);
			scenario.run(seed);
		}
		return {log.str(), warnings.str(), parseLog(log.str())};
	}

	std::vector<Event> find(
		const std::vector<Event>& events, const std::string& name, const std::string& key, uint32_t value)
	{
		std::vector<Event> result;
		for (const auto& event : events)
		{
			if (event.name == name && event.get(key) == value)
			{
				result.push_back(event);
			}
		}
		return result;
	}

	std::vector<Event> attacksBy(const std::vector<Event>& events, uint32_t unitId)
	{
		return find(events, "UNIT_ATTACKED", "attackerUnitId", unitId);
	}

	std::vector<Event> movesOf(const std::vector<Event>& events, uint32_t unitId)
	{
		return find(events, "UNIT_MOVED", "unitId", unitId);
	}

	constexpr uint32_t kHunterAgility = 5;
	constexpr uint32_t kHunterStrength = 1;
	constexpr uint32_t kHunterRange = 4;

	const std::string kMap = "CREATE_MAP 10 10\n";
	// Hunter 1 at (0,0): HP 10, Agility 5, Strength 1, Range 4
	const std::string kHunter = "SPAWN_HUNTER 1 0 0 10 5 1 4\n";

	std::string swordsman(uint32_t id, uint32_t x, uint32_t y, uint32_t hp, uint32_t strength = 0)
	{
		std::ostringstream command;
		command << "SPAWN_SWORDSMAN " << id << ' ' << x << ' ' << y << ' ' << hp << ' ' << strength << '\n';
		return command.str();
	}
}

// README, Hunter: shoots a random unit 2 to Range cells away and deals Agility damage
TEST(Hunter, ShootsOnlyAtDistanceFromTwoToRange)
{
	const std::map<uint32_t, bool> shootsAtDistance = {{2, true}, {kHunterRange, true}, {kHunterRange + 1, false}};
	for (const auto& [distance, shoots] : shootsAtDistance)
	{
		const ScenarioResult result = runScenario(kMap + kHunter + swordsman(2, distance, 0, 100));
		SCOPED_TRACE("distance " + std::to_string(distance) + "\n" + result.log);

		const auto attacks = attacksBy(result.events, 1);
		ASSERT_EQ(!attacks.empty(), shoots);
		if (shoots)
		{
			EXPECT_EQ(attacks[0].get("damage"), kHunterAgility);
		}
	}
}

// README, Hunter: can shoot only when no other unit is adjacent,
// otherwise hits a random adjacent unit and deals Strength damage
TEST(Hunter, HitsNeighbourInMeleeUntilItDiesAndThenShoots)
{
	// Unit 2 stands next to the hunter and dies after two hits, unit 3 is in shooting range
	const ScenarioResult result
		= runScenario(kMap + kHunter + swordsman(2, 1, 0, 2 * kHunterStrength) + swordsman(3, 3, 0, 100));
	SCOPED_TRACE(result.log);

	const auto attacks = attacksBy(result.events, 1);
	ASSERT_GE(attacks.size(), 3u);
	EXPECT_EQ(attacks[0].get("targetUnitId"), 2u);
	EXPECT_EQ(attacks[0].get("damage"), kHunterStrength);
	EXPECT_EQ(attacks[1].get("targetUnitId"), 2u);
	EXPECT_EQ(attacks[1].get("targetHp"), 0u);
	EXPECT_EQ(attacks[2].get("targetUnitId"), 3u);
	EXPECT_EQ(attacks[2].get("damage"), kHunterAgility);
}

// README, Task: a unit with HP <= 0 disappears on the next turn and does not act anymore
TEST(Turn, DeadUnitStopsActingAndFreesItsCellOnTheNextTurn)
{
	// The hunter (range 2) kills unit 2 before its turn. Unit 3 marches through unit 2's cell
	const ScenarioResult result = runScenario(
		kMap + "SPAWN_HUNTER 1 0 0 10 5 1 2\n" + swordsman(2, 2, 0, kHunterAgility, 1) + swordsman(3, 3, 0, 100)
		+ "MARCH 3 0 0\n");
	SCOPED_TRACE(result.log);

	const auto death = find(result.events, "UNIT_DIED", "unitId", 2);
	ASSERT_EQ(death.size(), 1u);
	EXPECT_EQ(death[0].tick, 2u);
	EXPECT_TRUE(attacksBy(result.events, 2).empty());

	const auto moves = movesOf(result.events, 3);
	ASSERT_FALSE(moves.empty());
	EXPECT_EQ(moves[0].tick, 3u);
	EXPECT_EQ(moves[0].get("x"), 2u);
}

// README, Task: a unit moves to any of the eight neighbouring cells per turn
// and keeps moving on the next turns until it reaches the target
TEST(March, StepsDiagonallyEachTurnAndEndsOnArrival)
{
	const ScenarioResult result = runScenario(kMap + swordsman(1, 0, 0, 10) + "MARCH 1 2 2\n" + swordsman(2, 9, 9, 10));
	SCOPED_TRACE(result.log);

	const auto moves = movesOf(result.events, 1);
	ASSERT_EQ(moves.size(), 2u);
	for (uint32_t step = 0; step < 2; ++step)
	{
		EXPECT_EQ(moves[step].tick, 2u + step);
		EXPECT_EQ(moves[step].get("x"), step + 1);
		EXPECT_EQ(moves[step].get("y"), step + 1);
	}

	const auto end = find(result.events, "MARCH_ENDED", "unitId", 1);
	ASSERT_EQ(end.size(), 1u);
	EXPECT_EQ(end[0].tick, 3u);
	EXPECT_EQ(end[0].get("x"), 2u);
	EXPECT_EQ(end[0].get("y"), 2u);
}

// A marching unit waits instead of stepping into a taken cell, the task does not ask for a detour
TEST(March, NeverStepsIntoTakenCell)
{
	const ScenarioResult result = runScenario(kMap + swordsman(1, 0, 0, 10) + "MARCH 1 2 0\n" + swordsman(2, 1, 0, 10));
	SCOPED_TRACE(result.log);

	EXPECT_TRUE(movesOf(result.events, 1).empty());
	EXPECT_TRUE(find(result.events, "MARCH_ENDED", "unitId", 1).empty());
}

// README, Swordsman: hits one random adjacent unit and deals Strength damage, moves if there is nobody to hit
TEST(Swordsman, HitsNeighbourAndThenGoesOnMarching)
{
	// Unit 2 blocks the way and dies from one hit, unit 3 far away keeps the simulation going
	const ScenarioResult result = runScenario(
		kMap + swordsman(1, 0, 0, 10, 5) + "MARCH 1 4 0\n" + swordsman(2, 1, 0, 5) + swordsman(3, 9, 9, 10));
	SCOPED_TRACE(result.log);

	const auto attacks = attacksBy(result.events, 1);
	ASSERT_EQ(attacks.size(), 1u);
	EXPECT_EQ(attacks[0].tick, 2u);
	EXPECT_EQ(attacks[0].get("targetUnitId"), 2u);
	EXPECT_EQ(attacks[0].get("damage"), 5u);

	const auto moves = movesOf(result.events, 1);
	ASSERT_EQ(moves.size(), 4u);
	EXPECT_EQ(moves[0].tick, 3u);
	EXPECT_EQ(moves[0].get("x"), 1u);

	const auto end = find(result.events, "MARCH_ENDED", "unitId", 1);
	ASSERT_EQ(end.size(), 1u);
	EXPECT_EQ(end[0].get("x"), 4u);
}

// README, Swordsman: hits one random unit in an adjacent cell
TEST(Swordsman, PicksTargetAtRandomAmongNeighbours)
{
	// Unit 1 stands between units 2 and 3, which do not reach each other
	const std::string commands = kMap + swordsman(1, 1, 0, 10, 1) + swordsman(2, 0, 0, 3) + swordsman(3, 2, 0, 3);

	std::set<uint32_t> firstTargets;
	for (uint32_t seed = 0; seed < 20; ++seed)
	{
		const ScenarioResult result = runScenario(commands, seed);
		SCOPED_TRACE("seed " + std::to_string(seed) + "\n" + result.log);

		const auto attacks = attacksBy(result.events, 1);
		ASSERT_FALSE(attacks.empty());
		for (const auto& attack : attacks)
		{
			EXPECT_TRUE(attack.get("targetUnitId") == 2 || attack.get("targetUnitId") == 3);
		}
		firstTargets.insert(attacks[0].get("targetUnitId"));
	}
	EXPECT_EQ(firstTargets.size(), 2u);
}

TEST(Simulation, SameSeedReplaysSameBattle)
{
	const std::string commands = kMap + swordsman(1, 1, 0, 10, 1) + swordsman(2, 0, 0, 3) + swordsman(3, 2, 0, 3);
	EXPECT_EQ(runScenario(commands, 7).log, runScenario(commands, 7).log);
}

// README, Task: the simulation ends when only one unit is left on the map
TEST(Simulation, StopsWhenOneUnitIsLeft)
{
	const ScenarioResult result = runScenario(kMap + kHunter + "MARCH 1 9 9\n" + swordsman(2, 2, 0, kHunterAgility));
	SCOPED_TRACE(result.log);

	ASSERT_FALSE(result.events.empty());
	EXPECT_EQ(result.events.back().name, "UNIT_DIED");
	EXPECT_EQ(result.events.back().tick, 2u);
	EXPECT_TRUE(movesOf(result.events, 1).empty());
}

// README, Task: the simulation ends when no unit can act on the next turn
TEST(Simulation, StopsWhenNobodyCanAct)
{
	const ScenarioResult result = runScenario(kMap + swordsman(1, 0, 0, 10, 1) + swordsman(2, 5, 5, 10, 1));
	SCOPED_TRACE(result.log);

	for (const auto& event : result.events)
	{
		EXPECT_EQ(event.tick, 1u) << event.name;
	}
}

// README, Task: the simulation ends when no unit can act on the next turn.
// A hit without damage is not an action, otherwise harmless neighbours would fight forever
TEST(Simulation, StopsWhenUnitsCannotHurtEachOther)
{
	const ScenarioResult result = runScenario(kMap + swordsman(1, 0, 0, 10) + swordsman(2, 1, 0, 10));
	SCOPED_TRACE(result.log);

	EXPECT_TRUE(attacksBy(result.events, 1).empty());
	EXPECT_TRUE(attacksBy(result.events, 2).empty());
}

TEST(Scenario, SkipsBrokenCommandLinesWithWarningAndRunsTheRest)
{
	// Each broken line spawns unit 2 at (1, 1) and must be skipped with its own warning
	const std::map<std::string, std::string> warningForLine = {
		{"SPAWN_ARCHER 2 1 1 10 5", "SPAWN_ARCHER skipped: unknown command"},
		{"SPAWN_SWORDSMAN 2 1 1 10", "SPAWN_SWORDSMAN skipped: missing or invalid strength"},
		{"SPAWN_SWORDSMAN 2 1 1 abc 1", "SPAWN_SWORDSMAN skipped: missing or invalid hp"},
		{"SPAWN_SWORDSMAN 2 1 1 -10 1", "SPAWN_SWORDSMAN skipped: missing or invalid hp"},
		{"SPAWN_SWORDSMAN 2 1 1 10 1 7", "SPAWN_SWORDSMAN skipped: unexpected argument 7"},
	};
	for (const auto& [line, warning] : warningForLine)
	{
		const ScenarioResult result = runScenario(kMap + line + "\n" + kHunter + swordsman(3, 2, 0, kHunterAgility));
		SCOPED_TRACE(line + "\n" + result.log);

		EXPECT_NE(result.warnings.find(warning), std::string::npos) << result.warnings;
		EXPECT_TRUE(find(result.events, "UNIT_SPAWNED", "unitId", 2).empty());
		EXPECT_EQ(find(result.events, "UNIT_DIED", "unitId", 3).size(), 1u);
	}
}

TEST(Scenario, SkipsMapWithoutCellsOrTooBig)
{
	const std::map<std::string, std::string> warningForMap = {
		{"CREATE_MAP 0 5", "CREATE_MAP skipped: map must have at least one cell"},
		{"CREATE_MAP 4294967295 4294967295", "CREATE_MAP skipped: map is too big"},
	};
	for (const auto& [map, warning] : warningForMap)
	{
		const ScenarioResult result = runScenario(map + "\n" + kMap + kHunter + swordsman(2, 2, 0, kHunterAgility));
		SCOPED_TRACE(map + "\n" + result.log);

		EXPECT_NE(result.warnings.find(warning), std::string::npos) << result.warnings;
		ASSERT_FALSE(result.events.empty());
		EXPECT_EQ(result.events[0].name, "MAP_CREATED");
		EXPECT_EQ(result.events[0].get("width"), 10u);
	}
}

TEST(Scenario, SkipsInvalidCommandsWithWarningAndRunsTheRest)
{
	const ScenarioResult result = runScenario(
		kMap + kHunter + swordsman(2, 0, 0, 10)	 // the cell is taken by the hunter
		+ "MARCH 2 5 5\n"						 // unit 2 was not spawned
		+ swordsman(3, 2, 0, kHunterAgility));
	SCOPED_TRACE(result.log);

	EXPECT_NE(result.warnings.find("SPAWN_SWORDSMAN skipped"), std::string::npos) << result.warnings;
	EXPECT_NE(result.warnings.find("MARCH skipped"), std::string::npos) << result.warnings;
	EXPECT_TRUE(find(result.events, "UNIT_SPAWNED", "unitId", 2).empty());
	EXPECT_EQ(find(result.events, "UNIT_DIED", "unitId", 3).size(), 1u);
}

// commands_example.txt and the reference log from the task's main.cpp
TEST(ExampleScenario, MatchesReferenceLogWhileItFollowsTheRules)
{
	const std::string commands
		= "CREATE_MAP 10 10\n"
		  "SPAWN_SWORDSMAN 1 0 0 5 2\n"
		  "SPAWN_HUNTER 2 9 0 10 5 1 4\n"
		  "MARCH 1 9 0\n"
		  "MARCH 2 0 0\n"
		  "SPAWN_SWORDSMAN 3 0 9 10 2\n"
		  "MARCH 3 0 0\n";
	// At tick 7 the reference shoots from distance 5 with Range 4, so the comparison stops at tick 6
	const std::vector<std::string> reference = {
		"[1] MAP_CREATED width=10 height=10 ",
		"[1] UNIT_SPAWNED unitId=1 unitType=Swordsman x=0 y=0 ",
		"[1] UNIT_SPAWNED unitId=2 unitType=Hunter x=9 y=0 ",
		"[1] MARCH_STARTED unitId=1 x=0 y=0 targetX=9 targetY=0 ",
		"[1] MARCH_STARTED unitId=2 x=9 y=0 targetX=0 targetY=0 ",
		"[1] UNIT_SPAWNED unitId=3 unitType=Swordsman x=0 y=9 ",
		"[1] MARCH_STARTED unitId=3 x=0 y=9 targetX=0 targetY=0 ",
		"[2] UNIT_MOVED unitId=1 x=1 y=0 ",
		"[2] UNIT_MOVED unitId=2 x=8 y=0 ",
		"[2] UNIT_MOVED unitId=3 x=0 y=8 ",
		"[3] UNIT_MOVED unitId=1 x=2 y=0 ",
		"[3] UNIT_MOVED unitId=2 x=7 y=0 ",
		"[3] UNIT_MOVED unitId=3 x=0 y=7 ",
		"[4] UNIT_MOVED unitId=1 x=3 y=0 ",
		"[4] UNIT_ATTACKED attackerUnitId=2 targetUnitId=1 damage=5 targetHp=0 ",
		"[4] UNIT_DIED unitId=1 ",
		"[4] UNIT_MOVED unitId=3 x=0 y=6 ",
		"[5] UNIT_MOVED unitId=2 x=6 y=0 ",
		"[5] UNIT_MOVED unitId=3 x=0 y=5 ",
		"[6] UNIT_MOVED unitId=2 x=5 y=0 ",
		"[6] UNIT_MOVED unitId=3 x=0 y=4 ",
	};

	const ScenarioResult result = runScenario(commands);
	SCOPED_TRACE(result.log);

	std::istringstream lines(result.log);
	std::string line;
	for (const auto& expected : reference)
	{
		ASSERT_TRUE(std::getline(lines, line));
		EXPECT_EQ(line, expected);
	}

	ASSERT_FALSE(result.events.empty());
	EXPECT_EQ(result.events.back().name, "UNIT_DIED");
	EXPECT_EQ(result.events.back().get("unitId"), 3u);
	EXPECT_TRUE(find(result.events, "UNIT_DIED", "unitId", 2).empty());
}
