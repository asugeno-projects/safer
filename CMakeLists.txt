CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# ソリューションファイルの名前を指定する
PROJECT(safer)

# define for project
SET(SwxPath "library/SimpleXmlWriter/")
SET(SaferPath "ER-StaticAnalysis/source")

# hunspell version
SET(HunspellVersion "hunspell-1.6")

# define for cmake
ADD_DEFINITIONS(
    -DCMAKE_CXX_COMPILER=clang++
    -DHUNSPELL_STATIC
)

# Confirm that the compiler version is c++11.
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
IF(COMPILER_SUPPORTS_CXX11)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSEIF(COMPILER_SUPPORTS_CXX0X)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ELSE()
        MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF()

# set CXX FLAG
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wextra -Wmismatched-new-delete")

# Find Source
FILE(GLOB_RECURSE Sources "${SaferPath}/*.cpp" "${SaferPath}/*/*.cpp")
FILE(GLOB_RECURSE SWXList "${SwxPath}/*.cpp")

# add executable
ADD_EXECUTABLE(safer ER-StaticAnalysis/source/main.cpp ${Sources} ${SWXList}) 

# Per OS setting
IF(WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=_WIN32")
ELSEIF(APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=_MACOSX")
  SET(LibrarySearchDir "/usr/local")
ENDIF()

# find boost
FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options system filesystem regex date_time)
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(safer LINK_PUBLIC ${Boost_LIBRARIES})
ENDIF()

# find Xerces-c
FIND_LIBRARY(
  XercesLibrary
  NAMES xerces-c
  PATHS
    ${LibrarySearchDir}
)

# LINK for Xerces-c
IF(NOT XercesLibrary)
  MESSAGE(FATAL_ERROR "Failed to find the Xerces library.")
ELSE()
  TARGET_LINK_LIBRARIES(safer LINK_PUBLIC ${XercesLibrary})
ENDIF()

# find Hunspell
FIND_LIBRARY(
  HunspellLibrary REQUIRED
  NAMES ${HunspellVersion}
  PATH_SUFFIXES
    hunspell
  PATHS
    ${LibrarySearchDir}
)

# Link and Include for Hunspell
IF(NOT HunspellLibrary)
  message(FATAL_ERROR "Failed to find the Hunspell library.")
ELSE()
  FIND_PATH(
    HunspellLibraryPath
    NAMES "hunspell.h"
    PATH_SUFFIXES
      hunspell
    PATHS
      ${LibrarySearchDir}
  )
  TARGET_LINK_LIBRARIES(safer LINK_PUBLIC ${HunspellLibrary})
  TARGET_INCLUDE_DIRECTORIES(safer PUBLIC ${HunspellLibraryPath})
ENDIF()

# outher
TARGET_INCLUDE_DIRECTORIES(safer PUBLIC "${SwxPath}")

# build setting
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()
SET(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=native")

# output dir
SET(outdir ${CMAKE_CURRENT_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}/bin/)
SET(EXECUTABLE_OUTPUT_PATH ${outdir} CACHE PATH "Build directory" FORCE)
SET(LIBRARY_OUTPUT_PATH ${outdir} CACHE PATH "Build directory" FORCE)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${outdir})
SET(CMAKE_BUILD_FILES_DIRECTORY ${outdir})
SET(CMAKE_BUILD_DIRECTORY ${outdir})
SET(CMAKE_BINARY_DIR  ${outdir})
SET(EXECUTABLE_OUTPUT_PATH ${outdir})
SET(LIBRARY_OUTPUT_PATH ${outdir}lib)
SET(CMAKE_CACHEFILE_DIR ${dir})