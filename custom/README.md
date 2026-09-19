# QGC "Stealth Ops" custom build

A custom build of QGroundControl that applies the Stealth Ops skin from
`design/ui-skins/`. It is derived from `custom-example/` and currently does the
minimum needed to see the skin in a real build:

- dark-theme palette override (`CustomPlugin::paletteOverride`) using the
  Stealth Ops colour table
- dark (indoor) colour scheme as the default
- app name `QGC-Stealth`, custom icon and vehicle icon
- PX4-only firmware set, as in the example

- Stealth HUD (`res/Custom/Widgets/StealthHud.qml`): heading tape, ground
  speed and altitude tapes, artificial horizon with pitch ladder and roll
  scale, flight mode and vertical speed in the footer. Drawn with a `Canvas`
  from the active vehicle's facts, repainted at 10 Hz.
- Stealth telemetry bar (`res/Custom/Widgets/StealthTelemetryBar.qml`): six
  label-over-value cells (altitude, ground speed, vertical speed, distance to
  home, heading, flight time) in the monospace font.

Both replace the stock bottom-right row of the Fly View through
`src/FlyViewBottomRightRowLayout.qml`, which the URL interceptor in
`CustomPlugin.cc` substitutes for the stock file (any stock QML file can be
overridden the same way by placing a copy under the `/Custom/qml` resource
prefix in `custom.qrc`). The widgets live in the `Custom.Widgets` QML module
built by `CMakeLists.txt`.

Colours come from `QGCPalette` only, so the NVG or Neon palettes from
`design/ui-skins/README.md` apply to these widgets unchanged.

Build exactly like stock QGC; CMake enables the custom build automatically when
this directory exists:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```
