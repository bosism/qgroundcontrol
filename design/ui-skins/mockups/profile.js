// Terrain profile strip shared by the Plan View mockups.
(function () {
  function drawTerrainProfile(svg, pal, opts) {
    opts = opts || {};
    const W = 1300, H = +svg.getAttribute('viewBox').split(' ')[3] || 156;
    const top = 14, bottom = H - 20;
    const pts = [];
    for (let x = 0; x <= W; x += 10) {
      const t = x / W;
      pts.push([x, 40 + 30 * Math.sin(t * 6.1) + 18 * Math.sin(t * 15 + 1.2) + 10 * Math.sin(t * 31)]);
    }
    const y = h => bottom - (h / 140) * (bottom - top);
    const terrain = 'M0 ' + bottom + ' ' + pts.map(p => `L${p[0]} ${y(p[1])}`).join(' ') + ` L${W} ${bottom} Z`;
    const mission = pts.map(p => {
      const t = p[0] / W;
      const agl = t < 0.04 ? 60 * (t / 0.04) : (t > 0.9 ? 80 : 60);
      return [p[0], y(p[1] + agl - (t > 0.55 && t < 0.62 ? 18 : 0))];
    });
    const mpath = 'M' + mission.map(p => `${p[0]} ${p[1]}`).join(' L');
    const floor = 'M' + pts.map(p => `${p[0]} ${y(p[1] + 50)}`).join(' L');
    const dip = mission.filter(p => p[0] / W > 0.55 && p[0] / W < 0.62);
    const dpath = 'M' + dip.map(p => `${p[0]} ${p[1]}`).join(' L');
    const wpx = [0.06, 0.16, 0.24, 0.3, 0.72, 0.8, 0.9];
    const sel = opts.selected === undefined ? 3 : opts.selected;
    const fs = opts.fontSize || 11;
    svg.innerHTML = `
      <g stroke="${pal.grid}"><line x1="0" y1="${y(0)}" x2="${W}" y2="${y(0)}"/><line x1="0" y1="${y(50)}" x2="${W}" y2="${y(50)}"/><line x1="0" y1="${y(100)}" x2="${W}" y2="${y(100)}"/></g>
      <path d="${terrain}" fill="${pal.terrainFill}" stroke="${pal.terrain}" stroke-width="1"/>
      <path d="${floor}" fill="none" stroke="${pal.floor}" stroke-dasharray="4 4"/>
      ${pal.missionGlow ? `<path d="${mpath}" fill="none" stroke="${pal.missionGlow}" stroke-width="6"/>` : ''}
      <path d="${mpath}" fill="none" stroke="${pal.mission}" stroke-width="2"/>
      <path d="${dpath}" fill="none" stroke="${pal.warn}" stroke-width="3"/>
      ${wpx.map((t, i) => { const p = mission[Math.round(t * pts.length - 1)]; return `<g transform="translate(${p[0]},${p[1]}) rotate(45)"><rect x="-5" y="-5" width="10" height="10" fill="${i === sel ? pal.wpSel : pal.wp}" stroke="${pal.bg}"/></g><text x="${p[0]}" y="${p[1] - 12}" fill="${pal.wpText}" font-family="${pal.font}" font-size="${fs}" text-anchor="middle">${i + 1}</text>`; }).join('')}
      <g fill="${pal.label}" font-family="${pal.font}" font-size="${fs}"><text x="6" y="${y(100) - 3}">100 m</text><text x="6" y="${y(50) - 3}">50 m</text><text x="6" y="${y(0) - 3}">0 m</text></g>
      <g fill="${pal.label}" font-family="${pal.font}" font-size="${fs}" text-anchor="middle">${[1, 2, 3, 4].map(k => `<text x="${k * W / 4.2}" y="${H - 2}">${k} km</text>`).join('')}</g>
      <text x="${W * 0.585}" y="${y(pts[Math.round(0.585 * pts.length)][1] + 42) - 16}" fill="${pal.warn}" font-family="${pal.font}" font-size="${fs}" text-anchor="middle">42 m</text>
    `;
  }
  window.drawTerrainProfile = drawTerrainProfile;
})();
