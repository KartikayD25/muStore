file(GLOB SOURCES "./*.cpp")
add_executable(mustore ${SOURCES})
include_directories(mustore PUBLIC "./include/")
include_directories(mustore PUBLIC "./uSer/src")
include_directories(mustore PUBLIC "./uSer/src/net")

set(PAPI_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/papi/install")

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/Makefile"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/configure" --prefix=${PAPI_INSTALL_DIR}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/papi/src"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/configure"
)

add_custom_target(papi_build
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/Makefile"
    COMMAND make -C "${CMAKE_CURRENT_SOURCE_DIR}/papi/src"
    COMMAND make -C "${CMAKE_CURRENT_SOURCE_DIR}/papi/src" install
)

add_dependencies(mustore papi_build)
target_link_libraries(mustore "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore PUBLIC "${PAPI_INSTALL_DIR}/include")
target_link_libraries(mustore pthread)