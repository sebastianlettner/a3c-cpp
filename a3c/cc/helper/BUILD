# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "zhelper",
    srcs = ["zhelper.cc"],
    hdrs = ["zhelper.h"],
    deps = [
        "@cppzmq//:main",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="fill_server_def",
    hdrs=["fill_server_def.h"],
    srcs=["fill_server_def.cc"],
    visibility = ["//visibility:public"],
    deps=[
    ]

)