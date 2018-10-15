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
            "src/zmq_draft.h"
        ]
    ),
    deps=[
        "zmq-draft",
        "tweet-nacl"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="zmq-cpp",
    hdrs=["zmq.hpp"],
    deps=[
        ":main"
    ],
    visibility = ["//visibility:public"],
)