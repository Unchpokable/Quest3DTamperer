# kiero (https://github.com/Rebzzel/kiero) - vendored as-is.

add_library(kiero STATIC kiero.cpp)

target_include_directories(kiero PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(kiero PUBLIC minhook dxsdk)
