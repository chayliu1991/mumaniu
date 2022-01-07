include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCS)


add_library(mumaniu_base ${SRCS})
target_link_libraries(mumaniu_base pthread rt)


