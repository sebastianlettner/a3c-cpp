cc_test(
    name="test-predictor",
    srcs=["predictor_test.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps=[
        ":predictor",
        "@gtest//:main",
        "@simple-ini//:main"
    ]
)

cc_library(
    name="cluster-node",
    hdrs=["cluster_node.h"],
    srcs=["cluster_node.cc"],
    deps=[
        "@org_tensorflow//tensorflow/core/distributed_runtime/rpc:grpc_runtime"
    ]
)

cc_library(
    name = "predictor",
    srcs = ["predictor.cc"],
    hdrs = ["predictor.h"],
    deps = [
        "@cppzmq//:main",
        "@simpleini//:main",
        "@flatbuffers//:flatbuffers",
        "//a3c/cc/serialization:graph-out",
        "//a3c/cc/helper:zhelper",
        "//a3c/cc/serialization:serializer"
    ]
)