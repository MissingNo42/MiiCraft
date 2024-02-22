name = "sounds/step1"

r = open(f"{name}.mod", "rb").read()

sz = len(r)
dt = ", ".join(map(hex, r))

c = f"""
static const u8 {name}_data[] ATTRIBUTE_ALIGN(32) = {{
    {dt}
}};
static const u32 {name}_sz = {sz};
"""

open(f"{name}.c", "w").write(c)
