name = "test"

r = open(f"{name}.mp3", "rb").read()

sz = len(r)
dt = ", ".join(map(hex, r))

c = f"""
static const u8 {name}_data[] ATTRIBUTE_ALIGN(32) = {{
    {dt}
}};
static const u32 {name}_sz = {sz};
"""

open(f"{name}.c", "w").write(c)
