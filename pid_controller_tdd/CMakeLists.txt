cmake_minimum_required(VERSION 3.2)
project(motor_controller_emulator)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  pid_controller_test
  pid_controller_test.cc
  pid_controller.c
)

target_link_libraries(
  pid_controller_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(pid_controller_test)
