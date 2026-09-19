// Render each mockup HTML page to a PNG with the bundled Chromium.
const { chromium } = require('playwright');
const path = require('path');

(async () => {
  const pages = process.argv.slice(2);
  const browser = await chromium.launch();
  for (const p of pages) {
    const [file, out, w, h] = p.split(':');
    const page = await browser.newPage({ viewport: { width: +w || 1920, height: +h || 1080 }, deviceScaleFactor: 1 });
    await page.goto('file://' + path.resolve(file));
    await page.evaluate(() => document.fonts.ready);
    await page.waitForTimeout(300);
    await page.screenshot(out.endsWith('.jpg') ? { path: out, type: 'jpeg', quality: 90 } : { path: out });
    console.log('wrote', out);
    await page.close();
  }
  await browser.close();
})();
