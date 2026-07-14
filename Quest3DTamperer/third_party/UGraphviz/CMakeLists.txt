# UGraphviz (https://github.com/Ubpa/UGraphviz) - vendored as-is.
# Pure STL DOT-source generator; no dependency on the real Graphviz library.

add_library(UGraphviz STATIC
    Graph.cpp
    Registry.cpp
    Subgraph.cpp
)

target_include_directories(UGraphviz PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
