set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Message.proto)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILE})

add_library(proto
        ${PROTO_SRCS}
        ${PROTO_HDRS}
)

target_link_libraries(proto
        PUBLIC
        protobuf::libprotobuf
)

target_include_directories(proto
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)