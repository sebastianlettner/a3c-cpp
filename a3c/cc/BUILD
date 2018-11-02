# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = ["@cppzmq//:main"],
)

# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
    name = "main2",
    srcs = ["main2.cc"],
    deps = ["@cppzmq//:main",
            "//a3c/cc:broker"],
)
