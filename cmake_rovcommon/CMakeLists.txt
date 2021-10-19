##################################
# Set folder name as project name
##################################
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})
message(STATUS "== ${PROJECT_NAME} ==")
##################################

include(FetchContent)

message(STATUS "Adding fmt formatter")
FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
        )
FetchContent_MakeAvailable(fmt)

file(GLOB allFiles
  "include/*.h"
  "src/*.cpp"
  )

# ---------- Create the library ----------
add_library(${PROJECT_NAME} ${allFiles})
target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(${PROJECT_NAME} pthread atomic fmt::fmt stdc++fs)





