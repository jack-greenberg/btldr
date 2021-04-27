workspace(name = "btldr")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name="oem-firmware",
    branch="main",
    init_submodules=True,
    remote="git@github.com:jack-greenberg/oem-firmware.git"
)

register_execution_platforms("@local_config_platform//:host", "@oem-firmware//bazel/platforms:atmega16m1_avr")

register_toolchains("@oem-firmware//bazel/toolchain:atmega16m1_toolchain")
