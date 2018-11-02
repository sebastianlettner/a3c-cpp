cc_library(
    name="zmq-draft",
    hdrs=["src/zmq_draft.h"]
)

cc_library(
    name="tweet-nacl",
    hdrs=["src/tweetnacl.h"]
)

cc_library(
    name = "main",
    srcs = glob([
        "src/*.cpp",
        "src/*.c"
    ]),
    hdrs = glob(
        [
            "src/*.hpp",
            "include/zmq.h",
        ],
        exclude=[
            "src/tweetnacl.h",
            "src/zmq_draft.h",
        ]
    ),
    deps=[
        "zmq-draft",
        "tweet-nacl",
        "@platform//:main"
    ],
    visibility = ["//visibility:public"],
)
