CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# ソリューションファイルの名前を指定する
PROJECT(safer)

# define for project
SET(SwxPath "library/SimpleXmlWriter/")
SET(SaferPath "ER-StaticAnalysis/source")

# Add module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# Set define
INCLUDE(define)
INCLUDE(define)

# Set compiler
INCLUDE(compiler)

# Find Source
FILE(GLOB_RECURSE Sources "${SaferPath}/*.cpp" "${SaferPath}/*/*.cpp")
FILE(GLOB_RECURSE SWXList "${SwxPath}/*.cpp")

# add executable
ADD_EXECUTABLE(${PROJECT_NAME} ER-StaticAnalysis/source/main.cpp ${Sources} ${SWXList}) 

# Include boost
INCLUDE(boost)

# Include Xerces-c
INCLUDE(xerces-c)

# Include Hunspell
INCLUDE(hunspell)

# outher
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC "${SwxPath}")

# output dir
SET(Outdir ${CMAKE_CURRENT_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}/bin/)
SET(EXECUTABLE_OUTPUT_PATH ${Outdir} CACHE PATH "Build directory" FORCE)
SET(LIBRARY_OUTPUT_PATH ${Outdir} CACHE PATH "Build directory" FORCE)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Outdir})
SET(CMAKE_BUILD_FILES_DIRECTORY ${Outdir})
SET(CMAKE_BUILD_DIRECTORY ${Outdir})
SET(CMAKE_BINARY_DIR  ${Outdir})
SET(EXECUTABLE_OUTPUT_PATH ${Outdir})
SET(LIBRARY_OUTPUT_PATH ${Outdir}lib)
SET(CMAKE_CACHEFILE_DIR ${dir})

# test
IF(NOT without-test)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY(ER-StaticAnalysis/test)
ENDIF()
