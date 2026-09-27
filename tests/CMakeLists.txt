include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(sw_tests SimulationTest.cpp)
target_link_libraries(sw_tests PRIVATE sw_core GTest::gtest_main)
sw_set_warnings(sw_tests)

include(GoogleTest)
gtest_discover_tests(sw_tests)
