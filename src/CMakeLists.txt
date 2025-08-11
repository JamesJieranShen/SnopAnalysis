file(GLOB_RECURSE SRC_FILES "*.cc")
add_library(SnopAnalysis OBJECT ${SRC_FILES})
target_include_directories(SnopAnalysis PUBLIC include)
target_link_libraries(SnopAnalysis PUBLIC
    ROOT::RIO ROOT::Tree ROOT::Hist ROOT::Core ROOT::ROOTDataFrame)
# Make generated header visible to your targets
target_include_directories(SnopAnalysis PUBLIC "${CMAKE_BINARY_DIR}/generated")
