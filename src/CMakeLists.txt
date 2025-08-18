file(GLOB_RECURSE SRC_FILES "*.cc")
add_library(SnopAnalysis OBJECT ${SRC_FILES})
target_include_directories(SnopAnalysis PUBLIC include)
target_link_libraries(SnopAnalysis PUBLIC
    ROOT::RIO ROOT::Tree ROOT::Hist ROOT::Core ROOT::ROOTDataFrame)
target_link_libraries(SnopAnalysis PUBLIC nlohmann_json::nlohmann_json CLI11::CLI11)
# Make generated header visible to your targets
target_include_directories(SnopAnalysis PUBLIC "${CMAKE_BINARY_DIR}/generated")
