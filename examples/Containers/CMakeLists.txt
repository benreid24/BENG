add_executable(containers main.cpp)

target_link_libraries(containers
    BLIB
)

set_target_properties(containers
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)