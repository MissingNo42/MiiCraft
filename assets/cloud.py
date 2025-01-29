print("init...")
import matplotlib.pyplot as plt
import cv2
import numpy as np

print("load...")
cloud = cv2.imread("cloud.png", cv2.IMREAD_UNCHANGED)
cloud = np.transpose(cloud, (1, 0, 2))
cloud = np.apply_along_axis(lambda x: x[3] > 128, 2, cloud)
print(cloud.shape)

type vertex = tuple[int, int]

borders = []  # px-coord of borders (ins & out)
vertices = []  # coord of border's vertices (ins & out)
rings = []  # coord of border's vertices (ins & out splitted ordered)
lines = []  # coord of border's vertices (ins & out merged ordered)


def propagate(shape: set[vertex], x: int, y: int, cloud: np.array) -> None:
    """
    [Internal] Propagate a cloud shape from a pixel to its neighbors.

    Args:
        shape: the cloud shape
        x: the x-coordinate of the pixel to explore
        y: the y-coordinate of the pixel to explore
        cloud: the cloud flat image
    """

    if x < 0 or y < 0 or x >= cloud.shape[0] or y >= cloud.shape[1] or not cloud[x, y]:
        return

    cloud[x, y] = False
    shape.add((x, y))

    propagate(shape, x + 1, y, cloud)
    propagate(shape, x - 1, y, cloud)
    propagate(shape, x, y + 1, cloud)
    propagate(shape, x, y - 1, cloud)


print("extract shapes...")


def extract_shapes(cloud: np.array) -> list[set[vertex]]:
    """
    Extract the different cloud shapes from a cloud flat image.

    Args:
        cloud: the cloud flat image

    Returns: a list of shapes, each shape is a set of pixels coordinates
    """

    shapes = []
    for x in range(cloud.shape[0]):
        for y in range(cloud.shape[1]):
            if cloud[x, y]:
                shapes.append(shape := set())
                propagate(shape, x, y, cloud)
    return shapes


print("extract borders...")


def extract_borders(shapes: list[set[vertex]]) -> list[set[vertex]]:
    """
    Extract the borders of the different cloud shapes.

    Args:
        shapes: the list of shapes

    Returns: a list of borders, each border is a set of pixels coordinates
    """

    borders = []
    for shape in shapes:
        border = set()
        for x, y in shape:
            if (x + 1, y) not in shape or (x - 1, y) not in shape or (x, y + 1) not in shape or (x, y - 1) not in shape:
                border.add((x, y))
        borders.append(border)
    return borders


print("extract vertices...")

def extract_vertices(shapes: list[set[vertex]], borders: list[set[vertex]]) -> list[set[vertex]]:
    vertices = []
    for bd, shape in zip(borders, shapes):
        vertex = set()
        for x, y in bd:
            if (x + 1, y) not in shape:
                vertex.add((x + 1, y))
                vertex.add((x + 1, y + 1))
            if (x - 1, y) not in shape:
                vertex.add((x, y))
                vertex.add((x, y + 1))
            if (x, y + 1) not in shape:
                vertex.add((x, y + 1))
                vertex.add((x + 1, y + 1))
            if (x, y - 1) not in shape:
                vertex.add((x, y))
                vertex.add((x + 1, y))
        vertices.append(vertex)
    return vertices

print("splitting...")
for n, (vt, sh) in enumerate(zip(vertices, shapes)):
    #print(f"splitting shape {n + 1} / {len(shapes)}...")
    unprocessed = vt.copy()
    ring = [r := {unprocessed.pop()}]

    while unprocessed:
        processed = set()
        for x, y in unprocessed:
            neighbors = set()

            if ((x, y) in sh) != ((x, y - 1) in sh):  # check border (x, y)::(x + 1, y)
                neighbors.add((x + 1, y))

            if ((x, y) in sh) != ((x - 1, y) in sh):  # check border (x, y)::(x, y + 1)
                neighbors.add((x, y + 1))

            if ((x - 1, y) in sh) != ((x - 1, y - 1) in sh):  # check border (x, y)::(x - 1, y)
                neighbors.add((x - 1, y))

            if ((x, y - 1) in sh) != ((x - 1, y - 1) in sh):  # check border (x, y)::(x, y - 1)
                neighbors.add((x, y - 1))

            if neighbors & r:
                r.add((x, y))
                processed.add((x, y))

        if not processed:
            ring.append(r := {unprocessed.pop()})  # start a new ring
        else:
            unprocessed -= processed

    ring.sort(key=lambda rg: min(rg, key=lambda cd: cd[0])[0])  # ring[0] = outer ring
    rcs = []

    for r in ring:  # reconstruct rings vertex order
        rx = r.copy()
        rc = [ls := rx.pop()]
        lls = None
        while rx:
            x, y = ls

            neighbors = set()
            distants = set()

            if (((x, y) in sh) != ((x, y - 1) in sh)) and (x + 1, y) in rx:  # check border (x, y)::(x + 1, y)
                neighbors.add((x + 1, y))
            else:
                distants.add((x + 1, y))

            if (((x, y) in sh) != ((x - 1, y) in sh)) and (x, y + 1) in rx:  # check border (x, y)::(x, y + 1)
                neighbors.add((x, y + 1))
            else:
                distants.add((x, y + 1))

            if (((x - 1, y) in sh) != ((x - 1, y - 1) in sh)) and (x - 1, y) in rx:  # check border (x, y)::(x - 1, y)
                neighbors.add((x - 1, y))
            else:
                distants.add((x - 1, y))

            if (((x, y - 1) in sh) != ((x - 1, y - 1) in sh)) and (x, y - 1) in rx:  # check border (x, y)::(x, y - 1)
                neighbors.add((x, y - 1))
            else:
                distants.add((x, y - 1))

            if not neighbors:
                raise Exception(f"Vertex not found:\n{ls=}\n{rx=}\n{rc=}\n{r=}")

            neighbors.discard(lls)  # always deny the last vertex

            if len(neighbors) >= 3:
                rx.add((x, y))  # we will reexplore this vertex later

            if len(neighbors) == 3:
                ox, oy = lls or distants.pop()  # origin
                vx = ox - x
                vy = oy - y

                lx = -vy  # leftward vector
                ly = vx

                nx = x + lx
                ny = y + ly

                cx = min(ox, nx)
                cy = min(oy, ny)

                if (cx, cy) in sh:
                    ls = nx, ny
                else:
                    ls = x - lx, y - ly
            else:
                for ls in neighbors:  # try to take a not explored vertex
                    if ls not in rc:
                        break

            lls = x, y
            rx.remove(ls)
            rc.append(ls)

        rc.append(rc[0])  # close the ring
        rcs.append(rc)

    rings.append(rcs)

print("reversing...")
for r in rings:
    policy = 1
    for rc in r:
        # check if not clockwise
        if policy * sum((rc[n + 1][0] - rc[n][0]) * (rc[n + 1][1] + rc[n][1]) for n in range(len(rc) - 1)) < 0:
            rc.reverse()
        policy = -1  # change the reverse policy for inner rings

print("linking...")
for r in rings:
    r = r.copy()
    line = r.pop(0)  # outer ring
    while r:
        inner = r.pop()
        for cn, (cx, cy) in enumerate(inner):  # find one of the rightmost vertex
            if not any(True for vx, vy in inner if vy == cy and vx > cx):
                break
        else:  # no rightmost vertex found
            raise Exception(f"Rightmost vertex not found:\n{inner=}")  # impossible

        hole_link = None
        dist = float("inf")
        for cn, (cx, cy) in enumerate(inner):
            for n, rg in enumerate([line] + r):  # check right-raycasting (cx, cy)
                for u, (x, y) in enumerate(rg):
                    if (x - cx) ** 2 + (y - cy) ** 2 < dist:
                        dist = (x - cx) ** 2 + (y - cy) ** 2
                        hole_link = (cn, n, u)

        cn, ridx, idx = hole_link
        rx = line if not ridx else r.pop(ridx - 1)
        assert idx < len(rx) - 1
        rx = rx[:idx + 1] + inner[cn:] + inner[1:cn + 1] + rx[idx:]

        if not ridx:
            line = rx
        else:
            r.append(rx)

    lines.append(line)


def simplify(ring):  # remove useless align vertices
    rn = []
    D = ring.pop(-1)  # open
    for n, (cx, cy) in enumerate(ring):
        px, py = ring[n - 1]
        nx, ny = ring[(n + 1) % len(ring)]
        if px == cx == nx or py == cy == ny:  # is align
            continue
        rn.append((cx, cy))
    ring.append(D)  # close
    if rn:
        rn.append(rn[0])  # close
    return rn

def simplify_cl(ring):  # remove useless align vertices
    it = iter(ring)
    rn = [ring[-1]]

    cx, cy = next(it)

    for nx, ny in it:
        px, py = rn[-1]

        vnx = nx - cx
        vny = ny - cy
        vpx = px - cx
        vpy = py - cy

        if vnx * vpy - vny * vpx != 0:  # is not align
            rn.append((cx, cy))

        cx, cy = nx, ny

    rn.append(rn.pop(0))  # rotate for consistency (optional)

    return rn


print("simplifying...")
for n, ln in enumerate(lines):
    lines[n] = simplify(ln)


def ear_clipping(vertices):
    triangles = []
    V = list(vertices)  # Make a copy of the vertices list
    assert V[0] == V[-1]  # Ensure the polygon is closed
    V.pop()  # open
    R = []
    strict = True

    DEBUG = False #(30, 240) in V

    while len(V) > 3:
        for i in range(len(V)):
            prev = V[(i - 1) % len(V)]
            curr = V[i]
            next = V[(i + 1) % len(V)]

            if isConvex(prev, curr, next):
                S = []
                ear = True
                for j in range(len(V)):
                    if j not in [(i - 1) % len(V), i, (i + 1) % len(V)]:
                        inside_or_border, not_border = isPointInTriangle(V[j], prev, curr, next)
                        if strict:
                            if inside_or_border and V[j] not in [prev, curr, next]:
                                ear = False
                                break
                        else:
                            if inside_or_border:
                                jprev = V[(j - 1) % len(V)]
                                jcurr = V[j]
                                jnext = V[(j + 1) % len(V)]

                                cross = [isSegmentCrossing(jcurr, jprev, prev, next),
                                         isSegmentCrossing(jcurr, jprev, curr, next),
                                         isSegmentCrossing(jcurr, jprev, curr, prev),
                                         isSegmentCrossing(jcurr, jnext, prev, next),
                                         isSegmentCrossing(jcurr, jnext, curr, next),
                                         isSegmentCrossing(jcurr, jnext, curr, prev),
                                         isSegmentCrossing(jprev, jnext, prev, next),
                                         isSegmentCrossing(jprev, jnext, curr, next),
                                         isSegmentCrossing(jprev, jnext, curr, prev)]
                                if DEBUG:
                                    print(f"{j=}, {jprev=}, {jcurr=}, {jnext=}, {prev=}, {curr=}, {next=}")
                                    plt.show()

                                    ax = plt.gca()
                                    ax.invert_yaxis()
                                    ax.set_aspect("equal")
                                    plot_triangulation(vertices, triangles, (0.9, .9, .9), before=True)
                                    plot_triangulation(V, [(prev, curr, next)], "blue", before=True, pcolor=(1., 0. ,1.))
                                    plot_triangulation([], [(jprev, jcurr, jnext)], "red")
                                    plt.show()
                                    print()
                                if not_border or any(cross):
                                    ear = False
                                    break
                                elif V[j] not in [prev, curr, next]:
                                    S.append(V[j])

                if ear:
                    triangles.append((prev, curr, next))
                    R.append(V[i])
                    S.reverse()
                    # print(f"RM {V[i]}, {S=}")
                    V[i:i + 1] = S
                    V = simplify_cl(V)
                    break
        else:
            if strict:
                print("unlock", len(V))
                strict = False
                continue
            #if (54, 160) in vertices:
            print(f"SL: {R=}, {V=}, \n{vertices=}")

            plt.show()

            ax = plt.gca()
            ax.invert_yaxis()
            ax.set_aspect("equal")
            n = len(vertices)
            vertices = list(reversed(vertices))
            colors = plt.cm.rainbow(np.linspace(0, 1, n))

            plot_triangulation([], triangles)
            for i in range(n):
                x1, y1 = vertices[i]
                x2, y2 = vertices[(i + 1) % n]
                plt.plot([x1, x2], [y1, y2], color="black")#colors[i])

            plt.scatter(*zip(*vertices), color='red', s=1)  # Plot vertices

            n = len(V)
            vertices = list(reversed(V))
            colors = plt.cm.rainbow(np.linspace(0, 1, n))

            plot_colored(vertices)
            #for i in range(n):
            #    x1, y1 = vertices[i]
            #    x2, y2 = vertices[(i + 1) % n]
            #    plt.plot([x1, x2], [y1, y2], color="black")

            plt.scatter(*zip(*vertices), color='black', s=1)  # Plot vertices

            plt.show()
            #plt.plot(*zip(*vertices))
            print("SL")
            raise
            break
        strict = True

    # Add the last remaining triangle
    if V:
        triangles.append((V[0], V[1], V[2]))
    return triangles


def plot_colored(vertices):
    n = len(vertices)
    colors = plt.cm.rainbow(np.linspace(0, 1, n))

    for i in range(n):
        x1, y1 = vertices[i]
        x2, y2 = vertices[(i + 1) % n]
        plt.plot([x1, x2], [y1, y2], color=colors[i])

    #plt.scatter(*zip(*vertices), color='black', s=1)  # Plot vertices


def isConvex(a, b, c):
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]) > 0


def isPointInTriangle(p, a, b, c):
    def sign(p1, p2, p3):
        return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1])

    d1 = sign(p, a, b)
    d2 = sign(p, b, c)
    d3 = sign(p, c, a)
    has_neg = (d1 < 0) or (d2 < 0) or (d3 < 0)
    has_pos = (d1 > 0) or (d2 > 0) or (d3 > 0)
    return not (has_neg and has_pos), d1 != 0 and d2 != 0 and d3 != 0


def isSegmentCrossing(a, b, c, d):

    ab = (a[0] - b[0], a[1] - b[1])
    cd = (c[0] - d[0], c[1] - d[1])

    if (ab[0] * cd[1] - ab[1] * cd[0]) == 0:  # check if segments are parallel
        return False

    X = c[0] - a[0]
    A = c[1] - a[1]

    Y = d[0] - c[0]
    B = d[1] - c[1]

    Z = b[1] - a[1]
    C = b[0] - a[0]

    kN = (C * A - Z * X)
    kD = (Y * Z - B * C)

    tN = X * kD + kN * Y
    tD = kD * C

    assert tN * kD * Z == tD * (A * kD + kN * B)

    # check 0 < kN/kD < 1 and 0 < tN/tD < 1
    if kD * kN <= 0: return False  # check k > 0
    if tD * tN <= 0: return False  # check t > 0

    if kN * kD >= kD * kD: return False  # check k < 1
    if tN * tD >= tD * tD: return False  # check t < 1

    return True


import matplotlib.patches as patches


def plot_triangulation(vertices, triangles, color="gray", pcolor=None, before=False):
    if vertices:
        vertices.append(vertices[0])

    if before:
        plt.plot(*zip(*vertices))

    for triangle in triangles:
        t = list(triangle)
        t.append(t[0])
        plt.plot(*zip(*t), color=color)

    if not before:
        plt.plot(*zip(*vertices))

    if vertices:
        vertices.pop(-1)

    if pcolor:
        plt.scatter(*zip(*vertices), s=.6, color=pcolor)


print("plotting...")
#for cl in shapes:
#    plt.scatter(*zip(*cl), s=1)

ax = plt.gca()
ax.invert_yaxis()
ax.set_aspect("equal")

#for cl in vertices:
#    plt.scatter(*zip(*cl), s=1, marker='s')

#for r in lines:
#    plt.plot(*zip(*r))

#for r in rings:
#    for n, rc in enumerate(r):
#        plot_colored(rc)

for n, r in enumerate(lines):
    #if n == 400: break
    print(f"plotting ring {n + 1} / {len(lines)}...")
    plot_triangulation(r, ear_clipping(list(reversed(r))))

print("showing...")
plt.show()

if __name__ == "__main__":
    pass
