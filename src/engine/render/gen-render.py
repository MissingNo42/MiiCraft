

def indent(s: str, tab: int) -> str:
    return (t := ('    ' * tab)) + ('\n' + t).join(s.splitlines()).rstrip(t)


def chunk(x, y, z):
    chunk = "c"

    if not isinstance(z, str):
        if z < 0:
            chunk += "south"
        if z > 15:
            chunk += "north"
        z &= 15

    if not isinstance(x, str):
        if x < 0:
            chunk += "west"
        if x > 15:
            chunk += "east"
        x &= 15

    return f"{chunk}.blocks[{y}][{x}][{z}]"


if __name__ == '__main__':
    for x, z in [(0, 0), (0, 15), (15, 0), (15, 15), ('x', 0), ('x', 15), (0, 'z'), (15, 'z'), ('x', 'z')]:

        if x == 'x':
            prx = indent("mx = x - 1;\nMx = x + 1;\n\n", 1)
            mx = "mx"
            Mx = "Mx"
        else:
            prx = ""
            mx = x - 1
            Mx = x + 1

        xP = f"{x} + " if x else ""
        mxP = "mx + " if isinstance(mx, str) else f"{mx} + "

        if z == 'z':
            prz = indent("mz = z - 1;\nMz = z + 1;\n\n", 1)
            mz = "mz"
            Mz = "Mz"
        else:
            prz = ""
            mz = z - 1
            Mz = z + 1

        zP = f"{z} + " if z else ""
        mzP = "mz + " if isinstance(mz, str) else f"{mz} + "

        code = f"""if ((block = c.blocks[y][{x}][{z}]).type) {{
{prx}{prz}
    fT = {chunk( x, "My",  z)};
    fB = {chunk( x, "my",  z)};
    fL = {chunk(mx,  "y",  z)};
    fR = {chunk(Mx,  "y",  z)};
    fF = {chunk( x,  "y", Mz)};
    fK = {chunk( x,  "y", mz)};

    target = &&light_x{x}_z{z};
    endTarget = &&end_x{x}_z{z};
    goto prepare;
    
    light_x{x}_z{z}:

    //if (isOpaque(type.type) || isSemiTransparent(type.type)) {{
    A = {chunk(mx, "My", Mz)};
    B = {chunk(mx, "My", mz)};
    C = {chunk(Mx, "My", mz)};
    D = {chunk(Mx, "My", Mz)};
    E = {chunk(mx, "my", Mz)};
    F = {chunk(mx, "my", mz)};
    G = {chunk(Mx, "my", mz)};
    H = {chunk(Mx, "my", Mz)};

    AB = {chunk(mx, "My", z)};
    BC = {chunk(x, "My", mz)};
    CD = {chunk(Mx, "My", z)};
    DA = {chunk(x, "My", Mz)};

    EF = {chunk(mx, "my", z)};
    FG = {chunk(x, "my", mz)};
    GH = {chunk(Mx, "my", z)};
    HE = {chunk(x, "my", Mz)};

    AE = {chunk(mx, "y", Mz)};
    BF = {chunk(mx, "y", mz)};
    CG = {chunk(Mx, "y", mz)};
    DH = {chunk(Mx, "y", Mz)};
    //}} else if (isTransparent(type.type)) {{
    //    // TODO: try some fun here
    //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
    //}}

    fx = static_cast<f32>({xP}px),
    fy = static_cast<f32>(y),
    fz = static_cast<f32>({zP}pz),
    fmx = static_cast<f32>({mxP}px),
    fmy = static_cast<f32>(my),
    fmz = static_cast<f32>({mzP}pz);
    
    goto render;
    end_x{x}_z{z}:;
}}"""

        if z == "z":
            code = f"""for(z = 1; z < 15; z++) {{
{indent(code, 1)} 
}}"""

        if x == "x":
            code = f"""for(x = 1; x < 15; x++) {{
{indent(code, 1)} 
}}"""

        code = f"// X {x} Z {z}\n{code}"
        if x == "x" and z == "z":
            code = f"""center:\n\n{code}"""

        print('\n')
        print(indent(code, 2))

