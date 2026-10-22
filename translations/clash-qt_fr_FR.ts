<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr_FR">
<context>
    <name>clashqt::AppContext</name>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="188"/>
        <source>Failed to turn off the system proxy on exit: %1 (the desktop still points at the local port)</source>
        <translation>Échec de la désactivation du proxy système à la fermeture : %1 (le bureau pointe toujours vers le port local)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="193"/>
        <source>System proxy turned off before exit, so the desktop stops pointing at a stopped local port</source>
        <translation>Proxy système désactivé avant la fermeture : le bureau ne pointe plus vers un port local arrêté</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="208"/>
        <source>Failed to load the config. Check that the files in %1 are writable</source>
        <translation>Impossible de charger la configuration. Vérifiez que les fichiers de %1 sont modifiables</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="224"/>
        <source>Data directory: %1 (portable: %2)</source>
        <translation>Dossier de données : %1 (portable : %2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="225"/>
        <source>yes</source>
        <translation>oui</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="226"/>
        <source>no</source>
        <translation>non</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="239"/>
        <source>Config generation failed: %1</source>
        <translation>Échec de la génération de la configuration : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="268"/>
        <source>Start failed: %1</source>
        <translation>Échec du démarrage : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="269"/>
        <source>Startup failed: %1</source>
        <translation>Échec du lancement : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="304"/>
        <source>Kernel failed to restart after config reload: %1</source>
        <translation>Le noyau n&apos;a pas redémarré après le rechargement de la configuration : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="384"/>
        <location filename="../src/app/src/AppContext.cpp" line="411"/>
        <source>Config not loaded yet</source>
        <translation>Configuration pas encore chargée</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="423"/>
        <source>TUN enable request: enabled=%1 running=%2 starting(old)=%3</source>
        <translation>Demande d&apos;activation TUN : enabled=%1 running=%2 starting(old)=%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="434"/>
        <source>TUN disabled; system routes restored</source>
        <translation>TUN désactivé ; routes système restaurées</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="446"/>
        <source>TUN helper not found (it should sit next to the main executable): %1</source>
        <translation>Assistant TUN introuvable (il doit se trouver à côté de l&apos;exécutable principal) : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="485"/>
        <source>Removed the wildcard &quot;*&quot; from fake-ip-filter (it makes every domain skip fake-ip)</source>
        <translation>Joker « * » retiré de fake-ip-filter (il faisait ignorer fake-ip à tous les domaines)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="487"/>
        <source>Fixed the DNS config: the &quot;*&quot; in fake-ip-filter made every domain skip fake-ip, so it was removed</source>
        <translation>Configuration DNS corrigée : le « * » de fake-ip-filter faisait ignorer fake-ip à tous les domaines, il a donc été retiré</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="501"/>
        <source>TUN device name changed from Mihomo to %1</source>
        <translation>Nom du périphérique TUN changé de Mihomo en %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="515"/>
        <source>TUN mode needs kernel DNS (fake-ip); DNS override was enabled automatically</source>
        <translation>Le mode TUN nécessite le DNS du noyau (fake-ip) ; la surcharge DNS a été activée automatiquement</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="517"/>
        <source>TUN mode needs kernel DNS; &quot;DNS override&quot; was enabled automatically</source>
        <translation>Le mode TUN nécessite le DNS du noyau ; « Surcharge DNS » a été activée automatiquement</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="532"/>
        <source>The kernel failed to start, so TUN cannot be enabled</source>
        <translation>Le noyau n&apos;a pas démarré : TUN ne peut pas être activé</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="555"/>
        <location filename="../src/app/src/AppContext.cpp" line="607"/>
        <source>Failed to enable TUN: %1</source>
        <translation>Échec de l&apos;activation de TUN : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="576"/>
        <source>Enabling TUN timed out; state was force-reset</source>
        <translation>Délai dépassé lors de l&apos;activation de TUN ; état réinitialisé de force</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="579"/>
        <source>Timed out enabling TUN; state was reset, please try again</source>
        <translation>Délai dépassé pour l&apos;activation de TUN ; état réinitialisé, réessayez</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="583"/>
        <source>Enabling TUN…</source>
        <translation>Activation de TUN…</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="608"/>
        <source>Could not take over the device</source>
        <translation>Impossible de prendre le contrôle du périphérique</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="620"/>
        <source>TUN enabled (system routes taken over)</source>
        <translation>TUN activé (routes système prises en charge)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="621"/>
        <source>TUN enabled (routes not taken over)</source>
        <translation>TUN activé (routes non prises en charge)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="630"/>
        <source>Kernel is not running; skipping switch-state restore</source>
        <translation>Le noyau n&apos;est pas démarré ; restauration des états ignorée</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="640"/>
        <source>Restored TUN to its previous state</source>
        <translation>TUN restauré à son état précédent</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="643"/>
        <source>Failed to restore TUN: %1</source>
        <translation>Échec de la restauration de TUN : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="645"/>
        <source>Failed to restore TUN mode: %1</source>
        <translation>Échec de la restauration du mode TUN : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="654"/>
        <source>Restored the system proxy to its previous state</source>
        <translation>Proxy système restauré à son état précédent</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="657"/>
        <location filename="../src/app/src/AppContext.cpp" line="659"/>
        <source>Failed to restore the system proxy: %1</source>
        <translation>Échec de la restauration du proxy système : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="674"/>
        <source>Start failed; system proxy not enabled: %1</source>
        <translation>Échec du démarrage ; proxy système non activé : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="686"/>
        <source>No inbound port available; cannot set the system proxy</source>
        <translation>Aucun port d&apos;entrée disponible ; impossible de définir le proxy système</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="696"/>
        <source>127.0.0.1:%1 is not listening locally; refusing to enable the system proxy (the whole desktop would lose connectivity by pointing at an empty port)</source>
        <translation>127.0.0.1:%1 n&apos;écoute pas en local ; activation du proxy système refusée (tout le bureau perdrait sa connexion en pointant vers un port vide)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="708"/>
        <source>Failed to set the system proxy: %1</source>
        <translation>Échec de la définition du proxy système : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="716"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>Échec de la désactivation du proxy système : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="749"/>
        <source>Unknown outbound mode: %1</source>
        <translation>Mode de sortie inconnu : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="788"/>
        <location filename="../src/app/src/AppContext.cpp" line="813"/>
        <location filename="../src/app/src/AppContext.cpp" line="901"/>
        <location filename="../src/app/src/AppContext.cpp" line="979"/>
        <source>Failed to write the profile contents</source>
        <translation>Échec de l&apos;écriture du contenu du profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="793"/>
        <location filename="../src/app/src/AppContext.cpp" line="905"/>
        <source>Failed to write the profile list</source>
        <translation>Échec de l&apos;écriture de la liste des profils</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="797"/>
        <source>Added local config: %1</source>
        <translation>Configuration locale ajoutée : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="805"/>
        <location filename="../src/app/src/AppContext.cpp" line="917"/>
        <location filename="../src/app/src/AppContext.cpp" line="1020"/>
        <source>Profile does not exist: %1</source>
        <translation>Le profil n&apos;existe pas : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="819"/>
        <source>Replaced the config file of &quot;%1&quot;</source>
        <translation>Fichier de configuration de « %1 » remplacé</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="831"/>
        <source>Invalid profile URL: %1</source>
        <translation>URL de profil invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="858"/>
        <source>Profile download timed out (%1 ms)</source>
        <translation>Délai dépassé lors du téléchargement du profil (%1 ms)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="865"/>
        <source>Profile download failed: %1</source>
        <translation>Échec du téléchargement du profil : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="884"/>
        <source>The profile is not a valid Clash config: %1</source>
        <translation>Le profil n&apos;est pas une configuration Clash valide : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="909"/>
        <source>Added profile: %1</source>
        <translation>Profil ajouté : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="921"/>
        <source>Profile &quot;%1&quot; is not a remote profile; nothing to update</source>
        <translation>Le profil « %1 » n&apos;est pas distant ; rien à mettre à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="948"/>
        <source>Updating &quot;%1&quot; timed out</source>
        <translation>Délai dépassé lors de la mise à jour de « %1 »</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="954"/>
        <source>Failed to update &quot;%1&quot;: %2</source>
        <translation>Échec de la mise à jour de « %1 » : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="972"/>
        <source>The update for &quot;%1&quot; returned invalid content; the old config was kept: %2</source>
        <translation>La mise à jour de « %1 » a renvoyé un contenu invalide ; l&apos;ancienne configuration a été conservée : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="993"/>
        <source>Profile updated: %1</source>
        <translation>Profil mis à jour : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1025"/>
        <source>Switched to profile: %1</source>
        <translation>Profil activé : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1040"/>
        <source>Override content must be a YAML mapping: %1</source>
        <translation>Le contenu de surcharge doit être un mapping YAML : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1047"/>
        <source>Unnamed override</source>
        <translation>Surcharge sans nom</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1054"/>
        <source>Failed to write the override contents</source>
        <translation>Échec de l&apos;écriture du contenu de la surcharge</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1058"/>
        <source>Failed to write the override list</source>
        <translation>Échec de l&apos;écriture de la liste des surcharges</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1062"/>
        <source>Added override: %1</source>
        <translation>Surcharge ajoutée : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1093"/>
        <source>Failed to parse the rule-override file; treating it as empty: %1</source>
        <translation>Échec de l&apos;analyse du fichier de surcharge de règles ; traité comme vide : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1117"/>
        <source>No profile selected; cannot save custom rules</source>
        <translation>Aucun profil sélectionné ; impossible d&apos;enregistrer les règles personnalisées</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1129"/>
        <source>%1 entry %2 contains a line break (each rule must stay on one line)</source>
        <translation>L&apos;entrée %2 de %1 contient un saut de ligne (chaque règle doit tenir sur une seule ligne)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1137"/>
        <source>%1 entry %2 is not a valid rule (expected at least &quot;type,target&quot;): %3</source>
        <translation>L&apos;entrée %2 de %1 n&apos;est pas une règle valide (« type,cible » attendu au minimum) : %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1147"/>
        <source>Custom rules</source>
        <translation>Règles personnalisées</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1148"/>
        <source>Fallback rules</source>
        <translation>Règles de repli</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1149"/>
        <source>Removal rules</source>
        <translation>Règles de suppression</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1182"/>
        <source>Failed to write custom rules: %1</source>
        <translation>Échec de l&apos;écriture des règles personnalisées : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1187"/>
        <source>Custom rules saved (profile %1): %2 total, %3 disabled, %4 fallback / %5 removal</source>
        <translation>Règles personnalisées enregistrées (profil %1) : %2 au total, %3 désactivées, %4 de repli / %5 de suppression</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1196"/>
        <source>Custom rules applied</source>
        <translation>Règles personnalisées appliquées</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionListModel</name>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="67"/>
        <source>Rule: %1</source>
        <translation>Règle : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="68"/>
        <source>Outbound: %1</source>
        <translation>Sortie : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="69"/>
        <source>Up %1 · Down %2 · %3</source>
        <translation>Envoi %1 · Réception %2 · %3</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="63"/>
        <source>Connections</source>
        <translation>Connexions</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="64"/>
        <source>Active connections, one card each. Double-click a card, or use the × on its right, to close that connection.</source>
        <translation>Connexions actives, une carte chacune. Double-cliquez sur une carte, ou utilisez le × à sa droite, pour fermer cette connexion.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="73"/>
        <source>Auto refresh (1 s)</source>
        <translation>Actualisation auto (1 s)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="77"/>
        <source>Close selected</source>
        <translation>Fermer la sélection</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="78"/>
        <source>Close all</source>
        <translation>Tout fermer</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="104"/>
        <source>No active connections</source>
        <translation>Aucune connexion active</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="145"/>
        <source>just now</source>
        <translation>à l&apos;instant</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="146"/>
        <source>%1 min ago</source>
        <translation>il y a %1 min</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="147"/>
        <source>%1 h ago</source>
        <translation>il y a %1 h</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="148"/>
        <source>%1 d ago</source>
        <translation>il y a %1 j</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="157"/>
        <source>%1 active connection(s) · Total up %2 · Total down %3</source>
        <translation>%1 connexion(s) active(s) · Envoi total %2 · Réception totale %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="239"/>
        <source>Select a connection first</source>
        <translation>Sélectionnez d&apos;abord une connexion</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="253"/>
        <source>Closed %1 connection(s)</source>
        <translation>%1 connexion(s) fermée(s)</translation>
    </message>
</context>
<context>
    <name>clashqt::LogsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="48"/>
        <source>Logs</source>
        <translation>Journaux</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="49"/>
        <source>Application log. Logs are also written to the logs/ folder in the data directory.</source>
        <translation>Journal de l&apos;application. Les journaux sont aussi écrits dans le dossier logs/ du répertoire de données.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="54"/>
        <source>Level</source>
        <translation>Niveau</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="61"/>
        <source>Pause scrolling</source>
        <translation>Suspendre le défilement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="64"/>
        <source>Clear view</source>
        <translation>Vider l&apos;affichage</translation>
    </message>
</context>
<context>
    <name>clashqt::MainWindow</name>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="236"/>
        <source>Rule</source>
        <translation>Règle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="237"/>
        <source>Global</source>
        <translation>Global</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="238"/>
        <source>Direct</source>
        <translation>Direct</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="272"/>
        <source>System proxy</source>
        <translation>Proxy système</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="282"/>
        <source>No system proxy backend found (gsettings / kwriteconfig); system proxy is unavailable</source>
        <translation>Aucun backend de proxy système détecté (gsettings / kwriteconfig) ; le proxy système est indisponible</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="288"/>
        <source>TUN mode</source>
        <translation>Mode TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="295"/>
        <source>Captures all local traffic, including apps that ignore the system proxy. The first enable requires a one-time password prompt.</source>
        <translation>Capture tout le trafic local, y compris les applis qui ignorent le proxy système. La première activation demande le mot de passe une seule fois.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="343"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="708"/>
        <source>Update this subscription</source>
        <translation>Mettre à jour cet abonnement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="412"/>
        <source>Proxies</source>
        <translation>Proxies</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="414"/>
        <source>Rules</source>
        <translation>Règles</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="416"/>
        <source>Overrides</source>
        <translation>Surcharges</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="418"/>
        <source>Logs</source>
        <translation>Journaux</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="420"/>
        <source>Connections</source>
        <translation>Connexions</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="423"/>
        <source>Settings</source>
        <translation>Paramètres</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="484"/>
        <source>Traffic</source>
        <translation>Trafic</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="694"/>
        <source>No profile selected</source>
        <translation>Aucun profil sélectionné</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="709"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="757"/>
        <source>Local profiles have no URL to update from</source>
        <translation>Les profils locaux n&apos;ont pas d&apos;URL de mise à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="721"/>
        <source>%1 / %2</source>
        <translation>%1 / %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="730"/>
        <source>Updated %1</source>
        <translation>%1 mis à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="745"/>
        <source>Open the profiles page</source>
        <translation>Ouvrir la page des profils</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="752"/>
        <source>Select a profile first</source>
        <translation>Sélectionnez d&apos;abord un profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="765"/>
        <source>Update failed: %1</source>
        <translation>Échec de la mise à jour : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="767"/>
        <source>Subscription updated</source>
        <translation>Abonnement mis à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="792"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="871"/>
        <source>Disable system proxy</source>
        <translation>Désactiver le proxy système</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="800"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="907"/>
        <source>Enable TUN mode</source>
        <translation>Activer le mode TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="807"/>
        <source>Show main window</source>
        <translation>Afficher la fenêtre principale</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="815"/>
        <source>Quit</source>
        <translation>Quitter</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="872"/>
        <source>Enable system proxy</source>
        <translation>Activer le proxy système</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="877"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="905"/>
        <source>Enabling TUN…</source>
        <translation>Activation de TUN…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="881"/>
        <source>System proxy + TUN enabled</source>
        <translation>Proxy système + TUN activés</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="882"/>
        <source>TUN enabled</source>
        <translation>TUN activé</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="883"/>
        <source>System proxy enabled</source>
        <translation>Proxy système activé</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="884"/>
        <source>System proxy disabled</source>
        <translation>Proxy système désactivé</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="900"/>
        <source>enabling…</source>
        <translation>activation…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="906"/>
        <source>Disable TUN mode</source>
        <translation>Désactiver le mode TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="916"/>
        <source>Mixed port %1</source>
        <translation>Port mixte %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="918"/>
        <source>Not running</source>
        <translation>Arrêté</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="946"/>
        <source>Failed to switch outbound mode: %1</source>
        <translation>Échec du changement de mode de sortie : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="952"/>
        <source>Outbound mode: %1</source>
        <translation>Mode de sortie : %1</translation>
    </message>
</context>
<context>
    <name>clashqt::OverridePage</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>Domain suffix (incl. subdomains)</source>
        <translation>Suffixe de domaine (sous-domaines inclus)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>e.g. example.com</source>
        <translation>ex. example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>Domain (exact)</source>
        <translation>Domaine (exact)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>e.g. www.example.com</source>
        <translation>ex. www.example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>Domain keyword</source>
        <translation>Mot-clé de domaine</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>e.g. google</source>
        <translation>ex. google</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>Domain regex</source>
        <translation>Regex de domaine</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>e.g. ^ads\..*</source>
        <translation>ex. ^ads\..*</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>Geosite category</source>
        <translation>Catégorie Geosite</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>e.g. cn / netflix / telegram</source>
        <translation>ex. cn / netflix / telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>IP country</source>
        <translation>Pays IP</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>e.g. CN / US / JP</source>
        <translation>ex. CN / US / JP</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>IPv4 CIDR</source>
        <translation>CIDR IPv4</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>e.g. 10.0.0.0/8</source>
        <translation>ex. 10.0.0.0/8</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>IPv6 CIDR</source>
        <translation>CIDR IPv6</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>e.g. 2001:db8::/32</source>
        <translation>ex. 2001:db8::/32</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>ASN</source>
        <translation>ASN</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>e.g. 13335</source>
        <translation>ex. 13335</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>Destination port</source>
        <translation>Port de destination</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>e.g. 443</source>
        <translation>ex. 443</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>Source port</source>
        <translation>Port source</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>e.g. 1234</source>
        <translation>ex. 1234</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>Network type</source>
        <translation>Type de réseau</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>tcp or udp</source>
        <translation>tcp ou udp</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>Process name</source>
        <translation>Nom du processus</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>e.g. telegram</source>
        <translation>ex. telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>Process path</source>
        <translation>Chemin du processus</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>e.g. /usr/bin/curl</source>
        <translation>ex. /usr/bin/curl</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Rule set</source>
        <translation>Jeu de règles</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Choose a rule set from the config</source>
        <translation>Choisir un jeu de règles dans la configuration</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="129"/>
        <source>Catch-all (match all remaining traffic)</source>
        <translation>Fourre-tout (tout le trafic restant)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="405"/>
        <source>Overrides</source>
        <translation>Surcharges</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="406"/>
        <source>Adds custom rules on top of the profile&apos;s rules. Pick a type, content and target when adding — no rule syntax needed. The list order is the match priority; changes are saved and applied automatically.</source>
        <translation>Ajoute des règles personnalisées par-dessus celles du profil. À l&apos;ajout, choisissez un type, un contenu et une cible — aucune syntaxe à connaître. L&apos;ordre de la liste fixe la priorité ; les modifications sont enregistrées et appliquées automatiquement.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="408"/>
        <source>Custom rules</source>
        <translation>Règles personnalisées</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="418"/>
        <source>New rule</source>
        <translation>Nouvelle règle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Enabled</source>
        <translation>Activée</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Rule type</source>
        <translation>Type de règle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Content</source>
        <translation>Contenu</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Target</source>
        <translation>Cible</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="432"/>
        <source>Actions</source>
        <translation>Actions</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="452"/>
        <source>Double-click any row to edit; uncheck it to disable (content is kept but not matched). Changes are saved and applied automatically.</source>
        <translation>Double-cliquez sur une ligne pour la modifier ; décochez-la pour la désactiver (le contenu est conservé mais non appliqué). Les modifications sont enregistrées et appliquées automatiquement.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="526"/>
        <source>No profile selected; custom rules cannot be saved</source>
        <translation>Aucun profil sélectionné ; les règles personnalisées ne peuvent pas être enregistrées</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="527"/>
        <source>Applies to profile: %1 (rules are stored per profile and differ between profiles)</source>
        <translation>S&apos;applique au profil : %1 (les règles sont stockées par profil et diffèrent d&apos;un profil à l&apos;autre)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="553"/>
        <source>%1 rules, %2 enabled</source>
        <translation>%1 règles, %2 activées</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="575"/>
        <source>Enabled; uncheck to disable</source>
        <translation>Activée ; décochez pour désactiver</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="576"/>
        <source>Disabled: not matched after saving</source>
        <translation>Désactivée : ignorée après enregistrement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="601"/>
        <source>%1 (%2)
Rule: %3</source>
        <translation>%1 (%2)
Règle : %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="612"/>
        <source>Move up (higher priority)</source>
        <translation>Monter (priorité plus haute)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="614"/>
        <source>Move down (lower priority)</source>
        <translation>Descendre (priorité plus basse)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="615"/>
        <source>Edit this rule</source>
        <translation>Modifier cette règle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="616"/>
        <source>Delete this rule</source>
        <translation>Supprimer cette règle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="726"/>
        <source>Save failed</source>
        <translation>Échec de l&apos;enregistrement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="733"/>
        <source>Applied automatically: %1 rules, %2 disabled</source>
        <translation>Appliqué automatiquement : %1 règles, %2 désactivées</translation>
    </message>
</context>
<context>
    <name>clashqt::ProfilesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="38"/>
        <source>Expired</source>
        <translation>Expiré</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="40"/>
        <source> (%1 days)</source>
        <translation> (%1 jours)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="235"/>
        <source>Profiles</source>
        <translation>Profils</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="236"/>
        <source>Manage config sources. The active profile is merged with &quot;Config overrides&quot; before use.</source>
        <translation>Gérez les sources de configuration. Le profil actif est fusionné avec les « Surcharges de configuration » avant utilisation.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="247"/>
        <source>Add profile</source>
        <translation>Ajouter un profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="248"/>
        <source>Import local config</source>
        <translation>Importer une configuration locale</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="249"/>
        <source>Edit</source>
        <translation>Modifier</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="250"/>
        <source>Update selected</source>
        <translation>Mettre à jour la sélection</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="251"/>
        <source>Update all</source>
        <translation>Tout mettre à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="252"/>
        <source>Set active</source>
        <translation>Activer</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="253"/>
        <source>Delete</source>
        <translation>Supprimer</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="263"/>
        <source>Edit the name or URL (or double-click a row)</source>
        <translation>Modifier le nom ou l&apos;URL (ou double-cliquez sur une ligne)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="276"/>
        <source>No profiles yet. Click &quot;Add profile&quot; to enter a subscription URL, or &quot;Import local config&quot; to pick an existing Clash config file.</source>
        <translation>Aucun profil pour l&apos;instant. Cliquez sur « Ajouter un profil » pour saisir une URL d&apos;abonnement, ou sur « Importer une configuration locale » pour choisir un fichier de configuration Clash existant.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Active</source>
        <translation>Actif</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Type</source>
        <translation>Type</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Traffic</source>
        <translation>Trafic</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Expires</source>
        <translation>Expiration</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Updated</source>
        <translation>Mise à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="341"/>
        <source>Remote</source>
        <translation>Distant</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="342"/>
        <source>Local</source>
        <translation>Local</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="343"/>
        <source>Plugin</source>
        <translation>Plugin</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="386"/>
        <source>Add failed</source>
        <translation>Échec de l&apos;ajout</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="404"/>
        <source>Choose a config file</source>
        <translation>Choisir un fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="405"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Configuration Clash (*.yaml *.yml);;Tous les fichiers (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="410"/>
        <source>Import failed</source>
        <translation>Échec de l&apos;importation</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="420"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="471"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="505"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="521"/>
        <source>Select a profile first</source>
        <translation>Sélectionnez d&apos;abord un profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Save failed</source>
        <translation>Échec de l&apos;enregistrement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Failed to write profile config</source>
        <translation>Échec de l&apos;écriture de la configuration du profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="446"/>
        <source>Replace failed</source>
        <translation>Échec du remplacement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="458"/>
        <source>URL saved</source>
        <translation>URL enregistrée</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="459"/>
        <source>URL saved, but fetching the new content failed:

%1

Try &quot;Update selected&quot; again later.</source>
        <translation>URL enregistrée, mais le téléchargement du nouveau contenu a échoué :

%1

Réessayez « Mettre à jour la sélection » plus tard.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="477"/>
        <source>Update failed</source>
        <translation>Échec de la mise à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="489"/>
        <source>Some profiles failed to update</source>
        <translation>Certains profils n&apos;ont pas pu être mis à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="490"/>
        <source>%1 succeeded, %2 failed:

%3</source>
        <translation>%1 réussis, %2 échoués :

%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="496"/>
        <source>Updated %1 profile(s)</source>
        <translation>%1 profil(s) mis à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="511"/>
        <source>Switch failed</source>
        <translation>Échec du changement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="527"/>
        <source>Delete profile</source>
        <translation>Supprimer le profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="528"/>
        <source>Delete &quot;%1&quot;? Its config file will be removed as well.</source>
        <translation>Supprimer « %1 » ? Son fichier de configuration sera aussi supprimé.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Delete failed</source>
        <translation>Échec de la suppression</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Profile not found</source>
        <translation>Profil introuvable</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="545"/>
        <source>Profile deleted, but reloading the config failed: %1</source>
        <translation>Profil supprimé, mais le rechargement de la configuration a échoué : %1</translation>
    </message>
</context>
<context>
    <name>clashqt::ProxiesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="66"/>
        <source>Group</source>
        <translation>Groupe</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="90"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="334"/>
        <source>Nodes</source>
        <translation>Nœuds</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="110"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="149"/>
        <source>Testing</source>
        <translation>Test…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="111"/>
        <source>Test</source>
        <translation>Tester</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="112"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="151"/>
        <source>Timeout</source>
        <translation>Expiré</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="291"/>
        <source>Proxies</source>
        <translation>Proxies</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="292"/>
        <source>Pick a node from the proxy group: click a card to switch to it. The text on the right is the last measured latency; click it to test that node again, or click Test on a node that has not been measured yet. In global mode, the GLOBAL group decides the exit for all traffic.</source>
        <translation>Choisissez un nœud dans le groupe de proxies : cliquez sur une carte pour basculer dessus. Le texte à droite indique la dernière latence mesurée ; cliquez dessus pour retester ce nœud, ou sur Tester pour un nœud jamais mesuré. En mode global, le groupe GLOBAL décide de la sortie de tout le trafic.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="298"/>
        <source>Proxy group</source>
        <translation>Groupe</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="309"/>
        <source>Sort</source>
        <translation>Trier</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="311"/>
        <source>Default order</source>
        <translation>Par défaut</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="312"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="313"/>
        <source>Type</source>
        <translation>Type</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="314"/>
        <source>Latency</source>
        <translation>Latence</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="316"/>
        <source>Sort nodes by name, protocol type or latest measured latency.
Latency sorts ascending; untested and timed-out nodes go last. Sorting never changes the selected node.</source>
        <translation>Trie les nœuds par nom, type de protocole ou dernière latence mesurée.
Le tri par latence est croissant ; les nœuds non testés ou expirés passent en dernier. Le tri ne change jamais le nœud sélectionné.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="323"/>
        <source>Test this group</source>
        <translation>Tester ce groupe</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="324"/>
        <source>Test all groups</source>
        <translation>Tout tester</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="436"/>
        <source>The active profile has no proxy groups</source>
        <translation>Le profil actif n&apos;a aucun groupe de proxies</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="469"/>
        <source>Type %1 · %2 members</source>
        <translation>Type %1 · %2 membres</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="472"/>
        <source>Current node: %1</source>
        <translation>Nœud actuel : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="501"/>
        <source>Proxy group · %1</source>
        <translation>Groupe de proxies · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="502"/>
        <source>Protocol · %1</source>
        <translation>Protocole · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="503"/>
        <source>Unknown</source>
        <translation>Inconnu</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="505"/>
        <source>This protocol is not supported yet; connections will fail explicitly</source>
        <translation>Ce protocole n&apos;est pas encore pris en charge ; les connexions échoueront explicitement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="511"/>
        <source>Last latency test timed out</source>
        <translation>Le dernier test de débit a expiré</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="516"/>
        <source>Last latency %1 ms</source>
        <translation>Dernière latence %1 ms</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="517"/>
        <source>Last test timed out</source>
        <translation>Le dernier test a expiré</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="522"/>
        <source>Currently pointing at %1</source>
        <translation>Pointe actuellement vers %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="524"/>
        <source>Click the text on the right of a card to test that node</source>
        <translation>Cliquez sur le texte à droite d&apos;une carte pour tester ce nœud</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="741"/>
        <source>Node no longer exists</source>
        <translation>Le nœud n&apos;existe plus</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="772"/>
        <source>Latency test finished: %1/%2 nodes available</source>
        <translation>Test de débit terminé : %1/%2 nœuds disponibles</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="776"/>
        <source>%1 latency test failed: %2</source>
        <translation>Échec du test de débit de %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="792"/>
        <source>These nodes are being tested, or none can be tested</source>
        <translation>Ces nœuds sont en cours de test, ou aucun ne peut être testé</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="805"/>
        <source>Testing %1 nodes (%2 at a time, results fill in as they finish)</source>
        <translation>Test de %1 nœuds (%2 à la fois, résultats au fur et à mesure)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="835"/>
        <source>Failed to switch node: %1</source>
        <translation>Échec du changement de nœud : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="843"/>
        <source>%1 switched to %2</source>
        <translation>%1 basculé sur %2</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="170"/>
        <source>New rule</source>
        <translation>Nouvelle règle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="171"/>
        <source>Edit rule</source>
        <translation>Modifier la règle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="180"/>
        <source>Follow the prompts; the rule syntax is assembled when you save.</source>
        <translation>Suivez les indications ; la syntaxe de la règle est assemblée à l&apos;enregistrement.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="198"/>
        <source>%1 (cannot be edited structurally)</source>
        <translation>%1 (modification structurelle impossible)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="201"/>
        <source>Rule type</source>
        <translation>Type de règle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="217"/>
        <source>Content</source>
        <translation>Contenu</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="223"/>
        <source>Choose a policy or proxy group</source>
        <translation>Choisir une stratégie ou un groupe de proxies</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="224"/>
        <source>Target</source>
        <translation>Cible</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="227"/>
        <source>Default</source>
        <translation>Par défaut</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="228"/>
        <source>Do not resolve domain (no-resolve)</source>
        <translation>Ne pas résoudre le domaine (no-resolve)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="229"/>
        <source>Match source address (src)</source>
        <translation>Correspondre à l&apos;adresse source (src)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="230"/>
        <source>Options</source>
        <translation>Options</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="253"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="254"/>
        <source>Save</source>
        <translation>Enregistrer</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="302"/>
        <source>No rule-providers in the config</source>
        <translation>Aucun rule-provider dans la configuration</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="303"/>
        <source>Choose a rule set</source>
        <translation>Choisir un jeu de règles</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="333"/>
        <source>A catch-all rule matches all remaining traffic; keep it last among your custom rules.</source>
        <translation>Une règle fourre-tout correspond à tout le trafic restant ; gardez-la en dernier parmi vos règles personnalisées.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="335"/>
        <source>example.com also matches all of its subdomains.</source>
        <translation>example.com correspond aussi à tous ses sous-domaines.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="337"/>
        <source>For LAN traffic, set Options to no-resolve.</source>
        <translation>Pour le trafic LAN, réglez Options sur no-resolve.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="339"/>
        <source>Rule sets come from rule-providers in the profile.</source>
        <translation>Les jeux de règles proviennent des rule-providers du profil.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="362"/>
        <source>Choose what handles this rule (policy / proxy group).</source>
        <translation>Choisissez ce qui traite cette règle (stratégie / groupe de proxies).</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="367"/>
        <source>Enter the content to match; it is what the rule checks.</source>
        <translation>Saisissez le contenu à faire correspondre : c&apos;est ce que la règle vérifie.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="372"/>
        <source>IPv4 CIDR needs a prefix length, e.g. 10.0.0.0/8.</source>
        <translation>Un CIDR IPv4 nécessite une longueur de préfixe, ex. 10.0.0.0/8.</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleRowDelegate</name>
    <message>
        <location filename="../src/app/src/ui/RuleList.cpp" line="156"/>
        <source>%1 hits</source>
        <translation>%1 correspondances</translation>
    </message>
</context>
<context>
    <name>clashqt::RulesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="43"/>
        <source>Rules</source>
        <translation>Règles</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="44"/>
        <source>Rules in match order, one card each. Turn the switch on the right off to disable a rule for this session; it is restored on restart.</source>
        <translation>Règles dans l&apos;ordre de correspondance, une carte chacune. Désactivez l&apos;interrupteur à droite pour neutraliser une règle le temps de cette session ; elle sera rétablie au redémarrage.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="64"/>
        <source>No rules loaded</source>
        <translation>Aucune règle chargée</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="90"/>
        <source>Failed to update rule state</source>
        <translation>Échec de la mise à jour de l&apos;état de la règle</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="132"/>
        <source>%1 hits in this session</source>
        <translation>%1 correspondances dans cette session</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="133"/>
        <source>Disabled until restart</source>
        <translation>Désactivée jusqu&apos;au redémarrage</translation>
    </message>
</context>
<context>
    <name>clashqt::SettingsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="42"/>
        <source>Settings</source>
        <translation>Paramètres</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="43"/>
        <source>Logs, DNS overrides, ports and system integration. Changes are saved and applied immediately.</source>
        <translation>Journaux, surcharges DNS, ports et intégration système. Les modifications sont enregistrées et appliquées immédiatement.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="46"/>
        <source>Appearance</source>
        <translation>Apparence</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="50"/>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="63"/>
        <source>Follow system</source>
        <translation>Suivre le système</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="51"/>
        <source>Light</source>
        <translation>Clair</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="52"/>
        <source>Dark</source>
        <translation>Sombre</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="58"/>
        <source>Theme</source>
        <translation>Thème</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="68"/>
        <source>Language</source>
        <translation>Langue</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="69"/>
        <source>Interface text is loaded at startup; restart the app after switching.</source>
        <translation>Le texte de l&apos;interface est chargé au démarrage ; redémarrez l&apos;application après le changement.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="74"/>
        <source>Logs</source>
        <translation>Journaux</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="81"/>
        <source>Log level</source>
        <translation>Niveau de journal</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="86"/>
        <source> days</source>
        <translation> jours</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="89"/>
        <source>Log retention (days)</source>
        <translation>Conservation des journaux (jours)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="90"/>
        <source>Log files older than the retention period are removed at startup.</source>
        <translation>Les journaux plus anciens que la durée de conservation sont supprimés au démarrage.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="93"/>
        <source>Open data folder</source>
        <translation>Ouvrir le dossier de données</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="102"/>
        <source>Config overrides</source>
        <translation>Surcharges de configuration</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="105"/>
        <source>Manage DNS settings in this app</source>
        <translation>Gérer les paramètres DNS dans cette application</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="108"/>
        <source>When on, the DNS section configured here is used; when off, the profile&apos;s own DNS settings apply.</source>
        <translation>Activé, la section DNS configurée ici est utilisée ; désactivé, ce sont les paramètres DNS du profil qui s&apos;appliquent.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="111"/>
        <source>Manage domain sniffing in this app</source>
        <translation>Gérer l&apos;inspection des domaines dans cette application</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="117"/>
        <source>Inbound ports</source>
        <translation>Ports d&apos;entrée</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="131"/>
        <source>Mixed port (HTTP + SOCKS5)</source>
        <translation>Port mixte (HTTP + SOCKS5)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="133"/>
        <source>SOCKS5 port</source>
        <translation>Port SOCKS5</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="134"/>
        <source>Set it equal to the mixed port to reuse the mixed port.</source>
        <translation>Indiquez la même valeur que le port mixte pour le réutiliser.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="136"/>
        <source>HTTP port</source>
        <translation>Port HTTP</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="141"/>
        <source>System integration</source>
        <translation>Intégration système</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="144"/>
        <source>Start at login</source>
        <translation>Lancer à la connexion</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="146"/>
        <source>Implemented via an XDG autostart entry (~/.config/autostart/clash-qt.desktop).</source>
        <translation>Implémenté via une entrée de démarrage XDG (~/.config/autostart/clash-qt.desktop).</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="149"/>
        <source>System proxy</source>
        <translation>Proxy système</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="152"/>
        <source>Points the system HTTP/HTTPS/SOCKS proxy at this app&apos;s mixed port. Current backend: %1</source>
        <translation>Fait pointer le proxy HTTP/HTTPS/SOCKS du système vers le port mixte de cette application. Backend actuel : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="156"/>
        <source>TUN mode</source>
        <translation>Mode TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="159"/>
        <source>Creates a virtual network interface to capture all traffic, including apps that ignore the system proxy (games, CLI tools, some Electron apps). The first enable asks for your password once; it only takes effect together with &quot;Route domain traffic&quot;.</source>
        <translation>Crée une interface réseau virtuelle pour capturer tout le trafic, y compris les applis qui ignorent le proxy système (jeux, outils en ligne de commande, certaines applis Electron). La première activation demande votre mot de passe une fois ; l&apos;effet n&apos;est complet qu&apos;avec « Router le trafic des domaines ».</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="162"/>
        <source>Route domain traffic (fake-IP)</source>
        <translation>Router le trafic des domaines (fake-IP)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="165"/>
        <source>Routes the fake-IP range to the TUN interface (removed together with the interface). The default route is untouched, so connectivity is unaffected; with this off, TUN receives no traffic.</source>
        <translation>Achemine la plage fake-IP vers l&apos;interface TUN (supprimée avec l&apos;interface). La route par défaut reste intacte, la connectivité n&apos;est donc pas affectée ; désactivé, TUN ne reçoit aucun trafic.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="171"/>
        <source>About</source>
        <translation>À propos</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="177"/>
        <source>Check for updates when the app starts</source>
        <translation>Rechercher des mises à jour au démarrage</translation>
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
        <translation>Rechercher des mises à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="198"/>
        <source>Reads the update manifest attached to the latest release, fetched through public GitHub mirrors. When an update exists, its changelog is shown and the release page can be opened in your browser; downloading and installing is up to you.</source>
        <translation>Lit le manifeste de mise à jour joint à la dernière version, récupéré via des miroirs GitHub publics. Si une mise à jour existe, son journal des modifications s&apos;affiche et la page de publication peut être ouverte dans le navigateur ; le téléchargement et l&apos;installation restent à votre charge.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="252"/>
        <source>Already up to date (v%1)</source>
        <translation>Déjà à jour (v%1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="260"/>
        <source> (the kernel is not running; start the proxy and try again)</source>
        <translation> (le noyau n&apos;est pas démarré ; lancez le proxy et réessayez)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="262"/>
        <source>Update check failed: %1%2</source>
        <translation>Échec de la recherche de mises à jour : %1%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="300"/>
        <source>Data folder: %1</source>
        <translation>Dossier de données : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="359"/>
        <source>Reload failed after port change: %1</source>
        <translation>Échec du rechargement après modification du port : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="372"/>
        <source>Failed to change startup setting: %1</source>
        <translation>Échec de la modification du réglage de démarrage : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="456"/>
        <source>Language saved; restart the app to apply.</source>
        <translation>Langue enregistrée ; redémarrez l&apos;application pour l&apos;appliquer.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="481"/>
        <source>Checking…</source>
        <translation>Vérification…</translation>
    </message>
</context>
<context>
    <name>clashqt::SubscriptionDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Add profile</source>
        <translation>Ajouter un profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Edit profile</source>
        <translation>Modifier le profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="84"/>
        <source>Enter the subscription URL; it will be downloaded and activated immediately.</source>
        <translation>Saisissez l&apos;URL d&apos;abonnement ; elle sera téléchargée et activée immédiatement.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="86"/>
        <source>Change the name or subscription URL. If this is the active profile, a changed URL is fetched again immediately.</source>
        <translation>Modifiez le nom ou l&apos;URL d&apos;abonnement. S&apos;il s&apos;agit du profil actif, une URL modifiée est retéléchargée immédiatement.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="87"/>
        <source>A local profile uses an imported config file, which you can replace below.</source>
        <translation>Un profil local utilise un fichier de configuration importé, que vous pouvez remplacer ci-dessous.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="90"/>
        <source>Profile info</source>
        <translation>Infos du profil</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="97"/>
        <source>Optional; defaults to the subscription&apos;s domain</source>
        <translation>Facultatif ; par défaut le domaine de l&apos;abonnement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="99"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="108"/>
        <source>Subscription URL</source>
        <translation>URL d&apos;abonnement</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="110"/>
        <source>Update this profile on startup</source>
        <translation>Mettre à jour ce profil au démarrage</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="115"/>
        <source>When enabled, the profile is refreshed automatically at startup.</source>
        <translation>Activé, le profil est actualisé automatiquement au démarrage.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="127"/>
        <source>Choose another file</source>
        <translation>Choisir un autre fichier</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="132"/>
        <source>Config file</source>
        <translation>Fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="133"/>
        <source>Picking a new file replaces this profile&apos;s content; its id and active state are preserved.</source>
        <translation>Choisir un nouveau fichier remplace le contenu de ce profil ; son identifiant et son état actif sont conservés.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="138"/>
        <source>Choose a config file</source>
        <translation>Choisir un fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="139"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Configuration Clash (*.yaml *.yml);;Tous les fichiers (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="153"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Add</source>
        <translation>Ajouter</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Save</source>
        <translation>Enregistrer</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="193"/>
        <source>Name required</source>
        <translation>Nom requis</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="194"/>
        <source>Give this profile a name.</source>
        <translation>Donnez un nom à ce profil.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="203"/>
        <source>Invalid subscription URL</source>
        <translation>URL d&apos;abonnement invalide</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="205"/>
        <source>Enter a subscription URL starting with http/https.</source>
        <translation>Saisissez une URL d&apos;abonnement commençant par http/https.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="206"/>
        <source>The subscription URL must start with http/https.</source>
        <translation>L&apos;URL d&apos;abonnement doit commencer par http/https.</translation>
    </message>
</context>
<context>
    <name>clashqt::TrafficPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="63"/>
        <source>Traffic</source>
        <translation>Trafic</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="64"/>
        <source>Live upload/download speed and cumulative traffic; the chart shows 1-second samples from the last 2 minutes.</source>
        <translation>Débit d&apos;envoi/réception en direct et trafic cumulé ; le graphique affiche des échantillons d&apos;une seconde sur les 2 dernières minutes.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="67"/>
        <source>Status</source>
        <translation>État</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="72"/>
        <source>Total 0 B / 0 B</source>
        <translation>Total 0 B / 0 B</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="73"/>
        <source>0 active connections</source>
        <translation>0 connexion active</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="82"/>
        <source>Speed chart</source>
        <translation>Graphique de débit</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="96"/>
        <source>s</source>
        <translation>s</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="188"/>
        <source>Session total ↑ %1 · ↓ %2</source>
        <translation>Total session ↑ %1 · ↓ %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="193"/>
        <source>%1 active connection(s) · Memory %2</source>
        <translation>%1 connexion(s) active(s) · Mémoire %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="242"/>
        <source>Upload (%1)</source>
        <translation>Envoi (%1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="243"/>
        <source>Download (%1)</source>
        <translation>Réception (%1)</translation>
    </message>
</context>
<context>
    <name>clashqt::UpdateDialog</name>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="20"/>
        <source>Update available</source>
        <translation>Mise à jour disponible</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="30"/>
        <source>Version %1 is available</source>
        <translation>La version %1 est disponible</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="31"/>
        <source>Version v%1. The changelog comes from the GitHub release; downloading and installing is up to you — this app never replaces itself.</source>
        <translation>Version v%1. Le journal des modifications vient de la publication GitHub ; le téléchargement et l&apos;installation restent à votre charge — l&apos;application ne se remplace jamais elle-même.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="51"/>
        <source>(No release notes for this version; see the release page.)</source>
        <translation>(Aucune note de version ; voir la page de publication.)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="97"/>
        <source>No package has been attached to this release yet; it may still be uploading.</source>
        <translation>Aucun paquet n&apos;est encore joint à cette version ; l&apos;envoi est peut-être en cours.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="100"/>
        <source>Published on %1. Download the package from the release page.</source>
        <translation>Publiée le %1. Téléchargez le paquet depuis la page de publication.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="103"/>
        <source>Download the package from the release page.</source>
        <translation>Téléchargez le paquet depuis la page de publication.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="112"/>
        <source>Later</source>
        <translation>Plus tard</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="113"/>
        <source>Open download page</source>
        <translation>Ouvrir la page de téléchargement</translation>
    </message>
</context>
<context>
    <name>clashqt::app</name>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="130"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable.</source>
        <translation>wintun.dll est introuvable. Placez wintun.dll à côté de l&apos;exécutable.</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="145"/>
        <source>Could not create the Wintun adapter</source>
        <translation>Impossible de créer l&apos;adaptateur Wintun</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="175"/>
        <source>Wintun is unavailable</source>
        <translation>Wintun est indisponible</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="182"/>
        <source>Could not start the Wintun session</source>
        <translation>Impossible de démarrer la session Wintun</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="65"/>
        <source>Clash Qt - a desktop client for Clash-style subscriptions</source>
        <translation>Clash Qt - un client de bureau pour les abonnements de type Clash</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="71"/>
        <source>Start without showing the main window (for autostart)</source>
        <translation>Démarrer sans afficher la fenêtre principale (pour le démarrage automatique)</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="73"/>
        <source>Use the data directory next to the executable</source>
        <translation>Utiliser le dossier de données à côté de l&apos;exécutable</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="97"/>
        <source>Another instance is already running: asked it to show its window, exiting</source>
        <translation>Une autre instance est déjà en cours : affichage de sa fenêtre demandé, fermeture</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="108"/>
        <source>Starting %1 %2 (%3, silent=%4)</source>
        <translation>Démarrage de %1 %2 (%3, silent=%4)</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="121"/>
        <source>Initialization failed: %1</source>
        <translation>Échec de l&apos;initialisation : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="128"/>
        <source>UI language: %1 (configured value %2)</source>
        <translation>Langue de l&apos;interface : %1 (valeur configurée %2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="148"/>
        <source>Start failed: %1</source>
        <translation>Échec du démarrage : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="177"/>
        <source>Silent start: not showing the main window</source>
        <translation>Démarrage silencieux : fenêtre principale non affichée</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="188"/>
        <source>%1 exited with code %2</source>
        <translation>%1 s&apos;est arrêté avec le code %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="72"/>
        <source>Could not read file %1: %2</source>
        <translation>Impossible de lire le fichier %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="82"/>
        <source>Not a valid Clash config: %1</source>
        <translation>Configuration Clash non valide : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/AppLogger.cpp" line="152"/>
        <source>Removed %1 expired log file(s) (retention %2 days)</source>
        <translation>%1 fichier(s) de journal expiré(s) supprimé(s) (conservation %2 jours)</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="18"/>
        <source>Could not create the directory: %1</source>
        <translation>Impossible de créer le dossier : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="26"/>
        <source>Could not write %1: %2</source>
        <translation>Impossible d&apos;écrire %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="32"/>
        <source>Incomplete write to %1: %2</source>
        <translation>Écriture incomplète dans %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="38"/>
        <source>Failed to commit %1: %2</source>
        <translation>Échec de la validation de %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="70"/>
        <source>Used up</source>
        <translation>Épuisé</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="77"/>
        <source>%1 d %2 h</source>
        <translation>%1 j %2 h</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="78"/>
        <source>%1 h %2 min</source>
        <translation>%1 h %2 min</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="79"/>
        <source>%1 min</source>
        <translation>%1 min</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="208"/>
        <source>Failed to parse the app config; using defaults: %1</source>
        <translation>Échec de l&apos;analyse de la configuration de l&apos;application ; valeurs par défaut utilisées : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="228"/>
        <source>Failed to write back the app config: %1</source>
        <translation>Échec de la réécriture de la configuration de l&apos;application : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="238"/>
        <source>Failed to save the app config: %1</source>
        <translation>Échec de l&apos;enregistrement de la configuration de l&apos;application : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="226"/>
        <source>Failed to parse the legacy managed config (%1): %2</source>
        <translation>Échec de l&apos;analyse de l&apos;ancienne configuration gérée (%1) : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="232"/>
        <source>Merged the legacy managed config into config.yaml and deleted it: %1</source>
        <translation>Ancienne configuration gérée fusionnée dans config.yaml puis supprimée : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="134"/>
        <source>Failed to parse the profile list: %1</source>
        <translation>Échec de l&apos;analyse de la liste des profils : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="157"/>
        <source>Failed to save the profile list: %1</source>
        <translation>Échec de l&apos;enregistrement de la liste des profils : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="242"/>
        <source>Failed to write profile content (%1): %2</source>
        <translation>Échec de l&apos;écriture du contenu du profil (%1) : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="74"/>
        <source>Failed to parse the override list: %1</source>
        <translation>Échec de l&apos;analyse de la liste des surcharges : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="92"/>
        <source>Failed to save the override list: %1</source>
        <translation>Échec de l&apos;enregistrement de la liste des surcharges : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="156"/>
        <source>Failed to write override content (%1.%2): %3</source>
        <translation>Échec de l&apos;écriture du contenu de la surcharge (%1.%2) : %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="98"/>
        <source>References an override that does not exist: %1</source>
        <translation>Référence une surcharge inexistante : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="107"/>
        <source>Override %1 has no content file</source>
        <translation>La surcharge %1 n&apos;a pas de fichier de contenu</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="116"/>
        <source>Override &quot;%1&quot; is a JS script, and the JS override engine is not supported yet; it was skipped (the config will not apply it)</source>
        <translation>La surcharge « %1 » est un script JS, et le moteur de surcharge JS n&apos;est pas encore pris en charge ; elle a été ignorée (la configuration ne l&apos;appliquera pas)</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="126"/>
        <source>Failed to parse override &quot;%1&quot;: %2</source>
        <translation>Échec de l&apos;analyse de la surcharge « %1 » : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="133"/>
        <source>The root of override &quot;%1&quot; must be a mapping</source>
        <translation>La racine de la surcharge « %1 » doit être un mapping</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="159"/>
        <source>Failed to parse the rule-override file: %1</source>
        <translation>Échec de l&apos;analyse du fichier de surcharge de règles : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="228"/>
        <source>No profile selected yet. Add one on the Profiles page and select it first.</source>
        <translation>Aucun profil sélectionné. Ajoutez-en un dans la page Profils et sélectionnez-le d&apos;abord.</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="239"/>
        <source>Profile &quot;%1&quot; is empty or missing</source>
        <translation>Le profil « %1 » est vide ou introuvable</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="249"/>
        <source>Failed to parse the profile content: %1</source>
        <translation>Échec de l&apos;analyse du contenu du profil : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="356"/>
        <source>Failed to write the config: %1</source>
        <translation>Échec de l&apos;écriture de la configuration : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="361"/>
        <source>Generated the runtime config: %1 (profile %2)</source>
        <translation>Configuration d&apos;exécution générée : %1 (profil %2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="279"/>
        <source>Request timed out</source>
        <translation>Délai de la requête dépassé</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="281"/>
        <source>The release has no update manifest</source>
        <translation>Cette version ne contient pas de manifeste de mise à jour</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="341"/>
        <source>No update manifest address available</source>
        <translation>Aucune adresse de manifeste de mise à jour disponible</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="354"/>
        <source>Could not parse the manifest: %1</source>
        <translation>Impossible d&apos;analyser le manifeste : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="363"/>
        <source>The manifest has no usable version field</source>
        <translation>Le manifeste n&apos;a pas de champ de version exploitable</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="234"/>
        <source>System-proxy backend: %1</source>
        <translation>Backend de proxy système : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="258"/>
        <source>System-proxy backend: %1 (XDG_CURRENT_DESKTOP=%2)</source>
        <translation>Backend de proxy système : %1 (XDG_CURRENT_DESKTOP=%2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="273"/>
        <source>unavailable</source>
        <translation>indisponible</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="327"/>
        <source>Could not write ProxyServer to the registry</source>
        <translation>Impossible d&apos;écrire ProxyServer dans le registre</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="335"/>
        <source>Failed to write the system-proxy ignore list</source>
        <translation>Échec de l&apos;écriture de la liste d&apos;exclusion du proxy système</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="339"/>
        <source>Could not enable the system proxy in the registry</source>
        <translation>Impossible d&apos;activer le proxy système dans le registre</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="350"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="378"/>
        <source>System proxy enabled (%1): %2:%3</source>
        <translation>Proxy système activé (%1) : %2:%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="356"/>
        <source>No usable system-proxy backend detected (gsettings or kwriteconfig5/6 is required)</source>
        <translation>Aucun backend de proxy système utilisable (gsettings ou kwriteconfig5/6 est requis)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="372"/>
        <source>Failed to enable the system proxy: %1</source>
        <translation>Échec de l&apos;activation du proxy système : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="387"/>
        <source>Could not turn off the system proxy in the registry</source>
        <translation>Impossible de désactiver le proxy système dans le registre</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="398"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="423"/>
        <source>System proxy disabled</source>
        <translation>Proxy système désactivé</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="402"/>
        <source>No usable system-proxy backend detected</source>
        <translation>Aucun backend de proxy système utilisable détecté</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="418"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>Échec de la désactivation du proxy système : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="438"/>
        <source>Could not start %1</source>
        <translation>Impossible de démarrer %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="443"/>
        <source>%1 timed out</source>
        <translation>Délai dépassé pour %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="448"/>
        <source>%1 failed: %2</source>
        <translation>Échec de %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="515"/>
        <source>Failed to write the system-proxy ignore list: %1</source>
        <translation>Échec de l&apos;écriture de la liste d&apos;exclusion du proxy système : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="577"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="620"/>
        <source>kwriteconfig5/6 not found</source>
        <translation>kwriteconfig5/6 introuvable</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="53"/>
        <source>Could not open the autostart registry key</source>
        <translation>Impossible d&apos;ouvrir la clé de registre du démarrage automatique</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="68"/>
        <source>Could not write the autostart entry (error %1)</source>
        <translation>Impossible d&apos;écrire l&apos;entrée de démarrage automatique (erreur %1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="109"/>
        <source>Could not create the autostart directory</source>
        <translation>Impossible de créer le dossier de démarrage automatique</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="161"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="193"/>
        <source>Failed to enable autostart: %1</source>
        <translation>Échec de l&apos;activation du démarrage automatique : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="167"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="196"/>
        <source>Autostart enabled: %1</source>
        <translation>Démarrage automatique activé : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="215"/>
        <source>Could not remove the autostart entry (error %1)</source>
        <translation>Impossible de supprimer l&apos;entrée de démarrage automatique (erreur %1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="224"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="235"/>
        <source>Autostart disabled</source>
        <translation>Démarrage automatique désactivé</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="231"/>
        <source>Could not remove the autostart entry: %1</source>
        <translation>Impossible de supprimer l&apos;entrée de démarrage automatique : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SingleInstance.cpp" line="53"/>
        <source>The single-instance guard failed to start (%1); running as multiple instances this time: %2</source>
        <translation>Le garde d&apos;instance unique n&apos;a pas démarré (%1) ; exécution en plusieurs instances cette fois : %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="117"/>
        <source>Could not set the TUN adapter address</source>
        <translation>Impossible de définir l&apos;adresse de l&apos;adaptateur TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="134"/>
        <source>Could not read the TUN adapter settings</source>
        <translation>Impossible de lire les paramètres de l&apos;adaptateur TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="150"/>
        <source>Could not set the TUN adapter MTU/metric</source>
        <translation>Impossible de définir le MTU/métrique de l&apos;adaptateur TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="175"/>
        <source>Could not add the TUN route</source>
        <translation>Impossible d&apos;ajouter la route TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="203"/>
        <source>Could not start netsh</source>
        <translation>Impossible de démarrer netsh</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="208"/>
        <source>netsh timed out</source>
        <translation>Délai dépassé pour netsh</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="312"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="383"/>
        <source>TUN mode on Windows requires administrator privileges. Close the app and start it again with &quot;Run as administrator&quot;.</source>
        <translation>Le mode TUN sous Windows nécessite des privilèges administrateur. Fermez l&apos;application et relancez-la avec « Exécuter en tant qu&apos;administrateur ».</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="321"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="513"/>
        <source>Helper not found: %1</source>
        <translation>Assistant introuvable : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="337"/>
        <source>Could not start pkexec: %1</source>
        <translation>Impossible de démarrer pkexec : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="344"/>
        <source>Authorization timed out (the password prompt may have been cancelled)</source>
        <translation>Délai d&apos;autorisation dépassé (l&apos;invite de mot de passe a peut-être été annulée)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="349"/>
        <source>%1 failed (exit code %2): %3</source>
        <translation>Échec de %1 (code de sortie %2) : %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="359"/>
        <source>TUN permissions granted (chown root:root + chmod +sx): %1</source>
        <translation>Permissions TUN accordées (chown root:root + chmod +sx) : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="369"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="509"/>
        <source>TUN is starting or already running</source>
        <translation>TUN démarre ou est déjà actif</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="389"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable and try again.</source>
        <translation>wintun.dll est introuvable. Placez wintun.dll à côté de l&apos;exécutable et réessayez.</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="402"/>
        <source>Invalid TUN address: %1</source>
        <translation>Adresse TUN invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="410"/>
        <source>Could not create the TUN adapter</source>
        <translation>Impossible de créer l&apos;adaptateur TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="415"/>
        <source>Could not read the TUN adapter LUID</source>
        <translation>Impossible de lire le LUID de l&apos;adaptateur TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="448"/>
        <source>Invalid fake-ip CIDR: %1</source>
        <translation>CIDR fake-ip invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="468"/>
        <source>Could not point system DNS at the TUN address: %1</source>
        <translation>Impossible de faire pointer le DNS système vers l&apos;adresse TUN : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="481"/>
        <source>TUN adapter ready: %1 %2/%3, MTU %4, routes %5, Wintun %6</source>
        <translation>Adaptateur TUN prêt : %1 %2/%3, MTU %4, routes %5, Wintun %6</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="487"/>
        <source>taken over</source>
        <translation>pris en charge</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="488"/>
        <source>not taken over</source>
        <translation>non pris en charge</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="505"/>
        <source>Found a stale TUN-enabled state; cleaned it up and retried</source>
        <translation>État TUN activé obsolète détecté ; nettoyé et nouvel essai</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="517"/>
        <source>TUN permission not granted yet (enter the password in the prompt that appears)</source>
        <translation>Permission TUN pas encore accordée (saisissez le mot de passe dans l&apos;invite qui s&apos;affiche)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="542"/>
        <source>Socket path too long</source>
        <translation>Chemin de socket trop long</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="553"/>
        <source>Failed to listen on the socket: %1</source>
        <translation>Échec de l&apos;écoute sur la socket : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="583"/>
        <source>Could not start the helper: %1</source>
        <translation>Impossible de démarrer l&apos;assistant : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="595"/>
        <source>Timed out waiting for the helper (20 s)</source>
        <translation>Délai dépassé en attendant l&apos;assistant (20 s)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="611"/>
        <source>The helper exited unexpectedly (exit code %1)</source>
        <translation>L&apos;assistant s&apos;est arrêté de façon inattendue (code de sortie %1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="616"/>
        <source>Started the TUN helper; waiting for the device: %1</source>
        <translation>Assistant TUN démarré ; attente du périphérique : %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="618"/>
        <source>TUN handshake details: socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</source>
        <translation>Détails de la négociation TUN : socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="700"/>
        <source>The TUN helper disconnected</source>
        <translation>L&apos;assistant TUN s&apos;est déconnecté</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="749"/>
        <source>The helper did not send a TUN fd</source>
        <translation>L&apos;assistant n&apos;a pas envoyé de descripteur TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="760"/>
        <source>TUN ready: %1 (%2)</source>
        <translation>TUN prêt : %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="762"/>
        <source>real device</source>
        <translation>périphérique réel</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="815"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="868"/>
        <source>TUN is closed</source>
        <translation>TUN est fermé</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="778"/>
        <source>System tray unavailable; skipping tray setup</source>
        <translation>Zone de notification indisponible ; configuration ignorée</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="767"/>
        <source>UI style: Fusion (%1) style=%2 field=%3 accent=%4 desktop dark=%5</source>
        <translation>Style de l&apos;interface : Fusion (%1) style=%2 field=%3 accent=%4 desktop dark=%5</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>dark</source>
        <translation>sombre</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>light</source>
        <translation>clair</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>yes</source>
        <translation>oui</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>no</source>
        <translation>non</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/Icons.cpp" line="53"/>
        <source>Icon resource missing: %1 (check resources/clash-qt.qrc)</source>
        <translation>Ressource d&apos;icône manquante : %1 (vérifiez resources/clash-qt.qrc)</translation>
    </message>
</context>
<context>
    <name>clashqt::core</name>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="141"/>
        <source>YAML parse failed: %1</source>
        <translation>Échec de l&apos;analyse YAML : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="165"/>
        <source>YAML emit failed: %1</source>
        <translation>Échec de la génération YAML : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="312"/>
        <source>Unknown error</source>
        <translation>Erreur inconnue</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="549"/>
        <source>Stream closed</source>
        <translation>Flux fermé</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="566"/>
        <source>Write timed out (flow-control window stayed closed)</source>
        <translation>Délai d&apos;écriture dépassé (la fenêtre de contrôle de flux est restée fermée)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="570"/>
        <source>Session released</source>
        <translation>Session libérée</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="627"/>
        <source>Session release</source>
        <translation>Libération de session</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="656"/>
        <source>Could not parse server address %1 (%2)</source>
        <translation>Impossible d&apos;analyser l&apos;adresse du serveur %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="676"/>
        <source>Failed to create the UDP socket: %1</source>
        <translation>Échec de la création de la socket UDP : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="689"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="700"/>
        <source>UDP connect failed: %1</source>
        <translation>Échec de la connexion UDP : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="714"/>
        <source>Failed to create the QUIC context</source>
        <translation>Échec de la création du contexte QUIC</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="728"/>
        <source>Failed to create the QUIC connection</source>
        <translation>Échec de la création de la connexion QUIC</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="741"/>
        <source>Failed to bind the UDP socket</source>
        <translation>Échec de la liaison de la socket UDP</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="756"/>
        <source>Failed to read the peer address: %1</source>
        <translation>Échec de la lecture de l&apos;adresse du pair : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="776"/>
        <source>Failed to build the initial QUIC peer address</source>
        <translation>Échec de la construction de l&apos;adresse de pair QUIC initiale</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="791"/>
        <source>QUIC handshake failed: %1</source>
        <translation>Échec de la poignée de main QUIC : %1</translation>
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
        <translation>Échec du traitement des événements QUIC : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="810"/>
        <source>QUIC handshake timed out (port %1)</source>
        <translation>Délai dépassé pour la poignée de main QUIC (port %1)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="820"/>
        <source>The server provided no certificate</source>
        <translation>Le serveur n&apos;a fourni aucun certificat</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="831"/>
        <source>Server certificate fingerprint does not match the config (fingerprint check failed)</source>
        <translation>L&apos;empreinte du certificat serveur ne correspond pas à la configuration (vérification de l&apos;empreinte échouée)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="852"/>
        <source>Failed to create a QUIC stream: %1</source>
        <translation>Échec de la création d&apos;un flux QUIC : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="856"/>
        <source>Timed out creating a QUIC stream</source>
        <translation>Délai dépassé lors de la création d&apos;un flux QUIC</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="876"/>
        <source>Failed to write to a QUIC stream: %1</source>
        <translation>Échec de l&apos;écriture dans un flux QUIC : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="881"/>
        <source>Timed out writing to a QUIC stream</source>
        <translation>Délai dépassé lors de l&apos;écriture dans un flux QUIC</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="971"/>
        <source>No response to authentication (timeout or connection reset, port %1)</source>
        <translation>Aucune réponse à l&apos;authentification (délai dépassé ou connexion réinitialisée, port %1)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="979"/>
        <source>Authentication rejected (HTTP status %1; check the password)</source>
        <translation>Authentification refusée (statut HTTP %1 ; vérifiez le mot de passe)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1008"/>
        <source>Node is missing server</source>
        <translation>Le nœud n&apos;a pas de serveur</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1012"/>
        <source>Node is missing password</source>
        <translation>Le nœud n&apos;a pas de mot de passe</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1019"/>
        <source>Socket setup</source>
        <translation>Configuration de la socket</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1020"/>
        <source>Handshake</source>
        <translation>Poignée de main</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1023"/>
        <source>Authentication</source>
        <translation>Authentification</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1034"/>
        <source>Session unavailable</source>
        <translation>Session indisponible</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1038"/>
        <source>Invalid target address</source>
        <translation>Adresse cible invalide</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1106"/>
        <source>Target %1 did not respond (timeout)</source>
        <translation>La cible %1 n&apos;a pas répondu (délai dépassé)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1114"/>
        <source>Connection to target %1 refused</source>
        <translation>Connexion à la cible %1 refusée</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1115"/>
        <source>Connection to target %1 refused: %2</source>
        <translation>Connexion à la cible %1 refusée : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="29"/>
        <source>Read more than %1 bytes without finding the delimiter</source>
        <translation>Plus de %1 octets lus sans trouver le délimiteur</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="42"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="87"/>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="247"/>
        <source>Connection closed</source>
        <translation>Connexion fermée</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="48"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="93"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="329"/>
        <source>Read timed out</source>
        <translation>Délai de lecture dépassé</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="62"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="104"/>
        <source>Read failed: %1</source>
        <translation>Échec de la lecture : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="126"/>
        <source>Write failed: %1</source>
        <translation>Échec de l&apos;écriture : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="133"/>
        <source>Write timed out</source>
        <translation>Délai d&apos;écriture dépassé</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="145"/>
        <source>Connection closed by the peer</source>
        <translation>Connexion fermée par le pair</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="461"/>
        <source>REALITY connection to %1:%2 failed: %3</source>
        <translation>Échec de la connexion REALITY à %1:%2 : %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="487"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="508"/>
        <source>X25519 key generation failed</source>
        <translation>Échec de la génération de clé X25519</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="514"/>
        <source>REALITY static ECDH failed</source>
        <translation>Échec de l&apos;ECDH statique REALITY</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="521"/>
        <source>Failed to build ClientHello</source>
        <translation>Échec de la construction du ClientHello</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="552"/>
        <source>Failed to generate REALITY authentication data</source>
        <translation>Échec de la génération des données d&apos;authentification REALITY</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="571"/>
        <source>REALITY received no ServerHello (record type=%1 length=%2 content=%3)</source>
        <translation>REALITY n&apos;a reçu aucun ServerHello (record type=%1 length=%2 content=%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="582"/>
        <source>Truncated ServerHello</source>
        <translation>ServerHello tronqué</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="618"/>
        <source>ServerHello contains no usable key_share</source>
        <translation>Le ServerHello ne contient aucun key_share exploitable</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="624"/>
        <source>TLS negotiated: cipher=0x%1 key-exchange group=%2</source>
        <translation>TLS négocié : cipher=0x%1 key-exchange group=%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="636"/>
        <source>The REALITY server negotiated an unsupported cipher 0x%1</source>
        <translation>Le serveur REALITY a négocié un chiffrement non pris en charge 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="704"/>
        <source>Unexpected record type %1 during the REALITY handshake</source>
        <translation>Type d&apos;enregistrement inattendu %1 pendant la poignée de main REALITY</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="713"/>
        <source>Failed to decrypt a REALITY handshake record (type=%1 length=%2 seq=%3)</source>
        <translation>Échec du déchiffrement d&apos;un enregistrement REALITY (type=%1 length=%2 seq=%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="717"/>
        <source>Failed to decrypt a REALITY handshake record</source>
        <translation>Échec du déchiffrement d&apos;un enregistrement de poignée de main REALITY</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="768"/>
        <source>The server returned no certificate</source>
        <translation>Le serveur n&apos;a renvoyé aucun certificat</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="799"/>
        <source>TLS certificate verification failed (chain=%1 hostname=%2)</source>
        <translation>Échec de la vérification du certificat TLS (chain=%1 hostname=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="800"/>
        <source>Pass</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="801"/>
        <source>Fail</source>
        <translation>Échec</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="802"/>
        <source>Match</source>
        <translation>Correspond</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="803"/>
        <source>No match</source>
        <translation>Pas de correspondance</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="809"/>
        <source>Node %1 skips TLS certificate verification</source>
        <translation>Le nœud %1 ignore la vérification du certificat TLS</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="818"/>
        <source>REALITY authentication failed (the peer is the borrowed site itself rather than a REALITY server, or public-key / short-id is misconfigured)</source>
        <translation>Échec de l&apos;authentification REALITY (le pair est le site emprunté et non un serveur REALITY, ou public-key / short-id est mal configuré)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="830"/>
        <source>Server Finished verification failed</source>
        <translation>Échec de la vérification du message Finished du serveur</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="993"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1008"/>
        <source>REALITY send failed: %1</source>
        <translation>Échec de l&apos;envoi REALITY : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1022"/>
        <source>Timed out reading a REALITY record</source>
        <translation>Délai dépassé lors de la lecture d&apos;un enregistrement REALITY</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1028"/>
        <source>REALITY failed to read a record: %1</source>
        <translation>REALITY n&apos;a pas pu lire un enregistrement : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1044"/>
        <source>Invalid REALITY record length %1</source>
        <translation>Longueur d&apos;enregistrement REALITY invalide %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1079"/>
        <source>Invalid REALITY application-record length</source>
        <translation>Longueur d&apos;enregistrement applicatif REALITY invalide</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1099"/>
        <source>Failed to decrypt a REALITY record</source>
        <translation>Échec du déchiffrement d&apos;un enregistrement REALITY</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1251"/>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="390"/>
        <source>Timed out reading the response</source>
        <translation>Délai dépassé lors de la lecture de la réponse</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1256"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="324"/>
        <source>Connection closed while reading</source>
        <translation>Connexion fermée pendant la lecture</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="73"/>
        <source>Failed to write the upgrade request: %1</source>
        <translation>Échec de l&apos;écriture de la requête d&apos;upgrade : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="77"/>
        <source>WS upgrade request: sent %1 bytes to %2:%3 (%4 pending, state %5)</source>
        <translation>Requête d&apos;upgrade WS : %1 octets envoyés à %2:%3 (%4 en attente, état %5)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="87"/>
        <source>%1 (received %2 bytes: %3)</source>
        <translation>%1 (%2 octets reçus : %3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="101"/>
        <source>WebSocket handshake rejected: %1
%2</source>
        <translation>Poignée de main WebSocket refusée : %1
%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="116"/>
        <source>WebSocket handshake verification failed (Sec-WebSocket-Accept mismatch)</source>
        <translation>Échec de la vérification de la poignée de main WebSocket (Sec-WebSocket-Accept non conforme)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="129"/>
        <source>The WebSocket handshake response carried extra data, which is not supported</source>
        <translation>La réponse de poignée de main WebSocket contenait des données supplémentaires, ce qui n&apos;est pas pris en charge</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="227"/>
        <source>The peer sent a WebSocket close frame</source>
        <translation>Le pair a envoyé une trame de fermeture WebSocket</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="236"/>
        <source>Fragmented WebSocket messages are not supported</source>
        <translation>Les messages WebSocket fragmentés ne sont pas pris en charge</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="262"/>
        <source>Timed out reading a WebSocket frame</source>
        <translation>Délai dépassé lors de la lecture d&apos;une trame WebSocket</translation>
    </message>
    <message>
        <location filename="../src/core/src/config/RawConfig.cpp" line="81"/>
        <source>The config root must be a mapping</source>
        <translation>La racine de la configuration doit être un mapping</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/cidr/IpCidrSet.cpp" line="12"/>
        <source>Ignoring an invalid CIDR rule entry: %1</source>
        <translation>Entrée de règle CIDR invalide ignorée : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="68"/>
        <source>DNS listener not started: %1</source>
        <translation>Écouteur DNS non démarré : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="77"/>
        <source>Not listening</source>
        <translation>N&apos;écoute pas</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="86"/>
        <source>dns.listen is not configured</source>
        <translation>dns.listen n&apos;est pas configuré</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="95"/>
        <source>Failed to bind UDP %1:%2: %3</source>
        <translation>Échec de la liaison UDP %1:%2 : %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="139"/>
        <source>DNS listener started: %1 (enhanced-mode=%2)</source>
        <translation>Écouteur DNS démarré : %1 (enhanced-mode=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="199"/>
        <source>DNS fake-ip: AAAA query for %1 returned an empty answer</source>
        <translation>DNS fake-ip : la requête AAAA pour %1 a renvoyé une réponse vide</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="222"/>
        <source>DNS real resolution: %1 -&gt; %2 record(s)</source>
        <translation>Résolution DNS réelle : %1 -&gt; %2 enregistrement(s)</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="285"/>
        <source>Invalid or non-IPv4 fake-ip-range: %1; falling back to 198.18.0.1/16</source>
        <translation>fake-ip-range invalide ou non IPv4 : %1 ; repli sur 198.18.0.1/16</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="545"/>
        <source>DNS configured: enhanced-mode=%1, nameserver=%2, fake-ip=%3</source>
        <translation>DNS configuré : enhanced-mode=%1, nameserver=%2, fake-ip=%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="915"/>
        <source>DoT (tls://) currently connects over plain TCP to %1</source>
        <translation>DoT (tls://) se connecte actuellement en TCP simple à %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="979"/>
        <source>Could not resolve the DNS upstream host through the bootstrap server: %1</source>
        <translation>Impossible de résoudre l&apos;hôte DNS amont via le serveur d&apos;amorçage : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1004"/>
        <source>Could not parse the DNS server config: %1</source>
        <translation>Impossible d&apos;analyser la configuration du serveur DNS : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1045"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="487"/>
        <source>DNS resolution failed: %1</source>
        <translation>Échec de la résolution DNS : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="92"/>
        <source>Malformed logical-rule sub-item: %1</source>
        <translation>Sous-élément de règle logique mal formé : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="103"/>
        <source>Failed to parse a logical-rule sub-item: %1 (%2)</source>
        <translation>Échec de l&apos;analyse d&apos;un sous-élément de règle logique : %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="119"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="125"/>
        <source>Empty rule</source>
        <translation>Règle vide</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="133"/>
        <source>Unknown rule type: %1</source>
        <translation>Type de règle inconnu : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="141"/>
        <source>MATCH rule is missing the target policy</source>
        <translation>La règle MATCH n&apos;a pas de stratégie cible</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="149"/>
        <source>Rule has too few fields (expected type,value,policy): %1</source>
        <translation>La règle a trop peu de champs (type,value,policy attendus) : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="197"/>
        <source>Invalid domain regular expression: %1</source>
        <translation>Expression régulière de domaine invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="213"/>
        <source>Invalid IP CIDR: %1</source>
        <translation>CIDR IP invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="225"/>
        <source>Invalid source IP CIDR: %1</source>
        <translation>CIDR IP source invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="251"/>
        <source>Invalid port: %1</source>
        <translation>Port invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="261"/>
        <source>Invalid source port: %1</source>
        <translation>Port source invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="271"/>
        <source>Invalid inbound port: %1</source>
        <translation>Port d&apos;entrée invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="288"/>
        <source>Invalid process-match expression: %1</source>
        <translation>Expression de correspondance de processus invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="298"/>
        <source>Invalid UID: %1</source>
        <translation>UID invalide : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="310"/>
        <source>References an undefined rule set: %1</source>
        <translation>Référence un jeu de règles non défini : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="325"/>
        <source>Logical rule has no usable sub-rules: %1</source>
        <translation>La règle logique n&apos;a aucune sous-règle exploitable : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="340"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="346"/>
        <source>Unsupported rule type: %1</source>
        <translation>Type de règle non pris en charge : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="352"/>
        <source>Failed to build the rule: %1</source>
        <translation>Échec de la construction de la règle : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="369"/>
        <source>Skipped a non-string rule entry</source>
        <translation>Entrée de règle non textuelle ignorée</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="420"/>
        <source>Could not parse an entry in rule set %1: %2</source>
        <translation>Impossible d&apos;analyser une entrée du jeu de règles %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="93"/>
        <source>Failed to connect to %1:%2: %3 (error=%4)</source>
        <translation>Échec de la connexion à %1:%2 : %3 (error=%4)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="177"/>
        <source>Invalid DIRECT target address</source>
        <translation>Adresse cible DIRECT invalide</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="186"/>
        <source>DIRECT UDP requires a literal IP target</source>
        <translation>DIRECT UDP exige une cible IP littérale</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="191"/>
        <source>UDP bind failed: %1</source>
        <translation>Échec de la liaison UDP : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="224"/>
        <source>Connection rejected by a rule (REJECT)</source>
        <translation>Connexion rejetée par une règle (REJECT)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="242"/>
        <source>HTTP proxy %1 claims https; connecting with plaintext CONNECT for now</source>
        <translation>Le proxy HTTP %1 annonce https ; connexion via CONNECT en clair pour l&apos;instant</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="250"/>
        <source>HTTP proxy %1 is missing server/port</source>
        <translation>Le proxy HTTP %1 n&apos;a pas de serveur/port</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="261"/>
        <source>The HTTP proxy does not support UDP forwarding</source>
        <translation>Le proxy HTTP ne prend pas en charge le transfert UDP</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="286"/>
        <source>HTTP proxy %1 did not respond, or its response headers are too long</source>
        <translation>Le proxy HTTP %1 n&apos;a pas répondu, ou ses en-têtes de réponse sont trop longs</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="300"/>
        <source>HTTP proxy returned %1</source>
        <translation>Le proxy HTTP a renvoyé %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="335"/>
        <source>SOCKS5 proxy %1 is missing server/port</source>
        <translation>Le proxy SOCKS5 %1 n&apos;a pas de serveur/port</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="356"/>
        <source>No response to SOCKS5 negotiation</source>
        <translation>Aucune réponse à la négociation SOCKS5</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="362"/>
        <source>SOCKS5 version mismatch</source>
        <translation>Version SOCKS5 incompatible</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="368"/>
        <source>The SOCKS5 server rejected all authentication methods</source>
        <translation>Le serveur SOCKS5 a refusé toutes les méthodes d&apos;authentification</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="377"/>
        <source>SOCKS5 username or password too long</source>
        <translation>Nom d&apos;utilisateur ou mot de passe SOCKS5 trop long</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="391"/>
        <source>No response to SOCKS5 authentication</source>
        <translation>Aucune réponse à l&apos;authentification SOCKS5</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="396"/>
        <source>SOCKS5 authentication failed</source>
        <translation>Échec de l&apos;authentification SOCKS5</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="401"/>
        <source>The SOCKS5 server requires an unsupported auth method 0x%1</source>
        <translation>Le serveur SOCKS5 exige une méthode d&apos;authentification non prise en charge 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="425"/>
        <source>SOCKS5 target domain name too long</source>
        <translation>Nom de domaine cible SOCKS5 trop long</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="442"/>
        <source>Could not resolve the SOCKS5 target address: %1</source>
        <translation>Impossible de résoudre l&apos;adresse cible SOCKS5 : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="455"/>
        <source>No response to SOCKS5 CONNECT</source>
        <translation>Aucune réponse au CONNECT SOCKS5</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="464"/>
        <source>Succeeded</source>
        <translation>Succès</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="465"/>
        <source>General SOCKS server failure</source>
        <translation>Échec général du serveur SOCKS</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="466"/>
        <source>Connection not allowed by ruleset</source>
        <translation>Connexion non autorisée par le jeu de règles</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="467"/>
        <source>Network unreachable</source>
        <translation>Réseau inaccessible</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="468"/>
        <source>Host unreachable</source>
        <translation>Hôte inaccessible</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="469"/>
        <source>Connection refused</source>
        <translation>Connexion refusée</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="470"/>
        <source>TTL expired</source>
        <translation>TTL expiré</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="471"/>
        <source>Command not supported</source>
        <translation>Commande non prise en charge</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="472"/>
        <source>Address type not supported</source>
        <translation>Type d&apos;adresse non pris en charge</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="475"/>
        <source>Unknown error %1</source>
        <translation>Erreur inconnue %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="477"/>
        <source>SOCKS5 connection refused: %1 (%2)</source>
        <translation>Connexion SOCKS5 refusée : %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="491"/>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="503"/>
        <source>Truncated SOCKS5 response</source>
        <translation>Réponse SOCKS5 tronquée</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="498"/>
        <source>The SOCKS5 response contains an unknown address type %1</source>
        <translation>La réponse SOCKS5 contient un type d&apos;adresse inconnu %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="521"/>
        <source>SOCKS5 UDP ASSOCIATE forwarding is not implemented yet (Phase 2)</source>
        <translation>Le transfert UDP ASSOCIATE de SOCKS5 n&apos;est pas encore implémenté (phase 2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="560"/>
        <source>Node %1 (%2) is not supported yet: %3</source>
        <translation>Nœud %1 (%2) pas encore pris en charge : %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="632"/>
        <source>Group %1 switched to %2</source>
        <translation>Groupe %1 basculé sur %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="645"/>
        <source>Proxy group %1 has no available members</source>
        <translation>Le groupe de proxies %1 n&apos;a aucun membre disponible</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="658"/>
        <source>Group %1 member %2 failed to connect: %3</source>
        <translation>Échec de connexion du membre %2 du groupe %1 : %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="67"/>
        <source>This protocol is not implemented yet</source>
        <translation>Ce protocole n&apos;est pas encore implémenté</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="82"/>
        <source>Skipped non-mapping entries in proxies</source>
        <translation>Entrées non-mapping ignorées dans proxies</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="91"/>
        <source>Unrecognized node: %1 (type=%2)</source>
        <translation>Nœud non reconnu : %1 (type=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="92"/>
        <source>&lt;unnamed&gt;</source>
        <translation>&lt;sans nom&gt;</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="96"/>
        <source>Duplicate node name; ignoring the later definition: %1</source>
        <translation>Nom de nœud en double ; la définition suivante est ignorée : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="115"/>
        <source>Unrecognized proxy group: %1 (type=%2)</source>
        <translation>Groupe de proxies non reconnu : %1 (type=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="154"/>
        <source>Proxy group %1 has no members</source>
        <translation>Le groupe de proxies %1 n&apos;a aucun membre</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="168"/>
        <source>Proxy group %1 has members that cannot be resolved (a missing node or a reference cycle); skipped. Members: %2</source>
        <translation>Le groupe de proxies %1 contient des membres non résolus (nœud manquant ou cycle de références) ; ignoré. Membres : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="180"/>
        <source>Adapters loaded: %1 nodes/groups, %2 unimplemented protocols, %3 warnings</source>
        <translation>Adaptateurs chargés : %1 nœuds/groupes, %2 protocoles non implémentés, %3 avertissements</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="197"/>
        <source>The config defines its own GLOBAL group; using it as-is</source>
        <translation>La configuration définit son propre groupe GLOBAL ; utilisation telle quelle</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="231"/>
        <source>Built the built-in GLOBAL group with %1 member(s)</source>
        <translation>Groupe GLOBAL intégré créé avec %1 membre(s)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="234"/>
        <source>The profile has no usable node; the GLOBAL group only contains DIRECT</source>
        <translation>Le profil n&apos;a aucun nœud exploitable ; le groupe GLOBAL ne contient que DIRECT</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="52"/>
        <source>fingerprint is not a valid SHA-256 hex string</source>
        <translation>fingerprint n&apos;est pas une chaîne hexadécimale SHA-256 valide</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="59"/>
        <source>obfs=%1 is set (Salamander obfuscation is not implemented yet)</source>
        <translation>obfs=%1 est défini (l&apos;obfuscation Salamander n&apos;est pas encore implémentée)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="63"/>
        <source>realm-opts is set (realm is not implemented yet)</source>
        <translation>realm-opts est défini (realm n&apos;est pas encore implémenté)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="67"/>
        <source>Missing password</source>
        <translation>Mot de passe manquant</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="78"/>
        <source>Node %1 does not support UDP relay yet (Hysteria2 needs QUIC DATAGRAM, which OpenSSL 3.2 does not provide)</source>
        <translation>Le nœud %1 ne prend pas encore en charge le relais UDP (Hysteria2 requiert QUIC DATAGRAM, qu&apos;OpenSSL 3.2 ne fournit pas)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="84"/>
        <source>Node %1 is not supported yet: %2</source>
        <translation>Nœud %1 pas encore pris en charge : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="92"/>
        <source>Node %1 has an invalid target address</source>
        <translation>Le nœud %1 a une adresse cible invalide</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="133"/>
        <source>Node %1 failed to connect (attempt %2); retrying on another port: %3</source>
        <translation>Échec de connexion du nœud %1 (tentative %2) ; nouvel essai sur un autre port : %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="140"/>
        <source>Node %1 failed to connect: %2</source>
        <translation>Échec de connexion du nœud %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="149"/>
        <source>Node %1 failed to establish the tunnel: %2</source>
        <translation>Le nœud %1 n&apos;a pas pu établir le tunnel : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="158"/>
        <source>Node %1: the server has UDP disabled; UDP traffic will not work</source>
        <translation>Nœud %1 : UDP est désactivé sur le serveur ; le trafic UDP ne fonctionnera pas</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="61"/>
        <source>Target domain name too long</source>
        <translation>Nom de domaine cible trop long</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="78"/>
        <source>Could not resolve the target address: %1</source>
        <translation>Impossible de résoudre l&apos;adresse cible : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="121"/>
        <source>Node %1 is missing server/port</source>
        <translation>Le nœud %1 n&apos;a pas de serveur/port</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="138"/>
        <source>Failed to connect to %1:%2: %3</source>
        <translation>Échec de la connexion à %1:%2 : %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="157"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="442"/>
        <source>Node %1 is missing uuid</source>
        <translation>Le nœud %1 n&apos;a pas d&apos;uuid</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="163"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="448"/>
        <source>Node %1 has an invalid uuid: %2</source>
        <translation>Le nœud %1 a un uuid invalide : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="176"/>
        <source>Node %1 uses an unsupported flow: %2</source>
        <translation>Le nœud %1 utilise un flow non pris en charge : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="181"/>
        <source>Node %1 uses vision but has no reality-opts configured; this combination is not implemented yet</source>
        <translation>Le nœud %1 utilise vision sans reality-opts configuré ; cette combinaison n&apos;est pas encore implémentée</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="196"/>
        <source>Node %1 has an invalid reality-opts.public-key (it should decode to 32 bytes)</source>
        <translation>Le nœud %1 a un reality-opts.public-key invalide (32 octets attendus après décodage)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="200"/>
        <source>Node %1 has a reality-opts.short-id that is too long</source>
        <translation>Le nœud %1 a un reality-opts.short-id trop long</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="208"/>
        <source>Node %1 REALITY handshake failed: %2</source>
        <translation>Échec de la poignée de main REALITY du nœud %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="236"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="293"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="322"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="541"/>
        <source>Node %1 failed to send the VLESS request</source>
        <translation>Le nœud %1 n&apos;a pas pu envoyer la requête VLESS</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="252"/>
        <source>Node %1 transport %2 is not implemented yet (ws, REALITY and plain TCP are supported)</source>
        <translation>Le transport %2 du nœud %1 n&apos;est pas encore implémenté (ws, REALITY et TCP simple sont pris en charge)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="280"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="317"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="497"/>
        <source>Node %1 TLS connection failed: %2</source>
        <translation>Échec de la connexion TLS du nœud %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="288"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="340"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="507"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="525"/>
        <source>Node %1 WebSocket handshake failed: %2</source>
        <translation>Échec de la poignée de main WebSocket du nœud %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="350"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="355"/>
        <source>Failed to send the VLESS request: %1</source>
        <translation>Échec de l&apos;envoi de la requête VLESS : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="373"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="397"/>
        <source>Failed to read the VLESS response: %1</source>
        <translation>Échec de la lecture de la réponse VLESS : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="377"/>
        <source>Truncated VLESS response</source>
        <translation>Réponse VLESS tronquée</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="382"/>
        <source>Truncated VLESS response addons</source>
        <translation>Addons de réponse VLESS tronqués</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="404"/>
        <source>Failed to read the VLESS response addons: %1</source>
        <translation>Échec de la lecture des addons de réponse VLESS : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="454"/>
        <source>VLESS UDP is missing the target address</source>
        <translation>Le VLESS UDP n&apos;a pas d&apos;adresse cible</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="488"/>
        <source>Node %1 has an invalid reality-opts.public-key</source>
        <translation>Le nœud %1 a un reality-opts.public-key invalide</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="515"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="533"/>
        <source>Node %1 transport %2 does not support UDP</source>
        <translation>Le transport %2 du nœud %1 ne prend pas en charge UDP</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="54"/>
        <source>%1 has an invalid listen address: %2</source>
        <translation>%1 a une adresse d&apos;écoute invalide : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="62"/>
        <source>%1 failed to listen on %2:%3: %4</source>
        <translation>%1 n&apos;a pas pu écouter sur %2:%3 : %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="76"/>
        <source>%1 inbound started: %2:%3 (concurrency limit %4)</source>
        <translation>Entrée %1 démarrée : %2:%3 (limite de concurrence %4)</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="126"/>
        <source>%1 still has %2 open connection(s)</source>
        <translation>%1 a encore %2 connexion(s) ouverte(s)</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="154"/>
        <source>Concurrency limit %1 reached; rejecting new connections</source>
        <translation>Limite de concurrence %1 atteinte ; nouvelles connexions rejetées</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="182"/>
        <source>Failed to duplicate the connection descriptor; connection dropped</source>
        <translation>Échec de la duplication du descripteur de connexion ; connexion abandonnée</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="216"/>
        <source>%1 accepted connection %2:%3</source>
        <translation>%1 a accepté la connexion %2:%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="231"/>
        <source>Could not rebuild a socket from descriptor %1</source>
        <translation>Impossible de reconstruire une socket depuis le descripteur %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="61"/>
        <source>Missing HTTP request line</source>
        <translation>Ligne de requête HTTP manquante</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="68"/>
        <source>Malformed HTTP request line: %1</source>
        <translation>Ligne de requête HTTP mal formée : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="135"/>
        <source>HTTP inbound read failed: %1</source>
        <translation>Échec de la lecture sur l&apos;entrée HTTP : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="58"/>
        <source>Unsupported SOCKS version: %1</source>
        <translation>Version SOCKS non prise en charge : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="75"/>
        <source>The client does not support username/password authentication</source>
        <translation>Le client ne prend pas en charge l&apos;authentification par nom d&apos;utilisateur/mot de passe</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="81"/>
        <source>The client offered no acceptable authentication method</source>
        <translation>Le client n&apos;a proposé aucune méthode d&apos;authentification acceptable</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="89"/>
        <source>Failed to write the SOCKS5 method selection</source>
        <translation>Échec de l&apos;écriture de la sélection de méthode SOCKS5</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="100"/>
        <source>Unsupported username/password auth version</source>
        <translation>Version d&apos;authentification nom d&apos;utilisateur/mot de passe non prise en charge</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="125"/>
        <source>SOCKS5 authentication failed (user %1)</source>
        <translation>Échec de l&apos;authentification SOCKS5 (utilisateur %1)</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="142"/>
        <source>Wrong SOCKS5 request version</source>
        <translation>Version de requête SOCKS5 incorrecte</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="152"/>
        <source>Unsupported SOCKS5 command 0x%1</source>
        <translation>Commande SOCKS5 non prise en charge 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="180"/>
        <source>Unsupported SOCKS5 address type 0x%1</source>
        <translation>Type d&apos;adresse SOCKS5 non pris en charge 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="219"/>
        <source>SOCKS5 negotiation failed: %1</source>
        <translation>Échec de la négociation SOCKS5 : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>SOCKS5 request failed: %1</source>
        <translation>Échec de la requête SOCKS5 : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>Invalid target</source>
        <translation>Cible invalide</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/MixedListener.cpp" line="37"/>
        <source>MIXED inbound timed out while sniffing the protocol</source>
        <translation>L&apos;entrée MIXED a expiré pendant l&apos;inspection du protocole</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="26"/>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="24"/>
        <source>No dialer implementation available</source>
        <translation>Aucune implémentation de composeur disponible</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="32"/>
        <source>Dial failed</source>
        <translation>Échec de la composition</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="71"/>
        <source>UDP socket closed</source>
        <translation>Socket UDP fermée</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="160"/>
        <source>UDP tunnel closed</source>
        <translation>Tunnel UDP fermé</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="30"/>
        <source>UDP dial failed</source>
        <translation>Échec de la composition UDP</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="97"/>
        <source>Could not resolve the TUN adapter from its LUID</source>
        <translation>Impossible de retrouver l&apos;adaptateur TUN depuis son LUID</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="116"/>
        <source>Could not get the Wintun read event</source>
        <translation>Impossible d&apos;obtenir l&apos;événement de lecture Wintun</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="161"/>
        <source>The Wintun session was closed</source>
        <translation>La session Wintun a été fermée</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="199"/>
        <source>DNS is not taken over on the TUN address (%1): %2</source>
        <translation>Le DNS n&apos;est pas pris en charge sur l&apos;adresse TUN (%1) : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="282"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="427"/>
        <source>Invalid TUN adapter</source>
        <translation>Adaptateur TUN invalide</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="287"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="432"/>
        <source>Invalid TUN device fd</source>
        <translation>Descripteur de périphérique TUN invalide</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="306"/>
        <source>Failed to send a packet to the TUN adapter</source>
        <translation>Échec de l&apos;envoi d&apos;un paquet à l&apos;adaptateur TUN</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="314"/>
        <source>Failed to write to the TUN device: %1</source>
        <translation>Échec de l&apos;écriture dans le périphérique TUN : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="342"/>
        <source>Could not open the TUN adapter</source>
        <translation>Impossible d&apos;ouvrir l&apos;adaptateur TUN</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="349"/>
        <source>TUN data plane ready (worker thread %1, Wintun %2)</source>
        <translation>Plan de données TUN prêt (thread de travail %1, Wintun %2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="382"/>
        <source>TUN data plane ready (worker thread %1, DNS 53 %2)</source>
        <translation>Plan de données TUN prêt (thread de travail %1, DNS 53 %2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Taken over</source>
        <translation>Pris en charge</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Not taken over</source>
        <translation>Non pris en charge</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="412"/>
        <source>Failed to read the TUN device: %1</source>
        <translation>Échec de la lecture du périphérique TUN : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="414"/>
        <source>Failed to read the TUN device</source>
        <translation>Échec de la lecture du périphérique TUN</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="185"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="193"/>
        <source>Idle timeout</source>
        <translation>Délai d&apos;inactivité dépassé</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="216"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="224"/>
        <source>Closing TUN</source>
        <translation>Fermeture de TUN</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="248"/>
        <source>Re-entered TUN packet handling; dropped this packet to avoid concurrent flow-table writes</source>
        <translation>Retour dans le traitement des paquets TUN ; paquet abandonné pour éviter des écritures concurrentes dans la table de flux</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="268"/>
        <source>Dropped a non-IPv4 packet from TUN (IPv6 is not supported yet)</source>
        <translation>Paquet non IPv4 provenant de TUN abandonné (IPv6 n&apos;est pas encore pris en charge)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="321"/>
        <source>Dropped an IP fragment from TUN (no reassembly)</source>
        <translation>Fragment IP provenant de TUN abandonné (pas de réassemblage)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="421"/>
        <source>TUN: dropped link-local/multicast UDP to %1</source>
        <translation>TUN : UDP link-local/multicast vers %1 abandonné</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="535"/>
        <source>TUN: answered ICMP echo locally (%1, fake-ip)</source>
        <translation>TUN : écho ICMP répondu localement (%1, fake-ip)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="565"/>
        <source>TUN TCP segment %1:%2 -&gt; %3 flags=0x%4 seq=%5 len=%6 known flows=%7</source>
        <translation>Segment TCP TUN %1:%2 -&gt; %3 flags=0x%4 seq=%5 len=%6 flux connus=%7</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="590"/>
        <source>TUN: TCP port 53 is not supported yet; replying RST so the client falls back to UDP</source>
        <translation>TUN : le port TCP 53 n&apos;est pas encore pris en charge ; réponse RST pour que le client repasse en UDP</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="640"/>
        <source>Client RST</source>
        <translation>RST client</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="668"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1227"/>
        <source>Client closed and upstream already disconnected</source>
        <translation>Client fermé et amont déjà déconnecté</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="674"/>
        <source>Four-way handshake complete</source>
        <translation>Poignée de main en quatre temps terminée</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="778"/>
        <source>TUN: UDP target failed recently; replying with ICMP: %1</source>
        <translation>TUN : la cible UDP a échoué récemment ; réponse ICMP : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="785"/>
        <source>TUN: UDP sessions exceed %1; rejecting new sessions</source>
        <translation>TUN : les sessions UDP dépassent %1 ; nouvelles sessions rejetées</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="869"/>
        <source>TUN UDP session established: %1:%2 -&gt; %3</source>
        <translation>Session UDP TUN établie : %1:%2 -&gt; %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="899"/>
        <source>TUN UDP dial failed for %1: %2</source>
        <translation>Échec de la composition UDP TUN pour %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="928"/>
        <source>TUN UDP channel closed: %1</source>
        <translation>Canal UDP TUN fermé : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="930"/>
        <source>Upstream closed</source>
        <translation>Amont fermé</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="950"/>
        <source>TUN closed UDP session %1:%2 (%3)</source>
        <translation>TUN a fermé la session UDP %1:%2 (%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1044"/>
        <source>TUN: target failed recently; sending RST: %1</source>
        <translation>TUN : la cible a échoué récemment ; envoi de RST : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1098"/>
        <source>TUN: concurrent connections exceed %1; rejecting new connections</source>
        <translation>TUN : les connexions simultanées dépassent %1 ; nouvelles connexions rejetées</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1149"/>
        <source>TUN new connection %1:%2 -&gt; %3:%4</source>
        <translation>Nouvelle connexion TUN %1:%2 -&gt; %3:%4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1179"/>
        <source>TUN dial failed for %1:%2: %3</source>
        <translation>Échec de la composition TUN pour %1:%2 : %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1204"/>
        <source>TUN upstream data: %1 bytes (%2 pending)</source>
        <translation>Données amont TUN : %1 octets (%2 en attente)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1336"/>
        <source>TUN closed connection %1:%2 (%3)</source>
        <translation>TUN a fermé la connexion %1:%2 (%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="61"/>
        <source>Connection %1: client disconnected (up %2 / down %3 bytes)</source>
        <translation>Connexion %1 : client déconnecté (envoi %2 / réception %3 octets)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="70"/>
        <source>Connection %1: upstream closed (up %2 / down %3 bytes) %4</source>
        <translation>Connexion %1 : amont fermé (envoi %2 / réception %3 octets) %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="111"/>
        <source>Connection %1: short upstream write, %2 / %3 bytes</source>
        <translation>Connexion %1 : écriture amont partielle, %2 / %3 octets</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="224"/>
        <source>Config not loaded yet</source>
        <translation>Configuration pas encore chargée</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="232"/>
        <source>GLOBAL mode is on, but the config has no GLOBAL group</source>
        <translation>Le mode GLOBAL est actif, mais la configuration n&apos;a pas de groupe GLOBAL</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="266"/>
        <source>No MATCH fallback rule in the rule list; treating as DIRECT</source>
        <translation>Aucune règle de repli MATCH dans la liste ; traitement en DIRECT</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="277"/>
        <source>Rule %1 points at a policy that does not exist: %2</source>
        <translation>La règle %1 pointe vers une stratégie inexistante : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="311"/>
        <source>Closed an inbound connection with no target address</source>
        <translation>Connexion entrante sans adresse cible fermée</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="332"/>
        <source>Could not determine an outbound policy (%1 -&gt; %2); connection closed</source>
        <translation>Impossible de déterminer une politique de sortie (%1 -&gt; %2) ; connexion fermée</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="333"/>
        <source>Unknown reason</source>
        <translation>Raison inconnue</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="351"/>
        <source>The DIRECT outbound needs DNS resolution, but no resolver is available</source>
        <translation>La sortie DIRECT nécessite une résolution DNS, mais aucun résolveur n&apos;est disponible</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="360"/>
        <source>DNS resolution failed; connection closed: %1</source>
        <translation>Échec de la résolution DNS ; connexion fermée : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="381"/>
        <source>Rule %1 dropped the connection: %2</source>
        <translation>La règle %1 a abandonné la connexion : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="394"/>
        <source>Outbound %1 failed to connect to %2:%3: %4</source>
        <translation>La sortie %1 n&apos;a pas pu se connecter à %2:%3 : %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="404"/>
        <source>UDP relay is not implemented yet; connection closed</source>
        <translation>Le relais UDP n&apos;est pas encore implémenté ; connexion fermée</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="430"/>
        <source>%1 -&gt; %2:%3 via %4 (rule %5)</source>
        <translation>%1 -&gt; %2:%3 via %4 (règle %5)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="309"/>
        <source>Vision: the server echoed an unknown UUID</source>
        <translation>Vision : le serveur a renvoyé un UUID inconnu</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="338"/>
        <source>Vision: read an unknown command %1</source>
        <translation>Vision : commande inconnue lue %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="385"/>
        <source>Connection closed while reading the response</source>
        <translation>Connexion fermée pendant la lecture de la réponse</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="88"/>
        <source>Failed to send the WebSocket upgrade request</source>
        <translation>Échec de l&apos;envoi de la requête d&apos;upgrade WebSocket</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="104"/>
        <source>Connection closed during the WebSocket upgrade (received %1 bytes: %2)</source>
        <translation>Connexion fermée pendant l&apos;upgrade WebSocket (%1 octets reçus : %2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="113"/>
        <source>WebSocket upgrade handshake timed out</source>
        <translation>Délai dépassé pour la poignée de main d&apos;upgrade WebSocket</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="127"/>
        <source>WebSocket handshake rejected: %1</source>
        <translation>Poignée de main WebSocket refusée : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="154"/>
        <source>Server-sent WebSocket frames must not be masked</source>
        <translation>Les trames WebSocket envoyées par le serveur ne doivent pas être masquées</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="179"/>
        <source>WebSocket frame too long</source>
        <translation>Trame WebSocket trop longue</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="127"/>
        <source>DNS override is off; using system resolution</source>
        <translation>La surcharge DNS est désactivée ; résolution système utilisée</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="152"/>
        <source>Rule set %1 has an invalid behavior: %2</source>
        <translation>Le jeu de règles %1 a un comportement invalide : %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="178"/>
        <source>Could not read the rule-set file: %1</source>
        <translation>Impossible de lire le fichier de jeu de règles : %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="181"/>
        <source>Remote rule sets (type=http) are not implemented yet; skipped</source>
        <translation>Les jeux de règles distants (type=http) ne sont pas encore implémentés ; ignorés</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="207"/>
        <source>Config hot-reloaded (inbound listeners kept)</source>
        <translation>Configuration rechargée à chaud (écouteurs d&apos;entrée conservés)</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="248"/>
        <source>Mixed port %1 failed to start</source>
        <translation>Le port mixte %1 n&apos;a pas pu démarrer</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="265"/>
        <source>SOCKS5 port %1 failed to start</source>
        <translation>Le port SOCKS5 %1 n&apos;a pas pu démarrer</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="280"/>
        <source>HTTP port %1 failed to start</source>
        <translation>Le port HTTP %1 n&apos;a pas pu démarrer</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="290"/>
        <source>redir-port / tproxy-port transparent proxying is not implemented yet (Phase 2); those ports are not listening</source>
        <translation>Le proxy transparent via redir-port / tproxy-port n&apos;est pas encore implémenté (phase 2) ; ces ports n&apos;écoutent pas</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="295"/>
        <source>No inbound port is enabled in the config (mixed-port / socks-port / port are all 0)</source>
        <translation>Aucun port d&apos;entrée n&apos;est activé dans la configuration (mixed-port / socks-port / port valent tous 0)</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="335"/>
        <source>No config has been applied yet</source>
        <translation>Aucune configuration n&apos;a encore été appliquée</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="349"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="350"/>
        <source>Started</source>
        <translation>Démarré</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="367"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="384"/>
        <source>Stopped</source>
        <translation>Arrêté</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="403"/>
        <source>Outbound mode switched to %1</source>
        <translation>Mode de sortie basculé sur %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="437"/>
        <source>TUN is already running</source>
        <translation>TUN est déjà actif</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="447"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="593"/>
        <source>TUN is closed</source>
        <translation>TUN est fermé</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="456"/>
        <source>No usable outbound matched</source>
        <translation>Aucune sortie exploitable correspondante</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="530"/>
        <source>TUN data-plane error (%1); TUN stopped</source>
        <translation>Erreur du plan de données TUN (%1) ; TUN arrêté</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="545"/>
        <source>TUN taken over (device=%1); data plane running on a worker thread</source>
        <translation>TUN pris en charge (device=%1) ; plan de données exécuté dans un thread de travail</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="586"/>
        <source>The TUN data-plane thread is still winding down; it will be released after exit</source>
        <translation>Le thread du plan de données TUN est encore en cours d&apos;arrêt ; il sera libéré après la sortie</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="633"/>
        <source>Group %1 does not exist</source>
        <translation>Le groupe %1 n&apos;existe pas</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="638"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="766"/>
        <source>%1 is not a proxy group</source>
        <translation>%1 n&apos;est pas un groupe de proxies</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="642"/>
        <source>Group %1 has no member %2</source>
        <translation>Le groupe %1 n&apos;a pas de membre %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="678"/>
        <source>Group %1 references member %2, which no longer exists; ignored</source>
        <translation>Le groupe %1 référence le membre %2, qui n&apos;existe plus ; ignoré</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="688"/>
        <source>Restored manual selections for %1 proxy group(s)</source>
        <translation>Sélections manuelles restaurées pour %1 groupe(s) de proxies</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="703"/>
        <source>Node does not exist: %1</source>
        <translation>Le nœud n&apos;existe pas : %1</translation>
    </message>
</context>
</TS>
