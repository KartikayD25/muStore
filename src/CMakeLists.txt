file(GLOB SOURCES "./*.cpp")
set(PAPI_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/papi/install")
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/Makefile"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/configure" --prefix=${PAPI_INSTALL_DIR} -with-static-lib=yes --with-shared-lib=no
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/papi/src"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/configure"
)

add_custom_target(papi_build
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/Makefile"
    COMMAND make -C "${CMAKE_CURRENT_SOURCE_DIR}/papi/src"
    COMMAND make -C "${CMAKE_CURRENT_SOURCE_DIR}/papi/src" install
)

add_executable(mustore_two_copy ${SOURCES})
target_compile_definitions(mustore_two_copy PRIVATE TWO_COPY)
add_dependencies(mustore_two_copy papi_build)
include_directories(mustore_two_copy PUBLIC "./include/")
include_directories(mustore_two_copy PUBLIC "./uSer/src")
include_directories(mustore_two_copy PUBLIC "./uSer/src/net")
target_link_libraries(mustore_two_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_two_copy "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_two_copy "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_two_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_two_copy PUBLIC "${PAPI_INSTALL_DIR}/include")
target_link_libraries(mustore_two_copy pthread)

add_executable(mustore_one_copy ${SOURCES})
target_compile_definitions(mustore_one_copy PRIVATE ONE_COPY)
add_dependencies(mustore_one_copy papi_build)
include_directories(mustore_one_copy PUBLIC "./include/")
include_directories(mustore_one_copy PUBLIC "./uSer/src")
include_directories(mustore_one_copy PUBLIC "./uSer/src/net")
target_link_libraries(mustore_one_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_one_copy "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_one_copy "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_one_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_one_copy PUBLIC "${PAPI_INSTALL_DIR}/include")
target_link_libraries(mustore_one_copy pthread)

add_executable(mustore_zero_copy ${SOURCES})
target_compile_definitions(mustore_zero_copy PRIVATE ZERO_COPY)
add_dependencies(mustore_zero_copy papi_build)
include_directories(mustore_zero_copy PUBLIC "./include/")
include_directories(mustore_zero_copy PUBLIC "./uSer/src")
include_directories(mustore_zero_copy PUBLIC "./uSer/src/net")
target_link_libraries(mustore_zero_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_zero_copy "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_zero_copy "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_zero_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_zero_copy PUBLIC "${PAPI_INSTALL_DIR}/include")
target_link_libraries(mustore_zero_copy pthread)

add_executable(mustore_muser ${SOURCES})
target_compile_definitions(mustore_muser PRIVATE MU_SER)
add_dependencies(mustore_muser papi_build)
include_directories(mustore_muser PUBLIC "./include/")
include_directories(mustore_muser PUBLIC "./uSer/src")
include_directories(mustore_muser PUBLIC "./uSer/src/net")
target_link_libraries(mustore_muser "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_muser "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_muser "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_muser "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_muser PUBLIC "${PAPI_INSTALL_DIR}/include")
target_link_libraries(mustore_muser pthread)