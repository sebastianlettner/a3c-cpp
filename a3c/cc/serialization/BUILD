cc_library(
    name="serializer",
    hdrs=["serializer.h"],
    deps=[
        "@flatbuffers//:flatbuffers"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="test_serialization_gen",
    hdrs=["test_generated.h"],
)

cc_library(
    name="observable",
    hdrs=["observable_generated.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name="graph-out",
    hdrs=["graphout_generated.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name="test_serialization",
    srcs=["serializer_test.cc"],
    copts =["-Iexternal/gtest/include"],
    deps=[
        ":serializer",
        "@gtest//:main",
        ":test_serialization_gen"
    ]
)