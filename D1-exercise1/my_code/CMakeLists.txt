cmake_minimum_required(VERSION 2.8)
project(diffusion CXX)
add_executable(main.x main.cc)

find_package(MPI)

target_link_libraries(main.x PRIVATE ${MPI_CXX_LIBRARIES})
target_include_directories(main.x PRIVATE ${MPI_CXX_INCLUDE_PATH})
include_directories(include)
