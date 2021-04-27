def _image_impl(ctx):


_image = rule(
    implementation=_image_rule,
    doc="""
Used to generate a binary image with custom metadata.

Example:
    image(
        name = "hacker_board",
        srcs = [
            "main.c",
        ],
        deps = [
            "//lib:can",
        ],
    )
""",
    attrs = {

    # name = "binary",
    # srcs = [elf],
    # outs = [elf + ".bin"],
    # cmd = [
    #     "avr-gcc -O binary $< $@",
    #     "$(location //scripts:patch_header) $@",
    # ],
    # tools = [
    #     "//scripts:patch_header",
    # ],
)

def image(name, srcs, **kwargs):
    _image(name = name, **kwargs)
    _patch(name = "%s.patch" % name, **kwargs)
    _flash(name = 
