load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "patch_header",
    srcs = ["patch_header.c"],
    deps = [
        "//lib:image_hdrs",
        "//lib:crc32",
    ],
)
