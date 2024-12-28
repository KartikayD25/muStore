file(GLOB SOURCES "./*.cpp")
# find_package(Protobuf CONFIG REQUIRED)
set(protobuf_BUILD_TESTS OFF)
add_subdirectory(protobuf)
add_subdirectory(flatbuffers)
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/uSer/test/proto")
set(DST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_custom_command(
    OUTPUT ${DST_DIR}/final.pb.cc ${DST_DIR}/final.pb.h
    COMMAND "${CMAKE_BINARY_DIR}/src/protobuf/protoc" -I=${SRC_DIR} --cpp_out=${DST_DIR} ${SRC_DIR}/final.proto
    DEPENDS ${SRC_DIR}/final.proto
    COMMENT "Running protoc on final.proto"
)

add_custom_target(protobuf_gen
    DEPENDS ${DST_DIR}/final.pb.cc ${DST_DIR}/final.pb.h
)

add_custom_command(
    OUTPUT ${DST_DIR}/final_generated.h
    COMMAND flatc -b --cpp -I ${SRC_DIR} -o ${DST_DIR} ${SRC_DIR}/final.fbs
    DEPENDS ${SRC_DIR}/final.fbs
    COMMENT "Running flatc on final.fbs"
)

add_custom_target(flatbuf_gen
    DEPENDS ${DST_DIR}/final_generated.h
)


add_executable(mustore_two_copy ${SOURCES})
target_compile_definitions(mustore_two_copy PRIVATE TWO_COPY)
include_directories(mustore_two_copy PUBLIC "./include/")
include_directories(mustore_two_copy PUBLIC "./uSer/src")
include_directories(mustore_two_copy PUBLIC "./uSer/src/net")
target_link_libraries(mustore_two_copy pthread)

add_executable(mustore_one_copy ${SOURCES})
target_compile_definitions(mustore_one_copy PRIVATE ONE_COPY)
include_directories(mustore_one_copy PUBLIC "./include/")
include_directories(mustore_one_copy PUBLIC "./uSer/src")
include_directories(mustore_one_copy PUBLIC "./uSer/src/net")
target_link_libraries(mustore_one_copy pthread)

add_executable(mustore_zero_copy ${SOURCES})
target_compile_definitions(mustore_zero_copy PRIVATE ZERO_COPY)
include_directories(mustore_zero_copy PUBLIC "./include/")
include_directories(mustore_zero_copy PUBLIC "./uSer/src")
include_directories(mustore_zero_copy PUBLIC "./uSer/src/net")
target_link_libraries(mustore_zero_copy pthread)

add_executable(mustore_muser ${SOURCES})
target_compile_definitions(mustore_muser PRIVATE MU_SER)
include_directories(mustore_muser PUBLIC "./include/")
include_directories(mustore_muser PUBLIC "./uSer/src")
include_directories(mustore_muser PUBLIC "./uSer/src/net")
target_link_libraries(mustore_muser pthread)

add_executable(mustore_protobuf ${SOURCES}  "${CMAKE_CURRENT_SOURCE_DIR}/include/final.pb.cc" "${CMAKE_CURRENT_SOURCE_DIR}/ext-serializers/Protobuf.cpp")
target_compile_definitions(mustore_protobuf PRIVATE PROTO_BUF)
add_dependencies(mustore_protobuf protobuf_gen )
include_directories(mustore_protobuf PUBLIC "./include/")
include_directories(mustore_protobuf PUBLIC "./uSer/src")
include_directories(mustore_protobuf PUBLIC "./uSer/src/net")
target_link_libraries(mustore_protobuf protobuf::libprotobuf)
target_link_libraries(mustore_protobuf pthread)

add_executable(mustore_flatbuffers ${SOURCES}  "${CMAKE_CURRENT_SOURCE_DIR}/include/final.pb.cc" "${CMAKE_CURRENT_SOURCE_DIR}/ext-serializers/FlatBuffers.cpp")
target_compile_definitions(mustore_flatbuffers PRIVATE FLAT_BUF)
add_dependencies(mustore_flatbuffers flatbuf_gen )
include_directories(mustore_flatbuffers PUBLIC "./include/")
include_directories(mustore_flatbuffers PUBLIC "./uSer/src")
include_directories(mustore_flatbuffers PUBLIC "./uSer/src/net")
target_link_libraries(mustore_flatbuffers flatbuffers)
target_link_libraries(mustore_flatbuffers pthread)