# add_library(assignment-lib main.cc)
set(Python_ADDITIONAL_VERSIONS 3.10)
build_exec(
        EXECNAME assignment
        SOURCE_FILES
            py_module.cc
            main.cc
        LIBRARIES_TO_LINK
            # assignment-lib
            ${Python3_LIBRARIES}
            ${libcore}
            ${libpoint-to-point}
            ${libnetwork}
            ${libapplications}
            ${libmobility}
            ${libcsma}
            ${libinternet}
            ${libflow-monitor}
            ${liblte}
            ${libai}
        EXECUTABLE_DIRECTORY_PATH ${CMAKE_OUTPUT_DIRECTORY}/
)

find_package(pybind11 REQUIRED)
add_library(ns3ai_rnn_py SHARED py_module.cc)
add_dependencies(assignment ns3ai_rnn_py)

set_target_properties(ns3ai_rnn_py PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ns3ai_rnn_py PRIVATE ${libai} ${Python3_LIBRARIES})

# Build Python interface along with C++ lib