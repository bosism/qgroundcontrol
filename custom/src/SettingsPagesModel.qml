import QtQml.Models

import QGroundControl
import QGroundControl.Controls

// Stealth FPV override of the generated settings page list (src/UI/AppSettings/pages/SettingsPages.json).
// Pages an FPV pilot does not need are advanced-mode only. Regenerate from the build
// directory (src/UI/AppSettings/generated/SettingsPagesModel.qml) after upstream changes.

ListModel {

    ListElement {
        name: qsTranslate("SettingsPages.json", "General")
        url: "qrc:/qml/QGroundControl/AppSettings/GeneralSettings.qml"
        iconUrl: "qrc:/res/QGCLogoWhite.svg"
        sections: '["General", "Vehicle Preferences", "Units"]'
        searchTerms: '[{"section": 0, "terms": "general language locale color scheme dark mode theme palette follow me audio volume sound gstreamer debug level ui scale font size zoom save path storage reset clear settings factory reset"}, {"section": 1, "terms": "general vehicle preferences firmware ardupilot px4 vehicle multirotor fixed wing vtol rover sub"}, {"section": 2, "terms": "general units metric imperial distance speed area temperature feet meters"}]'
        translatableTerms: '[{"section": 0, "context": "General.SettingsUI.json", "terms": ["General", "language", "locale", "color scheme", "dark mode", "theme", "palette", "follow me", "audio", "volume", "sound", "gstreamer", "debug level", "ui scale", "font size", "zoom", "save path", "storage", "reset", "clear settings", "factory reset"]}, {"section": 1, "context": "General.SettingsUI.json", "terms": ["Vehicle Preferences", "firmware", "ardupilot", "px4", "vehicle", "multirotor", "fixed wing", "vtol", "rover", "sub"]}, {"section": 2, "context": "General.SettingsUI.json", "terms": ["Units", "units", "metric", "imperial", "distance", "speed", "area", "temperature", "feet", "meters"]}]'
        pageVisible: function() { return true }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Fly View")
        url: "qrc:/qml/QGroundControl/AppSettings/FlyViewSettings.qml"
        iconUrl: "qrc:/qmlimages/PaperPlane.svg"
        sections: '["General", "Guided Commands", "MAVLink Actions", "Virtual Joystick", "Instrument Panel"]'
        searchTerms: '[{"section": 0, "terms": "fly view general checklist preflight multi vehicle center map log replay camera control home position mission popup"}, {"section": 1, "terms": "fly view guided commands altitude guided minimum maximum goto go to loiter radius confirmation"}, {"section": 2, "terms": "fly view mavlink actions mavlink action pre-arm post-arm disarm"}, {"section": 3, "terms": "fly view virtual joystick joystick virtual stick throttle left handed auto center"}, {"section": 4, "terms": "fly view instrument panel compass indicators hud instrument nose up"}]'
        translatableTerms: '[{"section": 0, "context": "FlyView.SettingsUI.json", "terms": ["General", "checklist", "preflight", "multi vehicle", "center map", "log replay", "camera control", "home position", "mission popup"]}, {"section": 1, "context": "FlyView.SettingsUI.json", "terms": ["Guided Commands", "altitude", "guided", "minimum", "maximum", "goto", "go to", "loiter radius", "confirmation"]}, {"section": 2, "context": "FlyView.SettingsUI.json", "terms": ["MAVLink Actions", "mavlink action", "pre-arm", "post-arm", "disarm"]}, {"section": 3, "context": "FlyView.SettingsUI.json", "terms": ["Virtual Joystick", "joystick", "virtual stick", "throttle", "left handed", "auto center"]}, {"section": 4, "context": "FlyView.SettingsUI.json", "terms": ["Instrument Panel", "compass", "indicators", "hud", "instrument", "nose up"]}]'
        pageVisible: function() { return true }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "3D View")
        url: "qrc:/qml/QGroundControl/AppSettings/Viewer3DSettings.qml"
        iconUrl: "qrc:/qml/QGroundControl/Viewer3D/City3DMapIcon.svg"
        sections: '["General", "Data"]'
        searchTerms: '[{"section": 0, "terms": "3d view general 3d map enable 3d map provider"}, {"section": 1, "terms": "3d view data osm openstreetmap 3d data building height altitude bias"}]'
        translatableTerms: '[{"section": 0, "context": "Viewer3D.SettingsUI.json", "terms": ["General", "3d view", "3d map", "enable 3d", "map provider"]}, {"section": 1, "context": "Viewer3D.SettingsUI.json", "terms": ["Data", "osm", "openstreetmap", "3d data", "building height", "altitude bias"]}]'
        pageVisible: function() { return QGroundControl.settingsManager.viewer3DSettings.userVisible }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Plan View")
        url: "qrc:/qml/QGroundControl/AppSettings/PlanViewSettings.qml"
        iconUrl: "qrc:/qmlimages/Plan.svg"
        sections: '["General"]'
        searchTerms: '[{"section": 0, "terms": "plan view general mission altitude default altitude vtol transition condition gate takeoff landing pattern waypoint"}]'
        translatableTerms: '[{"section": 0, "context": "PlanView.SettingsUI.json", "terms": ["General", "mission altitude", "default altitude", "vtol transition", "condition gate", "takeoff", "landing pattern", "waypoint"]}]'
        pageVisible: function() { return QGroundControl.corePlugin.showAdvancedUI }
    }

    ListElement {
        name: "Divider"
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "ADSB Server")
        url: "qrc:/qml/QGroundControl/AppSettings/ADSBServerSettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/airplane.svg"
        sections: '["ADSB Server"]'
        searchTerms: '[{"section": 0, "terms": "adsb server adsb ads-b traffic aircraft tracking sbs server host port"}]'
        translatableTerms: '[{"section": 0, "context": "ADSBVehicleManager.SettingsUI.json", "terms": ["ADSB Server", "adsb", "ads-b", "traffic", "aircraft", "tracking", "sbs", "server", "host", "port"]}]'
        pageVisible: function() { return QGroundControl.corePlugin.showAdvancedUI }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Comm Links")
        url: "qrc:/qml/QGroundControl/AppSettings/CommLinksSettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/usb.svg"
        sections: '["AutoConnect", "NMEA GPS", "Link Management"]'
        searchTerms: '[{"section": 0, "terms": "comm links autoconnect auto connect pixhawk sik radio librepilot udp rtk gps usb"}, {"section": 1, "terms": "comm links nmea gps nmea gps serial baud rate external gps"}, {"section": 2, "terms": "comm links link management link connection serial tcp udp bluetooth add link"}]'
        translatableTerms: '[{"section": 0, "context": "CommLinks.SettingsUI.json", "terms": ["AutoConnect", "auto connect", "pixhawk", "sik radio", "librepilot", "udp", "rtk gps", "usb"]}, {"section": 1, "context": "CommLinks.SettingsUI.json", "terms": ["NMEA GPS", "nmea", "gps", "serial", "baud rate", "external gps"]}, {"section": 2, "context": "CommLinks.SettingsUI.json", "terms": ["Link Management", "link", "connection", "serial", "tcp", "udp", "bluetooth", "add link"]}]'
        pageVisible: function() { return true }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Logging")
        url: "qrc:/qml/QGroundControl/LogManager/AppLogging.qml"
        iconUrl: "qrc:/InstrumentValueIcons/conversation.svg"
        sections: '[]'
        searchTerms: '[]'
        translatableTerms: '[]'
        pageVisible: function() { return true }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Maps")
        url: "qrc:/qml/QGroundControl/AppSettings/MapsSettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/globe.svg"
        sections: '["Map Provider", "Offline Maps", "Tokens", "Mapbox Login", "Custom Map URL", "Tile Cache"]'
        searchTerms: '[{"section": 0, "terms": "maps map provider satellite terrain street hybrid google bing"}, {"section": 1, "terms": "maps offline maps offline download cache tile set"}, {"section": 2, "terms": "maps tokens token api key tianditu mapbox esri vworld openaip"}, {"section": 3, "terms": "maps mapbox login mapbox account style"}, {"section": 4, "terms": "maps custom map url custom map wms tile url tile server"}, {"section": 5, "terms": "maps tile cache cache disk size memory size"}]'
        translatableTerms: '[{"section": 0, "context": "Maps.SettingsUI.json", "terms": ["Map Provider", "map provider", "satellite", "terrain", "street", "hybrid", "google", "bing"]}, {"section": 1, "context": "Maps.SettingsUI.json", "terms": ["Offline Maps", "offline", "download", "cache", "tile set"]}, {"section": 2, "context": "Maps.SettingsUI.json", "terms": ["Tokens", "token", "api key", "tianditu", "mapbox", "esri", "vworld", "openaip"]}, {"section": 3, "context": "Maps.SettingsUI.json", "terms": ["Mapbox Login", "mapbox", "account", "style"]}, {"section": 4, "context": "Maps.SettingsUI.json", "terms": ["Custom Map URL", "custom map", "wms", "tile url", "tile server"]}, {"section": 5, "context": "Maps.SettingsUI.json", "terms": ["Tile Cache", "cache", "disk size", "memory size", "tile cache"]}]'
        pageVisible: function() { return QGroundControl.corePlugin.showAdvancedUI }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "NTRIP/RTK")
        url: "qrc:/qml/QGroundControl/AppSettings/NTRIPSettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/globe.svg"
        sections: '["Connection Status", "Server Settings", "Mountpoint Browser", "Options", "UDP Forwarding"]'
        searchTerms: '[{"section": 0, "terms": "ntrip/rtk connection status ntrip rtk corrections"}, {"section": 1, "terms": "ntrip/rtk server settings ntrip server rtk caster host port username password mountpoint"}, {"section": 2, "terms": "ntrip/rtk mountpoint browser mountpoint browse stream"}, {"section": 3, "terms": "ntrip/rtk options rtcm whitelist message filter ntrip"}, {"section": 4, "terms": "ntrip/rtk udp forwarding udp forward relay corrections forward rtcm"}]'
        translatableTerms: '[{"section": 0, "context": "NTRIP.SettingsUI.json", "terms": ["Connection Status", "ntrip", "rtk", "connection status", "corrections"]}, {"section": 1, "context": "NTRIP.SettingsUI.json", "terms": ["Server Settings", "ntrip server", "rtk", "caster", "host", "port", "username", "password", "mountpoint"]}, {"section": 2, "context": "NTRIP.SettingsUI.json", "terms": ["Mountpoint Browser", "mountpoint", "browse", "stream"]}, {"section": 3, "context": "NTRIP.SettingsUI.json", "terms": ["Options", "rtcm", "whitelist", "message filter", "ntrip"]}, {"section": 4, "context": "NTRIP.SettingsUI.json", "terms": ["UDP Forwarding", "udp forward", "relay", "corrections forward", "rtcm"]}]'
        pageVisible: function() { return QGroundControl.corePlugin.showAdvancedUI && (QGroundControl.settingsManager && QGroundControl.settingsManager.ntripSettings !== undefined) }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "PX4 Log Transfer")
        url: "qrc:/qml/QGroundControl/AppSettings/PX4LogTransferSettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/inbox-download.svg"
        sections: '["Log Control", "Upload Settings", "Log Files"]'
        searchTerms: '[{"section": 0, "terms": "px4 log transfer log control px4 log logging start log stop log auto log"}, {"section": 1, "terms": "px4 log transfer upload settings upload flight review email description wind rating"}, {"section": 2, "terms": "px4 log transfer log files download delete upload logs"}]'
        translatableTerms: '[{"section": 0, "context": "PX4LogTransfer.SettingsUI.json", "terms": ["Log Control", "px4 log", "logging", "start log", "stop log", "auto log"]}, {"section": 1, "context": "PX4LogTransfer.SettingsUI.json", "terms": ["Upload Settings", "upload", "flight review", "email", "description", "wind", "rating"]}, {"section": 2, "context": "PX4LogTransfer.SettingsUI.json", "terms": ["Log Files", "log files", "download", "delete", "upload logs"]}]'
        pageVisible: function() { return QGroundControl.corePlugin.showAdvancedUI && QGroundControl.corePlugin.options.showPX4LogTransferOptions && QGroundControl.px4ProFirmwareSupported && (QGroundControl.multiVehicleManager.activeVehicle ? QGroundControl.multiVehicleManager.activeVehicle.px4Firmware : true) }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Remote ID")
        url: "qrc:/qml/QGroundControl/AppSettings/RemoteIDSettings.qml"
        iconUrl: "qrc:/qmlimages/RidIconManNoID.svg"
        sections: '["Region", "Basic ID", "Operator ID", "Self ID", "GroundStation Location", "GCS Position", "GPS Location", "EU Vehicle Info"]'
        searchTerms: '[{"section": 0, "terms": "remote id region faa eu regulation"}, {"section": 1, "terms": "remote id basic id serial number drone id ua type registration broadcast"}, {"section": 2, "terms": "remote id operator id operator pilot registration broadcast"}, {"section": 3, "terms": "remote id self id flight purpose emergency broadcast description"}, {"section": 4, "terms": "remote id groundstation location ground station gcs location latitude longitude altitude fixed position gnss"}, {"section": 5, "terms": "remote id gcs position latitude longitude hdop"}, {"section": 6, "terms": "remote id gps location gps nmea hdop position"}, {"section": 7, "terms": "remote id eu vehicle info eu classification category class ce marking"}]'
        translatableTerms: '[{"section": 0, "context": "RemoteID.SettingsUI.json", "terms": ["Region", "region", "faa", "eu", "remote id", "regulation"]}, {"section": 1, "context": "RemoteID.SettingsUI.json", "terms": ["Basic ID", "basic id", "serial number", "drone id", "ua type", "registration", "broadcast"]}, {"section": 2, "context": "RemoteID.SettingsUI.json", "terms": ["Operator ID", "operator", "pilot", "registration", "broadcast"]}, {"section": 3, "context": "RemoteID.SettingsUI.json", "terms": ["Self ID", "self id", "flight purpose", "emergency", "broadcast", "description"]}, {"section": 4, "context": "RemoteID.SettingsUI.json", "terms": ["GroundStation Location", "ground station", "gcs location", "latitude", "longitude", "altitude", "fixed position", "gnss"]}, {"section": 5, "context": "RemoteID.SettingsUI.json", "terms": ["GCS Position", "latitude", "longitude", "hdop", "gcs position"]}, {"section": 6, "context": "RemoteID.SettingsUI.json", "terms": ["GPS Location", "gps", "nmea", "hdop", "position"]}, {"section": 7, "context": "RemoteID.SettingsUI.json", "terms": ["EU Vehicle Info", "eu", "classification", "category", "class", "ce marking"]}]'
        pageVisible: function() { return QGroundControl.corePlugin.showAdvancedUI }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Telemetry")
        url: "qrc:/qml/QGroundControl/AppSettings/TelemetrySettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/drone.svg"
        sections: '["Ground Station", "MAVLink Forwarding", "Logging", "Stream Rates (ArduPilot Only)", "Signing Key", "Link Status"]'
        searchTerms: '[{"section": 0, "terms": "telemetry ground station system id mavlink id heartbeat initial download gcs"}, {"section": 1, "terms": "telemetry mavlink forwarding forward relay mavlink forward host"}, {"section": 2, "terms": "telemetry logging telemetry log tlog save log recording csv"}, {"section": 3, "terms": "telemetry stream rates (ardupilot only) stream rate ardupilot apm raw sensors rc channels position rate"}, {"section": 4, "terms": "telemetry signing key signing encryption key security mavlink2"}, {"section": 5, "terms": "telemetry link status packet loss received sent latency"}]'
        translatableTerms: '[{"section": 0, "context": "Telemetry.SettingsUI.json", "terms": ["Ground Station", "system id", "mavlink id", "heartbeat", "initial download", "gcs"]}, {"section": 1, "context": "Telemetry.SettingsUI.json", "terms": ["MAVLink Forwarding", "forward", "relay", "mavlink forward", "host"]}, {"section": 2, "context": "Telemetry.SettingsUI.json", "terms": ["Logging", "telemetry log", "tlog", "save log", "recording", "csv"]}, {"section": 3, "context": "Telemetry.SettingsUI.json", "terms": ["Stream Rates (ArduPilot Only)", "stream rate", "ardupilot", "apm", "raw sensors", "rc channels", "position rate"]}, {"section": 4, "context": "Telemetry.SettingsUI.json", "terms": ["Signing Key", "signing", "encryption", "key", "security", "mavlink2"]}, {"section": 5, "context": "Telemetry.SettingsUI.json", "terms": ["Link Status", "link status", "packet loss", "received", "sent", "latency"]}]'
        pageVisible: function() { return true }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Video")
        url: "qrc:/qml/QGroundControl/AppSettings/VideoSettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/camera.svg"
        sections: '["Video Source", "Connection", "Settings", "Local Video Storage"]'
        searchTerms: '[{"section": 0, "terms": "video video source camera stream"}, {"section": 1, "terms": "video connection rtsp tcp udp mpegts video url stream url"}, {"section": 2, "terms": "video settings aspect ratio low latency decoder hardware decode disable when disarmed gpu zero-copy"}, {"section": 3, "terms": "video local video storage record recording format mp4 mkv storage limit video file"}]'
        translatableTerms: '[{"section": 0, "context": "Video.SettingsUI.json", "terms": ["Video Source", "video source", "camera", "stream"]}, {"section": 1, "context": "Video.SettingsUI.json", "terms": ["Connection", "rtsp", "tcp", "udp", "mpegts", "video url", "stream url"]}, {"section": 2, "context": "Video.SettingsUI.json", "terms": ["Settings", "aspect ratio", "low latency", "decoder", "hardware decode", "disable when disarmed", "gpu", "zero-copy"]}, {"section": 3, "context": "Video.SettingsUI.json", "terms": ["Local Video Storage", "record", "recording format", "mp4", "mkv", "storage limit", "video file"]}]'
        pageVisible: function() { return QGroundControl.settingsManager.videoSettings.userVisible }
    }

    ListElement {
        name: "Divider"
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Help")
        url: "qrc:/qml/QGroundControl/AppSettings/HelpSettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/question.svg"
        sections: '[]'
        searchTerms: '[]'
        translatableTerms: '[]'
        pageVisible: function() { return true }
    }

    ListElement {
        name: "Divider"
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Mock Link")
        url: "qrc:/qml/QGroundControl/AppSettings/MockLink.qml"
        iconUrl: "qrc:/InstrumentValueIcons/drone.svg"
        sections: '[]'
        searchTerms: '[]'
        translatableTerms: '[]'
        pageVisible: function() { return ScreenTools.isDebug }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Debug")
        url: "qrc:/qml/QGroundControl/AppSettings/DebugWindow.qml"
        iconUrl: "qrc:/InstrumentValueIcons/bug.svg"
        sections: '[]'
        searchTerms: '[]'
        translatableTerms: '[]'
        pageVisible: function() { return ScreenTools.isDebug }
    }

    ListElement {
        name: qsTranslate("SettingsPages.json", "Palette Test")
        url: "qrc:/qml/QGroundControl/AppSettings/QmlTest.qml"
        iconUrl: "qrc:/InstrumentValueIcons/photo.svg"
        sections: '[]'
        searchTerms: '[]'
        translatableTerms: '[]'
        pageVisible: function() { return ScreenTools.isDebug }
    }
}
