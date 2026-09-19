# QGC "Stealth Ops" custom build

A custom build of QGroundControl that applies the Stealth Ops skin from
`design/ui-skins/`. It is derived from `custom-example/` and currently does the
minimum needed to see the skin in a real build:

- dark-theme palette override (`CustomPlugin::paletteOverride`) using the
  Stealth Ops colour table
- dark (indoor) colour scheme as the default
- app name `QGC-Stealth`, custom icon and vehicle icon
- PX4-only firmware set, as in the example

The example's demo instrument widget and toolbar overrides were removed so the
stock layout shows with the new palette. Widget replacements (HUD tapes, status
board, telemetry bar) come next and go through the URL interceptor in
`CustomPlugin.cc`, which swaps any stock QML file for a copy under the
`/Custom/qml` resource prefix.

Build exactly like stock QGC; CMake enables the custom build automatically when
this directory exists:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```
