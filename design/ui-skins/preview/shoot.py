#!/usr/bin/env python3
"""Launch the QGC-Stealth build on a virtual X display and capture screenshots.

usage: shoot.py NAME WIDTH HEIGHT SCALE FAKEMOBILE(0/1) [plan]

Environment: QGC_BIN (built binary), QGC_QT_PREFIX (Qt install used to build it),
QGC_PREVIEW_ROOT (where shots/ is written), PLAN_CLICK / PLAN_ITEM (x,y of the
view-selector logo and the Plan entry, in screen pixels), CLICKS (a ; separated
list of x,y,delay,shotname steps run after the Fly view shot).
Requires Xvfb, xdotool, ImageMagick (import), Pillow, and an unprivileged user
"qgc" because QGC refuses to run as root.
"""
import os
import subprocess
import sys
import time
import pathlib
import shutil

S = pathlib.Path(os.environ.get('QGC_PREVIEW_ROOT', '/tmp/qgc-preview'))
Q = pathlib.Path(os.environ.get('QGC_QT_PREFIX', str(S / 'conda/root/envs/qt')))
BIN = pathlib.Path(os.environ.get('QGC_BIN', str(S / 'build/Debug/QGC-Stealth')))
OUT = S / 'shots'
DISPLAY = ':99'

name, W, H, scale, fake = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4], sys.argv[5] == '1'
want_plan = 'plan' in sys.argv[6:]


def seed_settings():
    cfg = pathlib.Path('/home/qgc/.config/QGroundControl')
    cfg.mkdir(parents=True, exist_ok=True)
    for old in cfg.glob('QGC-Stealth*'):
        if old.is_dir():
            shutil.rmtree(old)
        else:
            old.unlink()
    ini = '''[General]
customURL=http://127.0.0.1:8765/{z}/{x}/{y}.png
firstRunPromptIdsShown=3
indoorPalette=1
telemetrySave=false
checkInternet=false

[MainWindowState]
x=0
y=0
width=%d
height=%d
visibility=2

[FlightMap]
mapProvider=CustomURL
mapType=Custom

[LinkConfigurations]
count=1
Link0\\name=MockLink PX4
Link0\\type=4
Link0\\auto=true
Link0\\high_latency=false
Link0\\FirmwareType=12
Link0\\VehicleType=2
Link0\\SendStatusText=false
Link0\\EnableCamera=false
Link0\\EnableGimbal=false
Link0\\IncrementVehicleId=true
Link0\\FailureMode=0
'''
    ini = ini % (int(W / float(scale)), int(H / float(scale)))
    for fn in ('QGC-Stealth.ini', 'QGC-Stealth Daily.ini'):
        (cfg / fn).write_text(ini)
    run('rm -rf "/home/qgc/.cache/QGroundControl"; chown -R qgc:qgc /home/qgc/.config')


def run(cmd, **kw):
    return subprocess.run(cmd, shell=True, **kw)


def shot(fname):
    run(f'DISPLAY={DISPLAY} import -window root {OUT / fname}')
    print('wrote', OUT / fname)


def main():
    OUT.mkdir(exist_ok=True)
    seed_settings()
    pidf = OUT / 'tiles.pid'
    if pidf.exists():
        run(f'kill {pidf.read_text().strip()}', stderr=subprocess.DEVNULL)
    run('pkill -x QGC-Stealth; pkill -x Xvfb; pkill -x xmessage', stderr=subprocess.DEVNULL)
    time.sleep(1)
    tiles = subprocess.Popen(['python3', str(pathlib.Path(__file__).parent / 'tileserver.py')])
    pidf.write_text(str(tiles.pid))
    xvfb = subprocess.Popen(['Xvfb', DISPLAY, '-screen', '0', f'{W}x{H}x24', '-dpi', '110', '-nolisten', 'tcp', '-ac'],
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(2)
    env = dict(os.environ)
    for k in ('HTTP_PROXY', 'HTTPS_PROXY', 'http_proxy', 'https_proxy', 'ALL_PROXY', 'all_proxy'):
        env.pop(k, None)
    env.update({
        'DISPLAY': DISPLAY,
        'NO_PROXY': '127.0.0.1,localhost',
        'QT_QPA_PLATFORM': 'xcb',
        'LD_LIBRARY_PATH': f'{Q}/lib',
        'QT_PLUGIN_PATH': f'{Q}/lib/qt6/plugins',
        'QML_IMPORT_PATH': f'{Q}/lib/qt6/qml',
        'QML2_IMPORT_PATH': f'{Q}/lib/qt6/qml',
        'QT_SCALE_FACTOR': scale,
        'LIBGL_ALWAYS_SOFTWARE': '1',
        'QT_QUICK_BACKEND': 'software' if os.environ.get('QGC_SW') else '',
        'XDG_RUNTIME_DIR': '/home/qgc/xdg',
        'HOME': '/home/qgc',
        'USER': 'qgc',
        'QT_LOGGING_RULES': 'qt.qml.*=false',
    })
    if not env['QT_QUICK_BACKEND']:
        del env['QT_QUICK_BACKEND']
    args = ['runuser', '-u', 'qgc', '--', str(BIN)]
    if fake:
        args.append('--fake-mobile')
    log = open(OUT / f'{name}.log', 'w')
    app = subprocess.Popen(args, env=env, stdout=log, stderr=subprocess.STDOUT)
    # debug build + mock vehicle handshake: give it time to settle
    for i in range(45):
        time.sleep(1)
        if app.poll() is not None:
            print('app exited early with', app.returncode)
            break
    # no window manager: resize every top-level client window to fill the screen
    for _ in range(3):
        wids = subprocess.run(f'DISPLAY={DISPLAY} xdotool search --maxdepth 1 --name .', shell=True,
                              capture_output=True, text=True).stdout.split()
        for wid in wids:
            run(f'DISPLAY={DISPLAY} xdotool windowmove {wid} 0 0 windowsize {wid} {W} {H}')
        time.sleep(2)
    print('resized windows', wids)
    time.sleep(4)
    shot(f'{name}-fly.png')
    if want_plan and app.poll() is None:
        # open the view selector (Q logo top-left) and pick Plan (second row)
        dx, dy = os.environ.get('PLAN_CLICK', '40,28').split(',')
        run(f'DISPLAY={DISPLAY} xdotool mousemove {dx} {dy} click 1')
        time.sleep(2)
        shot(f'{name}-menu.png')
        px, py = os.environ.get('PLAN_ITEM', '390,185').split(',')
        run(f'DISPLAY={DISPLAY} xdotool mousemove {px} {py} click 1')
        time.sleep(6)
        shot(f'{name}-plan.png')
    # optional click script: CLICKS="x,y,delay,shotname;x,y,delay,shotname;..."
    for step in [c for c in os.environ.get('CLICKS', '').split(';') if c.strip()]:
        cx, cy, delay, sname = step.split(',')
        run(f'DISPLAY={DISPLAY} xdotool mousemove {cx} {cy} click 1')
        time.sleep(float(delay))
        if sname:
            shot(f'{name}-{sname}.png')
    app.terminate()
    try:
        app.wait(10)
    except subprocess.TimeoutExpired:
        app.kill()
    tiles.terminate()
    xvfb.terminate()


if __name__ == '__main__':
    main()
