cmake_minimum_required(VERSION 3.14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${CMAKE_SOURCE_DIR}/compile_commands.json"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
add_compile_options(-Wall -Wextra -Werror)
add_compile_options($<$<CONFIG:Release>:-march=native>)
find_package(Git QUIET)
set(GIT_DESCRIBE "unknown")
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C ${CMAKE_SOURCE_DIR} describe --tags --dirty --always
    OUTPUT_VARIABLE GIT_DESCRIBE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()
configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/Config.hh.in"
  "${CMAKE_BINARY_DIR}/generated/Config.hh"
  @ONLY
)
include(FetchContent)

project(SnopAnalysis LANGUAGES CXX)

# ROOT
find_package(ROOT REQUIRED COMPONENTS RIO Tree Hist Core ROOTDataFrame)

# nlohmann json
fetchcontent_declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.12.0
)
fetchcontent_makeavailable(json)

# CLI11
fetchcontent_declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.5.0
)
fetchcontent_makeavailable(CLI11)


add_subdirectory(src)

set(TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tools")
# Compile each script into an executable linked against the core library
file(GLOB TOOL_FILES "${TOOLS_DIR}/*.cc")
foreach(tool ${TOOL_FILES})
  get_filename_component(tool_name ${tool} NAME_WE)
  add_executable(${tool_name} ${tool})
  target_link_libraries(${tool_name} PRIVATE SnopAnalysis CLI11::CLI11)
  target_include_directories(${tool_name} PRIVATE ${INCLUDE_DIR})

  add_custom_command(TARGET ${tool_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_ROOT_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${tool_name}>
        ${PROJECT_ROOT_DIR}/bin/
)

endforeach()



