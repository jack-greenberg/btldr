load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

cc_binary(
    name="btldr",
    srcs = ["src/btldr.c"],
    deps = [
        ":cfg",
        ":can_isp",
        "//lib:can",
        "//lib:crc32",
        "//lib:flash",
        "//lib:image",
        ":atmega16m1.ld",
    ],
    linkopts = [
        "-T atmega16m1.ld",
    ],
    target_compatible_with = [
        "@oem-firmware//bazel/constraints:atmega16m1"
    ],
    visibility=["//visibility:public"],
)

cc_library(
    name = "cfg",
    hdrs = [
        "src/config.h",
    ],
)

cc_library(
    name = "can_isp",
    srcs = [
        "src/can_isp.c",
        "src/can_isp_commands.c",
    ],
    hdrs = ["src/can_isp.h"],
    deps = [
        ":cfg",
        "//lib:can",
        "//lib:flash",
        "//lib:image",
    ],
    strip_include_prefix="src",
    target_compatible_with = [
        "@oem-firmware//bazel/constraints:atmega16m1"
    ],
)
