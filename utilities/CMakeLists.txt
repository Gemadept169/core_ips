file(GLOB_RECURSE utilities_sources *.cpp)
qt_add_library(utilities STATIC
    ${utilities_sources}
)
target_link_libraries(utilities 
    PUBLIC Qt6::Core)