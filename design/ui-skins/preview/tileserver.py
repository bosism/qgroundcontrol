#!/usr/bin/env python3
"""Tiny XYZ tile server that generates dark procedural terrain tiles.

Used only for screenshots in a sandbox without internet access, so QGC's
"Custom URL" map provider has something to draw.
"""
import math
import io
from http.server import BaseHTTPRequestHandler, HTTPServer
from PIL import Image, ImageDraw

BG = (13, 17, 19)
HI = (30, 40, 40)
CONTOUR = (44, 56, 52)
GRID = (28, 36, 40)
ROAD = (58, 64, 68)
WATER = (24, 44, 56)


def elev(wx, wy):
    return (0.5 + 0.5 * math.sin(wx * 0.9) * math.cos(wy * 1.1)
            + 0.25 * math.sin(wx * 2.3 + wy * 1.7)
            + 0.12 * math.sin(wx * 5.1 - wy * 3.3)
            + 0.06 * math.sin(wx * 11 + wy * 7))


def tile(z, x, y):
    n = 2 ** z
    small = 64
    img = Image.new('RGB', (small, small), BG)
    px = img.load()
    # world units chosen so that terrain features span a few tiles at z=17
    scale = 2 ** (z - 17) * 1.4
    for j in range(small):
        for i in range(small):
            wx = (x + i / small) / scale if scale else 0
            wy = (y + j / small) / scale if scale else 0
            e = elev(wx, wy)
            t = max(0.0, min(1.0, (e + 0.4) / 1.6))
            c = tuple(int(BG[k] + (HI[k] - BG[k]) * t) for k in range(3))
            band = int(e * 6)
            # contour where the band changes against the neighbour
            e2 = elev(wx + 1.0 / small / scale, wy + 1.0 / small / scale)
            if int(e2 * 6) != band:
                c = CONTOUR
            if e < -0.35:
                c = WATER
            px[i, j] = c
    img = img.resize((256, 256), Image.BILINEAR)
    d = ImageDraw.Draw(img)
    # faint grid on tile edges from z14 upward, plus a diagonal "road" per tile
    if z >= 13:
        d.line([(0, 0), (255, 0)], fill=GRID)
        d.line([(0, 0), (0, 255)], fill=GRID)
    # long straight roads: world lines that continue across tiles
    if z >= 13:
        for k in range(-2, 3):
            # road y = 0.37 x + k*7 tiles (in tile units at this zoom)
            yy0 = 0.37 * x + k * 7 * (2 ** (z - 17)) * 3
            for yy in (yy0,):
                py0 = (yy - y) * 256
                py1 = (yy + 0.37 - y) * 256
                if -300 < py0 < 600 or -300 < py1 < 600:
                    d.line([(0, py0), (255, py1)], fill=ROAD, width=3)
        for k in range(-2, 3):
            xx0 = 0.2 * y + k * 5 * (2 ** (z - 17)) * 3 + 1.3
            px0 = (xx0 - x) * 256
            px1 = (xx0 + 0.2 - x) * 256
            if -300 < px0 < 600 or -300 < px1 < 600:
                d.line([(px0, 0), (px1, 255)], fill=ROAD, width=2)
    buf = io.BytesIO()
    img.save(buf, 'PNG')
    return buf.getvalue()


class H(BaseHTTPRequestHandler):
    def do_GET(self):
        parts = self.path.strip('/').split('?')[0].split('/')
        try:
            z, x, y = int(parts[-3]), int(parts[-2]), int(parts[-1].split('.')[0])
            data = tile(z, x, y)
        except Exception:
            self.send_response(404)
            self.end_headers()
            return
        self.send_response(200)
        self.send_header('Content-Type', 'image/png')
        self.send_header('Content-Length', str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def log_message(self, *a):
        pass


if __name__ == '__main__':
    HTTPServer(('127.0.0.1', 8765), H).serve_forever()
