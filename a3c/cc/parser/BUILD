cc_library(
    name="convert-utf",
    srcs=["convert_utf.cc"],
    hdrs=["convert_utf.h"]
)

cc_library(
    name="simple_ini",
    hdrs=["simple_ini.h"],
    deps=[
        ":convert-utf"
    ],
    visibility=["//visibility:public"]
)

cc_test(
    name = "ini-parser-test",
    srcs = ["simple_ini_test.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        ":simple_ini",
    ],
)