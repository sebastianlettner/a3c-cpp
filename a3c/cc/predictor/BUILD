cc_test(
    name="test-predictor",
    srcs=["predictor_test.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps=[
        ":predictor",
        "@gtest//:main",
    ]
)

cc_library(
    name = "predictor",
    srcs = ["predictor.cc"],
    hdrs = ["predictor.h"],
    deps = [
        "//a3c/cc/parser:simple_ini",
        "@libzmq//:zmq-cpp",
    ]
)