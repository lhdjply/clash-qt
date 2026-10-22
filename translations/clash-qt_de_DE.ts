<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de_DE">
<context>
    <name>clashqt::AppContext</name>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="188"/>
        <source>Failed to turn off the system proxy on exit: %1 (the desktop still points at the local port)</source>
        <translation>Systemproxy beim Beenden nicht deaktivierbar: %1 (der Desktop zeigt weiterhin auf den lokalen Port)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="193"/>
        <source>System proxy turned off before exit, so the desktop stops pointing at a stopped local port</source>
        <translation>Systemproxy vor dem Beenden deaktiviert, damit der Desktop nicht länger auf einen gestoppten lokalen Port zeigt</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="208"/>
        <source>Failed to load the config. Check that the files in %1 are writable</source>
        <translation>Konfiguration konnte nicht geladen werden. Prüfen Sie, ob die Dateien in %1 beschreibbar sind</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="224"/>
        <source>Data directory: %1 (portable: %2)</source>
        <translation>Datenverzeichnis: %1 (portabel: %2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="225"/>
        <source>yes</source>
        <translation>ja</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="226"/>
        <source>no</source>
        <translation>nein</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="239"/>
        <source>Config generation failed: %1</source>
        <translation>Konfigurationserstellung fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="268"/>
        <source>Start failed: %1</source>
        <translation>Start fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="269"/>
        <source>Startup failed: %1</source>
        <translation>Startvorgang fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="304"/>
        <source>Kernel failed to restart after config reload: %1</source>
        <translation>Kernel konnte nach dem Neuladen der Konfiguration nicht neu starten: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="384"/>
        <location filename="../src/app/src/AppContext.cpp" line="411"/>
        <source>Config not loaded yet</source>
        <translation>Konfiguration noch nicht geladen</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="423"/>
        <source>TUN enable request: enabled=%1 running=%2 starting(old)=%3</source>
        <translation>TUN-Aktivierungsanfrage: enabled=%1 running=%2 starting(alt)=%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="434"/>
        <source>TUN disabled; system routes restored</source>
        <translation>TUN deaktiviert; Systemrouten wiederhergestellt</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="446"/>
        <source>TUN helper not found (it should sit next to the main executable): %1</source>
        <translation>TUN-Helfer nicht gefunden (er sollte neben der Hauptdatei liegen): %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="485"/>
        <source>Removed the wildcard &quot;*&quot; from fake-ip-filter (it makes every domain skip fake-ip)</source>
        <translation>Platzhalter „*“ aus fake-ip-filter entfernt (er ließ jede Domain fake-ip überspringen)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="487"/>
        <source>Fixed the DNS config: the &quot;*&quot; in fake-ip-filter made every domain skip fake-ip, so it was removed</source>
        <translation>DNS-Konfiguration korrigiert: Das „*“ in fake-ip-filter ließ jede Domain fake-ip überspringen und wurde entfernt</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="501"/>
        <source>TUN device name changed from Mihomo to %1</source>
        <translation>TUN-Gerätename von Mihomo zu %1 geändert</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="515"/>
        <source>TUN mode needs kernel DNS (fake-ip); DNS override was enabled automatically</source>
        <translation>Der TUN-Modus benötigt Kernel-DNS (fake-ip); die DNS-Überschreibung wurde automatisch aktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="517"/>
        <source>TUN mode needs kernel DNS; &quot;DNS override&quot; was enabled automatically</source>
        <translation>Der TUN-Modus benötigt Kernel-DNS; „DNS-Überschreibung“ wurde automatisch aktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="532"/>
        <source>The kernel failed to start, so TUN cannot be enabled</source>
        <translation>Der Kernel konnte nicht starten, daher lässt sich TUN nicht aktivieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="555"/>
        <location filename="../src/app/src/AppContext.cpp" line="607"/>
        <source>Failed to enable TUN: %1</source>
        <translation>TUN konnte nicht aktiviert werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="576"/>
        <source>Enabling TUN timed out; state was force-reset</source>
        <translation>Zeitüberschreitung beim Aktivieren von TUN; Status wurde zurückgesetzt</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="579"/>
        <source>Timed out enabling TUN; state was reset, please try again</source>
        <translation>Zeitüberschreitung bei der TUN-Aktivierung; Status zurückgesetzt, bitte erneut versuchen</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="583"/>
        <source>Enabling TUN…</source>
        <translation>TUN wird aktiviert…</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="608"/>
        <source>Could not take over the device</source>
        <translation>Gerät konnte nicht übernommen werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="620"/>
        <source>TUN enabled (system routes taken over)</source>
        <translation>TUN aktiviert (Systemrouten übernommen)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="621"/>
        <source>TUN enabled (routes not taken over)</source>
        <translation>TUN aktiviert (Routen nicht übernommen)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="630"/>
        <source>Kernel is not running; skipping switch-state restore</source>
        <translation>Kernel läuft nicht; Wiederherstellung des Schalterstatus wird übersprungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="640"/>
        <source>Restored TUN to its previous state</source>
        <translation>TUN in den vorherigen Zustand zurückversetzt</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="643"/>
        <source>Failed to restore TUN: %1</source>
        <translation>TUN konnte nicht wiederhergestellt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="645"/>
        <source>Failed to restore TUN mode: %1</source>
        <translation>TUN-Modus konnte nicht wiederhergestellt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="654"/>
        <source>Restored the system proxy to its previous state</source>
        <translation>Systemproxy in den vorherigen Zustand zurückversetzt</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="657"/>
        <location filename="../src/app/src/AppContext.cpp" line="659"/>
        <source>Failed to restore the system proxy: %1</source>
        <translation>Systemproxy konnte nicht wiederhergestellt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="674"/>
        <source>Start failed; system proxy not enabled: %1</source>
        <translation>Start fehlgeschlagen; Systemproxy nicht aktiviert: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="686"/>
        <source>No inbound port available; cannot set the system proxy</source>
        <translation>Kein eingehender Port verfügbar; Systemproxy kann nicht gesetzt werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="696"/>
        <source>127.0.0.1:%1 is not listening locally; refusing to enable the system proxy (the whole desktop would lose connectivity by pointing at an empty port)</source>
        <translation>127.0.0.1:%1 lauscht lokal nicht; Systemproxy wird nicht aktiviert (der gesamte Desktop verlöre die Verbindung, weil er auf einen leeren Port zeigen würde)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="708"/>
        <source>Failed to set the system proxy: %1</source>
        <translation>Systemproxy konnte nicht gesetzt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="716"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>Systemproxy konnte nicht deaktiviert werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="749"/>
        <source>Unknown outbound mode: %1</source>
        <translation>Unbekannter Ausgangsmodus: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="788"/>
        <location filename="../src/app/src/AppContext.cpp" line="813"/>
        <location filename="../src/app/src/AppContext.cpp" line="901"/>
        <location filename="../src/app/src/AppContext.cpp" line="979"/>
        <source>Failed to write the profile contents</source>
        <translation>Profilinhalt konnte nicht geschrieben werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="793"/>
        <location filename="../src/app/src/AppContext.cpp" line="905"/>
        <source>Failed to write the profile list</source>
        <translation>Profilliste konnte nicht geschrieben werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="797"/>
        <source>Added local config: %1</source>
        <translation>Lokale Konfiguration hinzugefügt: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="805"/>
        <location filename="../src/app/src/AppContext.cpp" line="917"/>
        <location filename="../src/app/src/AppContext.cpp" line="1020"/>
        <source>Profile does not exist: %1</source>
        <translation>Profil existiert nicht: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="819"/>
        <source>Replaced the config file of &quot;%1&quot;</source>
        <translation>Konfigurationsdatei von „%1“ ersetzt</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="831"/>
        <source>Invalid profile URL: %1</source>
        <translation>Ungültige Profil-URL: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="858"/>
        <source>Profile download timed out (%1 ms)</source>
        <translation>Zeitüberschreitung beim Profil-Download (%1 ms)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="865"/>
        <source>Profile download failed: %1</source>
        <translation>Profil-Download fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="884"/>
        <source>The profile is not a valid Clash config: %1</source>
        <translation>Das Profil ist keine gültige Clash-Konfiguration: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="909"/>
        <source>Added profile: %1</source>
        <translation>Profil hinzugefügt: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="921"/>
        <source>Profile &quot;%1&quot; is not a remote profile; nothing to update</source>
        <translation>Profil „%1“ ist kein Remote-Profil; nichts zu aktualisieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="948"/>
        <source>Updating &quot;%1&quot; timed out</source>
        <translation>Zeitüberschreitung beim Aktualisieren von „%1“</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="954"/>
        <source>Failed to update &quot;%1&quot;: %2</source>
        <translation>Aktualisieren von „%1“ fehlgeschlagen: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="972"/>
        <source>The update for &quot;%1&quot; returned invalid content; the old config was kept: %2</source>
        <translation>Das Update für „%1“ lieferte ungültigen Inhalt; die alte Konfiguration wurde beibehalten: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="993"/>
        <source>Profile updated: %1</source>
        <translation>Profil aktualisiert: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1025"/>
        <source>Switched to profile: %1</source>
        <translation>Zu Profil gewechselt: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1040"/>
        <source>Override content must be a YAML mapping: %1</source>
        <translation>Überschreibungsinhalt muss eine YAML-Map sein: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1047"/>
        <source>Unnamed override</source>
        <translation>Unbenannte Überschreibung</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1054"/>
        <source>Failed to write the override contents</source>
        <translation>Überschreibungsinhalt konnte nicht geschrieben werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1058"/>
        <source>Failed to write the override list</source>
        <translation>Überschreibungsliste konnte nicht geschrieben werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1062"/>
        <source>Added override: %1</source>
        <translation>Überschreibung hinzugefügt: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1093"/>
        <source>Failed to parse the rule-override file; treating it as empty: %1</source>
        <translation>Regel-Überschreibungsdatei konnte nicht gelesen werden; wird als leer behandelt: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1117"/>
        <source>No profile selected; cannot save custom rules</source>
        <translation>Kein Profil ausgewählt; benutzerdefinierte Regeln können nicht gespeichert werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1129"/>
        <source>%1 entry %2 contains a line break (each rule must stay on one line)</source>
        <translation>%1 Eintrag %2 enthält einen Zeilenumbruch (jede Regel muss in einer Zeile stehen)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1137"/>
        <source>%1 entry %2 is not a valid rule (expected at least &quot;type,target&quot;): %3</source>
        <translation>%1 Eintrag %2 ist keine gültige Regel (mindestens „type,target“ erwartet): %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1147"/>
        <source>Custom rules</source>
        <translation>Benutzerdefinierte Regeln</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1148"/>
        <source>Fallback rules</source>
        <translation>Fallback-Regeln</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1149"/>
        <source>Removal rules</source>
        <translation>Löschregeln</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1182"/>
        <source>Failed to write custom rules: %1</source>
        <translation>Benutzerdefinierte Regeln konnten nicht geschrieben werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1187"/>
        <source>Custom rules saved (profile %1): %2 total, %3 disabled, %4 fallback / %5 removal</source>
        <translation>Benutzerdefinierte Regeln gespeichert (Profil %1): %2 gesamt, %3 deaktiviert, %4 Fallback / %5 Löschung</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1196"/>
        <source>Custom rules applied</source>
        <translation>Benutzerdefinierte Regeln angewendet</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionListModel</name>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="67"/>
        <source>Rule: %1</source>
        <translation>Regel: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="68"/>
        <source>Outbound: %1</source>
        <translation>Ausgang: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="69"/>
        <source>Up %1 · Down %2 · %3</source>
        <translation>Hoch %1 · Runter %2 · %3</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="63"/>
        <source>Connections</source>
        <translation>Verbindungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="64"/>
        <source>Active connections, one card each. Double-click a card, or use the × on its right, to close that connection.</source>
        <translation>Aktive Verbindungen, je eine Karte. Doppelklicken Sie auf eine Karte oder nutzen Sie das × rechts, um die Verbindung zu schließen.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="73"/>
        <source>Auto refresh (1 s)</source>
        <translation>Automatisch aktualisieren (1 s)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="77"/>
        <source>Close selected</source>
        <translation>Ausgewählte schließen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="78"/>
        <source>Close all</source>
        <translation>Alle schließen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="104"/>
        <source>No active connections</source>
        <translation>Keine aktiven Verbindungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="145"/>
        <source>just now</source>
        <translation>gerade eben</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="146"/>
        <source>%1 min ago</source>
        <translation>vor %1 Min.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="147"/>
        <source>%1 h ago</source>
        <translation>vor %1 Std.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="148"/>
        <source>%1 d ago</source>
        <translation>vor %1 T.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="157"/>
        <source>%1 active connection(s) · Total up %2 · Total down %3</source>
        <translation>%1 aktive Verbindung(en) · Gesamt hoch %2 · Gesamt runter %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="239"/>
        <source>Select a connection first</source>
        <translation>Wählen Sie zuerst eine Verbindung</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="253"/>
        <source>Closed %1 connection(s)</source>
        <translation>%1 Verbindung(en) geschlossen</translation>
    </message>
</context>
<context>
    <name>clashqt::LogsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="48"/>
        <source>Logs</source>
        <translation>Protokolle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="49"/>
        <source>Application log. Logs are also written to the logs/ folder in the data directory.</source>
        <translation>Anwendungsprotokoll. Protokolle werden zusätzlich im Ordner logs/ im Datenverzeichnis gespeichert.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="54"/>
        <source>Level</source>
        <translation>Stufe</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="61"/>
        <source>Pause scrolling</source>
        <translation>Scrollen anhalten</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="64"/>
        <source>Clear view</source>
        <translation>Ansicht leeren</translation>
    </message>
</context>
<context>
    <name>clashqt::MainWindow</name>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="236"/>
        <source>Rule</source>
        <translation>Regel</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="237"/>
        <source>Global</source>
        <translation>Global</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="238"/>
        <source>Direct</source>
        <translation>Direkt</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="272"/>
        <source>System proxy</source>
        <translation>Systemproxy</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="282"/>
        <source>No system proxy backend found (gsettings / kwriteconfig); system proxy is unavailable</source>
        <translation>Kein Systemproxy-Backend gefunden (gsettings / kwriteconfig); Systemproxy ist nicht verfügbar</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="288"/>
        <source>TUN mode</source>
        <translation>TUN-Modus</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="295"/>
        <source>Captures all local traffic, including apps that ignore the system proxy. The first enable requires a one-time password prompt.</source>
        <translation>Erfasst den gesamten lokalen Datenverkehr, auch von Apps, die den Systemproxy ignorieren. Die erste Aktivierung verlangt einmalig Ihr Passwort.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="343"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="708"/>
        <source>Update this subscription</source>
        <translation>Dieses Abo aktualisieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="412"/>
        <source>Proxies</source>
        <translation>Proxies</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="414"/>
        <source>Rules</source>
        <translation>Regeln</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="416"/>
        <source>Overrides</source>
        <translation>Überschreibungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="418"/>
        <source>Logs</source>
        <translation>Protokolle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="420"/>
        <source>Connections</source>
        <translation>Verbindungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="423"/>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="484"/>
        <source>Traffic</source>
        <translation>Datenverkehr</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="694"/>
        <source>No profile selected</source>
        <translation>Kein Profil ausgewählt</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="709"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="757"/>
        <source>Local profiles have no URL to update from</source>
        <translation>Lokale Profile haben keine URL zum Aktualisieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="721"/>
        <source>%1 / %2</source>
        <translation>%1 / %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="730"/>
        <source>Updated %1</source>
        <translation>%1 aktualisiert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="745"/>
        <source>Open the profiles page</source>
        <translation>Profilseite öffnen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="752"/>
        <source>Select a profile first</source>
        <translation>Wählen Sie zuerst ein Profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="765"/>
        <source>Update failed: %1</source>
        <translation>Aktualisierung fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="767"/>
        <source>Subscription updated</source>
        <translation>Abo aktualisiert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="792"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="871"/>
        <source>Disable system proxy</source>
        <translation>Systemproxy deaktivieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="800"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="907"/>
        <source>Enable TUN mode</source>
        <translation>TUN-Modus aktivieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="807"/>
        <source>Show main window</source>
        <translation>Hauptfenster anzeigen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="815"/>
        <source>Quit</source>
        <translation>Beenden</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="872"/>
        <source>Enable system proxy</source>
        <translation>Systemproxy aktivieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="877"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="905"/>
        <source>Enabling TUN…</source>
        <translation>TUN wird aktiviert…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="881"/>
        <source>System proxy + TUN enabled</source>
        <translation>Systemproxy + TUN aktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="882"/>
        <source>TUN enabled</source>
        <translation>TUN aktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="883"/>
        <source>System proxy enabled</source>
        <translation>Systemproxy aktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="884"/>
        <source>System proxy disabled</source>
        <translation>Systemproxy deaktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="900"/>
        <source>enabling…</source>
        <translation>wird aktiviert…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="906"/>
        <source>Disable TUN mode</source>
        <translation>TUN-Modus deaktivieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="916"/>
        <source>Mixed port %1</source>
        <translation>Gemischter Port %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="918"/>
        <source>Not running</source>
        <translation>Nicht aktiv</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="946"/>
        <source>Failed to switch outbound mode: %1</source>
        <translation>Ausgangsmodus konnte nicht gewechselt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="952"/>
        <source>Outbound mode: %1</source>
        <translation>Ausgangsmodus: %1</translation>
    </message>
</context>
<context>
    <name>clashqt::OverridePage</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>Domain suffix (incl. subdomains)</source>
        <translation>Domain-Suffix (inkl. Subdomains)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>e.g. example.com</source>
        <translation>z. B. example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>Domain (exact)</source>
        <translation>Domain (exakt)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>e.g. www.example.com</source>
        <translation>z. B. www.example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>Domain keyword</source>
        <translation>Domain-Schlüsselwort</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>e.g. google</source>
        <translation>z. B. google</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>Domain regex</source>
        <translation>Domain-Regex</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>e.g. ^ads\..*</source>
        <translation>z. B. ^ads\..*</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>Geosite category</source>
        <translation>Geosite-Kategorie</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>e.g. cn / netflix / telegram</source>
        <translation>z. B. cn / netflix / telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>IP country</source>
        <translation>IP-Land</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>e.g. CN / US / JP</source>
        <translation>z. B. CN / US / JP</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>IPv4 CIDR</source>
        <translation>IPv4-CIDR</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>e.g. 10.0.0.0/8</source>
        <translation>z. B. 10.0.0.0/8</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>IPv6 CIDR</source>
        <translation>IPv6-CIDR</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>e.g. 2001:db8::/32</source>
        <translation>z. B. 2001:db8::/32</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>ASN</source>
        <translation>ASN</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>e.g. 13335</source>
        <translation>z. B. 13335</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>Destination port</source>
        <translation>Zielport</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>e.g. 443</source>
        <translation>z. B. 443</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>Source port</source>
        <translation>Quellport</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>e.g. 1234</source>
        <translation>z. B. 1234</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>Network type</source>
        <translation>Netzwerktyp</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>tcp or udp</source>
        <translation>tcp oder udp</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>Process name</source>
        <translation>Prozessname</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>e.g. telegram</source>
        <translation>z. B. telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>Process path</source>
        <translation>Prozesspfad</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>e.g. /usr/bin/curl</source>
        <translation>z. B. /usr/bin/curl</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Rule set</source>
        <translation>Regelsatz</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Choose a rule set from the config</source>
        <translation>Regelsatz aus der Konfiguration wählen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="129"/>
        <source>Catch-all (match all remaining traffic)</source>
        <translation>Catch-all (trifft auf den restlichen Datenverkehr zu)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="405"/>
        <source>Overrides</source>
        <translation>Überschreibungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="406"/>
        <source>Adds custom rules on top of the profile&apos;s rules. Pick a type, content and target when adding — no rule syntax needed. The list order is the match priority; changes are saved and applied automatically.</source>
        <translation>Ergänzt die Regeln des Profils um benutzerdefinierte Regeln. Wählen Sie beim Hinzufügen Typ, Inhalt und Ziel – Regel-Syntax ist nicht nötig. Die Reihenfolge in der Liste bestimmt die Priorität; Änderungen werden automatisch gespeichert und angewendet.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="408"/>
        <source>Custom rules</source>
        <translation>Benutzerdefinierte Regeln</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="418"/>
        <source>New rule</source>
        <translation>Neue Regel</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Enabled</source>
        <translation>Aktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Rule type</source>
        <translation>Regeltyp</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Content</source>
        <translation>Inhalt</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Target</source>
        <translation>Ziel</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="432"/>
        <source>Actions</source>
        <translation>Aktionen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="452"/>
        <source>Double-click any row to edit; uncheck it to disable (content is kept but not matched). Changes are saved and applied automatically.</source>
        <translation>Doppelklicken Sie zum Bearbeiten auf eine Zeile; entfernen Sie das Häkchen zum Deaktivieren (der Inhalt bleibt erhalten, wird aber nicht abgeglichen). Änderungen werden automatisch gespeichert und angewendet.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="526"/>
        <source>No profile selected; custom rules cannot be saved</source>
        <translation>Kein Profil ausgewählt; benutzerdefinierte Regeln können nicht gespeichert werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="527"/>
        <source>Applies to profile: %1 (rules are stored per profile and differ between profiles)</source>
        <translation>Gilt für Profil: %1 (Regeln werden pro Profil gespeichert und unterscheiden sich je Profil)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="553"/>
        <source>%1 rules, %2 enabled</source>
        <translation>%1 Regeln, %2 aktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="575"/>
        <source>Enabled; uncheck to disable</source>
        <translation>Aktiviert; Häkchen entfernen zum Deaktivieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="576"/>
        <source>Disabled: not matched after saving</source>
        <translation>Deaktiviert: wird nach dem Speichern nicht abgeglichen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="601"/>
        <source>%1 (%2)
Rule: %3</source>
        <translation>%1 (%2)
Regel: %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="612"/>
        <source>Move up (higher priority)</source>
        <translation>Nach oben (höhere Priorität)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="614"/>
        <source>Move down (lower priority)</source>
        <translation>Nach unten (niedrigere Priorität)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="615"/>
        <source>Edit this rule</source>
        <translation>Diese Regel bearbeiten</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="616"/>
        <source>Delete this rule</source>
        <translation>Diese Regel löschen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="726"/>
        <source>Save failed</source>
        <translation>Speichern fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="733"/>
        <source>Applied automatically: %1 rules, %2 disabled</source>
        <translation>Automatisch angewendet: %1 Regeln, %2 deaktiviert</translation>
    </message>
</context>
<context>
    <name>clashqt::ProfilesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="38"/>
        <source>Expired</source>
        <translation>Abgelaufen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="40"/>
        <source> (%1 days)</source>
        <translation> (%1 Tage)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="235"/>
        <source>Profiles</source>
        <translation>Profile</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="236"/>
        <source>Manage config sources. The active profile is merged with &quot;Config overrides&quot; before use.</source>
        <translation>Verwalten Sie die Konfigurationsquellen. Das aktive Profil wird vor der Verwendung mit den „Konfigurations-Überschreibungen“ zusammengeführt.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="247"/>
        <source>Add profile</source>
        <translation>Profil hinzufügen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="248"/>
        <source>Import local config</source>
        <translation>Lokale Konfiguration importieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="249"/>
        <source>Edit</source>
        <translation>Bearbeiten</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="250"/>
        <source>Update selected</source>
        <translation>Ausgewählte aktualisieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="251"/>
        <source>Update all</source>
        <translation>Alle aktualisieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="252"/>
        <source>Set active</source>
        <translation>Als aktiv setzen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="253"/>
        <source>Delete</source>
        <translation>Löschen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="263"/>
        <source>Edit the name or URL (or double-click a row)</source>
        <translation>Name oder URL bearbeiten (oder auf eine Zeile doppelklicken)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="276"/>
        <source>No profiles yet. Click &quot;Add profile&quot; to enter a subscription URL, or &quot;Import local config&quot; to pick an existing Clash config file.</source>
        <translation>Noch keine Profile. Klicken Sie auf „Profil hinzufügen“, um eine Abo-URL einzugeben, oder auf „Lokale Konfiguration importieren“, um eine vorhandene Clash-Konfigurationsdatei zu wählen.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Active</source>
        <translation>Aktiv</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Traffic</source>
        <translation>Datenverkehr</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Expires</source>
        <translation>Läuft ab</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Updated</source>
        <translation>Aktualisiert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="341"/>
        <source>Remote</source>
        <translation>Remote</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="342"/>
        <source>Local</source>
        <translation>Lokal</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="343"/>
        <source>Plugin</source>
        <translation>Plugin</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="386"/>
        <source>Add failed</source>
        <translation>Hinzufügen fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="404"/>
        <source>Choose a config file</source>
        <translation>Konfigurationsdatei wählen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="405"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Clash-Konfiguration (*.yaml *.yml);;Alle Dateien (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="410"/>
        <source>Import failed</source>
        <translation>Import fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="420"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="471"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="505"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="521"/>
        <source>Select a profile first</source>
        <translation>Wählen Sie zuerst ein Profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Save failed</source>
        <translation>Speichern fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Failed to write profile config</source>
        <translation>Profilkonfiguration konnte nicht geschrieben werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="446"/>
        <source>Replace failed</source>
        <translation>Ersetzen fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="458"/>
        <source>URL saved</source>
        <translation>URL gespeichert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="459"/>
        <source>URL saved, but fetching the new content failed:

%1

Try &quot;Update selected&quot; again later.</source>
        <translation>URL gespeichert, aber der neue Inhalt konnte nicht geladen werden:

%1

Versuchen Sie später erneut „Ausgewählte aktualisieren“.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="477"/>
        <source>Update failed</source>
        <translation>Aktualisierung fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="489"/>
        <source>Some profiles failed to update</source>
        <translation>Einige Profile konnten nicht aktualisiert werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="490"/>
        <source>%1 succeeded, %2 failed:

%3</source>
        <translation>%1 erfolgreich, %2 fehlgeschlagen:

%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="496"/>
        <source>Updated %1 profile(s)</source>
        <translation>%1 Profil(e) aktualisiert</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="511"/>
        <source>Switch failed</source>
        <translation>Wechseln fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="527"/>
        <source>Delete profile</source>
        <translation>Profil löschen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="528"/>
        <source>Delete &quot;%1&quot;? Its config file will be removed as well.</source>
        <translation>„%1“ löschen? Die zugehörige Konfigurationsdatei wird ebenfalls entfernt.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Delete failed</source>
        <translation>Löschen fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Profile not found</source>
        <translation>Profil nicht gefunden</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="545"/>
        <source>Profile deleted, but reloading the config failed: %1</source>
        <translation>Profil gelöscht, aber das Neuladen der Konfiguration ist fehlgeschlagen: %1</translation>
    </message>
</context>
<context>
    <name>clashqt::ProxiesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="66"/>
        <source>Group</source>
        <translation>Gruppe</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="90"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="334"/>
        <source>Nodes</source>
        <translation>Knoten</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="110"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="149"/>
        <source>Testing</source>
        <translation>Test läuft</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="111"/>
        <source>Test</source>
        <translation>Testen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="112"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="151"/>
        <source>Timeout</source>
        <translation>Timeout</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="291"/>
        <source>Proxies</source>
        <translation>Proxies</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="292"/>
        <source>Pick a node from the proxy group: click a card to switch to it. The text on the right is the last measured latency; click it to test that node again, or click Test on a node that has not been measured yet. In global mode, the GLOBAL group decides the exit for all traffic.</source>
        <translation>Wählen Sie einen Knoten aus der Proxy-Gruppe: Klicken Sie auf eine Karte, um dorthin zu wechseln. Der Text rechts ist die zuletzt gemessene Latenz; klicken Sie darauf, um den Knoten erneut zu testen, oder klicken Sie bei einem noch nicht gemessenen Knoten auf Testen. Im Global-Modus bestimmt die GLOBAL-Gruppe den Ausgang für den gesamten Datenverkehr.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="298"/>
        <source>Proxy group</source>
        <translation>Gruppe</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="309"/>
        <source>Sort</source>
        <translation>Sortieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="311"/>
        <source>Default order</source>
        <translation>Standard</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="312"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="313"/>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="314"/>
        <source>Latency</source>
        <translation>Latenz</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="316"/>
        <source>Sort nodes by name, protocol type or latest measured latency.
Latency sorts ascending; untested and timed-out nodes go last. Sorting never changes the selected node.</source>
        <translation>Sortiert Knoten nach Name, Protokolltyp oder zuletzt gemessener Latenz.
Latenz sortiert aufsteigend; noch nicht getestete Knoten und Knoten mit Zeitüberschreitung stehen am Ende. Die Sortierung ändert nie den ausgewählten Knoten.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="323"/>
        <source>Test this group</source>
        <translation>Gruppe testen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="324"/>
        <source>Test all groups</source>
        <translation>Alle testen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="436"/>
        <source>The active profile has no proxy groups</source>
        <translation>Das aktive Profil enthält keine Proxy-Gruppen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="469"/>
        <source>Type %1 · %2 members</source>
        <translation>Typ %1 · %2 Mitglieder</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="472"/>
        <source>Current node: %1</source>
        <translation>Aktueller Knoten: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="501"/>
        <source>Proxy group · %1</source>
        <translation>Proxy-Gruppe · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="502"/>
        <source>Protocol · %1</source>
        <translation>Protokoll · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="503"/>
        <source>Unknown</source>
        <translation>Unbekannt</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="505"/>
        <source>This protocol is not supported yet; connections will fail explicitly</source>
        <translation>Dieses Protokoll wird noch nicht unterstützt; Verbindungen schlagen mit einer Meldung fehl</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="511"/>
        <source>Last latency test timed out</source>
        <translation>Letzter Latenztest: Zeitüberschreitung</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="516"/>
        <source>Last latency %1 ms</source>
        <translation>Letzte Latenz %1 ms</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="517"/>
        <source>Last test timed out</source>
        <translation>Letzter Test: Zeitüberschreitung</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="522"/>
        <source>Currently pointing at %1</source>
        <translation>Zeigt aktuell auf %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="524"/>
        <source>Click the text on the right of a card to test that node</source>
        <translation>Klicken Sie auf den Text rechts in einer Karte, um den Knoten zu testen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="741"/>
        <source>Node no longer exists</source>
        <translation>Knoten existiert nicht mehr</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="772"/>
        <source>Latency test finished: %1/%2 nodes available</source>
        <translation>Latenztest beendet: %1/%2 Knoten verfügbar</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="776"/>
        <source>%1 latency test failed: %2</source>
        <translation>Latenztest von %1 fehlgeschlagen: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="792"/>
        <source>These nodes are being tested, or none can be tested</source>
        <translation>Diese Knoten werden gerade getestet oder können nicht getestet werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="805"/>
        <source>Testing %1 nodes (%2 at a time, results fill in as they finish)</source>
        <translation>Test von %1 Knoten läuft (%2 gleichzeitig, Ergebnisse erscheinen nach und nach)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="835"/>
        <source>Failed to switch node: %1</source>
        <translation>Knotenwechsel fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="843"/>
        <source>%1 switched to %2</source>
        <translation>%1 gewechselt zu %2</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="170"/>
        <source>New rule</source>
        <translation>Neue Regel</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="171"/>
        <source>Edit rule</source>
        <translation>Regel bearbeiten</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="180"/>
        <source>Follow the prompts; the rule syntax is assembled when you save.</source>
        <translation>Folgen Sie den Hinweisen; die Regel-Syntax wird beim Speichern zusammengesetzt.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="198"/>
        <source>%1 (cannot be edited structurally)</source>
        <translation>%1 (nicht strukturell bearbeitbar)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="201"/>
        <source>Rule type</source>
        <translation>Regeltyp</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="217"/>
        <source>Content</source>
        <translation>Inhalt</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="223"/>
        <source>Choose a policy or proxy group</source>
        <translation>Richtlinie oder Proxy-Gruppe wählen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="224"/>
        <source>Target</source>
        <translation>Ziel</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="227"/>
        <source>Default</source>
        <translation>Standard</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="228"/>
        <source>Do not resolve domain (no-resolve)</source>
        <translation>Domain nicht auflösen (no-resolve)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="229"/>
        <source>Match source address (src)</source>
        <translation>Quelladresse abgleichen (src)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="230"/>
        <source>Options</source>
        <translation>Optionen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="253"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="254"/>
        <source>Save</source>
        <translation>Speichern</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="302"/>
        <source>No rule-providers in the config</source>
        <translation>Keine rule-providers in der Konfiguration</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="303"/>
        <source>Choose a rule set</source>
        <translation>Regelsatz wählen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="333"/>
        <source>A catch-all rule matches all remaining traffic; keep it last among your custom rules.</source>
        <translation>Eine Catch-all-Regel trifft auf den restlichen Datenverkehr zu; halten Sie sie am Ende Ihrer benutzerdefinierten Regeln.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="335"/>
        <source>example.com also matches all of its subdomains.</source>
        <translation>example.com erfasst auch alle Subdomains.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="337"/>
        <source>For LAN traffic, set Options to no-resolve.</source>
        <translation>Für LAN-Datenverkehr setzen Sie die Optionen auf no-resolve.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="339"/>
        <source>Rule sets come from rule-providers in the profile.</source>
        <translation>Regelsätze stammen aus den rule-providers im Profil.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="362"/>
        <source>Choose what handles this rule (policy / proxy group).</source>
        <translation>Wählen Sie, was diese Regel behandelt (Richtlinie / Proxy-Gruppe).</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="367"/>
        <source>Enter the content to match; it is what the rule checks.</source>
        <translation>Geben Sie den Inhalt für den Abgleich ein; er ist das Prüfkriterium der Regel.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="372"/>
        <source>IPv4 CIDR needs a prefix length, e.g. 10.0.0.0/8.</source>
        <translation>IPv4-CIDR benötigt eine Präfixlänge, z. B. 10.0.0.0/8.</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleRowDelegate</name>
    <message>
        <location filename="../src/app/src/ui/RuleList.cpp" line="156"/>
        <source>%1 hits</source>
        <translation>%1 Treffer</translation>
    </message>
</context>
<context>
    <name>clashqt::RulesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="43"/>
        <source>Rules</source>
        <translation>Regeln</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="44"/>
        <source>Rules in match order, one card each. Turn the switch on the right off to disable a rule for this session; it is restored on restart.</source>
        <translation>Regeln in Abgleichreihenfolge, je eine Karte. Schalten Sie den Schalter rechts aus, um eine Regel für diese Sitzung zu deaktivieren; beim Neustart gilt sie wieder.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="64"/>
        <source>No rules loaded</source>
        <translation>Keine Regeln geladen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="90"/>
        <source>Failed to update rule state</source>
        <translation>Regelstatus konnte nicht aktualisiert werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="132"/>
        <source>%1 hits in this session</source>
        <translation>%1 Treffer in dieser Sitzung</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="133"/>
        <source>Disabled until restart</source>
        <translation>Bis zum Neustart deaktiviert</translation>
    </message>
</context>
<context>
    <name>clashqt::SettingsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="42"/>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="43"/>
        <source>Logs, DNS overrides, ports and system integration. Changes are saved and applied immediately.</source>
        <translation>Protokolle, DNS-Überschreibungen, Ports und Systemintegration. Änderungen werden sofort gespeichert und angewendet.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="46"/>
        <source>Appearance</source>
        <translation>Darstellung</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="50"/>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="63"/>
        <source>Follow system</source>
        <translation>Systemeinstellung folgen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="51"/>
        <source>Light</source>
        <translation>Hell</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="52"/>
        <source>Dark</source>
        <translation>Dunkel</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="58"/>
        <source>Theme</source>
        <translation>Design</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="68"/>
        <source>Language</source>
        <translation>Sprache</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="69"/>
        <source>Interface text is loaded at startup; restart the app after switching.</source>
        <translation>Die Oberflächentexte werden beim Start geladen; starten Sie die App nach dem Wechsel neu.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="74"/>
        <source>Logs</source>
        <translation>Protokolle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="81"/>
        <source>Log level</source>
        <translation>Protokollstufe</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="86"/>
        <source> days</source>
        <translation> Tage</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="89"/>
        <source>Log retention (days)</source>
        <translation>Protokoll-Aufbewahrung (Tage)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="90"/>
        <source>Log files older than the retention period are removed at startup.</source>
        <translation>Protokolldateien, die älter als die Aufbewahrungsdauer sind, werden beim Start entfernt.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="93"/>
        <source>Open data folder</source>
        <translation>Datenordner öffnen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="102"/>
        <source>Config overrides</source>
        <translation>Konfigurations-Überschreibungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="105"/>
        <source>Manage DNS settings in this app</source>
        <translation>DNS-Einstellungen in dieser App verwalten</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="108"/>
        <source>When on, the DNS section configured here is used; when off, the profile&apos;s own DNS settings apply.</source>
        <translation>Wenn aktiv, gilt der hier konfigurierte DNS-Abschnitt; wenn aus, gelten die DNS-Einstellungen des Profils.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="111"/>
        <source>Manage domain sniffing in this app</source>
        <translation>Domain-Sniffing in dieser App verwalten</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="117"/>
        <source>Inbound ports</source>
        <translation>Eingehende Ports</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="131"/>
        <source>Mixed port (HTTP + SOCKS5)</source>
        <translation>Gemischter Port (HTTP + SOCKS5)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="133"/>
        <source>SOCKS5 port</source>
        <translation>SOCKS5-Port</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="134"/>
        <source>Set it equal to the mixed port to reuse the mixed port.</source>
        <translation>Setzen Sie ihn gleich dem gemischten Port, um diesen mitzubenutzen.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="136"/>
        <source>HTTP port</source>
        <translation>HTTP-Port</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="141"/>
        <source>System integration</source>
        <translation>Systemintegration</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="144"/>
        <source>Start at login</source>
        <translation>Beim Anmelden starten</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="146"/>
        <source>Implemented via an XDG autostart entry (~/.config/autostart/clash-qt.desktop).</source>
        <translation>Umgesetzt über einen XDG-Autostart-Eintrag (~/.config/autostart/clash-qt.desktop).</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="149"/>
        <source>System proxy</source>
        <translation>Systemproxy</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="152"/>
        <source>Points the system HTTP/HTTPS/SOCKS proxy at this app&apos;s mixed port. Current backend: %1</source>
        <translation>Richtet den HTTP/HTTPS/SOCKS-Systemproxy auf den gemischten Port dieser App ein. Aktuelles Backend: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="156"/>
        <source>TUN mode</source>
        <translation>TUN-Modus</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="159"/>
        <source>Creates a virtual network interface to capture all traffic, including apps that ignore the system proxy (games, CLI tools, some Electron apps). The first enable asks for your password once; it only takes effect together with &quot;Route domain traffic&quot;.</source>
        <translation>Erstellt eine virtuelle Netzwerkschnittstelle, um den gesamten Datenverkehr zu erfassen, auch von Apps, die den Systemproxy ignorieren (Spiele, CLI-Tools, einige Electron-Apps). Die erste Aktivierung fragt einmalig nach Ihrem Passwort; wirksam wird sie nur zusammen mit „Domain-Datenverkehr routen“.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="162"/>
        <source>Route domain traffic (fake-IP)</source>
        <translation>Domain-Datenverkehr routen (fake-IP)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="165"/>
        <source>Routes the fake-IP range to the TUN interface (removed together with the interface). The default route is untouched, so connectivity is unaffected; with this off, TUN receives no traffic.</source>
        <translation>Leitet den fake-IP-Bereich an die TUN-Schnittstelle (wird mit der Schnittstelle entfernt). Die Standardroute bleibt unberührt, die Verbindung ist also nicht betroffen; ohne diese Option erhält TUN keinen Datenverkehr.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="171"/>
        <source>About</source>
        <translation>Über</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="177"/>
        <source>Check for updates when the app starts</source>
        <translation>Beim App-Start nach Updates suchen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="187"/>
        <source>Version v%1</source>
        <translation>Version v%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="192"/>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="481"/>
        <source>Check for updates</source>
        <translation>Nach Updates suchen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="198"/>
        <source>Reads the update manifest attached to the latest release, fetched through public GitHub mirrors. When an update exists, its changelog is shown and the release page can be opened in your browser; downloading and installing is up to you.</source>
        <translation>Liest das Update-Manifest des neuesten Releases, abgerufen über öffentliche GitHub-Mirrors. Liegt ein Update vor, werden die Änderungen angezeigt und die Release-Seite lässt sich im Browser öffnen; Herunterladen und Installieren bleibt Ihnen überlassen.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="252"/>
        <source>Already up to date (v%1)</source>
        <translation>Bereits aktuell (v%1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="260"/>
        <source> (the kernel is not running; start the proxy and try again)</source>
        <translation> (der Kernel läuft nicht; starten Sie den Proxy und versuchen Sie es erneut)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="262"/>
        <source>Update check failed: %1%2</source>
        <translation>Update-Prüfung fehlgeschlagen: %1%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="300"/>
        <source>Data folder: %1</source>
        <translation>Datenordner: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="359"/>
        <source>Reload failed after port change: %1</source>
        <translation>Neuladen nach Portänderung fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="372"/>
        <source>Failed to change startup setting: %1</source>
        <translation>Starteinstellung konnte nicht geändert werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="456"/>
        <source>Language saved; restart the app to apply.</source>
        <translation>Sprache gespeichert; starten Sie die App neu, um sie anzuwenden.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="481"/>
        <source>Checking…</source>
        <translation>Wird geprüft…</translation>
    </message>
</context>
<context>
    <name>clashqt::SubscriptionDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Add profile</source>
        <translation>Profil hinzufügen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Edit profile</source>
        <translation>Profil bearbeiten</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="84"/>
        <source>Enter the subscription URL; it will be downloaded and activated immediately.</source>
        <translation>Geben Sie die Abo-URL ein; sie wird sofort heruntergeladen und aktiviert.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="86"/>
        <source>Change the name or subscription URL. If this is the active profile, a changed URL is fetched again immediately.</source>
        <translation>Ändern Sie Name oder Abo-URL. Ist dies das aktive Profil, wird eine geänderte URL sofort erneut geladen.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="87"/>
        <source>A local profile uses an imported config file, which you can replace below.</source>
        <translation>Ein lokales Profil nutzt eine importierte Konfigurationsdatei, die Sie unten ersetzen können.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="90"/>
        <source>Profile info</source>
        <translation>Profilinformationen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="97"/>
        <source>Optional; defaults to the subscription&apos;s domain</source>
        <translation>Optional; standardmäßig die Domain des Abos</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="99"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="108"/>
        <source>Subscription URL</source>
        <translation>Abo-URL</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="110"/>
        <source>Update this profile on startup</source>
        <translation>Dieses Profil beim Start aktualisieren</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="115"/>
        <source>When enabled, the profile is refreshed automatically at startup.</source>
        <translation>Wenn aktiv, wird das Profil beim Start automatisch aktualisiert.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="127"/>
        <source>Choose another file</source>
        <translation>Andere Datei wählen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="132"/>
        <source>Config file</source>
        <translation>Konfigurationsdatei</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="133"/>
        <source>Picking a new file replaces this profile&apos;s content; its id and active state are preserved.</source>
        <translation>Eine neue Datei ersetzt den Inhalt dieses Profils; ID und Aktivstatus bleiben erhalten.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="138"/>
        <source>Choose a config file</source>
        <translation>Konfigurationsdatei wählen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="139"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Clash-Konfiguration (*.yaml *.yml);;Alle Dateien (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="153"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Add</source>
        <translation>Hinzufügen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Save</source>
        <translation>Speichern</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="193"/>
        <source>Name required</source>
        <translation>Name erforderlich</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="194"/>
        <source>Give this profile a name.</source>
        <translation>Geben Sie diesem Profil einen Namen.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="203"/>
        <source>Invalid subscription URL</source>
        <translation>Ungültige Abo-URL</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="205"/>
        <source>Enter a subscription URL starting with http/https.</source>
        <translation>Geben Sie eine Abo-URL ein, die mit http/https beginnt.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="206"/>
        <source>The subscription URL must start with http/https.</source>
        <translation>Die Abo-URL muss mit http/https beginnen.</translation>
    </message>
</context>
<context>
    <name>clashqt::TrafficPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="63"/>
        <source>Traffic</source>
        <translation>Datenverkehr</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="64"/>
        <source>Live upload/download speed and cumulative traffic; the chart shows 1-second samples from the last 2 minutes.</source>
        <translation>Aktuelle Hochlade-/Herunterladegeschwindigkeit und Gesamtdatenverkehr; das Diagramm zeigt Messwerte im Sekundentakt der letzten 2 Minuten.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="67"/>
        <source>Status</source>
        <translation>Status</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="72"/>
        <source>Total 0 B / 0 B</source>
        <translation>Gesamt 0 B / 0 B</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="73"/>
        <source>0 active connections</source>
        <translation>0 aktive Verbindungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="82"/>
        <source>Speed chart</source>
        <translation>Geschwindigkeitsdiagramm</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="96"/>
        <source>s</source>
        <translation>s</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="188"/>
        <source>Session total ↑ %1 · ↓ %2</source>
        <translation>Sitzung gesamt ↑ %1 · ↓ %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="193"/>
        <source>%1 active connection(s) · Memory %2</source>
        <translation>%1 aktive Verbindung(en) · Speicher %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="242"/>
        <source>Upload (%1)</source>
        <translation>Hochladen (%1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="243"/>
        <source>Download (%1)</source>
        <translation>Herunterladen (%1)</translation>
    </message>
</context>
<context>
    <name>clashqt::UpdateDialog</name>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="20"/>
        <source>Update available</source>
        <translation>Update verfügbar</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="30"/>
        <source>Version %1 is available</source>
        <translation>Version %1 ist verfügbar</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="31"/>
        <source>Version v%1. The changelog comes from the GitHub release; downloading and installing is up to you — this app never replaces itself.</source>
        <translation>Version v%1. Die Änderungen stammen aus dem GitHub-Release; Herunterladen und Installieren bleibt Ihnen überlassen – diese App ersetzt sich nie selbst.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="51"/>
        <source>(No release notes for this version; see the release page.)</source>
        <translation>(Keine Versionshinweise für diese Version; siehe Release-Seite.)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="97"/>
        <source>No package has been attached to this release yet; it may still be uploading.</source>
        <translation>Für dieses Release ist noch kein Paket hinterlegt; es wird möglicherweise noch hochgeladen.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="100"/>
        <source>Published on %1. Download the package from the release page.</source>
        <translation>Veröffentlicht am %1. Laden Sie das Paket von der Release-Seite herunter.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="103"/>
        <source>Download the package from the release page.</source>
        <translation>Laden Sie das Paket von der Release-Seite herunter.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="112"/>
        <source>Later</source>
        <translation>Später</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="113"/>
        <source>Open download page</source>
        <translation>Downloadseite öffnen</translation>
    </message>
</context>
<context>
    <name>clashqt::app</name>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="130"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable.</source>
        <translation>wintun.dll wurde nicht gefunden. Legen Sie wintun.dll neben die Programmdatei.</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="145"/>
        <source>Could not create the Wintun adapter</source>
        <translation>Wintun-Adapter konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="175"/>
        <source>Wintun is unavailable</source>
        <translation>Wintun ist nicht verfügbar</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="182"/>
        <source>Could not start the Wintun session</source>
        <translation>Wintun-Sitzung konnte nicht gestartet werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="65"/>
        <source>Clash Qt - a desktop client for Clash-style subscriptions</source>
        <translation>Clash Qt – ein Desktop-Client für Abos im Clash-Stil</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="71"/>
        <source>Start without showing the main window (for autostart)</source>
        <translation>Ohne Anzeige des Hauptfensters starten (für Autostart)</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="73"/>
        <source>Use the data directory next to the executable</source>
        <translation>Datenverzeichnis neben der Programmdatei verwenden</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="97"/>
        <source>Another instance is already running: asked it to show its window, exiting</source>
        <translation>Eine andere Instanz läuft bereits: Sie wurde gebeten, ihr Fenster zu zeigen; Programm wird beendet</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="108"/>
        <source>Starting %1 %2 (%3, silent=%4)</source>
        <translation>Start von %1 %2 (%3, silent=%4)</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="121"/>
        <source>Initialization failed: %1</source>
        <translation>Initialisierung fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="128"/>
        <source>UI language: %1 (configured value %2)</source>
        <translation>UI-Sprache: %1 (konfigurierter Wert %2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="148"/>
        <source>Start failed: %1</source>
        <translation>Start fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="177"/>
        <source>Silent start: not showing the main window</source>
        <translation>Stiller Start: Hauptfenster wird nicht angezeigt</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="188"/>
        <source>%1 exited with code %2</source>
        <translation>%1 wurde mit Code %2 beendet</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="72"/>
        <source>Could not read file %1: %2</source>
        <translation>Datei %1 konnte nicht gelesen werden: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="82"/>
        <source>Not a valid Clash config: %1</source>
        <translation>Keine gültige Clash-Konfiguration: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/AppLogger.cpp" line="152"/>
        <source>Removed %1 expired log file(s) (retention %2 days)</source>
        <translation>%1 abgelaufene Protokolldatei(en) entfernt (Aufbewahrung %2 Tage)</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="18"/>
        <source>Could not create the directory: %1</source>
        <translation>Verzeichnis konnte nicht erstellt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="26"/>
        <source>Could not write %1: %2</source>
        <translation>%1 konnte nicht geschrieben werden: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="32"/>
        <source>Incomplete write to %1: %2</source>
        <translation>Unvollständiges Schreiben von %1: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="38"/>
        <source>Failed to commit %1: %2</source>
        <translation>%1 konnte nicht übernommen werden: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="70"/>
        <source>Used up</source>
        <translation>Aufgebraucht</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="77"/>
        <source>%1 d %2 h</source>
        <translation>%1 T. %2 Std.</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="78"/>
        <source>%1 h %2 min</source>
        <translation>%1 Std. %2 Min.</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="79"/>
        <source>%1 min</source>
        <translation>%1 Min.</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="208"/>
        <source>Failed to parse the app config; using defaults: %1</source>
        <translation>App-Konfiguration konnte nicht gelesen werden; Standardwerte werden verwendet: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="228"/>
        <source>Failed to write back the app config: %1</source>
        <translation>App-Konfiguration konnte nicht zurückgeschrieben werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="238"/>
        <source>Failed to save the app config: %1</source>
        <translation>App-Konfiguration konnte nicht gespeichert werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="226"/>
        <source>Failed to parse the legacy managed config (%1): %2</source>
        <translation>Alte verwaltete Konfiguration (%1) konnte nicht gelesen werden: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="232"/>
        <source>Merged the legacy managed config into config.yaml and deleted it: %1</source>
        <translation>Alte verwaltete Konfiguration in config.yaml zusammengeführt und gelöscht: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="134"/>
        <source>Failed to parse the profile list: %1</source>
        <translation>Profilliste konnte nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="157"/>
        <source>Failed to save the profile list: %1</source>
        <translation>Profilliste konnte nicht gespeichert werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="242"/>
        <source>Failed to write profile content (%1): %2</source>
        <translation>Profilinhalt konnte nicht geschrieben werden (%1): %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="74"/>
        <source>Failed to parse the override list: %1</source>
        <translation>Überschreibungsliste konnte nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="92"/>
        <source>Failed to save the override list: %1</source>
        <translation>Überschreibungsliste konnte nicht gespeichert werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="156"/>
        <source>Failed to write override content (%1.%2): %3</source>
        <translation>Überschreibungsinhalt konnte nicht geschrieben werden (%1.%2): %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="98"/>
        <source>References an override that does not exist: %1</source>
        <translation>Verweist auf eine nicht vorhandene Überschreibung: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="107"/>
        <source>Override %1 has no content file</source>
        <translation>Überschreibung %1 hat keine Inhaltsdatei</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="116"/>
        <source>Override &quot;%1&quot; is a JS script, and the JS override engine is not supported yet; it was skipped (the config will not apply it)</source>
        <translation>Überschreibung „%1“ ist ein JS-Skript, und die JS-Überschreibungs-Engine wird noch nicht unterstützt; sie wurde übersprungen (die Konfiguration wendet sie nicht an)</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="126"/>
        <source>Failed to parse override &quot;%1&quot;: %2</source>
        <translation>Überschreibung „%1“ konnte nicht gelesen werden: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="133"/>
        <source>The root of override &quot;%1&quot; must be a mapping</source>
        <translation>Die Wurzel der Überschreibung „%1“ muss eine Map sein</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="159"/>
        <source>Failed to parse the rule-override file: %1</source>
        <translation>Regel-Überschreibungsdatei konnte nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="228"/>
        <source>No profile selected yet. Add one on the Profiles page and select it first.</source>
        <translation>Noch kein Profil ausgewählt. Fügen Sie auf der Seite „Profile“ eines hinzu und wählen Sie es zuerst aus.</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="239"/>
        <source>Profile &quot;%1&quot; is empty or missing</source>
        <translation>Profil „%1“ ist leer oder fehlt</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="249"/>
        <source>Failed to parse the profile content: %1</source>
        <translation>Profilinhalt konnte nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="356"/>
        <source>Failed to write the config: %1</source>
        <translation>Konfiguration konnte nicht geschrieben werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="361"/>
        <source>Generated the runtime config: %1 (profile %2)</source>
        <translation>Laufzeitkonfiguration erzeugt: %1 (Profil %2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="279"/>
        <source>Request timed out</source>
        <translation>Zeitüberschreitung bei der Anfrage</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="281"/>
        <source>The release has no update manifest</source>
        <translation>Das Release enthält kein Update-Manifest</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="341"/>
        <source>No update manifest address available</source>
        <translation>Keine Adresse für das Update-Manifest verfügbar</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="354"/>
        <source>Could not parse the manifest: %1</source>
        <translation>Manifest konnte nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="363"/>
        <source>The manifest has no usable version field</source>
        <translation>Das Manifest enthält kein nutzbares Versionsfeld</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="234"/>
        <source>System-proxy backend: %1</source>
        <translation>Systemproxy-Backend: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="258"/>
        <source>System-proxy backend: %1 (XDG_CURRENT_DESKTOP=%2)</source>
        <translation>Systemproxy-Backend: %1 (XDG_CURRENT_DESKTOP=%2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="273"/>
        <source>unavailable</source>
        <translation>nicht verfügbar</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="327"/>
        <source>Could not write ProxyServer to the registry</source>
        <translation>ProxyServer konnte nicht in die Registrierung geschrieben werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="335"/>
        <source>Failed to write the system-proxy ignore list</source>
        <translation>Ausnahmeliste des Systemproxys konnte nicht geschrieben werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="339"/>
        <source>Could not enable the system proxy in the registry</source>
        <translation>Systemproxy konnte in der Registrierung nicht aktiviert werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="350"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="378"/>
        <source>System proxy enabled (%1): %2:%3</source>
        <translation>Systemproxy aktiviert (%1): %2:%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="356"/>
        <source>No usable system-proxy backend detected (gsettings or kwriteconfig5/6 is required)</source>
        <translation>Kein nutzbares Systemproxy-Backend gefunden (gsettings oder kwriteconfig5/6 erforderlich)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="372"/>
        <source>Failed to enable the system proxy: %1</source>
        <translation>Systemproxy konnte nicht aktiviert werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="387"/>
        <source>Could not turn off the system proxy in the registry</source>
        <translation>Systemproxy konnte in der Registrierung nicht deaktiviert werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="398"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="423"/>
        <source>System proxy disabled</source>
        <translation>Systemproxy deaktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="402"/>
        <source>No usable system-proxy backend detected</source>
        <translation>Kein nutzbares Systemproxy-Backend gefunden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="418"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>Systemproxy konnte nicht deaktiviert werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="438"/>
        <source>Could not start %1</source>
        <translation>%1 konnte nicht gestartet werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="443"/>
        <source>%1 timed out</source>
        <translation>Zeitüberschreitung bei %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="448"/>
        <source>%1 failed: %2</source>
        <translation>%1 fehlgeschlagen: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="515"/>
        <source>Failed to write the system-proxy ignore list: %1</source>
        <translation>Ausnahmeliste des Systemproxys konnte nicht geschrieben werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="577"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="620"/>
        <source>kwriteconfig5/6 not found</source>
        <translation>kwriteconfig5/6 nicht gefunden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="53"/>
        <source>Could not open the autostart registry key</source>
        <translation>Autostart-Registrierungsschlüssel konnte nicht geöffnet werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="68"/>
        <source>Could not write the autostart entry (error %1)</source>
        <translation>Autostart-Eintrag konnte nicht geschrieben werden (Fehler %1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="109"/>
        <source>Could not create the autostart directory</source>
        <translation>Autostart-Verzeichnis konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="161"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="193"/>
        <source>Failed to enable autostart: %1</source>
        <translation>Autostart konnte nicht aktiviert werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="167"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="196"/>
        <source>Autostart enabled: %1</source>
        <translation>Autostart aktiviert: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="215"/>
        <source>Could not remove the autostart entry (error %1)</source>
        <translation>Autostart-Eintrag konnte nicht entfernt werden (Fehler %1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="224"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="235"/>
        <source>Autostart disabled</source>
        <translation>Autostart deaktiviert</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="231"/>
        <source>Could not remove the autostart entry: %1</source>
        <translation>Autostart-Eintrag konnte nicht entfernt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SingleInstance.cpp" line="53"/>
        <source>The single-instance guard failed to start (%1); running as multiple instances this time: %2</source>
        <translation>Die Einzelinstanz-Sperre konnte nicht starten (%1); diesmal laufen mehrere Instanzen: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="117"/>
        <source>Could not set the TUN adapter address</source>
        <translation>Adresse des TUN-Adapters konnte nicht gesetzt werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="134"/>
        <source>Could not read the TUN adapter settings</source>
        <translation>Einstellungen des TUN-Adapters konnten nicht gelesen werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="150"/>
        <source>Could not set the TUN adapter MTU/metric</source>
        <translation>MTU/Metrik des TUN-Adapters konnte nicht gesetzt werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="175"/>
        <source>Could not add the TUN route</source>
        <translation>TUN-Route konnte nicht hinzugefügt werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="203"/>
        <source>Could not start netsh</source>
        <translation>netsh konnte nicht gestartet werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="208"/>
        <source>netsh timed out</source>
        <translation>Zeitüberschreitung bei netsh</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="312"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="383"/>
        <source>TUN mode on Windows requires administrator privileges. Close the app and start it again with &quot;Run as administrator&quot;.</source>
        <translation>Der TUN-Modus erfordert unter Windows Administratorrechte. Schließen Sie die App und starten Sie sie erneut mit „Als Administrator ausführen“.</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="321"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="513"/>
        <source>Helper not found: %1</source>
        <translation>Helfer nicht gefunden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="337"/>
        <source>Could not start pkexec: %1</source>
        <translation>pkexec konnte nicht gestartet werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="344"/>
        <source>Authorization timed out (the password prompt may have been cancelled)</source>
        <translation>Zeitüberschreitung bei der Autorisierung (die Passwortabfrage wurde möglicherweise abgebrochen)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="349"/>
        <source>%1 failed (exit code %2): %3</source>
        <translation>%1 fehlgeschlagen (Exit-Code %2): %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="359"/>
        <source>TUN permissions granted (chown root:root + chmod +sx): %1</source>
        <translation>TUN-Berechtigungen erteilt (chown root:root + chmod +sx): %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="369"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="509"/>
        <source>TUN is starting or already running</source>
        <translation>TUN startet gerade oder läuft bereits</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="389"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable and try again.</source>
        <translation>wintun.dll wurde nicht gefunden. Legen Sie wintun.dll neben die Programmdatei und versuchen Sie es erneut.</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="402"/>
        <source>Invalid TUN address: %1</source>
        <translation>Ungültige TUN-Adresse: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="410"/>
        <source>Could not create the TUN adapter</source>
        <translation>TUN-Adapter konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="415"/>
        <source>Could not read the TUN adapter LUID</source>
        <translation>LUID des TUN-Adapters konnte nicht gelesen werden</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="448"/>
        <source>Invalid fake-ip CIDR: %1</source>
        <translation>Ungültiges fake-ip-CIDR: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="468"/>
        <source>Could not point system DNS at the TUN address: %1</source>
        <translation>System-DNS konnte nicht auf die TUN-Adresse gesetzt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="481"/>
        <source>TUN adapter ready: %1 %2/%3, MTU %4, routes %5, Wintun %6</source>
        <translation>TUN-Adapter bereit: %1 %2/%3, MTU %4, Routen %5, Wintun %6</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="487"/>
        <source>taken over</source>
        <translation>übernommen</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="488"/>
        <source>not taken over</source>
        <translation>nicht übernommen</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="505"/>
        <source>Found a stale TUN-enabled state; cleaned it up and retried</source>
        <translation>Veralteten TUN-Status gefunden; bereinigt und erneut versucht</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="517"/>
        <source>TUN permission not granted yet (enter the password in the prompt that appears)</source>
        <translation>TUN-Berechtigung noch nicht erteilt (geben Sie das Passwort in der erscheinenden Abfrage ein)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="542"/>
        <source>Socket path too long</source>
        <translation>Socket-Pfad zu lang</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="553"/>
        <source>Failed to listen on the socket: %1</source>
        <translation>Lauschen auf dem Socket fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="583"/>
        <source>Could not start the helper: %1</source>
        <translation>Helfer konnte nicht gestartet werden: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="595"/>
        <source>Timed out waiting for the helper (20 s)</source>
        <translation>Zeitüberschreitung beim Warten auf den Helfer (20 s)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="611"/>
        <source>The helper exited unexpectedly (exit code %1)</source>
        <translation>Der Helfer wurde unerwartet beendet (Exit-Code %1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="616"/>
        <source>Started the TUN helper; waiting for the device: %1</source>
        <translation>TUN-Helfer gestartet; warte auf das Gerät: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="618"/>
        <source>TUN handshake details: socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</source>
        <translation>TUN-Handshake-Details: socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="700"/>
        <source>The TUN helper disconnected</source>
        <translation>Die Verbindung zum TUN-Helfer wurde getrennt</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="749"/>
        <source>The helper did not send a TUN fd</source>
        <translation>Der Helfer hat kein TUN-fd gesendet</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="760"/>
        <source>TUN ready: %1 (%2)</source>
        <translation>TUN bereit: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="762"/>
        <source>real device</source>
        <translation>echtes Gerät</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="815"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="868"/>
        <source>TUN is closed</source>
        <translation>TUN ist geschlossen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="778"/>
        <source>System tray unavailable; skipping tray setup</source>
        <translation>Systemtray nicht verfügbar; Tray-Einrichtung wird übersprungen</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="767"/>
        <source>UI style: Fusion (%1) style=%2 field=%3 accent=%4 desktop dark=%5</source>
        <translation>UI-Stil: Fusion (%1) style=%2 field=%3 accent=%4 desktop dark=%5</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>dark</source>
        <translation>dunkel</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>light</source>
        <translation>hell</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>yes</source>
        <translation>ja</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>no</source>
        <translation>nein</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/Icons.cpp" line="53"/>
        <source>Icon resource missing: %1 (check resources/clash-qt.qrc)</source>
        <translation>Icon-Ressource fehlt: %1 (prüfen Sie resources/clash-qt.qrc)</translation>
    </message>
</context>
<context>
    <name>clashqt::core</name>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="141"/>
        <source>YAML parse failed: %1</source>
        <translation>YAML-Parsing fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="165"/>
        <source>YAML emit failed: %1</source>
        <translation>YAML-Ausgabe fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="312"/>
        <source>Unknown error</source>
        <translation>Unbekannter Fehler</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="549"/>
        <source>Stream closed</source>
        <translation>Stream geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="566"/>
        <source>Write timed out (flow-control window stayed closed)</source>
        <translation>Zeitüberschreitung beim Schreiben (Flow-Control-Fenster blieb geschlossen)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="570"/>
        <source>Session released</source>
        <translation>Sitzung freigegeben</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="627"/>
        <source>Session release</source>
        <translation>Sitzungsfreigabe</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="656"/>
        <source>Could not parse server address %1 (%2)</source>
        <translation>Serveradresse %1 konnte nicht gelesen werden (%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="676"/>
        <source>Failed to create the UDP socket: %1</source>
        <translation>UDP-Socket konnte nicht erstellt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="689"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="700"/>
        <source>UDP connect failed: %1</source>
        <translation>UDP-Verbindung fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="714"/>
        <source>Failed to create the QUIC context</source>
        <translation>QUIC-Kontext konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="728"/>
        <source>Failed to create the QUIC connection</source>
        <translation>QUIC-Verbindung konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="741"/>
        <source>Failed to bind the UDP socket</source>
        <translation>UDP-Socket konnte nicht gebunden werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="756"/>
        <source>Failed to read the peer address: %1</source>
        <translation>Peer-Adresse konnte nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="776"/>
        <source>Failed to build the initial QUIC peer address</source>
        <translation>Anfängliche QUIC-Peer-Adresse konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="791"/>
        <source>QUIC handshake failed: %1</source>
        <translation>QUIC-Handshake fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="805"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="861"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="887"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="894"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="947"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1089"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1152"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1168"/>
        <source>QUIC event handling failed: %1</source>
        <translation>QUIC-Ereignisverarbeitung fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="810"/>
        <source>QUIC handshake timed out (port %1)</source>
        <translation>Zeitüberschreitung beim QUIC-Handshake (Port %1)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="820"/>
        <source>The server provided no certificate</source>
        <translation>Der Server hat kein Zertifikat geliefert</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="831"/>
        <source>Server certificate fingerprint does not match the config (fingerprint check failed)</source>
        <translation>Der Zertifikats-Fingerabdruck des Servers passt nicht zur Konfiguration (Prüfung fehlgeschlagen)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="852"/>
        <source>Failed to create a QUIC stream: %1</source>
        <translation>QUIC-Stream konnte nicht erstellt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="856"/>
        <source>Timed out creating a QUIC stream</source>
        <translation>Zeitüberschreitung beim Erstellen eines QUIC-Streams</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="876"/>
        <source>Failed to write to a QUIC stream: %1</source>
        <translation>Schreiben in einen QUIC-Stream fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="881"/>
        <source>Timed out writing to a QUIC stream</source>
        <translation>Zeitüberschreitung beim Schreiben in einen QUIC-Stream</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="971"/>
        <source>No response to authentication (timeout or connection reset, port %1)</source>
        <translation>Keine Antwort auf die Authentifizierung (Zeitüberschreitung oder Verbindungsabbruch, Port %1)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="979"/>
        <source>Authentication rejected (HTTP status %1; check the password)</source>
        <translation>Authentifizierung abgelehnt (HTTP-Status %1; prüfen Sie das Passwort)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1008"/>
        <source>Node is missing server</source>
        <translation>Beim Knoten fehlt der Server</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1012"/>
        <source>Node is missing password</source>
        <translation>Beim Knoten fehlt das Passwort</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1019"/>
        <source>Socket setup</source>
        <translation>Socket-Einrichtung</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1020"/>
        <source>Handshake</source>
        <translation>Handshake</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1023"/>
        <source>Authentication</source>
        <translation>Authentifizierung</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1034"/>
        <source>Session unavailable</source>
        <translation>Sitzung nicht verfügbar</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1038"/>
        <source>Invalid target address</source>
        <translation>Ungültige Zieladresse</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1106"/>
        <source>Target %1 did not respond (timeout)</source>
        <translation>Ziel %1 antwortet nicht (Zeitüberschreitung)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1114"/>
        <source>Connection to target %1 refused</source>
        <translation>Verbindung zum Ziel %1 abgelehnt</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1115"/>
        <source>Connection to target %1 refused: %2</source>
        <translation>Verbindung zum Ziel %1 abgelehnt: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="29"/>
        <source>Read more than %1 bytes without finding the delimiter</source>
        <translation>Mehr als %1 Bytes gelesen, ohne das Trennzeichen zu finden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="42"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="87"/>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="247"/>
        <source>Connection closed</source>
        <translation>Verbindung geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="48"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="93"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="329"/>
        <source>Read timed out</source>
        <translation>Zeitüberschreitung beim Lesen</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="62"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="104"/>
        <source>Read failed: %1</source>
        <translation>Lesen fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="126"/>
        <source>Write failed: %1</source>
        <translation>Schreiben fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="133"/>
        <source>Write timed out</source>
        <translation>Zeitüberschreitung beim Schreiben</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="145"/>
        <source>Connection closed by the peer</source>
        <translation>Verbindung vom Peer geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="461"/>
        <source>REALITY connection to %1:%2 failed: %3</source>
        <translation>REALITY-Verbindung zu %1:%2 fehlgeschlagen: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="487"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="508"/>
        <source>X25519 key generation failed</source>
        <translation>X25519-Schlüsselerzeugung fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="514"/>
        <source>REALITY static ECDH failed</source>
        <translation>REALITY statisches ECDH fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="521"/>
        <source>Failed to build ClientHello</source>
        <translation>ClientHello konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="552"/>
        <source>Failed to generate REALITY authentication data</source>
        <translation>REALITY-Authentifizierungsdaten konnten nicht erzeugt werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="571"/>
        <source>REALITY received no ServerHello (record type=%1 length=%2 content=%3)</source>
        <translation>REALITY erhielt kein ServerHello (record type=%1 length=%2 content=%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="582"/>
        <source>Truncated ServerHello</source>
        <translation>Abgeschnittenes ServerHello</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="618"/>
        <source>ServerHello contains no usable key_share</source>
        <translation>ServerHello enthält kein nutzbares key_share</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="624"/>
        <source>TLS negotiated: cipher=0x%1 key-exchange group=%2</source>
        <translation>TLS ausgehandelt: cipher=0x%1 key-exchange group=%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="636"/>
        <source>The REALITY server negotiated an unsupported cipher 0x%1</source>
        <translation>Der REALITY-Server hat eine nicht unterstützte Cipher 0x%1 ausgehandelt</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="704"/>
        <source>Unexpected record type %1 during the REALITY handshake</source>
        <translation>Unerwarteter Record-Typ %1 während des REALITY-Handshakes</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="713"/>
        <source>Failed to decrypt a REALITY handshake record (type=%1 length=%2 seq=%3)</source>
        <translation>REALITY-Handshake-Record konnte nicht entschlüsselt werden (type=%1 length=%2 seq=%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="717"/>
        <source>Failed to decrypt a REALITY handshake record</source>
        <translation>REALITY-Handshake-Record konnte nicht entschlüsselt werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="768"/>
        <source>The server returned no certificate</source>
        <translation>Der Server hat kein Zertifikat zurückgegeben</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="799"/>
        <source>TLS certificate verification failed (chain=%1 hostname=%2)</source>
        <translation>TLS-Zertifikatsprüfung fehlgeschlagen (chain=%1 hostname=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="800"/>
        <source>Pass</source>
        <translation>Bestanden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="801"/>
        <source>Fail</source>
        <translation>Fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="802"/>
        <source>Match</source>
        <translation>Übereinstimmung</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="803"/>
        <source>No match</source>
        <translation>Keine Übereinstimmung</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="809"/>
        <source>Node %1 skips TLS certificate verification</source>
        <translation>Knoten %1 überspringt die TLS-Zertifikatsprüfung</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="818"/>
        <source>REALITY authentication failed (the peer is the borrowed site itself rather than a REALITY server, or public-key / short-id is misconfigured)</source>
        <translation>REALITY-Authentifizierung fehlgeschlagen (der Peer ist die geliehene Website selbst und kein REALITY-Server, oder public-key / short-id ist falsch konfiguriert)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="830"/>
        <source>Server Finished verification failed</source>
        <translation>Prüfung der Server-Finished-Nachricht fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="993"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1008"/>
        <source>REALITY send failed: %1</source>
        <translation>REALITY-Senden fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1022"/>
        <source>Timed out reading a REALITY record</source>
        <translation>Zeitüberschreitung beim Lesen eines REALITY-Records</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1028"/>
        <source>REALITY failed to read a record: %1</source>
        <translation>REALITY konnte einen Record nicht lesen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1044"/>
        <source>Invalid REALITY record length %1</source>
        <translation>Ungültige REALITY-Record-Länge %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1079"/>
        <source>Invalid REALITY application-record length</source>
        <translation>Ungültige Länge des REALITY-Anwendungs-Records</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1099"/>
        <source>Failed to decrypt a REALITY record</source>
        <translation>REALITY-Record konnte nicht entschlüsselt werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1251"/>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="390"/>
        <source>Timed out reading the response</source>
        <translation>Zeitüberschreitung beim Lesen der Antwort</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1256"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="324"/>
        <source>Connection closed while reading</source>
        <translation>Verbindung wurde beim Lesen geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="73"/>
        <source>Failed to write the upgrade request: %1</source>
        <translation>Upgrade-Anfrage konnte nicht geschrieben werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="77"/>
        <source>WS upgrade request: sent %1 bytes to %2:%3 (%4 pending, state %5)</source>
        <translation>WS-Upgrade-Anfrage: %1 Bytes an %2:%3 gesendet (%4 ausstehend, Status %5)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="87"/>
        <source>%1 (received %2 bytes: %3)</source>
        <translation>%1 (%2 Bytes empfangen: %3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="101"/>
        <source>WebSocket handshake rejected: %1
%2</source>
        <translation>WebSocket-Handshake abgelehnt: %1
%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="116"/>
        <source>WebSocket handshake verification failed (Sec-WebSocket-Accept mismatch)</source>
        <translation>WebSocket-Handshake-Prüfung fehlgeschlagen (Sec-WebSocket-Accept stimmt nicht überein)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="129"/>
        <source>The WebSocket handshake response carried extra data, which is not supported</source>
        <translation>Die WebSocket-Handshake-Antwort enthielt zusätzliche Daten, was nicht unterstützt wird</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="227"/>
        <source>The peer sent a WebSocket close frame</source>
        <translation>Der Peer hat ein WebSocket-Close-Frame gesendet</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="236"/>
        <source>Fragmented WebSocket messages are not supported</source>
        <translation>Fragmentierte WebSocket-Nachrichten werden nicht unterstützt</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="262"/>
        <source>Timed out reading a WebSocket frame</source>
        <translation>Zeitüberschreitung beim Lesen eines WebSocket-Frames</translation>
    </message>
    <message>
        <location filename="../src/core/src/config/RawConfig.cpp" line="81"/>
        <source>The config root must be a mapping</source>
        <translation>Die Wurzel der Konfiguration muss eine Map sein</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/cidr/IpCidrSet.cpp" line="12"/>
        <source>Ignoring an invalid CIDR rule entry: %1</source>
        <translation>Ungültiger CIDR-Regel-Eintrag wird ignoriert: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="68"/>
        <source>DNS listener not started: %1</source>
        <translation>DNS-Listener nicht gestartet: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="77"/>
        <source>Not listening</source>
        <translation>Lauscht nicht</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="86"/>
        <source>dns.listen is not configured</source>
        <translation>dns.listen ist nicht konfiguriert</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="95"/>
        <source>Failed to bind UDP %1:%2: %3</source>
        <translation>UDP-Bind auf %1:%2 fehlgeschlagen: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="139"/>
        <source>DNS listener started: %1 (enhanced-mode=%2)</source>
        <translation>DNS-Listener gestartet: %1 (enhanced-mode=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="199"/>
        <source>DNS fake-ip: AAAA query for %1 returned an empty answer</source>
        <translation>DNS fake-ip: AAAA-Anfrage für %1 lieferte eine leere Antwort</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="222"/>
        <source>DNS real resolution: %1 -&gt; %2 record(s)</source>
        <translation>DNS-Echtauflösung: %1 -&gt; %2 Datensatz/Datensätze</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="285"/>
        <source>Invalid or non-IPv4 fake-ip-range: %1; falling back to 198.18.0.1/16</source>
        <translation>Ungültiger oder nicht-IPv4 fake-ip-range: %1; es wird auf 198.18.0.1/16 zurückgegriffen</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="545"/>
        <source>DNS configured: enhanced-mode=%1, nameserver=%2, fake-ip=%3</source>
        <translation>DNS konfiguriert: enhanced-mode=%1, nameserver=%2, fake-ip=%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="915"/>
        <source>DoT (tls://) currently connects over plain TCP to %1</source>
        <translation>DoT (tls://) verbindet derzeit über einfaches TCP zu %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="979"/>
        <source>Could not resolve the DNS upstream host through the bootstrap server: %1</source>
        <translation>Der DNS-Upstream-Host konnte über den Bootstrap-Server nicht aufgelöst werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1004"/>
        <source>Could not parse the DNS server config: %1</source>
        <translation>DNS-Server-Konfiguration konnte nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1045"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="487"/>
        <source>DNS resolution failed: %1</source>
        <translation>DNS-Auflösung fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="92"/>
        <source>Malformed logical-rule sub-item: %1</source>
        <translation>Fehlerhaftes Unterelement einer logischen Regel: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="103"/>
        <source>Failed to parse a logical-rule sub-item: %1 (%2)</source>
        <translation>Unterelement einer logischen Regel konnte nicht gelesen werden: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="119"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="125"/>
        <source>Empty rule</source>
        <translation>Leere Regel</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="133"/>
        <source>Unknown rule type: %1</source>
        <translation>Unbekannter Regeltyp: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="141"/>
        <source>MATCH rule is missing the target policy</source>
        <translation>Der MATCH-Regel fehlt die Zielrichtlinie</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="149"/>
        <source>Rule has too few fields (expected type,value,policy): %1</source>
        <translation>Regel hat zu wenige Felder (erwartet: type,value,policy): %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="197"/>
        <source>Invalid domain regular expression: %1</source>
        <translation>Ungültiger regulärer Ausdruck für Domain: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="213"/>
        <source>Invalid IP CIDR: %1</source>
        <translation>Ungültiges IP-CIDR: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="225"/>
        <source>Invalid source IP CIDR: %1</source>
        <translation>Ungültiges Quell-IP-CIDR: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="251"/>
        <source>Invalid port: %1</source>
        <translation>Ungültiger Port: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="261"/>
        <source>Invalid source port: %1</source>
        <translation>Ungültiger Quellport: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="271"/>
        <source>Invalid inbound port: %1</source>
        <translation>Ungültiger eingehender Port: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="288"/>
        <source>Invalid process-match expression: %1</source>
        <translation>Ungültiger Prozess-Match-Ausdruck: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="298"/>
        <source>Invalid UID: %1</source>
        <translation>Ungültige UID: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="310"/>
        <source>References an undefined rule set: %1</source>
        <translation>Verweist auf einen nicht definierten Regelsatz: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="325"/>
        <source>Logical rule has no usable sub-rules: %1</source>
        <translation>Logische Regel hat keine nutzbaren Unterregeln: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="340"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="346"/>
        <source>Unsupported rule type: %1</source>
        <translation>Nicht unterstützter Regeltyp: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="352"/>
        <source>Failed to build the rule: %1</source>
        <translation>Regel konnte nicht erstellt werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="369"/>
        <source>Skipped a non-string rule entry</source>
        <translation>Regel-Eintrag ohne Text übersprungen</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="420"/>
        <source>Could not parse an entry in rule set %1: %2</source>
        <translation>Eintrag in Regelsatz %1 konnte nicht gelesen werden: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="93"/>
        <source>Failed to connect to %1:%2: %3 (error=%4)</source>
        <translation>Verbindung zu %1:%2 fehlgeschlagen: %3 (error=%4)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="177"/>
        <source>Invalid DIRECT target address</source>
        <translation>Ungültige DIRECT-Zieladresse</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="186"/>
        <source>DIRECT UDP requires a literal IP target</source>
        <translation>DIRECT-UDP erfordert ein direktes IP-Ziel</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="191"/>
        <source>UDP bind failed: %1</source>
        <translation>UDP-Bind fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="224"/>
        <source>Connection rejected by a rule (REJECT)</source>
        <translation>Verbindung durch eine Regel abgelehnt (REJECT)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="242"/>
        <source>HTTP proxy %1 claims https; connecting with plaintext CONNECT for now</source>
        <translation>HTTP-Proxy %1 gibt https an; es wird vorerst unverschlüsselt per CONNECT verbunden</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="250"/>
        <source>HTTP proxy %1 is missing server/port</source>
        <translation>HTTP-Proxy %1 fehlt Server/Port</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="261"/>
        <source>The HTTP proxy does not support UDP forwarding</source>
        <translation>Der HTTP-Proxy unterstützt keine UDP-Weiterleitung</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="286"/>
        <source>HTTP proxy %1 did not respond, or its response headers are too long</source>
        <translation>HTTP-Proxy %1 antwortet nicht, oder seine Antwort-Header sind zu lang</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="300"/>
        <source>HTTP proxy returned %1</source>
        <translation>HTTP-Proxy lieferte %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="335"/>
        <source>SOCKS5 proxy %1 is missing server/port</source>
        <translation>SOCKS5-Proxy %1 fehlt Server/Port</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="356"/>
        <source>No response to SOCKS5 negotiation</source>
        <translation>Keine Antwort auf die SOCKS5-Aushandlung</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="362"/>
        <source>SOCKS5 version mismatch</source>
        <translation>SOCKS5-Version stimmt nicht überein</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="368"/>
        <source>The SOCKS5 server rejected all authentication methods</source>
        <translation>Der SOCKS5-Server hat alle Authentifizierungsmethoden abgelehnt</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="377"/>
        <source>SOCKS5 username or password too long</source>
        <translation>SOCKS5-Benutzername oder -Passwort zu lang</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="391"/>
        <source>No response to SOCKS5 authentication</source>
        <translation>Keine Antwort auf die SOCKS5-Authentifizierung</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="396"/>
        <source>SOCKS5 authentication failed</source>
        <translation>SOCKS5-Authentifizierung fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="401"/>
        <source>The SOCKS5 server requires an unsupported auth method 0x%1</source>
        <translation>Der SOCKS5-Server verlangt eine nicht unterstützte Auth-Methode 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="425"/>
        <source>SOCKS5 target domain name too long</source>
        <translation>SOCKS5-Ziel-Domainname zu lang</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="442"/>
        <source>Could not resolve the SOCKS5 target address: %1</source>
        <translation>SOCKS5-Zieladresse konnte nicht aufgelöst werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="455"/>
        <source>No response to SOCKS5 CONNECT</source>
        <translation>Keine Antwort auf SOCKS5 CONNECT</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="464"/>
        <source>Succeeded</source>
        <translation>Erfolgreich</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="465"/>
        <source>General SOCKS server failure</source>
        <translation>Allgemeiner SOCKS-Serverfehler</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="466"/>
        <source>Connection not allowed by ruleset</source>
        <translation>Verbindung durch Regelsatz nicht erlaubt</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="467"/>
        <source>Network unreachable</source>
        <translation>Netzwerk nicht erreichbar</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="468"/>
        <source>Host unreachable</source>
        <translation>Host nicht erreichbar</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="469"/>
        <source>Connection refused</source>
        <translation>Verbindung abgelehnt</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="470"/>
        <source>TTL expired</source>
        <translation>TTL abgelaufen</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="471"/>
        <source>Command not supported</source>
        <translation>Befehl nicht unterstützt</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="472"/>
        <source>Address type not supported</source>
        <translation>Adresstyp nicht unterstützt</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="475"/>
        <source>Unknown error %1</source>
        <translation>Unbekannter Fehler %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="477"/>
        <source>SOCKS5 connection refused: %1 (%2)</source>
        <translation>SOCKS5-Verbindung abgelehnt: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="491"/>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="503"/>
        <source>Truncated SOCKS5 response</source>
        <translation>Abgeschnittene SOCKS5-Antwort</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="498"/>
        <source>The SOCKS5 response contains an unknown address type %1</source>
        <translation>Die SOCKS5-Antwort enthält einen unbekannten Adresstyp %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="521"/>
        <source>SOCKS5 UDP ASSOCIATE forwarding is not implemented yet (Phase 2)</source>
        <translation>SOCKS5-UDP-ASSOCIATE-Weiterleitung ist noch nicht implementiert (Phase 2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="560"/>
        <source>Node %1 (%2) is not supported yet: %3</source>
        <translation>Knoten %1 (%2) wird noch nicht unterstützt: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="632"/>
        <source>Group %1 switched to %2</source>
        <translation>Gruppe %1 gewechselt zu %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="645"/>
        <source>Proxy group %1 has no available members</source>
        <translation>Proxy-Gruppe %1 hat keine verfügbaren Mitglieder</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="658"/>
        <source>Group %1 member %2 failed to connect: %3</source>
        <translation>Mitglied %2 von Gruppe %1 konnte nicht verbinden: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="67"/>
        <source>This protocol is not implemented yet</source>
        <translation>Dieses Protokoll ist noch nicht implementiert</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="82"/>
        <source>Skipped non-mapping entries in proxies</source>
        <translation>Einträge ohne Map-Struktur in proxies übersprungen</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="91"/>
        <source>Unrecognized node: %1 (type=%2)</source>
        <translation>Unbekannter Knoten: %1 (type=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="92"/>
        <source>&lt;unnamed&gt;</source>
        <translation>&lt;unbenannt&gt;</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="96"/>
        <source>Duplicate node name; ignoring the later definition: %1</source>
        <translation>Doppelter Knotenname; spätere Definition wird ignoriert: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="115"/>
        <source>Unrecognized proxy group: %1 (type=%2)</source>
        <translation>Unbekannte Proxy-Gruppe: %1 (type=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="154"/>
        <source>Proxy group %1 has no members</source>
        <translation>Proxy-Gruppe %1 hat keine Mitglieder</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="168"/>
        <source>Proxy group %1 has members that cannot be resolved (a missing node or a reference cycle); skipped. Members: %2</source>
        <translation>Proxy-Gruppe %1 enthält nicht auflösbare Mitglieder (fehlender Knoten oder Referenzzyklus); übersprungen. Mitglieder: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="180"/>
        <source>Adapters loaded: %1 nodes/groups, %2 unimplemented protocols, %3 warnings</source>
        <translation>Adapter geladen: %1 Knoten/Gruppen, %2 nicht implementierte Protokolle, %3 Warnungen</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="197"/>
        <source>The config defines its own GLOBAL group; using it as-is</source>
        <translation>Die Konfiguration definiert eine eigene GLOBAL-Gruppe; sie wird unverändert verwendet</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="231"/>
        <source>Built the built-in GLOBAL group with %1 member(s)</source>
        <translation>Integrierte GLOBAL-Gruppe mit %1 Mitglied(ern) erstellt</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="234"/>
        <source>The profile has no usable node; the GLOBAL group only contains DIRECT</source>
        <translation>Das Profil hat keinen nutzbaren Knoten; die GLOBAL-Gruppe enthält nur DIRECT</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="52"/>
        <source>fingerprint is not a valid SHA-256 hex string</source>
        <translation>fingerprint ist keine gültige SHA-256-Hex-Zeichenkette</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="59"/>
        <source>obfs=%1 is set (Salamander obfuscation is not implemented yet)</source>
        <translation>obfs=%1 ist gesetzt (Salamander-Verschleierung ist noch nicht implementiert)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="63"/>
        <source>realm-opts is set (realm is not implemented yet)</source>
        <translation>realm-opts ist gesetzt (realm ist noch nicht implementiert)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="67"/>
        <source>Missing password</source>
        <translation>Passwort fehlt</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="78"/>
        <source>Node %1 does not support UDP relay yet (Hysteria2 needs QUIC DATAGRAM, which OpenSSL 3.2 does not provide)</source>
        <translation>Knoten %1 unterstützt noch keine UDP-Weiterleitung (Hysteria2 benötigt QUIC DATAGRAM, das OpenSSL 3.2 nicht bietet)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="84"/>
        <source>Node %1 is not supported yet: %2</source>
        <translation>Knoten %1 wird noch nicht unterstützt: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="92"/>
        <source>Node %1 has an invalid target address</source>
        <translation>Knoten %1 hat eine ungültige Zieladresse</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="133"/>
        <source>Node %1 failed to connect (attempt %2); retrying on another port: %3</source>
        <translation>Knoten %1 konnte nicht verbinden (Versuch %2); neuer Versuch über anderen Port: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="140"/>
        <source>Node %1 failed to connect: %2</source>
        <translation>Knoten %1 konnte nicht verbinden: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="149"/>
        <source>Node %1 failed to establish the tunnel: %2</source>
        <translation>Knoten %1 konnte den Tunnel nicht aufbauen: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="158"/>
        <source>Node %1: the server has UDP disabled; UDP traffic will not work</source>
        <translation>Knoten %1: Auf dem Server ist UDP deaktiviert; UDP-Datenverkehr funktioniert nicht</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="61"/>
        <source>Target domain name too long</source>
        <translation>Ziel-Domainname zu lang</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="78"/>
        <source>Could not resolve the target address: %1</source>
        <translation>Zieladresse konnte nicht aufgelöst werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="121"/>
        <source>Node %1 is missing server/port</source>
        <translation>Bei Knoten %1 fehlt Server/Port</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="138"/>
        <source>Failed to connect to %1:%2: %3</source>
        <translation>Verbindung zu %1:%2 fehlgeschlagen: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="157"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="442"/>
        <source>Node %1 is missing uuid</source>
        <translation>Bei Knoten %1 fehlt die uuid</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="163"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="448"/>
        <source>Node %1 has an invalid uuid: %2</source>
        <translation>Knoten %1 hat eine ungültige uuid: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="176"/>
        <source>Node %1 uses an unsupported flow: %2</source>
        <translation>Knoten %1 nutzt einen nicht unterstützten flow: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="181"/>
        <source>Node %1 uses vision but has no reality-opts configured; this combination is not implemented yet</source>
        <translation>Knoten %1 nutzt vision, hat aber keine reality-opts konfiguriert; diese Kombination ist noch nicht implementiert</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="196"/>
        <source>Node %1 has an invalid reality-opts.public-key (it should decode to 32 bytes)</source>
        <translation>Knoten %1 hat einen ungültigen reality-opts.public-key (er sollte sich zu 32 Bytes dekodieren lassen)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="200"/>
        <source>Node %1 has a reality-opts.short-id that is too long</source>
        <translation>Knoten %1 hat eine zu lange reality-opts.short-id</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="208"/>
        <source>Node %1 REALITY handshake failed: %2</source>
        <translation>REALITY-Handshake von Knoten %1 fehlgeschlagen: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="236"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="293"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="322"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="541"/>
        <source>Node %1 failed to send the VLESS request</source>
        <translation>Knoten %1 konnte die VLESS-Anfrage nicht senden</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="252"/>
        <source>Node %1 transport %2 is not implemented yet (ws, REALITY and plain TCP are supported)</source>
        <translation>Knoten %1: Transport %2 ist noch nicht implementiert (unterstützt werden ws, REALITY und einfaches TCP)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="280"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="317"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="497"/>
        <source>Node %1 TLS connection failed: %2</source>
        <translation>TLS-Verbindung von Knoten %1 fehlgeschlagen: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="288"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="340"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="507"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="525"/>
        <source>Node %1 WebSocket handshake failed: %2</source>
        <translation>WebSocket-Handshake von Knoten %1 fehlgeschlagen: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="350"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="355"/>
        <source>Failed to send the VLESS request: %1</source>
        <translation>VLESS-Anfrage konnte nicht gesendet werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="373"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="397"/>
        <source>Failed to read the VLESS response: %1</source>
        <translation>VLESS-Antwort konnte nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="377"/>
        <source>Truncated VLESS response</source>
        <translation>Abgeschnittene VLESS-Antwort</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="382"/>
        <source>Truncated VLESS response addons</source>
        <translation>Abgeschnittene VLESS-Antwort-Addons</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="404"/>
        <source>Failed to read the VLESS response addons: %1</source>
        <translation>VLESS-Antwort-Addons konnten nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="454"/>
        <source>VLESS UDP is missing the target address</source>
        <translation>VLESS-UDP fehlt die Zieladresse</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="488"/>
        <source>Node %1 has an invalid reality-opts.public-key</source>
        <translation>Knoten %1 hat einen ungültigen reality-opts.public-key</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="515"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="533"/>
        <source>Node %1 transport %2 does not support UDP</source>
        <translation>Knoten %1: Transport %2 unterstützt kein UDP</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="54"/>
        <source>%1 has an invalid listen address: %2</source>
        <translation>%1 hat eine ungültige Listen-Adresse: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="62"/>
        <source>%1 failed to listen on %2:%3: %4</source>
        <translation>%1 konnte nicht auf %2:%3 lauschen: %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="76"/>
        <source>%1 inbound started: %2:%3 (concurrency limit %4)</source>
        <translation>%1 eingehend gestartet: %2:%3 (Parallelitätslimit %4)</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="126"/>
        <source>%1 still has %2 open connection(s)</source>
        <translation>%1 hat noch %2 offene Verbindung(en)</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="154"/>
        <source>Concurrency limit %1 reached; rejecting new connections</source>
        <translation>Parallelitätslimit %1 erreicht; neue Verbindungen werden abgelehnt</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="182"/>
        <source>Failed to duplicate the connection descriptor; connection dropped</source>
        <translation>Verbindungsdeskriptor konnte nicht dupliziert werden; Verbindung verworfen</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="216"/>
        <source>%1 accepted connection %2:%3</source>
        <translation>%1 hat Verbindung %2:%3 angenommen</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="231"/>
        <source>Could not rebuild a socket from descriptor %1</source>
        <translation>Socket konnte nicht aus Deskriptor %1 neu aufgebaut werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="61"/>
        <source>Missing HTTP request line</source>
        <translation>HTTP-Anfragezeile fehlt</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="68"/>
        <source>Malformed HTTP request line: %1</source>
        <translation>Fehlerhafte HTTP-Anfragezeile: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="135"/>
        <source>HTTP inbound read failed: %1</source>
        <translation>Lesen im HTTP-Eingang fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="58"/>
        <source>Unsupported SOCKS version: %1</source>
        <translation>Nicht unterstützte SOCKS-Version: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="75"/>
        <source>The client does not support username/password authentication</source>
        <translation>Der Client unterstützt keine Authentifizierung mit Benutzername/Passwort</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="81"/>
        <source>The client offered no acceptable authentication method</source>
        <translation>Der Client hat keine akzeptable Authentifizierungsmethode angeboten</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="89"/>
        <source>Failed to write the SOCKS5 method selection</source>
        <translation>SOCKS5-Methodenauswahl konnte nicht geschrieben werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="100"/>
        <source>Unsupported username/password auth version</source>
        <translation>Nicht unterstützte Version der Benutzername/Passwort-Authentifizierung</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="125"/>
        <source>SOCKS5 authentication failed (user %1)</source>
        <translation>SOCKS5-Authentifizierung fehlgeschlagen (Benutzer %1)</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="142"/>
        <source>Wrong SOCKS5 request version</source>
        <translation>Falsche Version der SOCKS5-Anfrage</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="152"/>
        <source>Unsupported SOCKS5 command 0x%1</source>
        <translation>Nicht unterstützter SOCKS5-Befehl 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="180"/>
        <source>Unsupported SOCKS5 address type 0x%1</source>
        <translation>Nicht unterstützter SOCKS5-Adresstyp 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="219"/>
        <source>SOCKS5 negotiation failed: %1</source>
        <translation>SOCKS5-Aushandlung fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>SOCKS5 request failed: %1</source>
        <translation>SOCKS5-Anfrage fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>Invalid target</source>
        <translation>Ungültiges Ziel</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/MixedListener.cpp" line="37"/>
        <source>MIXED inbound timed out while sniffing the protocol</source>
        <translation>MIXED-Eingang: Zeitüberschreitung beim Erkennen des Protokolls</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="26"/>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="24"/>
        <source>No dialer implementation available</source>
        <translation>Keine Dialer-Implementierung verfügbar</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="32"/>
        <source>Dial failed</source>
        <translation>Verbindungsaufbau fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="71"/>
        <source>UDP socket closed</source>
        <translation>UDP-Socket geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="160"/>
        <source>UDP tunnel closed</source>
        <translation>UDP-Tunnel geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="30"/>
        <source>UDP dial failed</source>
        <translation>UDP-Verbindungsaufbau fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="97"/>
        <source>Could not resolve the TUN adapter from its LUID</source>
        <translation>TUN-Adapter konnte nicht über seine LUID ermittelt werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="116"/>
        <source>Could not get the Wintun read event</source>
        <translation>Wintun-Leseereignis konnte nicht abgerufen werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="161"/>
        <source>The Wintun session was closed</source>
        <translation>Die Wintun-Sitzung wurde geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="199"/>
        <source>DNS is not taken over on the TUN address (%1): %2</source>
        <translation>DNS wird an der TUN-Adresse (%1) nicht übernommen: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="282"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="427"/>
        <source>Invalid TUN adapter</source>
        <translation>Ungültiger TUN-Adapter</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="287"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="432"/>
        <source>Invalid TUN device fd</source>
        <translation>Ungültiges TUN-Geräte-fd</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="306"/>
        <source>Failed to send a packet to the TUN adapter</source>
        <translation>Paket konnte nicht an den TUN-Adapter gesendet werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="314"/>
        <source>Failed to write to the TUN device: %1</source>
        <translation>Schreiben auf das TUN-Gerät fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="342"/>
        <source>Could not open the TUN adapter</source>
        <translation>TUN-Adapter konnte nicht geöffnet werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="349"/>
        <source>TUN data plane ready (worker thread %1, Wintun %2)</source>
        <translation>TUN-Datenebene bereit (Worker-Thread %1, Wintun %2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="382"/>
        <source>TUN data plane ready (worker thread %1, DNS 53 %2)</source>
        <translation>TUN-Datenebene bereit (Worker-Thread %1, DNS 53 %2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Taken over</source>
        <translation>Übernommen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Not taken over</source>
        <translation>Nicht übernommen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="412"/>
        <source>Failed to read the TUN device: %1</source>
        <translation>Lesen vom TUN-Gerät fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="414"/>
        <source>Failed to read the TUN device</source>
        <translation>Lesen vom TUN-Gerät fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="185"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="193"/>
        <source>Idle timeout</source>
        <translation>Leerlauf-Timeout</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="216"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="224"/>
        <source>Closing TUN</source>
        <translation>TUN wird geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="248"/>
        <source>Re-entered TUN packet handling; dropped this packet to avoid concurrent flow-table writes</source>
        <translation>TUN-Paketverarbeitung erneut betreten; Paket verworfen, um parallele Schreibzugriffe auf die Flow-Tabelle zu vermeiden</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="268"/>
        <source>Dropped a non-IPv4 packet from TUN (IPv6 is not supported yet)</source>
        <translation>Nicht-IPv4-Paket aus TUN verworfen (IPv6 wird noch nicht unterstützt)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="321"/>
        <source>Dropped an IP fragment from TUN (no reassembly)</source>
        <translation>IP-Fragment aus TUN verworfen (keine Reassemblierung)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="421"/>
        <source>TUN: dropped link-local/multicast UDP to %1</source>
        <translation>TUN: Link-Local-/Multicast-UDP an %1 verworfen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="535"/>
        <source>TUN: answered ICMP echo locally (%1, fake-ip)</source>
        <translation>TUN: ICMP-Echo lokal beantwortet (%1, fake-ip)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="565"/>
        <source>TUN TCP segment %1:%2 -&gt; %3 flags=0x%4 seq=%5 len=%6 known flows=%7</source>
        <translation>TUN-TCP-Segment %1:%2 -&gt; %3 flags=0x%4 seq=%5 len=%6 bekannte Flows=%7</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="590"/>
        <source>TUN: TCP port 53 is not supported yet; replying RST so the client falls back to UDP</source>
        <translation>TUN: TCP-Port 53 wird noch nicht unterstützt; es wird RST geantwortet, damit der Client auf UDP zurückfällt</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="640"/>
        <source>Client RST</source>
        <translation>RST vom Client</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="668"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1227"/>
        <source>Client closed and upstream already disconnected</source>
        <translation>Client geschlossen, Upstream bereits getrennt</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="674"/>
        <source>Four-way handshake complete</source>
        <translation>Vier-Wege-Handshake abgeschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="778"/>
        <source>TUN: UDP target failed recently; replying with ICMP: %1</source>
        <translation>TUN: UDP-Ziel ist kürzlich fehlgeschlagen; Antwort mit ICMP: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="785"/>
        <source>TUN: UDP sessions exceed %1; rejecting new sessions</source>
        <translation>TUN: UDP-Sitzungen überschreiten %1; neue Sitzungen werden abgelehnt</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="869"/>
        <source>TUN UDP session established: %1:%2 -&gt; %3</source>
        <translation>TUN-UDP-Sitzung aufgebaut: %1:%2 -&gt; %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="899"/>
        <source>TUN UDP dial failed for %1: %2</source>
        <translation>TUN-UDP-Verbindungsaufbau für %1 fehlgeschlagen: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="928"/>
        <source>TUN UDP channel closed: %1</source>
        <translation>TUN-UDP-Kanal geschlossen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="930"/>
        <source>Upstream closed</source>
        <translation>Upstream geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="950"/>
        <source>TUN closed UDP session %1:%2 (%3)</source>
        <translation>TUN hat UDP-Sitzung %1:%2 geschlossen (%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1044"/>
        <source>TUN: target failed recently; sending RST: %1</source>
        <translation>TUN: Ziel ist kürzlich fehlgeschlagen; RST wird gesendet: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1098"/>
        <source>TUN: concurrent connections exceed %1; rejecting new connections</source>
        <translation>TUN: parallele Verbindungen überschreiten %1; neue Verbindungen werden abgelehnt</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1149"/>
        <source>TUN new connection %1:%2 -&gt; %3:%4</source>
        <translation>TUN neue Verbindung %1:%2 -&gt; %3:%4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1179"/>
        <source>TUN dial failed for %1:%2: %3</source>
        <translation>TUN-Verbindungsaufbau für %1:%2 fehlgeschlagen: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1204"/>
        <source>TUN upstream data: %1 bytes (%2 pending)</source>
        <translation>TUN-Upstream-Daten: %1 Bytes (%2 ausstehend)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1336"/>
        <source>TUN closed connection %1:%2 (%3)</source>
        <translation>TUN hat Verbindung %1:%2 geschlossen (%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="61"/>
        <source>Connection %1: client disconnected (up %2 / down %3 bytes)</source>
        <translation>Verbindung %1: Client getrennt (hoch %2 / runter %3 Bytes)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="70"/>
        <source>Connection %1: upstream closed (up %2 / down %3 bytes) %4</source>
        <translation>Verbindung %1: Upstream geschlossen (hoch %2 / runter %3 Bytes) %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="111"/>
        <source>Connection %1: short upstream write, %2 / %3 bytes</source>
        <translation>Verbindung %1: unvollständiges Schreiben zum Upstream, %2 / %3 Bytes</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="224"/>
        <source>Config not loaded yet</source>
        <translation>Konfiguration noch nicht geladen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="232"/>
        <source>GLOBAL mode is on, but the config has no GLOBAL group</source>
        <translation>Der GLOBAL-Modus ist aktiv, aber die Konfiguration enthält keine GLOBAL-Gruppe</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="266"/>
        <source>No MATCH fallback rule in the rule list; treating as DIRECT</source>
        <translation>Keine MATCH-Fallback-Regel in der Regelliste; wird als DIRECT behandelt</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="277"/>
        <source>Rule %1 points at a policy that does not exist: %2</source>
        <translation>Regel %1 verweist auf eine nicht vorhandene Richtlinie: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="311"/>
        <source>Closed an inbound connection with no target address</source>
        <translation>Eingehende Verbindung ohne Zieladresse geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="332"/>
        <source>Could not determine an outbound policy (%1 -&gt; %2); connection closed</source>
        <translation>Ausgangsrichtlinie konnte nicht bestimmt werden (%1 -&gt; %2); Verbindung geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="333"/>
        <source>Unknown reason</source>
        <translation>Unbekannter Grund</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="351"/>
        <source>The DIRECT outbound needs DNS resolution, but no resolver is available</source>
        <translation>Der DIRECT-Ausgang benötigt eine DNS-Auflösung, aber es ist kein Resolver verfügbar</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="360"/>
        <source>DNS resolution failed; connection closed: %1</source>
        <translation>DNS-Auflösung fehlgeschlagen; Verbindung geschlossen: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="381"/>
        <source>Rule %1 dropped the connection: %2</source>
        <translation>Regel %1 hat die Verbindung verworfen: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="394"/>
        <source>Outbound %1 failed to connect to %2:%3: %4</source>
        <translation>Ausgang %1 konnte nicht zu %2:%3 verbinden: %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="404"/>
        <source>UDP relay is not implemented yet; connection closed</source>
        <translation>UDP-Weiterleitung ist noch nicht implementiert; Verbindung geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="430"/>
        <source>%1 -&gt; %2:%3 via %4 (rule %5)</source>
        <translation>%1 -&gt; %2:%3 über %4 (Regel %5)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="309"/>
        <source>Vision: the server echoed an unknown UUID</source>
        <translation>Vision: Der Server hat eine unbekannte UUID zurückgegeben</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="338"/>
        <source>Vision: read an unknown command %1</source>
        <translation>Vision: unbekannter Befehl %1 gelesen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="385"/>
        <source>Connection closed while reading the response</source>
        <translation>Verbindung wurde beim Lesen der Antwort geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="88"/>
        <source>Failed to send the WebSocket upgrade request</source>
        <translation>WebSocket-Upgrade-Anfrage konnte nicht gesendet werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="104"/>
        <source>Connection closed during the WebSocket upgrade (received %1 bytes: %2)</source>
        <translation>Verbindung während des WebSocket-Upgrades geschlossen (%1 Bytes empfangen: %2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="113"/>
        <source>WebSocket upgrade handshake timed out</source>
        <translation>Zeitüberschreitung beim WebSocket-Upgrade-Handshake</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="127"/>
        <source>WebSocket handshake rejected: %1</source>
        <translation>WebSocket-Handshake abgelehnt: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="154"/>
        <source>Server-sent WebSocket frames must not be masked</source>
        <translation>Vom Server gesendete WebSocket-Frames dürfen nicht maskiert sein</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="179"/>
        <source>WebSocket frame too long</source>
        <translation>WebSocket-Frame zu lang</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="127"/>
        <source>DNS override is off; using system resolution</source>
        <translation>DNS-Überschreibung ist aus; Systemauflösung wird verwendet</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="152"/>
        <source>Rule set %1 has an invalid behavior: %2</source>
        <translation>Regelsatz %1 hat einen ungültigen behavior-Wert: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="178"/>
        <source>Could not read the rule-set file: %1</source>
        <translation>Regelsatzdatei konnte nicht gelesen werden: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="181"/>
        <source>Remote rule sets (type=http) are not implemented yet; skipped</source>
        <translation>Remote-Regelsätze (type=http) sind noch nicht implementiert; übersprungen</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="207"/>
        <source>Config hot-reloaded (inbound listeners kept)</source>
        <translation>Konfiguration im laufenden Betrieb neu geladen (eingehende Listener beibehalten)</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="248"/>
        <source>Mixed port %1 failed to start</source>
        <translation>Gemischter Port %1 konnte nicht gestartet werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="265"/>
        <source>SOCKS5 port %1 failed to start</source>
        <translation>SOCKS5-Port %1 konnte nicht gestartet werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="280"/>
        <source>HTTP port %1 failed to start</source>
        <translation>HTTP-Port %1 konnte nicht gestartet werden</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="290"/>
        <source>redir-port / tproxy-port transparent proxying is not implemented yet (Phase 2); those ports are not listening</source>
        <translation>Transparentes Proxying über redir-port / tproxy-port ist noch nicht implementiert (Phase 2); diese Ports lauschen nicht</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="295"/>
        <source>No inbound port is enabled in the config (mixed-port / socks-port / port are all 0)</source>
        <translation>In der Konfiguration ist kein eingehender Port aktiviert (mixed-port / socks-port / port sind alle 0)</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="335"/>
        <source>No config has been applied yet</source>
        <translation>Es wurde noch keine Konfiguration angewendet</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="349"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="350"/>
        <source>Started</source>
        <translation>Gestartet</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="367"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="384"/>
        <source>Stopped</source>
        <translation>Gestoppt</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="403"/>
        <source>Outbound mode switched to %1</source>
        <translation>Ausgangsmodus auf %1 gewechselt</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="437"/>
        <source>TUN is already running</source>
        <translation>TUN läuft bereits</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="447"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="593"/>
        <source>TUN is closed</source>
        <translation>TUN ist geschlossen</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="456"/>
        <source>No usable outbound matched</source>
        <translation>Kein nutzbarer Ausgang passt</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="530"/>
        <source>TUN data-plane error (%1); TUN stopped</source>
        <translation>Fehler in der TUN-Datenebene (%1); TUN gestoppt</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="545"/>
        <source>TUN taken over (device=%1); data plane running on a worker thread</source>
        <translation>TUN übernommen (device=%1); Datenebene läuft in einem Worker-Thread</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="586"/>
        <source>The TUN data-plane thread is still winding down; it will be released after exit</source>
        <translation>Der Thread der TUN-Datenebene läuft noch aus; er wird nach dem Beenden freigegeben</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="633"/>
        <source>Group %1 does not exist</source>
        <translation>Gruppe %1 existiert nicht</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="638"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="766"/>
        <source>%1 is not a proxy group</source>
        <translation>%1 ist keine Proxy-Gruppe</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="642"/>
        <source>Group %1 has no member %2</source>
        <translation>Gruppe %1 hat kein Mitglied %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="678"/>
        <source>Group %1 references member %2, which no longer exists; ignored</source>
        <translation>Gruppe %1 verweist auf Mitglied %2, das nicht mehr existiert; ignoriert</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="688"/>
        <source>Restored manual selections for %1 proxy group(s)</source>
        <translation>Manuelle Auswahl für %1 Proxy-Gruppe(n) wiederhergestellt</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="703"/>
        <source>Node does not exist: %1</source>
        <translation>Knoten existiert nicht: %1</translation>
    </message>
</context>
</TS>
