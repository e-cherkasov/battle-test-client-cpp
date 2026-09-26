#pragma once

#include "Core/Node.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"

#include <memory>

namespace sw::features
{
	std::unique_ptr<core::Node> makeSwordsman(const io::SpawnSwordsman& command);
}
