file(GLOB SOURCES "./*.cpp")
include(FetchContent)

set(protobuf_BUILD_TESTS OFF)
add_subdirectory(protobuf)
add_subdirectory(flatbuffers)
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/uSer/test/proto")
set(DST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_custom_target(proto_build
    DEPENDS protobuf::libprotobuf protobuf::libprotoc protoc
    COMMAND cmake --install . --prefix "${CMAKE_BINARY_DIR}/src/protobuf/install"
)
add_custom_command(
    OUTPUT ${DST_DIR}/final.pb.cc ${DST_DIR}/final.pb.h
    COMMAND "${CMAKE_BINARY_DIR}/src/protobuf/protoc" -I=${SRC_DIR} --cpp_out=${DST_DIR} ${SRC_DIR}/final.proto
    DEPENDS ${SRC_DIR}/final.proto proto_build
    COMMENT "Running protoc on final.proto"
)

add_custom_target(protobuf_gen
    DEPENDS ${DST_DIR}/final.pb.cc ${DST_DIR}/final.pb.h
)

add_custom_command(
    OUTPUT ${DST_DIR}/final_generated.h
    COMMAND flatc -b --cpp -I ${SRC_DIR} -o ${DST_DIR} ${SRC_DIR}/final.fbs
    DEPENDS ${SRC_DIR}/final.fbs flatbuffers
    COMMENT "Running flatc on final.fbs"
)

add_custom_target(flatbuf_gen
    DEPENDS ${DST_DIR}/final_generated.h
)

set(PAPI_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/papi/install")
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/Makefile"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/papi/src/configure" --prefix=${PAPI_INSTALL_DIR} -with-static-lib=yes
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
include_directories(mustore_two_copy PUBLIC "./include/")
include_directories(mustore_two_copy PUBLIC "./uSer/src")
include_directories(mustore_two_copy PUBLIC "./uSer/src/net")
add_dependencies(mustore_two_copy papi_build)
target_include_directories(mustore_two_copy PUBLIC "${PAPI_INSTALL_DIR}/include")
target_link_libraries(mustore_two_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_two_copy "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_two_copy "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_two_copy pthread)

add_executable(mustore_one_copy ${SOURCES})
target_compile_definitions(mustore_one_copy PRIVATE ONE_COPY)
include_directories(mustore_one_copy PUBLIC "./include/")
include_directories(mustore_one_copy PUBLIC "./uSer/src")
include_directories(mustore_one_copy PUBLIC "./uSer/src/net")
add_dependencies(mustore_one_copy papi_build)
target_link_libraries(mustore_one_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_one_copy "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_one_copy "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_one_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_one_copy PUBLIC "${PAPI_INSTALL_DIR}/include")
target_link_libraries(mustore_one_copy pthread)

add_executable(mustore_zero_copy ${SOURCES})
target_compile_definitions(mustore_zero_copy PRIVATE ZERO_COPY)
include_directories(mustore_zero_copy PUBLIC "./include/")
include_directories(mustore_zero_copy PUBLIC "./uSer/src")
include_directories(mustore_zero_copy PUBLIC "./uSer/src/net")
add_dependencies(mustore_zero_copy papi_build)
target_link_libraries(mustore_zero_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_zero_copy "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_zero_copy "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_zero_copy "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_zero_copy PUBLIC "${PAPI_INSTALL_DIR}/include")
target_link_libraries(mustore_zero_copy pthread)

add_executable(mustore_muser_tc ${SOURCES})
target_compile_definitions(mustore_muser_tc PRIVATE MU_SER_TC)
target_compile_definitions(mustore_muser_tc PRIVATE MU_SER)
include_directories(mustore_muser_tc PUBLIC "./include/")
include_directories(mustore_muser_tc PUBLIC "./uSer/src")
include_directories(mustore_muser_tc PUBLIC "./uSer/src/net")
target_link_libraries(mustore_muser_tc pthread)
add_dependencies(mustore_muser_tc papi_build)
target_link_libraries(mustore_muser_tc "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_muser_tc "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_muser_tc "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_muser_tc "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_muser_tc PUBLIC "${PAPI_INSTALL_DIR}/include")


add_executable(mustore_muser_oc ${SOURCES})
target_compile_definitions(mustore_muser_oc PRIVATE MU_SER_OC)
target_compile_definitions(mustore_muser_oc PRIVATE MU_SER)
include_directories(mustore_muser_oc PUBLIC "./include/")
include_directories(mustore_muser_oc PUBLIC "./uSer/src")
include_directories(mustore_muser_oc PUBLIC "./uSer/src/net")
target_link_libraries(mustore_muser_oc pthread)
add_dependencies(mustore_muser_oc papi_build)
target_link_libraries(mustore_muser_oc "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_muser_oc "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_muser_oc "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_muser_oc "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_muser_oc PUBLIC "${PAPI_INSTALL_DIR}/include")

add_executable(mustore_muser_zc ${SOURCES})
target_compile_definitions(mustore_muser_zc PRIVATE MU_SER_ZC)
target_compile_definitions(mustore_muser_zc PRIVATE MU_SER)
include_directories(mustore_muser_zc PUBLIC "./include/")
include_directories(mustore_muser_zc PUBLIC "./uSer/src")
include_directories(mustore_muser_zc PUBLIC "./uSer/src/net")
target_link_libraries(mustore_muser_oc pthread)
add_dependencies(mustore_muser_zc papi_build)
target_link_libraries(mustore_muser_zc "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_muser_zc "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_muser_zc "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_muser_zc "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_muser_zc PUBLIC "${PAPI_INSTALL_DIR}/include")

add_executable(mustore_protobuf ${SOURCES}  "${CMAKE_CURRENT_SOURCE_DIR}/include/final.pb.cc" "${CMAKE_CURRENT_SOURCE_DIR}/ext-serializers/Protobuf.cpp")
target_compile_definitions(mustore_protobuf PRIVATE PROTO_BUF)
add_dependencies(mustore_protobuf protobuf_gen )
include_directories(mustore_protobuf PUBLIC "./include/")
include_directories(mustore_protobuf PUBLIC "${CMAKE_BINARY_DIR}/src/protobuf/install/include")
include_directories(mustore_protobuf PUBLIC "./uSer/src")
include_directories(mustore_protobuf PUBLIC "./uSer/src/net")
target_link_libraries(mustore_protobuf protobuf::libprotobuf)
target_link_libraries(mustore_protobuf pthread)
add_dependencies(mustore_protobuf papi_build)
target_link_libraries(mustore_protobuf "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_protobuf "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_protobuf "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_protobuf "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_protobuf PUBLIC "${PAPI_INSTALL_DIR}/include")



add_executable(mustore_flatbuffers ${SOURCES}  "${CMAKE_CURRENT_SOURCE_DIR}/ext-serializers/FlatBuffers.cpp")
target_compile_definitions(mustore_flatbuffers PRIVATE FLAT_BUF)
add_dependencies(mustore_flatbuffers flatbuf_gen  )
include_directories(mustore_flatbuffers PUBLIC "./include/")
include_directories(mustore_flatbuffers PUBLIC "./uSer/src")
include_directories(mustore_flatbuffers PUBLIC "./uSer/src/net")
target_link_libraries(mustore_flatbuffers flatbuffers)
target_link_libraries(mustore_flatbuffers pthread)
add_dependencies(mustore_flatbuffers papi_build)
target_link_libraries(mustore_flatbuffers "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_link_libraries(mustore_flatbuffers "${PAPI_INSTALL_DIR}/lib/libpfm.a")
target_link_libraries(mustore_flatbuffers "${PAPI_INSTALL_DIR}/lib/libsde.a")
target_link_libraries(mustore_flatbuffers "${PAPI_INSTALL_DIR}/lib/libpapi.a")
target_include_directories(mustore_flatbuffers PUBLIC "${PAPI_INSTALL_DIR}/include")


# target_compile_definitions(mustore_two_copy PRIVATE DEBUGMODE)
# target_compile_definitions(mustore_one_copy PRIVATE DEBUGMODE)
# target_compile_definitions(mustore_zero_copy PRIVATE DEBUGMODE)
# target_compile_definitions(mustore_muser_tc PRIVATE DEBUGMODE)
# target_compile_definitions(mustore_muser_oc PRIVATE DEBUGMODE)
# target_compile_definitions(mustore_muser_zc PRIVATE DEBUGMODE)
# target_compile_definitions(mustore_protobuf PRIVATE DEBUGMODE)
# target_compile_definitions(mustore_flatbuffers PRIVATE DEBUGMODE)
