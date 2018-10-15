workspace(name = "a3c")

# To update TensorFlow to a new revision.
# 1. Update the 'git_commit' args below to include the new git hash.
# 2. Get the sha256 hash of the archive with a command such as...
#    curl -L https://github.com/tensorflow/tensorflow/archive/<git hash>.tar.gz | sha256sum
#    and update the 'sha256' arg with the result.
# 3. Request the new archive to be mirrored on mirror.bazel.build for more
#    reliable downloads.
load("//a3c:repo.bzl", "tensorflow_http_archive")

tensorflow_http_archive(
    name = "org_tensorflow",
    git_commit = "9e0fa9578638f9147c0b180e6ea89d67d5c0bae3",
)

# TensorFlow depends on "io_bazel_rules_closure" so we need this here.
# Needs to be kept in sync with the same target in TensorFlow's WORKSPACE file.
http_archive(
    name = "io_bazel_rules_closure",
    strip_prefix = "rules_closure-dbb96841cc0a5fb2664c37822803b06dab20c7d1",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_closure/archive/dbb96841cc0a5fb2664c37822803b06dab20c7d1.tar.gz",
        "https://github.com/bazelbuild/rules_closure/archive/dbb96841cc0a5fb2664c37822803b06dab20c7d1.tar.gz",  # 2018-04-13
    ],
)

# Google test
new_http_archive(
    name = "gtest",
    url = "https://github.com/google/googletest/archive/release-1.7.0.zip",
    sha256 = "b58cb7547a28b2c718d1e38aee18a3659c9e3ff52440297e965f5edffe34b6d0",
    build_file = "gtest.BUILD",
    strip_prefix = "googletest-release-1.7.0",
)

# new_http_archive(
#     name="libzmq",
#     url="https://github.com/zeromq/libzmq/archive/master.zip",
#     build_file="libzmq.BUILD",
#     sha256="a39f72e9a1f359562d7054bded2b6564dd886f9e58758b2d3d19f9f59f014124",
#     strip_prefix="libzmq-master"
#)

new_local_repository(
    name="libzmq",
    path="/usr/local/include/libzmq",
    build_file="libzmq.BUILD"
)

# new_http_archive(
#     name="zeromq",
#     url="https://github.com/zeromq/cppzmq/archive/master.zip",
#     build_file="zeromq.BUILD",
#     sha256="9ca9f943246e362c3bc990f24890ad57a00a499b31d5c781526c5cf8dde6bcb6",
#     strip_prefix="cppzmq-master"
# )

# Please add all new TensorFlow Serving dependencies in workspace.bzl.
load("//a3c:workspace.bzl", "a3c_workspace")

a3c_workspace()

# Specify the minimum required bazel version.
load("@org_tensorflow//tensorflow:version_check.bzl", "check_bazel_version_at_least")

check_bazel_version_at_least("0.15.0")