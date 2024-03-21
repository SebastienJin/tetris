cmake_minimum_required(VERSION 3.16)
project(tetris)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

# add GTKMM
link_directories(${GTKMM_LIBRARIES})
include_directories(${GTKMM_INCLUDE_DIRS})
add_definitions(${GTKMM_CFLAGS_OTHER})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )

add_executable(
tetris 

src/main.cpp
src/matrix.h 
src/piece.h
src/game_logic.h

src/scene/GameWindow.cpp
src/scene/GameWindow.h 
src/scene/MatrixToGraph.h
src/scene/Scoreboard.cpp
src/scene/Scoreboard.h 

src/modemulti/server.cpp 
src/modemulti/server.h
src/modemulti/communication.cpp 
src/modemulti/communication.h
src/modemulti/player.cpp 
src/modemulti/player.h
src/modemulti/message_player.cpp 
src/modemulti/message_player.h
src/modemulti/message_disconnect.cpp 
src/modemulti/message_disconnect.h
src/modemulti/message.cpp 
src/modemulti/message.h
src/modemulti/message_start.cpp 
src/modemulti/message_start.h
)

# link GTKMM libs
target_link_libraries(tetris ${CMAKE_THREAD_LIBS_INIT} ${GTKMM_LIBRARIES} )


