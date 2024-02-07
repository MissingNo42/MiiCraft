name = "texture"

r = open(f"{name}.tpl", "rb").read()

sz = len(r)
dt = ", ".join(map(hex, r))

c = f"""
static const u8 {name}_data[] = {{
    {dt}
}};
static const u32 {name}_sz = {sz};
static const float offset = {16 / 256};
"""

open(f"{name}.c", "w").write(c)
