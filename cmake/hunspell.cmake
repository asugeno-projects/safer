include(include_guard)
include_guard()

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
  MESSAGE(FATAL_ERROR "Failed to find the Hunspell library.")
ELSE()
  FIND_PATH(
    HunspellLibraryPath
    NAMES "hunspell.h"
    PATH_SUFFIXES
      hunspell
    PATHS
      "${LibrarySearchDir}"
  )
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC ${HunspellLibrary})
  TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${HunspellLibraryPath})
ENDIF()
