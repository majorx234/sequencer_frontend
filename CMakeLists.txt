cmake_minimum_required(VERSION 3.16)
project(sequencer_frontend)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(raylib)

set(SEQUENCER_FRONTEND_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)
add_executable(sequencer_frontend)
target_sources(sequencer_frontend
 PRIVATE
  ${SEQUENCER_FRONTEND_SOURCES}
)
target_include_directories(sequencer_frontend
 PRIVATE
  include
)
target_link_libraries(sequencer_frontend
 PRIVATE
   ${raylib_LIBRARIES}
)
