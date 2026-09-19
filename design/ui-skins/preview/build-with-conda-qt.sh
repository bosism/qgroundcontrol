#!/bin/bash
# Build the QGC-Stealth custom build against a conda-forge Qt (see ../README.md,
# section "Building a preview without the Qt installer"). Extra flags used here
# work around that setup; a normal Qt SDK install needs none of them.
S=${QGC_PREVIEW_ROOT:-/tmp/qgc-preview}
Q=$S/conda/root/envs/qt
LOG=$S/build-qgc.log
cd /home/user/qgroundcontrol
export CPM_SOURCE_CACHE=$S/cpm-cache
export PATH=$Q/bin:$PATH
echo "=== configure $(date)" >> $LOG
cmake -S . -B $S/build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DQGC_BUILD_TESTING=OFF -DQGC_DEBUG_QML=OFF \
  -DCMAKE_PREFIX_PATH=$Q -DQGC_QT_MAXIMUM_VERSION=6.12.0 \
  -DQGC_ENABLE_GST_VIDEOSTREAMING=OFF -DQGC_ENABLE_WERROR=OFF \
  -DQGC_BUILD_INSTALLER=OFF -DQGC_UNITY_BUILD=OFF \
  -DLIBSECRET_SUPPORT=OFF "-DCMAKE_EXE_LINKER_FLAGS=-L$Q/lib -liconv" >> $LOG 2>&1
echo "=== configure exit $? $(date)" >> $LOG
echo "=== build $(date)" >> $LOG
cmake --build $S/build --parallel 4 >> $LOG 2>&1
echo "=== build exit $? $(date)" >> $LOG
echo "QGC BUILD FINISHED" >> $LOG
