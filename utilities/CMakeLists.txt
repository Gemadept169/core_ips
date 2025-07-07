file(GLOB_RECURSE sources *.cpp)
qt_add_library(utilities STATIC
    ${sources}
)
target_link_libraries(utilities 
    PUBLIC Qt6::Core)