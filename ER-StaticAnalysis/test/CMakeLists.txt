CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# ソリューションファイルの名前を指定する
PROJECT(safer_test)

# define for project
SET(SwxPath "../library/SimpleXmlWriter/")
SET(SaferPath "../ER-StaticAnalysis/source")

# Add module path
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
# Set define
INCLUDE(define)

# Set compiler
INCLUDE(compiler)

# Find Source
FILE(GLOB_RECURSE Sources "${SaferPath}/*.cpp" "${SaferPath}/*/*.cpp")
FILE(GLOB_RECURSE SWXList "${SwxPath}/*.cpp")

# test
ENABLE_TESTING()
ADD_EXECUTABLE(${PROJECT_NAME} testMain.cpp ${Sources} ${SWXList})

FIND_PACKAGE(GTest REQUIRED)
IF(NOT GTest_FOUND)
  MESSAGE(FATAL_ERROR "Failed to find the Google Test.")
ENDIF()

include_directories(${GTEST_INCLUDE_DIRS})
SET(CMAKE_EXE_LINKER_FLAGS="-fprofile-arcs -ftest-coverage")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC ${GTEST_BOTH_LIBRARIES} pthread)
IF(UNIX AND NOT APPLE)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC gcov)
ENDIF()

SET(CMAKE_CXX_FLAGS "-g -O0 --coverage")
ADD_TEST(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} -e ../test.a5er -c ../test-command.xml)

# Include boost
INCLUDE(boost)

# Include Xerces-c
INCLUDE(xerces-c)

# Include Hunspell
INCLUDE(hunspell)

# outher
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC "${SwxPath}")

# output dir
SET(Outdir ${CMAKE_CURRENT_SOURCE_DIR}../../../out/${CMAKE_BUILD_TYPE}/bin/)
SET(EXECUTABLE_OUTPUT_PATH ${Outdir} CACHE PATH "Build directory" FORCE)
SET(LIBRARY_OUTPUT_PATH ${Outdir} CACHE PATH "Build directory" FORCE)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Outdir})
SET(CMAKE_BUILD_FILES_DIRECTORY ${Outdir})
SET(CMAKE_BUILD_DIRECTORY ${Outdir})
SET(CMAKE_BINARY_DIR  ${Outdir})
SET(EXECUTABLE_OUTPUT_PATH ${Outdir})
SET(LIBRARY_OUTPUT_PATH ${Outdir}lib)
SET(CMAKE_CACHEFILE_DIR ${dir})
