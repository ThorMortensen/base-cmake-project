# Created by thm on 19/10/2021.
############################
#   Project setup
############################
cmake_minimum_required(VERSION 3.1...3.19)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif ()

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

############################
#   Git Updater
############################
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_UP "Update git and submodules during build" OFF)
    if(GIT_UP)
        message(STATUS "Git and submodule update please wait")
        execute_process(COMMAND ${GIT_EXECUTABLE} pull
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT
                OUTPUT_QUIET
                ERROR_QUIET)
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT
                OUTPUT_QUIET
                ERROR_QUIET)
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule foreach --recursive git checkout master
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT
                OUTPUT_QUIET
                ERROR_QUIET)
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule foreach --recursive git pull
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT
                OUTPUT_QUIET
                ERROR_QUIET)
    endif()
endif()

########################
#    Build type stuff
########################
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 20)
#set(default_build_type "Release")

########################
#    Version stuff
########################
add_subdirectory(cmake_autoversion)
message(STATUS "gitAutoVersion version --> ${AUTO_VERSION_COMPLETE}")
project(${PROJECT_NAME} VERSION ${AUTO_VERSION}
        DESCRIPTION "PROJECT_NAME_CHANGE_ME"
        LANGUAGES CXX)

########################
#      Options
########################
message(STATUS "== Root CMake ==")
OPTION(ISABLE_DBG_PRINT "Disable debug print to std-out" ON) # Disable by default
OPTION(IRECT_LOG_TO_STD_OUT "Direct log print to std-out" OFF) # Disable by default
OPTION(ISABLE_RASPI "Disable RasPi specific functionality and libs" OFF) # Disable by default

IF(ISABLE_RASPI)
    message(STATUS "RasPi specific functionality is off!!!")
    add_definitions(-DISABLE_RASPI)
else(ISABLE_RASPI)
    message(STATUS "RasPi build")
ENDIF(ISABLE_RASPI)

IF (ISABLE_DBG_PRINT)
    message(STATUS "Debug print is DISABLED for this build")
    add_definitions(-DISABLE_LXO_DBG_PRINT)
else (ISABLE_DBG_PRINT)
    message(STATUS "Debug print is ENABLED for this build")
ENDIF (ISABLE_DBG_PRINT)

########################
#   Compile Options
########################
add_compile_options(
#        -Wall
#        -Wextra # reasonable and standard
        -Wno-psabi
        -Wno-comment # Ignore comments inside another comment
#        -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
        -Wcast-align # warn for potential performance problem casts
#        -Woverloaded-virtual # warn if you overload (not override) a virtual function
        -Wmisleading-indentation # warn if identation implies blocks where blocks do not exist
        -Wduplicated-branches # warn if if / else branches have duplicated code
        -Wduplicated-cond # warn if if / else chain has duplicated conditions
        -Wlogical-op # warn about logical operations being used where bitwise were probably wanted

#        -Wshadow # warn the user if a variable declaration shadows one from a parent context
        #  -Wno-c++98-compat

#        -Wold-style-cast # warn for c-style casts
#        -Wunused # warn on anything being unused
        #        -Wpedantic # warn if non-standard C++ is used
        #        -Wconversion # warn on type conversions that may lose
        #        -Wsign-conversion # warn on sign conversions
        #        -Wnull-dereference # warn if a null dereference is detected
        #        -Wuseless-cast # warn if you perform a cast to the same type
        -Wdouble-promotion # warn if float is implicit promoted to double
        -Wformat=2 # warn on security issues around functions that format output (ie printf)
#        -rdynamic
        )

########################
#     Sanitizers
########################
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
        CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel tsan asan lsan msan ubsan"
        FORCE)

# ThreadSanitizer
set(CMAKE_C_FLAGS_TSAN
        "-fsanitize=thread -g -O1"
        CACHE STRING "Flags used by the C compiler during ThreadSanitizer builds."
        FORCE)
set(CMAKE_CXX_FLAGS_TSAN
        "-fsanitize=thread -g -O1"
        CACHE STRING "Flags used by the C++ compiler during ThreadSanitizer builds."
        FORCE)

# AddressSanitize
set(CMAKE_C_FLAGS_ASAN
        "-static-libasan -fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
        CACHE STRING "Flags used by the C compiler during AddressSanitizer builds."
        FORCE)
set(CMAKE_CXX_FLAGS_ASAN
        "-static-libasan -fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
        CACHE STRING "Flags used by the C++ compiler during AddressSanitizer builds."
        FORCE)

# LeakSanitizer
set(CMAKE_C_FLAGS_LSAN
        "-fsanitize=leak -fno-omit-frame-pointer -g -O1"
        CACHE STRING "Flags used by the C compiler during LeakSanitizer builds."
        FORCE)
set(CMAKE_CXX_FLAGS_LSAN
        "-fsanitize=leak -fno-omit-frame-pointer -g -O1"
        CACHE STRING "Flags used by the C++ compiler during LeakSanitizer builds."
        FORCE)

# MemorySanitizer
set(CMAKE_C_FLAGS_MSAN
        "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2"
        CACHE STRING "Flags used by the C compiler during MemorySanitizer builds."
        FORCE)
set(CMAKE_CXX_FLAGS_MSAN
        "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2"
        CACHE STRING "Flags used by the C++ compiler during MemorySanitizer builds."
        FORCE)

# UndefinedBehaviour
set(CMAKE_C_FLAGS_UBSAN
        "-fsanitize=undefined"
        CACHE STRING "Flags used by the C compiler during UndefinedBehaviourSanitizer builds."
        FORCE)
set(CMAKE_CXX_FLAGS_UBSAN
        "-fsanitize=undefined"
        CACHE STRING "Flags used by the C++ compiler during UndefinedBehaviourSanitizer builds."
        FORCE)

########################
#  Project Composition
########################
# library's
add_subdirectory(cmake_rovcommon)
add_subdirectory(rov_protobuf)

# Executables
add_subdirectory(${PROJECT_NAME})