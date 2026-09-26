#pragma once

#include "Core/Node.hpp"
#include "IO/Commands/SpawnHunter.hpp"

#include <memory>

namespace sw::features
{
	std::unique_ptr<core::Node> makeHunter(const io::SpawnHunter& command);
}
