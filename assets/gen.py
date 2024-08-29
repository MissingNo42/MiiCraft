#!/usr/bin/env python3

name = "texture"

r = open(f"{name}.tpl", "rb").read()

sz = len(r)
dt = ", ".join(map(hex, r))

c = f"""
static const u8 {name}_data[] ATTRIBUTE_ALIGN(32) = {{
    {dt}
}};
static const u32 {name}_sz = {sz};
"""

open(f"{name}.h", "w").write(c)

print("SUCCESS")
