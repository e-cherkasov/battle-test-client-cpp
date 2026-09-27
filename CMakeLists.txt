cmake_minimum_required(VERSION 3.27)
project(sw_battle_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SW_WARNINGS_AS_ERRORS "Treat warnings in the project code as errors" OFF)

function(sw_set_warnings target)
	if(MSVC)
		target_compile_options(${target} PRIVATE /W4)
	else()
		target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
	endif()
	if(SW_WARNINGS_AS_ERRORS)
		set_target_properties(${target} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
	endif()
endfunction()

# Headers are listed too so IDEs (Xcode, Visual Studio) show them in the project tree.
# CONFIGURE_DEPENDS re-runs the glob on every build, so new files are picked up without re-running cmake.
file(GLOB_RECURSE SW_CORE_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
list(REMOVE_ITEM SW_CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${SW_CORE_SOURCES} src/main.cpp)

add_library(sw_core STATIC ${SW_CORE_SOURCES})
target_include_directories(sw_core PUBLIC src)
sw_set_warnings(sw_core)

add_executable(sw_battle_test src/main.cpp)
target_link_libraries(sw_battle_test PRIVATE sw_core)
sw_set_warnings(sw_battle_test)

option(SW_BUILD_TESTS "Build the tests" OFF)
if(SW_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

set_target_properties(sw_battle_test PROPERTIES
	XCODE_GENERATE_SCHEME ON
	XCODE_SCHEME_ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/commands_example.txt"
	XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
