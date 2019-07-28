cmake_minimum_required (VERSION 3.0.2)

project(magisterka)

set(CMAKE_CXX_FLAGS "-g -O0")

add_executable(magisterka main.cc)
add_library(tree_lib tree.cc)

add_library(cnc_steiner_tree_lib cnc_steiner_tree.cc)
add_library(standard_steiner_tree_lib standard_steiner_tree.cc)
add_library(standard_hamiltonian_lib standard_hamiltonian.cc)

target_link_libraries(magisterka tree_lib)
target_link_libraries(magisterka cnc_steiner_tree_lib)
target_link_libraries(magisterka standard_steiner_tree_lib)
target_link_libraries(magisterka standard_hamiltonian_lib)


# ==== TESTING =================================================================
add_subdirectory(googletest
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

add_executable(example_test tests.cc)
target_link_libraries(example_test tree_lib)
target_link_libraries(example_test cnc_steiner_tree_lib)
target_link_libraries(example_test standard_steiner_tree_lib)
target_link_libraries(example_test standard_hamiltonian_lib)
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

# =============================================================================

set(
    TEX_FILES
    "thesis.tex"
)

string(REPLACE "tex" "pdf" PDF_FILES "${TEX_FILES}")

set(
    THESIS_OUT
    "${CMAKE_CURRENT_BINARY_DIR}/${PDF_FILES}"
)

set(
    THESIS_TEXT
    "${PROJECT_SOURCE_DIR}/${TEX_FILES}"
)



add_custom_command(OUTPUT ${THESIS_OUT}
    COMMAND pdflatex ${THESIS_TEXT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${THESIS_TEXT}
    COMMENT "Generating thesis PDF"
    VERBATIM
)


add_custom_target(generate_pdf DEPENDS ${THESIS_OUT})
