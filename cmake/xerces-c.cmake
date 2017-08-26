include(include_guard)
include_guard()

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
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC ${XercesLibrary})
ENDIF()
