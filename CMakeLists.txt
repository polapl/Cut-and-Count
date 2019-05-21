cmake_minimum_required (VERSION 3.0.2)

project(magisterka)

add_executable(magisterka main.cc)
add_library(tree_lib tree.cc)
add_library(dynamic_lib dynamic.cc)
add_library(standard_dynamic_lib standard_dynamic.cc)

target_link_libraries(magisterka tree_lib)
target_link_libraries(magisterka dynamic_lib)
target_link_libraries(magisterka standard_dynamic_lib)


# ==== TESTING =================================================================
add_subdirectory(googletest
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

add_executable(example_test tests.cc)
target_link_libraries(example_test tree_lib)
target_link_libraries(example_test dynamic_lib)
target_link_libraries(example_test standard_dynamic_lib)
target_link_libraries(example_test gtest)

# =============================================================================

set(
  DOT_FILES
  "${CMAKE_CURRENT_BINARY_DIR}/example.dot"
)

add_custom_command(OUTPUT ${DOT_FILES}
  COMMAND ./magisterka
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/magisterka"
  COMMENT "Running magisterka"
  VERBATIM
)
 
add_custom_target(generate_dot DEPENDS ${DOT_FILES})

set(
  IMAGES
  "${CMAKE_CURRENT_BINARY_DIR}/example.svg"
)

add_custom_command(OUTPUT ${IMAGES}
  COMMAND dot -Tsvg ${DOT_FILES} -o ${IMAGES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${DOT_FILES}
  COMMENT "Generating Graphviz images"
  VERBATIM
)

add_custom_target(generate_image DEPENDS ${IMAGES})
