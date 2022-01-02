package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mma_client_lib",
    srcs = [
        "mma_client.cc",
        "array_list.cc",
        "memory_manager.cc",
    ],
    hdrs = [
        "mma_client.h",
        "array_list.h",
        "memory_manager.h",
        ],
    defines = ["BAZEL_BUILD"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//proto:mma_grpc",
    ],
)

cc_library(
    name = "mma_server_lib",
    srcs = [
        "mma_server.cc",
        "memory_manager.cc",
        "array_list.cc",
        "mma_client.cc",
    ],
    hdrs = [
        "mma_server.h",
        "memory_manager.h",
        "array_list.h",
        "mma_client.h",
        ],
    defines = ["BAZEL_BUILD"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//proto:mma_grpc",
    ],
)

cc_library(
    name = "utils_lib",
    srcs = [
        "utils.cc",
        ],
    hdrs = [
        "utils.h",
        ],
)