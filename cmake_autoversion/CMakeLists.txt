# Created by thm on 19/10/2021.
##################################
# Set folder name as project name
##################################
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})
message(STATUS "== ${PROJECT_NAME} ==")
##################################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/")

# Fetch the git version of parent directory.
# (We are not interested this submodule version but in the project this was incorporated in.)
include(GetGitRevisionDescription)
git_describe(VERSION --tags)

# Parse the version information into pieces.
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Push version to parent scope. This is a submodule after all
set (AUTO_VERSION_COMPLETE ${VERSION} PARENT_SCOPE)
set (AUTO_VERSION ${VERSION_SHORT} PARENT_SCOPE)

# Write to the version files so we are able to get the version info in the code. #include the files below to get version in cpp.
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/gitAutoVersion.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/gitAutoVersion.cpp" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/gitAutoVersion.h" "${CMAKE_CURRENT_BINARY_DIR}/gitAutoVersion.h" @ONLY)

# Make lib. Use target_link_libraries(autoVersion) to add autoVersion to your code.
add_library(${PROJECT_NAME} "${CMAKE_CURRENT_BINARY_DIR}/gitAutoVersion.cpp" "${CMAKE_CURRENT_BINARY_DIR}/gitAutoVersion.h")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}) # Add current bin dir for auto generated files

