include(include_guard)
include_guard()

# define for cmake
ADD_DEFINITIONS(
    -DCMAKE_CXX_COMPILER=clang++
    -DHUNSPELL_STATIC
)

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

# build setting
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()
SET(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -Wall -D_DEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=native")
