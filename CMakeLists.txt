cmake_minimum_required(VERSION 2.8)

# ソリューションファイルの名前を指定する
project( safer )

# define for project
set(SwxPath "library/SimpleXmlWriter/")
set(SaferPath "ER-StaticAnalysis/source")

# define for cmake
add_definitions(
    -DCMAKE_CXX_COMPILER=clang++
    -DHUNSPELL_STATIC
)

# Confirm that the compiler version is c++11.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# set CXX FLAG
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wall -Wextra -Wmismatched-new-delete")

# Find Source
file(GLOB_RECURSE Sources "${SaferPath}/*.cpp" "${SaferPath}/*/*.cpp")
file(GLOB_RECURSE SWXList "${SwxPath}/*.cpp")

# add executable
add_executable(safer ER-StaticAnalysis/source/main.cpp ${Sources} ${SWXList}) 

# find boost
FIND_PACKAGE( Boost REQUIRED COMPONENTS program_options system filesystem regex date_time )
if (Boost_FOUND)
  INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
  TARGET_LINK_LIBRARIES( safer LINK_PUBLIC ${Boost_LIBRARIES})
endif()

# find Xerces-c
find_library(XercesLibrary NAMES xerces-c
  PATHS
    /usr/local/
)
if(NOT XercesLibrary)
  message(FATAL_ERROR "Failed to find the Xerces library.")
else()
  TARGET_LINK_LIBRARIES(safer LINK_PUBLIC ${XercesLibrary})
endif()

# find Hunspell
find_library(HunspellLibrary NAMES hunspell-1.6 PATH_SUFFIXES hunspell PATHS /usr/local/)
if(NOT HunspellLibrary)
  message(FATAL_ERROR "Failed to find the Hunspell library.")
else()
  find_path(HunspellLibraryPath NAMES "hunspell.h"
    PATH_SUFFIXES
      hunspell
    PATHS
      /usr/local
  )
  TARGET_LINK_LIBRARIES(safer LINK_PUBLIC ${HunspellLibrary})
  target_include_directories(safer PUBLIC ${HunspellLibraryPath})
endif()

# outher
target_include_directories(safer PUBLIC "${SwxPath}")
