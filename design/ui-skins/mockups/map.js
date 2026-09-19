// Synthetic dark tactical map used as the background of the skin mockups.
// Everything is drawn procedurally so the mockups need no network or map tiles.
(function () {
  function rng(seed) {
    let s = seed >>> 0;
    return function () {
      s = (s * 1664525 + 1013904223) >>> 0;
      return s / 4294967296;
    };
  }

  function drawMap(canvas, pal, opts) {
    opts = opts || {};
    const W = canvas.width, H = canvas.height;
    const ctx = canvas.getContext('2d');
    const rand = rng(opts.seed || 7);

    // base
    ctx.fillStyle = pal.bg;
    ctx.fillRect(0, 0, W, H);

    // terrain shading blobs
    for (let i = 0; i < 26; i++) {
      const x = rand() * W, y = rand() * H, r = 160 + rand() * 420;
      const g = ctx.createRadialGradient(x, y, 0, x, y, r);
      g.addColorStop(0, pal.terrainHi);
      g.addColorStop(1, 'rgba(0,0,0,0)');
      ctx.fillStyle = g;
      ctx.fillRect(x - r, y - r, r * 2, r * 2);
    }

    // contour lines around hills
    ctx.lineWidth = 1;
    ctx.strokeStyle = pal.contour;
    const hills = [];
    for (let i = 0; i < 9; i++) hills.push({ x: rand() * W, y: rand() * H, r: 90 + rand() * 260, ph: rand() * 6.28 });
    hills.forEach(h => {
      for (let k = 1; k <= 7; k++) {
        const rr = h.r * k / 7;
        ctx.beginPath();
        for (let a = 0; a <= 6.3; a += 0.05) {
          const wob = 1 + 0.12 * Math.sin(3 * a + h.ph) + 0.07 * Math.sin(7 * a + k) + 0.04 * Math.sin(11 * a);
          const px = h.x + Math.cos(a) * rr * wob * 1.35;
          const py = h.y + Math.sin(a) * rr * wob;
          if (a === 0) ctx.moveTo(px, py); else ctx.lineTo(px, py);
        }
        ctx.closePath();
        ctx.globalAlpha = k === 7 ? 0.9 : 0.55;
        ctx.stroke();
      }
    });
    ctx.globalAlpha = 1;

    // river
    ctx.strokeStyle = pal.water;
    ctx.lineWidth = 14;
    ctx.lineCap = 'round';
    ctx.beginPath();
    let rx = -20, ry = H * 0.62;
    ctx.moveTo(rx, ry);
    while (rx < W + 40) {
      rx += 60;
      ry += Math.sin(rx / 140) * 38 + (rand() - 0.5) * 30;
      ctx.lineTo(rx, ry);
    }
    ctx.stroke();

    // roads
    ctx.lineWidth = 3;
    ctx.strokeStyle = pal.road;
    const roads = [
      [[0, H * 0.18], [W * 0.3, H * 0.28], [W * 0.55, H * 0.22], [W, H * 0.34]],
      [[W * 0.42, 0], [W * 0.46, H * 0.4], [W * 0.52, H * 0.75], [W * 0.5, H]],
      [[W * 0.7, 0], [W * 0.78, H * 0.5], [W * 0.95, H]],
      [[0, H * 0.85], [W * 0.25, H * 0.8], [W * 0.5, H * 0.9]],
    ];
    roads.forEach(r => {
      ctx.beginPath();
      r.forEach((p, i) => i ? ctx.lineTo(p[0], p[1]) : ctx.moveTo(p[0], p[1]));
      ctx.stroke();
    });
    // minor roads
    ctx.lineWidth = 1.2;
    ctx.globalAlpha = 0.6;
    for (let i = 0; i < 14; i++) {
      ctx.beginPath();
      const x0 = rand() * W, y0 = rand() * H;
      ctx.moveTo(x0, y0);
      ctx.lineTo(x0 + (rand() - 0.5) * 500, y0 + (rand() - 0.5) * 500);
      ctx.stroke();
    }
    ctx.globalAlpha = 1;

    // buildings
    ctx.fillStyle = pal.building;
    for (let i = 0; i < 90; i++) {
      const bx = rand() * W, by = rand() * H;
      if (Math.abs(by - (H * 0.62 + Math.sin(bx / 140) * 38)) < 40) continue;
      ctx.fillRect(bx, by, 6 + rand() * 22, 6 + rand() * 18);
    }

    // grid
    ctx.strokeStyle = pal.grid;
    ctx.lineWidth = 1;
    const step = opts.grid || 200;
    ctx.font = `12px ${pal.font}`;
    ctx.fillStyle = pal.gridText;
    for (let x = step; x < W; x += step) {
      ctx.beginPath(); ctx.moveTo(x + 0.5, 0); ctx.lineTo(x + 0.5, H); ctx.stroke();
      ctx.fillText(String(48 + x / step).padStart(2, '0'), x + 4, H - 6);
    }
    for (let y = step; y < H; y += step) {
      ctx.beginPath(); ctx.moveTo(0, y + 0.5); ctx.lineTo(W, y + 0.5); ctx.stroke();
      ctx.fillText(String(21 + y / step).padStart(2, '0'), 6, y - 4);
    }

    // geofence
    if (opts.fence !== false) {
      ctx.setLineDash([10, 8]);
      ctx.strokeStyle = pal.fence;
      ctx.lineWidth = 2;
      ctx.beginPath();
      const f = [[0.22, 0.2], [0.72, 0.16], [0.86, 0.5], [0.7, 0.9], [0.3, 0.86], [0.16, 0.55]];
      f.forEach((p, i) => i ? ctx.lineTo(p[0] * W, p[1] * H) : ctx.moveTo(p[0] * W, p[1] * H));
      ctx.closePath();
      ctx.stroke();
      ctx.setLineDash([]);
    }

    // mission
    const wps = opts.waypoints || [[0.34, 0.66], [0.36, 0.42], [0.47, 0.33], [0.6, 0.3], [0.68, 0.45], [0.66, 0.66], [0.55, 0.74]];
    const pts = wps.map(p => [p[0] * W, p[1] * H]);
    ctx.strokeStyle = pal.mission;
    ctx.lineWidth = 2;
    ctx.beginPath();
    pts.forEach((p, i) => i ? ctx.lineTo(p[0], p[1]) : ctx.moveTo(p[0], p[1]));
    ctx.stroke();

    // survey lines inside polygon between wp3..wp5
    ctx.strokeStyle = pal.survey;
    ctx.lineWidth = 1;
    ctx.globalAlpha = 0.7;
    for (let i = 0; i < 9; i++) {
      const t = i / 8;
      ctx.beginPath();
      ctx.moveTo(pts[2][0] + (pts[4][0] - pts[2][0]) * t + 20, pts[2][1] + (pts[4][1] - pts[2][1]) * t);
      ctx.lineTo(pts[3][0] + (pts[5][0] - pts[3][0]) * t - 20, pts[3][1] + (pts[5][1] - pts[3][1]) * t);
      ctx.stroke();
    }
    ctx.globalAlpha = 1;

    // waypoint markers
    ctx.font = `bold 12px ${pal.font}`;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    pts.forEach((p, i) => {
      ctx.save();
      ctx.translate(p[0], p[1]);
      ctx.rotate(Math.PI / 4);
      ctx.fillStyle = i === (opts.currentWp || 3) ? pal.missionActive : pal.mission;
      ctx.fillRect(-9, -9, 18, 18);
      ctx.strokeStyle = pal.bg;
      ctx.lineWidth = 2;
      ctx.strokeRect(-9, -9, 18, 18);
      ctx.restore();
      ctx.fillStyle = pal.bg;
      ctx.fillText(String(i + 1), p[0], p[1] + 1);
    });

    // home
    const home = opts.home || [0.34 * W, 0.66 * H + 46];
    ctx.strokeStyle = pal.home;
    ctx.lineWidth = 2;
    ctx.beginPath(); ctx.arc(home[0], home[1], 11, 0, 6.3); ctx.stroke();
    ctx.fillStyle = pal.home;
    ctx.font = `bold 12px ${pal.font}`;
    ctx.fillText('H', home[0], home[1] + 1);

    // trail
    const veh = opts.vehicle || { x: 0.52 * W, y: 0.31 * H, hdg: 78 };
    ctx.strokeStyle = pal.trail;
    ctx.lineWidth = 2;
    ctx.setLineDash([2, 4]);
    ctx.beginPath();
    ctx.moveTo(home[0], home[1]);
    ctx.lineTo(pts[0][0], pts[0][1]);
    ctx.lineTo(pts[1][0], pts[1][1]);
    ctx.lineTo(pts[2][0], pts[2][1]);
    ctx.lineTo(veh.x, veh.y);
    ctx.stroke();
    ctx.setLineDash([]);

    // range rings around vehicle
    ctx.strokeStyle = pal.ring;
    ctx.lineWidth = 1;
    [80, 160, 240].forEach(r => { ctx.beginPath(); ctx.arc(veh.x, veh.y, r, 0, 6.3); ctx.stroke(); });
    ctx.fillStyle = pal.gridText;
    ctx.font = `11px ${pal.font}`;
    ctx.textAlign = 'left';
    ctx.fillText('100 m', veh.x + 84, veh.y - 4);
    ctx.fillText('200 m', veh.x + 164, veh.y - 4);
    ctx.fillText('300 m', veh.x + 244, veh.y - 4);

    // heading line
    const hr = veh.hdg * Math.PI / 180;
    ctx.strokeStyle = pal.vehicle;
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    ctx.moveTo(veh.x, veh.y);
    ctx.lineTo(veh.x + Math.sin(hr) * 120, veh.y - Math.cos(hr) * 120);
    ctx.stroke();

    // vehicle chevron
    ctx.save();
    ctx.translate(veh.x, veh.y);
    ctx.rotate(hr);
    ctx.fillStyle = pal.vehicle;
    ctx.strokeStyle = pal.bg;
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(0, -18); ctx.lineTo(13, 14); ctx.lineTo(0, 7); ctx.lineTo(-13, 14); ctx.closePath();
    ctx.fill(); ctx.stroke();
    ctx.restore();

    // second vehicle / friendly + unknown ADS-B track
    const others = opts.others || [
      { x: 0.78 * W, y: 0.62 * H, hdg: 300, kind: 'friend', label: 'UAS-02' },
      { x: 0.2 * W, y: 0.36 * H, hdg: 130, kind: 'unknown', label: 'ADSB 4C3A' },
    ];
    others.forEach(o => {
      ctx.save();
      ctx.translate(o.x, o.y);
      ctx.strokeStyle = o.kind === 'friend' ? pal.friend : pal.unknown;
      ctx.lineWidth = 2;
      if (o.kind === 'friend') {
        ctx.beginPath(); ctx.arc(0, 0, 10, Math.PI, 0); ctx.stroke();
        ctx.beginPath(); ctx.moveTo(-10, 0); ctx.lineTo(10, 0); ctx.stroke();
      } else {
        ctx.beginPath(); ctx.moveTo(0, -12); ctx.lineTo(12, 0); ctx.lineTo(0, 12); ctx.lineTo(-12, 0); ctx.closePath(); ctx.stroke();
      }
      const a = o.hdg * Math.PI / 180;
      ctx.beginPath(); ctx.moveTo(0, 0); ctx.lineTo(Math.sin(a) * 26, -Math.cos(a) * 26); ctx.stroke();
      ctx.fillStyle = o.kind === 'friend' ? pal.friend : pal.unknown;
      ctx.font = `11px ${pal.font}`;
      ctx.textAlign = 'left';
      ctx.fillText(o.label, 16, -10);
      ctx.restore();
    });
  }

  window.drawTacticalMap = drawMap;
})();
