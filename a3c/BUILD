package(
    default_visibility = ["//a3c:internal"],
)

licenses(["notice"])  # Apache 2.0

exports_files(["LICENSE"])

# open source marker; do not remove

package_group(
    name = "internal",
    packages = [
        "//a3c/...",
    ],
)

filegroup(
    name = "all_files",
    srcs = glob(
        ["**/*"],
    ),
)