file(GLOB SOURCES "./*.cpp")
add_executable(mustore ${SOURCES})
include_directories(mustore PUBLIC "./include/")