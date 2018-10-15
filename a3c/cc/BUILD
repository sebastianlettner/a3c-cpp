cc_library(
    name="predictor",
    srcs=["predictor/predictor.cc"],
    hdrs=["predictor/predictor.h"],
)

cc_binary(
    name="main",
    srcs=["main.cc"],
    deps=[
	":predictor",
	"@org_tensorflow//tensorflow/core:lib"
    ]
)