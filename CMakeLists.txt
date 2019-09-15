cmake_minimum_required (VERSION 3.0.2)

project(magisterka)

set(CMAKE_CXX_FLAGS "-O3")

OPTION(GENERATE_DOT "When ON tests will generate dot files for each testcase and save it to `{Hamiltonian,Steiner}TestExample_*.dot`" OFF) # Disabled by default

IF(GENERATE_DOT)
    ADD_DEFINITIONS(-DGENERATE_DOT)
ENDIF(GENERATE_DOT)

add_executable(magisterka main.cc)
add_library(tree_lib tree.cc)

add_library(state_utils_lib SHARED state_utils.cc)
add_library(cnc_steiner_tree_lib SHARED cnc_steiner_tree.cc)
add_library(standard_steiner_tree_lib SHARED standard_steiner_tree.cc)
add_library(standard_hamiltonian_lib STATIC standard_hamiltonian.cc)
add_library(cnc_hamiltonian_lib STATIC cnc_hamiltonian.cc)


target_link_libraries(cnc_steiner_tree_lib state_utils_lib)
target_link_libraries(standard_hamiltonian_lib state_utils_lib)

target_link_libraries(magisterka tree_lib)
target_link_libraries(magisterka state_utils_lib)
target_link_libraries(magisterka cnc_steiner_tree_lib)  
target_link_libraries(magisterka standard_steiner_tree_lib)
target_link_libraries(magisterka standard_hamiltonian_lib)
target_link_libraries(magisterka cnc_hamiltonian_lib)



# ==== TESTING =================================================================
add_subdirectory(googletest
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

add_executable(example_test tests.cc)
target_link_libraries(example_test tree_lib)
target_link_libraries(example_test state_utils_lib)
target_link_libraries(example_test cnc_steiner_tree_lib)
target_link_libraries(example_test standard_steiner_tree_lib)
target_link_libraries(example_test standard_hamiltonian_lib)
target_link_libraries(example_test cnc_hamiltonian_lib)
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


set (
  STEINER_TEST_OUT
  "${CMAKE_CURRENT_BINARY_DIR}/steiner_test_out.txt"
)

set (
  HAMILTONIAN_TEST_OUT
  "${CMAKE_CURRENT_BINARY_DIR}/hamiltonian_test_out.txt"
)

set (
  STEINER_TEST_GRAPH
  "steiner_test_time.png"
)

set (
  HAMILTONIAN_TEST_GRAPH
  "hamiltonian_test_time.png"
)

add_custom_command(OUTPUT ${STEINER_TEST_OUT}
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/example_test --gtest_filter="SteinerTest*" > ${STEINER_TEST_OUT}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  DEPENDS example_test
)

add_custom_command(OUTPUT ${HAMILTONIAN_TEST_OUT}
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/example_test --gtest_filter="HamiltonianTest_*.*" > ${HAMILTONIAN_TEST_OUT}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  DEPENDS example_test
)

add_custom_target(test_output DEPENDS ${STEINER_TEST_OUT} ${HAMILTONIAN_TEST_OUT})

add_custom_command(OUTPUT ${STEINER_TEST_GRAPH}
  COMMAND ${PROJECT_SOURCE_DIR}/build_graph.py -i=${STEINER_TEST_OUT} -o=${STEINER_TEST_GRAPH}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  DEPENDS test_output
  # ${STEINER_TEST_OUT}
)

add_custom_command(OUTPUT ${HAMILTONIAN_TEST_GRAPH}
  COMMAND ${PROJECT_SOURCE_DIR}/build_graph.py -i=${HAMILTONIAN_TEST_OUT} -o=${HAMILTONIAN_TEST_GRAPH}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  DEPENDS ${HAMILTONIAN_TEST_OUT}
)

add_custom_target(steiner_graph DEPENDS ${STEINER_TEST_GRAPH})
add_custom_target(hamiltonian_graph DEPENDS ${HAMILTONIAN_TEST_GRAPH})

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
    COMMAND pdflatex ${THESIS_TEXT}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    DEPENDS ${THESIS_TEXT} steiner_graph hamiltonian_graph
    COMMENT "Generating thesis PDF"
)

add_custom_target(generate_pdf DEPENDS ${THESIS_OUT})
