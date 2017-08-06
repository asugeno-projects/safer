CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# ソリューションファイルの名前を指定する
PROJECT(safer)

# define for project
SET(SwxPath "library/SimpleXmlWriter/")
SET(SaferPath "ER-StaticAnalysis/source")

# define for cmake
ADD_DEFINITIONS(
    -DCMAKE_CXX_COMPILER=clang++
    -DHUNSPELL_STATIC
)
# set CXX FLAG
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wmismatched-new-delete")

# Confirm that the compiler version is c++11.
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
IF(COMPILER_SUPPORTS_CXX14)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
ELSEIF(COMPILER_SUPPORTS_CXX11)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSEIF(COMPILER_SUPPORTS_CXX0X)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ELSE()
        MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF()

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
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  SET(LibrarySearchDir "/usr/local")
ELSEIF(UNIX AND NOT APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=_UNIX")
  SET(LibrarySearchDir "/usr")
  SET(HunspellVersion "hunspell")
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
IF(APPLE AND NOT HunspellVersion)
  EXECUTE_PROCESS(
    COMMAND brew list hunspell
    COMMAND sort -r
    COMMAND head -n 1
    COMMAND grep -o -e "[0-9]\\+.[0-9]\\+"
    COMMAND awk "{print \"hunspell-\"$0}"
    COMMAND tr -d "\n"
    OUTPUT_VARIABLE HunspellVersion
  )
  MESSAGE(STATUS "Found hunspell Library : ${HunspellVersion}")
ENDIF()

FIND_LIBRARY(
  HunspellLibrary REQUIRED
  NAMES "${HunspellVersion}"
  PATHS
    "${LibrarySearchDir}"
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
      "${LibrarySearchDir}"
  )
  TARGET_LINK_LIBRARIES(safer LINK_PUBLIC ${HunspellLibrary})
  TARGET_INCLUDE_DIRECTORIES(safer PUBLIC ${HunspellLibraryPath})
ENDIF()

# outher
TARGET_INCLUDE_DIRECTORIES(safer PUBLIC "${SwxPath}")

# test
IF(NOT without-test)
  ENABLE_TESTING()
  find_package(GTest REQUIRED)
  IF(NOT GTest_FOUND)
    message(FATAL_ERROR "Failed to find the Google Test.")
  ENDIF()
  include_directories(${GTEST_INCLUDE_DIRS})
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=_TEST -fprofile-arcs -ftest-coverage")
  SET(CMAKE_EXE_LINKER_FLAGS="-fprofile-arcs -ftest-coverage")
  TARGET_LINK_LIBRARIES(safer LINK_PUBLIC ${GTEST_BOTH_LIBRARIES} pthread)
  IF(UNIX AND NOT APPLE)
    target_link_libraries(safer gcov)
  ENDIF()
  ADD_TEST(NAME safer COMMAND safer -e ../test.a5er -c ../test-command.xml)
ENDIF()

# build setting
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()
SET(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -Wall -D_DEBUG")
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
