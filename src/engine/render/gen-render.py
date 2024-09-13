

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

    return f"{chunk}.blocks[{x}][{y}][{z}]"


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

        code = f"""if ((type = c.blocks[{x}][y][{z}]).type) {{
{prx}{prz}
    fT = {chunk( x, "My",  z)};
    fB = {chunk( x, "my",  z)};
    fL = {chunk(mx,  "y",  z)};
    fR = {chunk(Mx,  "y",  z)};
    fF = {chunk( x,  "y", Mz)};
    fK = {chunk( x,  "y", mz)};

    if (isOpaque(type.type)) {{
        tT = !isOpaque(fT.type);
        tB = !isOpaque(fB.type);
        tL = !isOpaque(fL.type);
        tR = !isOpaque(fR.type);
        tF = !isOpaque(fF.type);
        tK = !isOpaque(fK.type);
    }}
    else if (isTransparent(type.type)) {{
        tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
        tB = fB.type != type.type; // !isTransparent(fB.type);
        tL = fL.type != type.type; // !isTransparent(fL.type);
        tR = fR.type != type.type; // !isTransparent(fR.type);
        tF = fF.type != type.type; // !isTransparent(fF.type);
        tK = fK.type != type.type; // !isTransparent(fK.type);
    }} else if (isSemiTransparent(type.type)) {{
        tT = tB = tL = tR = tF = tK = true;
    }} else {{
        blockData[type.type].render();
        printf("Render failed : %d\\n", type.type); // TODO call special render mode
    }}

    sz = tT + tB + tL + tR + tF + tK;
    if (sz) {{

        lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
        lB = fB.naturalLight;
        lL = fL.naturalLight;
        lR = fR.naturalLight;
        lF = fF.naturalLight;
        lK = fK.naturalLight;

        if (isOpaque(type.type) || isSemiTransparent(type.type)) {{
            A = !isTransparent({chunk(mx, "My", Mz)}.type);
            B = !isTransparent({chunk(mx, "My", mz)}.type);
            C = !isTransparent({chunk(Mx, "My", mz)}.type);
            D = !isTransparent({chunk(Mx, "My", Mz)}.type);
            E = !isTransparent({chunk(mx, "my", Mz)}.type);
            F = !isTransparent({chunk(mx, "my", mz)}.type);
            G = !isTransparent({chunk(Mx, "my", mz)}.type);
            H = !isTransparent({chunk(Mx, "my", Mz)}.type);

            AB = !isTransparent({chunk(mx, "My", z)}.type);
            BC = !isTransparent({chunk(x, "My", mz)}.type);
            CD = !isTransparent({chunk(Mx, "My", z)}.type);
            DA = !isTransparent({chunk(x, "My", Mz)}.type);

            EF = !isTransparent({chunk(mx, "my", z)}.type);
            FG = !isTransparent({chunk(x, "my", mz)}.type);
            GH = !isTransparent({chunk(Mx, "my", z)}.type);
            HE = !isTransparent({chunk(x, "my", Mz)}.type);

            AE = !isTransparent({chunk(mx, "y", Mz)}.type);
            BF = !isTransparent({chunk(mx, "y", mz)}.type);
            CG = !isTransparent({chunk(Mx, "y", mz)}.type);
            DH = !isTransparent({chunk(Mx, "y", Mz)}.type);
        }} else if (isTransparent(type.type)) {{
            // TODO: try some fun here
            A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
        }}


        //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
        f32 fx = (f32)({xP}px),
            fy = (f32)y,
            fz = (f32)({zP}pz),
            fmx = (f32)({mxP}px),
            fmy = (f32)my,
            fmz = (f32)({mzP}pz);
            
        if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
        if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
        if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
        if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
        if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
        if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
        //GX_End();
    }}
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

