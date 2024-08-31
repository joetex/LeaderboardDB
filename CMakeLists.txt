# CMakeList.txt : CMake project for MovStat, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.28)
project(KineticStatDB)

# Add source to this project's executable.
add_executable (MovStat src/MovStat.cc src/db/datastructures/BTree.h src/db/datastructures/ska_flat_hash_map.h src/db/datastructures/SkipList.h src/db/datastructures/SkipList.cc src/cmdline/CommandLoop.h src/cmdline/CommandLoop.cc src/db/actions/ZADD.h src/db/actions/IAction.h src/db/actions/ActionSystem.h src/db/MemoryDB.h src/db/tables/Member.h src/db/tables/Stat.h src/db/tables/Leaderboard.h "src/db/Engine.h" "src/db/datastructures/HashTree.h" "src/db/datastructures/Base62.h" "src/db/Pager.h" "src/db/datastructures/json.h" "src/db/FileDiskManager.h" "src/db/datastructures/MinMaxSort.h")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_VERSION VERSION_GREATER 3.28)
  set_property(TARGET MovStat PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
    target_compile_options(MovStat PUBLIC "/Zc:__cplusplus")
endif()

include_directories(
    src 
    src/cmdline 
    src/db
    src/db/datastructures
    src/db/tables
    ${Boost_INCLUDE_DIR}
    libs/BTree/include
)


set(BOOST_REQUIRED_COMPONENTS
    filesystem
    iostreams
)
set(BOOST_MIN_VERSION 1.74.0) # or whatever you need!
# Here you can either append or set a root to look into
# In general, **don't do that**! Your boost build and installation
# should come with the correct CMake configure scripts to automate this!
# If you set it here, the people trying to build your code on a different
# machine **WILL** hate you.
# set(BOOST_ROOT "/Volumes/Code/boost_1_79_0")
find_package(
    Boost ${BOOST_MIN_VERSION} REQUIRED
    COMPONENTS filesystem iostreams
)
target_link_libraries(MovStat
    PRIVATE
    Boost::filesystem
    Boost::iostreams
    Boost::boost
)
#target_include_directories(KineticStatDB SYSTEM PRIVATE ${Boost_INCLUDE_DIR})

#find_package(BTree)
#target_link_libraries(MovStat PRIVATE BTree)
# TODO: Add tests and install targets if needed.
