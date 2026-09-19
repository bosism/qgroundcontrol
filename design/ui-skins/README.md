# QGroundControl UI skin concepts

Visual design exploration for a dark, defense-oriented QGroundControl skin.
Nothing in this folder is compiled into QGC. The mockups are static HTML pages
rendered with headless Chromium so that layout, palette and widget ideas can be
judged before any QML is written.

## Stock UI for comparison

The plain upstream Fly View and Plan View screenshots ship with the docs:

- `docs/assets/fly/fly_view_overview.jpg` (annotated Fly View)
- `docs/assets/plan/plan_view_overview.png` (Plan View)
- `custom-example/README.jpg` (the bundled custom-build example, which already
  swaps the palette and the instrument widget)

## Concepts

| Screenshot | Concept | Mockup source |
|------------|---------|---------------|
| `screenshots/stealth-ops.jpg` | **A. Stealth Ops** – charcoal panels, amber accent, olive "go" green. Mil-style HUD (heading tape, speed/altitude tapes, pitch ladder, flight-path vector), system status board with square go/no-go lamps, mission progress strip, EO/IR inset with reticle and MGRS target readout. | `mockups/stealth.html` |
| `screenshots/stealth-ops-nvg.jpg` | **A2. Stealth Ops, NVG variant** – same layout, monochrome phosphor-green palette for night/low-light use. Only CSS variables change. | `mockups/stealth-nvg.html` |
| `screenshots/stealth-ops-plan.jpg` | **A3. Stealth Ops, Plan View** – same skin applied to mission planning. Plan statistics strip (distance, time, max telemetry range, photos, batteries, minimum terrain clearance, fence check), validation callouts, mission item list with the selected survey item expanded inline (altitude, grid angle, spacing, trigger distance, altitude reference, camera, per-item stats), segment edit handles and callouts on the map, terrain profile with a 50 m planning floor and the offending leg highlighted, and a primary "upload to vehicle" action. | `mockups/stealth-plan.html` |
| `screenshots/neon-command.jpg` | **B. Neon Command** – sci-fi cyan/magenta on deep navy. Hexagonal tool-strip buttons, chamfered panels with corner brackets, ring gauges in the toolbar, glowing bar gauges, radar-style compass with attitude ball and sweep, hex-grid and scan-line overlays. | `mockups/neon.html` |

Both concepts keep the stock Fly View arrangement so the mapping to QGC is
direct: toolbar on top, tool strip left, camera/instrument column right,
telemetry values along the bottom, video inset bottom-left, compass/attitude
bottom-right.

## Re-rendering

```bash
cd design/ui-skins/mockups
node shoot.js stealth.html:../screenshots/stealth-ops.jpg \
              stealth-nvg.html:../screenshots/stealth-ops-nvg.jpg \
              stealth-plan.html:../screenshots/stealth-ops-plan.jpg \
              neon.html:../screenshots/neon-command.jpg
```

`shoot.js` needs the `playwright` npm package and a Chromium it can launch.
`fonts.css` pulls Barlow Condensed, Rajdhani, Orbitron and Share Tech Mono from
Google Fonts; offline the pages fall back to Liberation Sans and DejaVu Sans
Mono. `map.js` draws a procedural dark map (contours, grid, fence, mission,
tracks) so no tiles or network are needed.

## How this maps onto QGC

QGC skins are done as a custom build (`custom-example/`), which gives three
hooks. None of them require forking core files.

### 1. Palette (`QGCCorePlugin::paletteOverride`)

Every QML control reads colours from `QGCPalette` (`src/QmlControls/QGCPalette.cc`).
A custom plugin overrides them per colour name; the example in
`custom-example/src/CustomPlugin.cc` shows the pattern. Proposed dark-theme
values (light theme can mirror the stock values or be disabled):

| Palette name | Stock dark | Stealth Ops | NVG | Neon Command |
|--------------|-----------|-------------|-----|--------------|
| `window` | `#222222` | `#0a0c0e` | `#061008` | `#03070d` |
| `windowShade` | `#333333` | `#121517` | `#0a140c` | `#071324` |
| `windowShadeDark` | `#282828` | `#0d1113` | `#040b06` | `#040b16` |
| `windowShadeLight` | `#626262` | `#2b3239` | `#1f3a26` | `#0f2a44` |
| `text` | `#ffffff` | `#d9dde1` | `#b8f2c2` | `#d6eef8` |
| `text` (disabled) | `#707070` | `#7d868e` | `#4f8a5c` | `#5f8fa8` |
| `warningText` | `#f85761` | `#ff4b4b` | `#ffffff` | `#ff3b5c` |
| `button` | `#626270` | `#15191c` | `#0d1a10` | `#0a1a2e` |
| `buttonBorder` | `#adadb8` | `#4a545c` | `#2f5a3a` | `#1fd1f9` |
| `buttonText` | `#ffffff` | `#d9dde1` | `#b8f2c2` | `#d6eef8` |
| `buttonHighlight` | `#3A9BDC` | `#f0b429` | `#79ff8a` | `#1fd1f9` |
| `buttonHighlightText` | `#ffffff` | `#0a0c0e` | `#061008` | `#03070d` |
| `primaryButton` | `#8cb3be` | `#f0b429` | `#79ff8a` | `#1fd1f9` |
| `primaryButtonText` | `#000000` | `#0a0c0e` | `#061008` | `#03070d` |
| `groupBorder` | `#707070` | `#2b3239` | `#1f3a26` | `#1fd1f9` (35 % alpha) |
| `toolbarBackground` | transparent | `#e60f1215` | `#eb060c08` | `#cc050e1a` |
| `toolStripHoverColor` | `#585d83` | `#f0b429` | `#79ff8a` | `#1fd1f9` |
| `colorGreen` | `#00e04b` | `#8bc34a` | `#79ff8a` | `#35f0a8` |
| `colorOrange` | `#de8500` | `#ff8a3d` | `#d9ff5c` | `#ffb020` |
| `colorRed` | `#f32836` | `#ff4b4b` | `#ffffff` | `#ff3b5c` |
| `colorBlue` | `#536dff` | `#f0b429` | `#79ff8a` | `#1fd1f9` |
| `mapButton` | `#000000` | `#0f1215` | `#060c08` | `#050e1a` |
| `mapButtonHighlight` | `#be781c` | `#f0b429` | `#79ff8a` | `#1fd1f9` |
| `mapIndicator` | `#be781c` | `#8bc34a` | `#79ff8a` | `#1fd1f9` |
| `mapMissionTrajectory` | `#be781c` | `#8bc34a` | `#79ff8a` | `#1fd1f9` |
| `alertBackground` | `#eecc44` | `#3a2a08` | `#1a3a1c` | `#3a1030` |
| `alertBorder` | `#808080` | `#f0b429` | `#79ff8a` | `#ff3ea5` |
| `alertText` | `#000000` | `#f0b429` | `#b8f2c2` | `#ffb4dc` |
| `brandingPurple` / `brandingBlue` | purple/blue | `#f0b429` | `#79ff8a` | `#1fd1f9` |

The palette only recolours existing controls. Shape changes (chamfers, corner
brackets, hexagonal buttons, glow) and the new widgets need QML.

### 2. Widgets (QML overrides via the custom build's resource prefix)

Stock file to replace or wrap, and what the mockup puts there:

| Mockup element | Stock QML | Notes |
|----------------|-----------|-------|
| Toolbar | `src/UI/toolbar/FlyViewToolBar.qml`, `FlyViewToolBarIndicators.qml`, `MainStatusIndicator.qml` | Stealth: stacked label/value indicators in a mono font. Neon: ring gauges (`Canvas` arcs) per indicator. Custom-example already overrides `CustomToolBarButton.qml`. |
| Tool strip | `src/FlyView/FlyViewToolStrip.qml`, `FlyViewToolStripActionList.qml` | Stealth: square cells with thin dividers. Neon: hexagonal `Shape` items. |
| HUD (A) / radar compass (B) | `src/FlyView/FlyViewInstrumentPanel.qml`, `src/FlightMap/Widgets/IntegratedCompassAttitude.qml`, `QGCAttitudeWidget.qml`, `QGCCompassWidget.qml` | Custom-example replaces this via `CustomAttitudeWidget.qml` / `CustomArtificialHorizon.qml`; extend that with heading/speed/altitude tapes (A) or radar rings + sweep (B). |
| System status board / bar gauges | `src/FlyView/FlyViewInstrumentPanel.qml` (health page) | Lives in the same instrument column; data comes from `Vehicle` facts already used by the health page. |
| Telemetry values bar | `src/FlyView/TelemetryValuesBar.qml` | Already fact-driven; restyle with label above value, mono digits, per-cell borders/chamfers. |
| Mission progress strip | new, sits in `FlyViewWidgetLayer.qml` | Reads `missionController` current item and item count. |
| Video inset with reticle | `src/FlyView/FlyViewVideo.qml` overlay layer | Reticle, gimbal pitch and LRF text are overlay items on top of the existing video output. |
| Messages / alerts | `src/FlyView/VehicleWarnings.qml` and the message indicator | Stealth: dense log lines with severity bar. Neon: skewed alert chips. |
| Plan toolbar and stats strip | `src/PlanView/PlanEditToolbar.qml`, `PlanToolBarIndicators.qml`, `MissionStats.qml` | Stats become label-over-value cells; upload is the highlighted action. |
| Plan tool strip | `src/PlanView/PlanView.qml` (tool strip actions) | Same square-cell strip as the Fly View; active tool highlighted. |
| Mission item list and inline editor | `src/PlanView/PlanViewRightPanel.qml`, `PlanTreeView.qml`, `MissionItemEditor.qml`, `SimpleItemEditor.qml`, `SurveyItemEditor.qml`, `TransectStyleComplexItemStats.qml` | Diamond index badges, mono sub-lines, selected item expands into a two-column field grid with segmented altitude-reference control. |
| Validation callouts | `src/PlanView/MissionItemStatus.qml`, `PlanView.qml` | Severity-coded lines top-left of the map, mirrors the Fly View message panel. |
| Terrain profile | `src/PlanView/TerrainStatus.qml` | Mission line over terrain fill, dashed planning floor, legs below the floor drawn in the warning colour. |
| Scan-line / hex overlay (B) | `src/FlyView/FlyViewCustomLayer.qml` | Full-screen `Rectangle` with `ShaderEffect` or tiled image, `enabled: false` by default for readability. |

Rules from `AGENTS.md` still apply: sizes from `ScreenTools`, colours from
`QGCPalette` (add new names such as `accent`, `hudTape`, `glow` via
`DECLARE_QGC_COLOR` rather than hard-coding), fonts via `ScreenTools`.

### 3. Fonts

QGC ships its own fonts under `resources/fonts` and exposes them through
`ScreenTools`. Barlow Condensed (UI), Share Tech Mono (numbers) and Orbitron
(headings, Neon only) are all OFL-licensed and can be bundled the same way.

## Follow-up prompt ideas

Concept variants:

- "Amber-only cockpit skin: single hue, no green/red, with brightness-coded severity, for pilots wearing tinted visors."
- "Cold-weather / arctic variant: high-contrast white-on-black with cyan accent, large touch targets for gloved use."
- "Naval / maritime skin for the Sub and Rover vehicle types: depth tape instead of altitude, heading in true and magnetic, sonar-style range rings."
- "Minimal 'silent running' mode: everything but the map, one status line and the kill switch fades out until a warning fires."
- "Tablet portrait layout of the Stealth skin for a hand-held GCS."

New widgets:

- "Design a threat/contact panel that lists ADS-B and friendly vehicles by bearing, range and closure rate, with a select-to-slew action for the gimbal."
- "Design a link-budget widget: RSSI, packet loss, latency history sparkline, predicted range at current loss rate."
- "Design an endurance/RTL widget: battery-remaining vs distance-to-home with a 'bingo' line and a wind-corrected return estimate."
- "Design a multi-vehicle swarm bar: one compact card per vehicle (mode, battery, link, next WP), tap to make active."
- "Design a pre-flight checklist screen in the Stealth style with a go/no-go summary block."

Other views in the same skin:

- "Mock up the Plan View in the Neon Command skin: survey polygon editor, waypoint list, terrain profile strip."
- "Mock up the Plan View fence and rally tabs in the Stealth skin, with a polygon vertex editor and rally point list."
- "Mock up the Vehicle Setup and Application Settings pages in the Stealth skin."
- "Mock up the toolbar indicator pop-ups (GPS, battery, RC) as chamfered dropdowns."

Implementation steps once a direction is chosen:

- "Create a `custom/` build from `custom-example/` that applies the Stealth Ops palette table via `paletteOverride`, and add a screenshot from the running app."
- "Replace `CustomAttitudeWidget.qml` in the custom build with the tape-based HUD from the Stealth mockup, driven by `Vehicle` facts."
- "Add `accent`, `panelBorder` and `glow` colours to `QGCPalette` and use them in the custom toolbar."
