# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_library(
    name="serializer",
    hdrs=["obs_generated.h"]
)

cc_binary(
    name = "agent",
    srcs = ["agent.cc"],
    deps = [
        "@flatbuffers//:flatbuffers",
        "@cppzmq//:main",
        ":serializer"
    ],
)

cc_binary(
    name="pred",
    srcs=["pred.cc"],
    deps = [
        "@flatbuffers//:flatbuffers",
        "@cppzmq//:main",
        ":serializer",
	    "@org_tensorflow//tensorflow/core:tensorflow"
    ],
)

cc_binary(
    name="tf_dist",
    srcs=["tf_dist.cc"],
    deps=[
        "@org_tensorflow//tensorflow/core/distributed_runtime/rpc:grpc_runtime"
    ]
)
