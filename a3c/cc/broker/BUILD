cc_library(
    name="broker",
    srcs=["broker.cc"],
    hdrs=["broker.h"],
    deps=[
	"//a3c/cc/helper:zhelper",
    "@simpleini//:main",
    ]
)

cc_test(
    name="test_broker",
    srcs=["broker_test.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps=[
        ":broker",
        "@gtest//:main"
    ],

)