cmake_minimum_required(VERSION 3.12)
project(nesemu2)

set(CMAKE_CXX_STANDARD 17)

add_executable(nesemu2 main.cpp nes_logger.cpp nes_logger.h nes_system.cpp nes_system.h nes_cpu.cpp nes_cpu.h nes_ppu.h nes_ppu.cpp nes_memory.h nes_memory.cpp nes_mapper.h nes_mapper_nrom.cpp opcodes.h common.h nes_cycle.h nes_input.h)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
