#!/usr/bin/env python3

name = "texture"

r = open(f"{name}.tpl", "rb").read()

sz = len(r)
dt = ", ".join(map(hex, r))


def read_color(name: str) -> str:
    with open(name, "rb") as f:
        x = []
        y = 0
        while d := f.read(4):
            x.append(("\n\t" if y % 8 == 0 else "") + '{ ' + ', '.join(map(lambda v: f"0x{v:02X}", d)) + ' }')
            y += 1

        assert len(x) == 4096

        return ", ".join(x)


c = f"""
static const GXColor dayLight[] ATTRIBUTE_ALIGN(32) = {{
    {read_color("day64.data")}
}};

static const GXColor nightLight[] ATTRIBUTE_ALIGN(32) = {{
    {read_color("night64.data")}
}};

static const u8 {name}_data[] ATTRIBUTE_ALIGN(32) = {{
    {dt}
}};

static const u32 {name}_sz = {sz};
"""

open(f"{name}.h", "w").write(c)

print("SUCCESS")
