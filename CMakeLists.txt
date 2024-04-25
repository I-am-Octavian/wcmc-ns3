# add_library(assignment-lib main.cc)
build_exec(
        EXECNAME assignment
        SOURCE_FILES
            main.cc
            py_module.cc
        LIBRARIES_TO_LINK
            # assignment-lib
            ${libai}
            ${libcore}
            ${libpoint-to-point}
            ${libnetwork}
            ${libapplications}
            ${libmobility}
            ${libcsma}
            ${libinternet}
            ${libflow-monitor}
            ${liblte}
            ${PYTHON_LIBRARIES}
        EXECUTABLE_DIRECTORY_PATH ${CMAKE_OUTPUT_DIRECTORY}/
)

find_package(pybind11 REQUIRED)
pybind11_add_module(ns3ai_rnn_py py_module.cc)
add_dependencies(assignment ns3ai_rnn_py)

set_target_properties(ns3ai_rnn_py PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ns3ai_rnn_py PRIVATE ${libai} ${PYTHON_LIBRARIES})

# Build Python interface along with C++ lib

