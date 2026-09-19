#pragma once

#include <QtCore/QTranslator>
#include <QtQml/QQmlAbstractUrlInterceptor>

#include "QGCCorePlugin.h"
#include "QGCOptions.h"

class CustomOptions;
class CustomPlugin;
class CustomSettings;
class QQmlApplicationEngine;

Q_DECLARE_LOGGING_CATEGORY(CustomLog)

class CustomFlyViewOptions : public QGCFlyViewOptions
{
    Q_OBJECT

public:
    explicit CustomFlyViewOptions(CustomOptions *options, QObject *parent = nullptr);

    // Overrides from CustomFlyViewOptions

    /// The bottom-right row is replaced by the Stealth HUD, so the stock panel is never shown.
    bool showInstrumentPanel() const final { return false; }
    /// Single-vehicle FPV build: no multi-vehicle list, no mission-style guided actions.
    bool showMultiVehicleList() const final { return false; }
    bool guidedBarShowOrbit() const final { return false; }
    bool guidedBarShowROI() const final { return false; }
};

/*===========================================================================*/

class CustomOptions : public QGCOptions
{
    Q_OBJECT

public:
    explicit CustomOptions(CustomPlugin *plugin, QObject *parent = nullptr);

    // Overrides from QGCOptions

    /// Firmware upgrade and sensor calibration pages are only shown in Advanced Mode.
    bool showFirmwareUpgrade() const final { return _plugin->showAdvancedUI(); }
    bool showSensorCalibrationAccel() const final { return _plugin->showAdvancedUI(); }
    bool showSensorCalibrationAirspeed() const final { return _plugin->showAdvancedUI(); }
    bool showSensorCalibrationCompass() const final { return _plugin->showAdvancedUI(); }
    bool showSensorCalibrationGyro() const final { return _plugin->showAdvancedUI(); }
    bool showSensorCalibrationLevel() const final { return _plugin->showAdvancedUI(); }
    /// FPV-oriented build: no mission status bar, offline map tools or PX4 log transfer.
    bool showMissionStatus() const final { return false; }
    bool showOfflineMapExport() const final { return false; }
    bool showOfflineMapImport() const final { return false; }
    bool showPX4LogTransferOptions() const final { return false; }
    QGCFlyViewOptions *flyViewOptions() const final { return _flyViewOptions; }

private:
    QGCCorePlugin *_plugin = nullptr;
    CustomFlyViewOptions *_flyViewOptions = nullptr;
};

/*===========================================================================*/

class CustomPlugin : public QGCCorePlugin
{
    Q_OBJECT

public:
    explicit CustomPlugin(QObject *parent = nullptr);

    static QGCCorePlugin *instance();

    // Overrides from QGCCorePlugin

    void cleanup() final;
    QGCOptions *options() final { return _options; }
    /// This allows you to override/hide QGC Application settings
    void adjustSettingMetaData(const QString &settingsGroup, FactMetaData &metaData, bool &userVisible) final;
    /// Hides whole settings groups (and their settings pages) that an FPV build does not need.
    bool overrideSettingsGroupVisibility(const QString &name) final;
    /// This modifies QGC colors palette to match possible custom corporate branding
    void paletteOverride(const QString &colorName, QGCPalette::PaletteColorInfo_t &colorInfo) final;
    /// We override this so we can get access to QQmlApplicationEngine and use it to register our qml module
    QQmlApplicationEngine *createQmlApplicationEngine(QObject *parent) final;

private slots:
    void _advancedChanged(bool advanced);

private:
    void _addSettingsEntry(const QString& title, const char* qmlFile, const char* iconFile = nullptr);

    CustomOptions *_options = nullptr;
    QQmlApplicationEngine *_qmlEngine = nullptr;
    class CustomOverrideInterceptor *_selector = nullptr;
    QVariantList _customSettingsList; // Not to be mixed up with QGCCorePlugin implementation
};

/*===========================================================================*/

class CustomOverrideInterceptor : public QQmlAbstractUrlInterceptor
{
public:
    CustomOverrideInterceptor();

    QUrl intercept(const QUrl &url, QQmlAbstractUrlInterceptor::DataType type) final;
};
