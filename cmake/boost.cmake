INCLUDE(include_guard)
include_guard()

# find boost
FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options system filesystem regex date_time)
IF(Boost_FOUND)
  # Include and link.
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})
ENDIF()
