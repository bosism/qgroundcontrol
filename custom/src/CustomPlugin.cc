#include "CustomPlugin.h"
#include "QmlComponentInfo.h"
#include "QGCLoggingCategory.h"
#include "QGCPalette.h"
#include "QGCMAVLink.h"
#include "AppSettings.h"

#include <QtCore/QApplicationStatic>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlFile>

QGC_LOGGING_CATEGORY(CustomLog, "Custom.CustomPlugin")

Q_APPLICATION_STATIC(CustomPlugin, _customPluginInstance);

CustomFlyViewOptions::CustomFlyViewOptions(CustomOptions* options, QObject* parent)
    : QGCFlyViewOptions(options, parent)
{
    qCDebug(CustomLog) << this;
}

CustomOptions::CustomOptions(CustomPlugin *plugin, QObject *parent)
    : QGCOptions(parent)
    , _plugin(plugin)
    , _flyViewOptions(new CustomFlyViewOptions(this, this))
{
    Q_CHECK_PTR(_plugin);
}

/*===========================================================================*/

CustomPlugin::CustomPlugin(QObject *parent)
    : QGCCorePlugin(parent)
    , _options(new CustomOptions(this, this))
{
    qCDebug(CustomLog) << this;

    _showAdvancedUI = false;
    (void) connect(this, &QGCCorePlugin::showAdvancedUIChanged, this, &CustomPlugin::_advancedChanged);
}

QGCCorePlugin *CustomPlugin::instance()
{
    return _customPluginInstance();
}

void CustomPlugin::cleanup()
{
    if (_qmlEngine) {
        _qmlEngine->removeUrlInterceptor(_selector);
    }

    delete _selector;
}

void CustomPlugin::_advancedChanged(bool changed)
{
    // Firmware Upgrade page is only show in Advanced mode
    emit _options->showFirmwareUpgradeChanged(changed);
}

void CustomPlugin::_addSettingsEntry(const QString &title, const char *qmlFile, const char *iconFile)
{
    Q_CHECK_PTR(qmlFile);
    // 'this' instance will take ownership on the QmlComponentInfo instance
    _customSettingsList.append(QVariant::fromValue(
        new QmlComponentInfo(
            title,
            QUrl::fromUserInput(qmlFile),
            !iconFile ? QUrl() : QUrl::fromUserInput(iconFile),
            this)
        )
    );
}

bool CustomPlugin::overrideSettingsGroupVisibility(const QString &name)
{
    // Mission planning, 3D viewer, ADS-B feeds and RTK corrections are outside the FPV use case.
    static const QStringList hiddenGroups = {
        QStringLiteral("PlanView"),
        QStringLiteral("Viewer3D"),
        QStringLiteral("ADSBVehicleManager"),
        QStringLiteral("NTRIP"),
        QStringLiteral("RTK"),
    };
    return !hiddenGroups.contains(name);
}

void CustomPlugin::adjustSettingMetaData(const QString& settingsGroup, FactMetaData& metaData, bool &userVisible)
{
    QGCCorePlugin::adjustSettingMetaData(settingsGroup, metaData, userVisible);

    if (settingsGroup == AppSettings::settingsGroup) {
        // This tells QGC than when you are creating Plans while not connected to a vehicle
        // the specific firmware/vehicle the plan is for.
        if (metaData.name() == AppSettings::offlineEditingFirmwareClassName) {
            metaData.setRawDefaultValue(QGCMAVLink::FirmwareClassPX4);
            userVisible = false;
            return;
        } else if (metaData.name() == AppSettings::offlineEditingVehicleClassName) {
            metaData.setRawDefaultValue(QGCMAVLink::VehicleClassMultiRotor);
            userVisible = false;
            return;
        } else if (metaData.name() == AppSettings::indoorPaletteName) {
            // The Stealth skin only styles the dark scheme, so make it the default.
            metaData.setRawDefaultValue(1);
            return;
        }
    }
}

namespace {

/// Stealth Ops dark palette: charcoal panels, amber accent, olive "go" green.
/// Only the dark (indoor) theme is restyled; the light theme keeps stock QGC colors.
struct StealthColor {
    const char *name;
    const char *enabled;
    const char *disabled;
};

constexpr StealthColor kStealthDarkColors[] = {
    { "window",               "#0a0c0e", "#0a0c0e" },
    { "windowTransparent",    "#e60a0c0e", "#e60a0c0e" },
    { "windowShade",          "#121517", "#121517" },
    { "windowShadeDark",      "#0d1113", "#0d1113" },
    { "windowShadeLight",     "#2b3239", "#2b3239" },
    { "text",                 "#d9dde1", "#7d868e" },
    { "warningText",          "#ff4b4b", "#ff4b4b" },
    { "button",               "#15191c", "#15191c" },
    { "buttonBorder",         "#4a545c", "#2b3239" },
    { "buttonText",           "#d9dde1", "#7d868e" },
    { "buttonHighlight",      "#f0b429", "#2b3239" },
    { "buttonHighlightText",  "#0a0c0e", "#0a0c0e" },
    { "primaryButton",        "#f0b429", "#4a545c" },
    { "primaryButtonText",    "#0a0c0e", "#0a0c0e" },
    { "textField",            "#0d1113", "#0d1113" },
    { "textFieldText",        "#d9dde1", "#7d868e" },
    { "mapButton",            "#0f1215", "#0f1215" },
    { "mapButtonHighlight",   "#f0b429", "#4a545c" },
    { "mapIndicator",         "#8bc34a", "#4a545c" },
    { "mapIndicatorChild",    "#5a7a35", "#4a545c" },
    { "colorGreen",           "#8bc34a", "#8bc34a" },
    { "colorYellow",          "#f0b429", "#f0b429" },
    { "colorYellowGreen",     "#b7c94a", "#b7c94a" },
    { "colorOrange",          "#ff8a3d", "#ff8a3d" },
    { "colorRed",             "#ff4b4b", "#ff4b4b" },
    { "colorGrey",            "#7d868e", "#7d868e" },
    { "colorBlue",            "#f0b429", "#f0b429" },
    { "alertBackground",      "#3a2a08", "#3a2a08" },
    { "alertBorder",          "#f0b429", "#f0b429" },
    { "alertText",            "#f0b429", "#f0b429" },
    { "missionItemEditor",    "#1a1d20", "#121517" },
    { "toolStripHoverColor",  "#f0b429", "#2b3239" },
    { "toolbarBackground",    "#e60f1215", "#e60f1215" },
    { "groupBorder",          "#2b3239", "#2b3239" },
    { "modifiedParamValue",   "#f0b429", "#f0b429" },
    { "brandingPurple",       "#f0b429", "#f0b429" },
    { "brandingBlue",         "#f0b429", "#f0b429" },
    { "toolStripFGColor",     "#d9dde1", "#7d868e" },
    { "mapMissionTrajectory", "#8bc34a", "#8bc34a" },
};

} // namespace

void CustomPlugin::paletteOverride(const QString &colorName, QGCPalette::PaletteColorInfo_t& colorInfo)
{
    for (const StealthColor &c : kStealthDarkColors) {
        if (colorName == QLatin1StringView(c.name)) {
            colorInfo[QGCPalette::Dark][QGCPalette::ColorGroupEnabled]  = QColor(QLatin1StringView(c.enabled));
            colorInfo[QGCPalette::Dark][QGCPalette::ColorGroupDisabled] = QColor(QLatin1StringView(c.disabled));
            return;
        }
    }
}

QQmlApplicationEngine* CustomPlugin::createQmlApplicationEngine(QObject* parent)
{
    _qmlEngine = QGCCorePlugin::createQmlApplicationEngine(parent);
    _qmlEngine->addImportPath(QStringLiteral("qrc:/qml"));
    _selector = new CustomOverrideInterceptor();
    _qmlEngine->addUrlInterceptor(_selector);

    return _qmlEngine;
}

/*===========================================================================*/

CustomOverrideInterceptor::CustomOverrideInterceptor()
    : QQmlAbstractUrlInterceptor()
{

}

QUrl CustomOverrideInterceptor::intercept(const QUrl &url, QQmlAbstractUrlInterceptor::DataType type)
{
    switch (type) {
    case QQmlAbstractUrlInterceptor::QmlFile:
    case QQmlAbstractUrlInterceptor::UrlString:
        if (url.scheme() == QStringLiteral("qrc")) {
            const QString origPath = url.path();
            const QString overrideRes = QStringLiteral(":/Custom%1").arg(origPath);
            if (QFile::exists(overrideRes)) {
                const QString relPath = overrideRes.mid(2);
                QUrl result;
                result.setScheme(QStringLiteral("qrc"));
                result.setPath('/' + relPath);
                return result;
            }
        }
        break;
    default:
        break;
    }

    return url;
}
