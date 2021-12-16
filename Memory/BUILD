##load("@rules_cc//cc:defs.bzl", "cc_test")  #load the test target

cc_test(
    name = "mma_test",
    size = "small",
    srcs = ["mma_test.cc"],
    deps = [
        "@gtest//:gtest_main",
        "//lib:utils_lib",
        "//lib:memory_manager_lib",
        ],
    copts = [
        "-std=c++17",
    ],
    linkopts = [
        "-pthread",
    ]
)

cc_binary(
    name = "q1",
    srcs = [
        "q1.cc"
            ],
    deps = [
        "//lib:utils_lib",
        "//lib:memory_manager_lib",
    ],
    linkopts = [
        "-lpthread"
    ],
    copts = [
        "-std=c++17",
    ],
)

cc_binary(
    name = "q2",
    srcs = [
        "q2.cc"
            ],
    deps = [
        "//lib:utils_lib",
        "//lib:memory_manager_lib",
    ],
    linkopts = [
        "-lpthread"
    ],
    copts = [
        "-std=c++17",
    ],
)

cc_binary(
    name = "q3",
    srcs = [
        "q3.cc"
            ],
    deps = [
        "//lib:utils_lib",
        "//lib:memory_manager_lib",
    ],
    linkopts = [
        "-lpthread"
    ],
    copts = [
        "-std=c++17",
    ],
)

cc_binary(
    name = "q4",
    srcs = [
        "q4.cc"
            ],
    deps = [
        "//lib:utils_lib",
        "//lib:memory_manager_lib",
    ],
    linkopts = [
        "-lpthread"
    ],
    copts = [
        "-std=c++17",
    ],
)