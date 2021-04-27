load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name="can_drv",
    srcs = ["can_drv.c"],
    hdrs = ["can_drv.h"],
    target_compatible_with = [
        "@oem-firmware//bazel/constraints:atmega16m1"
    ],
)

cc_library(
    name="can",
    srcs = ["can_lib.c"],
    hdrs = ["can_lib.h"],
    deps = [":can_drv"],
    target_compatible_with = [
        "@oem-firmware//bazel/constraints:atmega16m1"
    ],
)

cc_library(
    name="crc32",
    srcs = ["crc32.c"],
    hdrs = ["crc32.h"],
)

cc_library(
    name="flash",
    srcs = ["flash.c"],
    hdrs = ["flash.h"],
    target_compatible_with = [
        "@oem-firmware//bazel/constraints:atmega16m1"
    ],
)

cc_library(
    name = "image_hdrs",
    hdrs = ["image.h"],
    include_prefix = "lib",
)

cc_library(
    name = "image",
    srcs = ["image.c"],
    deps = [
        ":image_hdrs",
        ":crc32",
    ],
)
