project(${CMAKE_PROJECT_NAME})
message(STATUS "== ${PROJECT_NAME} ==")

file(GLOB allFiles
        "include/*.h"
        "src/*.cpp"
        "run/*.cpp"
        )

# Settings
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Create the executable
add_executable(${PROJECT_NAME}.out ${allFiles} ${PROTO_SRCS} ${PROTO_HDRS})
#target_include_directories(${PROJECT_NAME}.out PRIVATE "include" <PUT EXTERNAL LIB FILES HERE IF NEEDED>)
target_link_libraries(${PROJECT_NAME}.out
        pthread
        cmake_autoversion
        cmake_rovcommon
#        <PUT EXTERNAL LIB FILES HERE IF NEEDED>
        )

########################
# If running on both RasPi and PC add RasPi specefic libs here
# and use #ifdef ISABLE_RASPI to remove it from PC builds
# --------------
IF (ISABLE_RASPI)
    message(WARNING "WiringPi (RasPi) link is disabled for this build")
else (ISABLE_RASPI)
    # RasPi libs here:
    target_link_libraries(${PROJECT_NAME}.out wiringPi)
ENDIF (ISABLE_RASPI)

# Installation
install(TARGETS ${PROJECT_NAME}.out
        CONFIGURATIONS Release
        RUNTIME DESTINATION /usr/local/bin)
# Make service
install(FILES ${PROJECT_NAME}.service DESTINATION /etc/systemd/system)
install(CODE "execute_process(COMMAND bash -c \"systemctl start ${PROJECT_NAME}\")")
install(CODE "execute_process(COMMAND bash -c \"systemctl enable ${PROJECT_NAME}\")")
