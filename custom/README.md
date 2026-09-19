# QGC-Stealth custom build

A custom build of QGroundControl that applies the Stealth Ops skin from
`design/ui-skins/` and trims the app for one use case: manual FPV flying from a
small tablet, no goggles, no mission planning, no map tiles.

Derived from `custom-example/`. What it does today:

## Look

- Dark-theme palette override (`CustomPlugin::paletteOverride`) using the
  Stealth Ops colour table; dark (indoor) scheme is the default.
- App name `QGC-Stealth`, custom icon and vehicle icon.
- Stealth HUD (`res/Custom/Widgets/StealthHud.qml`): heading tape, ground
  speed and altitude tapes, artificial horizon with pitch ladder and roll
  scale, flight mode and vertical speed in the footer. Drawn with a `Canvas`
  from the active vehicle's facts, repainted at 10 Hz.
- Stealth telemetry bar (`res/Custom/Widgets/StealthTelemetryBar.qml`): six
  label-over-value cells in the monospace font.
- Stealth status board (`res/Custom/Widgets/StealthStatusBoard.qml`): go/no-go
  rows for GPS, battery (volts first), link latency, EKF, sensors and ready
  state, with square lamps and an ALL GO / CHECK verdict.

The HUD and telemetry bar replace the stock bottom-right row through
`src/FlyViewBottomRightRowLayout.qml`; the status board sits in the overridden
`src/FlyViewCustomLayer.qml`. The URL interceptor in `CustomPlugin.cc`
substitutes any stock QML file for a copy under the `/Custom/qml` prefix in
`custom.qrc`. The widgets live in the `Custom.Widgets` QML module.

Colours come from `QGCPalette` only, so the NVG or Neon palettes from
`design/ui-skins/README.md` apply unchanged.

## Typography

`res/fonts/` ships Barlow Condensed (UI text, Medium and SemiBold) and Share
Tech Mono (numbers), both under the SIL Open Font License (licence texts
alongside). `CustomPlugin::init` registers them and the plugin returns them
from `normalFontFamily()` / `fixedFontFamily()`, the two small hooks this
branch adds to `QGCCorePlugin` (read by `ScreenToolsController`). Korean
locales keep NanumGothic, as in stock QGC.

## FPV trim

- Fly View tool strip (`src/FlyViewToolStripActionList.qml`): checklist, land,
  return, pause and the extra-actions menu. No takeoff, 3D viewer or gripper.
- Toolbar indicators (`CustomFirmwarePlugin::toolIndicators`): telemetry link,
  RC link, battery and joystick only. GPS, Remote ID, gimbal, ESC and
  multi-vehicle indicators are removed.
- Battery indicator shows voltage by default.
- Video is the main window and the map picture-in-picture starts hidden
  (seeded once in `CustomPlugin::init`; the user's later choice is kept).
- Application Settings pages for Plan View, ADS-B, Maps, NTRIP/RTK, Remote ID
  and PX4 log transfer are advanced-mode only (`src/SettingsPagesModel.qml`,
  a copy of the generated page list with `pageVisible` gated). Guided-flight
  and mission items are removed from the Fly View settings page.
- Plan view, Analyze, firmware upgrade and sensor calibration are advanced-mode
  only. The Plan View, 3D viewer, ADS-B, NTRIP/RTK, Maps, Flight Map, Offline
  Maps and Remote ID settings groups are hidden until advanced mode is switched
  on (view menu, tap the version text), and hide again when it is switched off.
- No orbit/ROI guided actions, mission status bar, offline map tools or PX4 log
  transfer options; no multi-vehicle list.

## Link latency

`src/StealthLinkStats.cc` sends a MAVLink PING request to the active vehicle
once a second and times the reply; the result is the `StealthLink` QML
singleton (`latencyMs`, `latencyValid`) shown on the status board. PX4 and
ArduPilot answer pings; the built-in MockLink does not, so the preview shows
"no ping reply".

## Building

Exactly like stock QGC; CMake enables the custom build automatically when this
directory exists:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Use a Debug build to get the MockLink simulated vehicle for previews.
