<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ja_JP">
<context>
    <name>clashqt::AppContext</name>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="188"/>
        <source>Failed to turn off the system proxy on exit: %1 (the desktop still points at the local port)</source>
        <translation>終了時にシステムプロキシを無効にできませんでした: %1（デスクトップは依然ローカルポートを参照しています）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="193"/>
        <source>System proxy turned off before exit, so the desktop stops pointing at a stopped local port</source>
        <translation>終了前にシステムプロキシを無効にしたため、停止したローカルポートを参照しなくなります</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="208"/>
        <source>Failed to load the config. Check that the files in %1 are writable</source>
        <translation>設定を読み込めませんでした。%1 内のファイルが書き込み可能か確認してください</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="224"/>
        <source>Data directory: %1 (portable: %2)</source>
        <translation>データディレクトリ: %1（ポータブル: %2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="225"/>
        <source>yes</source>
        <translation>はい</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="226"/>
        <source>no</source>
        <translation>いいえ</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="239"/>
        <source>Config generation failed: %1</source>
        <translation>設定の生成に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="268"/>
        <source>Start failed: %1</source>
        <translation>起動に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="269"/>
        <source>Startup failed: %1</source>
        <translation>起動に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="304"/>
        <source>Kernel failed to restart after config reload: %1</source>
        <translation>設定の再読み込み後、カーネルの再起動に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="384"/>
        <location filename="../src/app/src/AppContext.cpp" line="411"/>
        <source>Config not loaded yet</source>
        <translation>設定がまだ読み込まれていません</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="423"/>
        <source>TUN enable request: enabled=%1 running=%2 starting(old)=%3</source>
        <translation>TUN 有効化リクエスト: enabled=%1 running=%2 starting(old)=%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="434"/>
        <source>TUN disabled; system routes restored</source>
        <translation>TUN を無効化しました。システムルートを復元しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="446"/>
        <source>TUN helper not found (it should sit next to the main executable): %1</source>
        <translation>TUN ヘルパーが見つかりません（メイン実行ファイルと同じ場所に置く必要があります）: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="485"/>
        <source>Removed the wildcard &quot;*&quot; from fake-ip-filter (it makes every domain skip fake-ip)</source>
        <translation>fake-ip-filter からワイルドカード「*」を削除しました（これがあるとすべてのドメインが fake-ip をスキップします）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="487"/>
        <source>Fixed the DNS config: the &quot;*&quot; in fake-ip-filter made every domain skip fake-ip, so it was removed</source>
        <translation>DNS 設定を修正しました: fake-ip-filter の「*」がすべてのドメインで fake-ip をスキップさせていたため、削除しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="501"/>
        <source>TUN device name changed from Mihomo to %1</source>
        <translation>TUN デバイス名を Mihomo から %1 に変更しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="515"/>
        <source>TUN mode needs kernel DNS (fake-ip); DNS override was enabled automatically</source>
        <translation>TUN モードにはカーネル DNS（fake-ip）が必要なため、DNS 上書きを自動で有効にしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="517"/>
        <source>TUN mode needs kernel DNS; &quot;DNS override&quot; was enabled automatically</source>
        <translation>TUN モードにはカーネル DNS が必要なため、「DNS 上書き」を自動で有効にしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="532"/>
        <source>The kernel failed to start, so TUN cannot be enabled</source>
        <translation>カーネルの起動に失敗したため、TUN を有効にできません</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="555"/>
        <location filename="../src/app/src/AppContext.cpp" line="607"/>
        <source>Failed to enable TUN: %1</source>
        <translation>TUN を有効にできませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="576"/>
        <source>Enabling TUN timed out; state was force-reset</source>
        <translation>TUN の有効化がタイムアウトしたため、状態を強制リセットしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="579"/>
        <source>Timed out enabling TUN; state was reset, please try again</source>
        <translation>TUN の有効化がタイムアウトしました。状態をリセットしたので、もう一度お試しください</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="583"/>
        <source>Enabling TUN…</source>
        <translation>TUN を有効化しています…</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="608"/>
        <source>Could not take over the device</source>
        <translation>デバイスを引き継げませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="620"/>
        <source>TUN enabled (system routes taken over)</source>
        <translation>TUN を有効化しました（システムルートを引き継ぎ済み）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="621"/>
        <source>TUN enabled (routes not taken over)</source>
        <translation>TUN を有効化しました（ルートは未引き継ぎ）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="630"/>
        <source>Kernel is not running; skipping switch-state restore</source>
        <translation>カーネルが実行されていないため、スイッチ状態の復元をスキップします</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="640"/>
        <source>Restored TUN to its previous state</source>
        <translation>TUN を以前の状態に戻しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="643"/>
        <source>Failed to restore TUN: %1</source>
        <translation>TUN を元に戻せませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="645"/>
        <source>Failed to restore TUN mode: %1</source>
        <translation>TUN モードを元に戻せませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="654"/>
        <source>Restored the system proxy to its previous state</source>
        <translation>システムプロキシを以前の状態に戻しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="657"/>
        <location filename="../src/app/src/AppContext.cpp" line="659"/>
        <source>Failed to restore the system proxy: %1</source>
        <translation>システムプロキシを元に戻せませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="674"/>
        <source>Start failed; system proxy not enabled: %1</source>
        <translation>起動に失敗したため、システムプロキシは有効にしていません: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="686"/>
        <source>No inbound port available; cannot set the system proxy</source>
        <translation>利用可能なインバウンドポートがないため、システムプロキシを設定できません</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="696"/>
        <source>127.0.0.1:%1 is not listening locally; refusing to enable the system proxy (the whole desktop would lose connectivity by pointing at an empty port)</source>
        <translation>127.0.0.1:%1 がローカルで待ち受けていないため、システムプロキシを有効にしません（空のポートを参照するとデスクトップ全体が通信できなくなります）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="708"/>
        <source>Failed to set the system proxy: %1</source>
        <translation>システムプロキシを設定できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="716"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>システムプロキシを無効にできませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="749"/>
        <source>Unknown outbound mode: %1</source>
        <translation>不明なアウトバウンドモード: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="788"/>
        <location filename="../src/app/src/AppContext.cpp" line="813"/>
        <location filename="../src/app/src/AppContext.cpp" line="901"/>
        <location filename="../src/app/src/AppContext.cpp" line="979"/>
        <source>Failed to write the profile contents</source>
        <translation>プロファイルの内容を書き込めませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="793"/>
        <location filename="../src/app/src/AppContext.cpp" line="905"/>
        <source>Failed to write the profile list</source>
        <translation>プロファイル一覧を書き込めませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="797"/>
        <source>Added local config: %1</source>
        <translation>ローカル設定を追加しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="805"/>
        <location filename="../src/app/src/AppContext.cpp" line="917"/>
        <location filename="../src/app/src/AppContext.cpp" line="1020"/>
        <source>Profile does not exist: %1</source>
        <translation>プロファイルが存在しません: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="819"/>
        <source>Replaced the config file of &quot;%1&quot;</source>
        <translation>「%1」の設定ファイルを置き換えました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="831"/>
        <source>Invalid profile URL: %1</source>
        <translation>無効なプロファイル URL: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="858"/>
        <source>Profile download timed out (%1 ms)</source>
        <translation>プロファイルのダウンロードがタイムアウトしました（%1 ms）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="865"/>
        <source>Profile download failed: %1</source>
        <translation>プロファイルのダウンロードに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="884"/>
        <source>The profile is not a valid Clash config: %1</source>
        <translation>有効な Clash 設定ではありません: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="909"/>
        <source>Added profile: %1</source>
        <translation>プロファイルを追加しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="921"/>
        <source>Profile &quot;%1&quot; is not a remote profile; nothing to update</source>
        <translation>プロファイル「%1」はリモートではないため、更新する内容がありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="948"/>
        <source>Updating &quot;%1&quot; timed out</source>
        <translation>「%1」の更新がタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="954"/>
        <source>Failed to update &quot;%1&quot;: %2</source>
        <translation>「%1」の更新に失敗しました: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="972"/>
        <source>The update for &quot;%1&quot; returned invalid content; the old config was kept: %2</source>
        <translation>「%1」の更新で無効な内容が返されたため、以前の設定を保持しました: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="993"/>
        <source>Profile updated: %1</source>
        <translation>プロファイルを更新しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1025"/>
        <source>Switched to profile: %1</source>
        <translation>プロファイルを切り替えました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1040"/>
        <source>Override content must be a YAML mapping: %1</source>
        <translation>上書き内容は YAML マッピングである必要があります: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1047"/>
        <source>Unnamed override</source>
        <translation>名前のない上書き</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1054"/>
        <source>Failed to write the override contents</source>
        <translation>上書きの内容を書き込めませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1058"/>
        <source>Failed to write the override list</source>
        <translation>上書き一覧を書き込めませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1062"/>
        <source>Added override: %1</source>
        <translation>上書きを追加しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1093"/>
        <source>Failed to parse the rule-override file; treating it as empty: %1</source>
        <translation>ルール上書きファイルを解析できないため、空として扱います: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1117"/>
        <source>No profile selected; cannot save custom rules</source>
        <translation>プロファイルが選択されていないため、カスタムルールを保存できません</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1129"/>
        <source>%1 entry %2 contains a line break (each rule must stay on one line)</source>
        <translation>%1 の %2 番目に改行が含まれています（各ルールは 1 行で記述してください）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1137"/>
        <source>%1 entry %2 is not a valid rule (expected at least &quot;type,target&quot;): %3</source>
        <translation>%1 の %2 番目は有効なルールではありません（少なくとも「type,target」が必要です）: %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1147"/>
        <source>Custom rules</source>
        <translation>カスタムルール</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1148"/>
        <source>Fallback rules</source>
        <translation>フォールバックルール</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1149"/>
        <source>Removal rules</source>
        <translation>削除ルール</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1182"/>
        <source>Failed to write custom rules: %1</source>
        <translation>カスタムルールを書き込めませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1187"/>
        <source>Custom rules saved (profile %1): %2 total, %3 disabled, %4 fallback / %5 removal</source>
        <translation>カスタムルールを保存しました（プロファイル %1）: 全 %2 件、無効 %3 件、フォールバック %4 件 / 削除 %5 件</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1196"/>
        <source>Custom rules applied</source>
        <translation>カスタムルールを適用しました</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionListModel</name>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="67"/>
        <source>Rule: %1</source>
        <translation>ルール: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="68"/>
        <source>Outbound: %1</source>
        <translation>アウトバウンド: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="69"/>
        <source>Up %1 · Down %2 · %3</source>
        <translation>アップロード %1 · ダウンロード %2 · %3</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="63"/>
        <source>Connections</source>
        <translation>接続</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="64"/>
        <source>Active connections, one card each. Double-click a card, or use the × on its right, to close that connection.</source>
        <translation>アクティブな接続を 1 件ずつカードで表示します。カードをダブルクリックするか、右側の × で接続を切断できます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="73"/>
        <source>Auto refresh (1 s)</source>
        <translation>自動更新（1 秒）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="77"/>
        <source>Close selected</source>
        <translation>選択項目を切断</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="78"/>
        <source>Close all</source>
        <translation>すべて切断</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="104"/>
        <source>No active connections</source>
        <translation>アクティブな接続はありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="145"/>
        <source>just now</source>
        <translation>たった今</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="146"/>
        <source>%1 min ago</source>
        <translation>%1 分前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="147"/>
        <source>%1 h ago</source>
        <translation>%1 時間前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="148"/>
        <source>%1 d ago</source>
        <translation>%1 日前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="157"/>
        <source>%1 active connection(s) · Total up %2 · Total down %3</source>
        <translation>アクティブな接続 %1 件 · 合計アップロード %2 · 合計ダウンロード %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="239"/>
        <source>Select a connection first</source>
        <translation>先に接続を選択してください</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="253"/>
        <source>Closed %1 connection(s)</source>
        <translation>%1 件の接続を切断しました</translation>
    </message>
</context>
<context>
    <name>clashqt::LogsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="48"/>
        <source>Logs</source>
        <translation>ログ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="49"/>
        <source>Application log. Logs are also written to the logs/ folder in the data directory.</source>
        <translation>アプリケーションログ。ログはデータディレクトリの logs/ フォルダにも出力されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="54"/>
        <source>Level</source>
        <translation>レベル</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="61"/>
        <source>Pause scrolling</source>
        <translation>スクロールを停止</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="64"/>
        <source>Clear view</source>
        <translation>表示をクリア</translation>
    </message>
</context>
<context>
    <name>clashqt::MainWindow</name>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="236"/>
        <source>Rule</source>
        <translation>ルール</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="237"/>
        <source>Global</source>
        <translation>グローバル</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="238"/>
        <source>Direct</source>
        <translation>ダイレクト</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="272"/>
        <source>System proxy</source>
        <translation>システムプロキシ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="282"/>
        <source>No system proxy backend found (gsettings / kwriteconfig); system proxy is unavailable</source>
        <translation>システムプロキシのバックエンドが見つかりません（gsettings / kwriteconfig）。システムプロキシは利用できません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="288"/>
        <source>TUN mode</source>
        <translation>TUN モード</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="295"/>
        <source>Captures all local traffic, including apps that ignore the system proxy. The first enable requires a one-time password prompt.</source>
        <translation>システムプロキシを無視するアプリも含め、ローカルの通信をすべて捕捉します。初回の有効化時のみパスワードの入力を求めます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="343"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="708"/>
        <source>Update this subscription</source>
        <translation>このサブスクリプションを更新</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="412"/>
        <source>Proxies</source>
        <translation>プロキシ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="414"/>
        <source>Rules</source>
        <translation>ルール</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="416"/>
        <source>Overrides</source>
        <translation>上書き</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="418"/>
        <source>Logs</source>
        <translation>ログ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="420"/>
        <source>Connections</source>
        <translation>接続</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="423"/>
        <source>Settings</source>
        <translation>設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="484"/>
        <source>Traffic</source>
        <translation>通信量</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="694"/>
        <source>No profile selected</source>
        <translation>プロファイルが選択されていません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="709"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="757"/>
        <source>Local profiles have no URL to update from</source>
        <translation>ローカルプロファイルには更新元の URL がありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="721"/>
        <source>%1 / %2</source>
        <translation>%1 / %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="730"/>
        <source>Updated %1</source>
        <translation>%1 を更新しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="745"/>
        <source>Open the profiles page</source>
        <translation>プロファイルページを開く</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="752"/>
        <source>Select a profile first</source>
        <translation>先にプロファイルを選択してください</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="765"/>
        <source>Update failed: %1</source>
        <translation>更新に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="767"/>
        <source>Subscription updated</source>
        <translation>サブスクリプションを更新しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="792"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="871"/>
        <source>Disable system proxy</source>
        <translation>システムプロキシを無効化</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="800"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="907"/>
        <source>Enable TUN mode</source>
        <translation>TUN モードを有効化</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="807"/>
        <source>Show main window</source>
        <translation>メインウィンドウを表示</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="815"/>
        <source>Quit</source>
        <translation>終了</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="872"/>
        <source>Enable system proxy</source>
        <translation>システムプロキシを有効化</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="877"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="905"/>
        <source>Enabling TUN…</source>
        <translation>TUN を有効化しています…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="881"/>
        <source>System proxy + TUN enabled</source>
        <translation>システムプロキシ + TUN を有効化済み</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="882"/>
        <source>TUN enabled</source>
        <translation>TUN 有効</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="883"/>
        <source>System proxy enabled</source>
        <translation>システムプロキシ有効</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="884"/>
        <source>System proxy disabled</source>
        <translation>システムプロキシ無効</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="900"/>
        <source>enabling…</source>
        <translation>有効化中…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="906"/>
        <source>Disable TUN mode</source>
        <translation>TUN モードを無効化</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="916"/>
        <source>Mixed port %1</source>
        <translation>混合ポート %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="918"/>
        <source>Not running</source>
        <translation>未起動</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="946"/>
        <source>Failed to switch outbound mode: %1</source>
        <translation>アウトバウンドモードを切り替えられませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="952"/>
        <source>Outbound mode: %1</source>
        <translation>アウトバウンドモード: %1</translation>
    </message>
</context>
<context>
    <name>clashqt::OverridePage</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>Domain suffix (incl. subdomains)</source>
        <translation>ドメイン接尾辞（サブドメインを含む）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>e.g. example.com</source>
        <translation>例: example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>Domain (exact)</source>
        <translation>ドメイン（完全一致）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>e.g. www.example.com</source>
        <translation>例: www.example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>Domain keyword</source>
        <translation>ドメインキーワード</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>e.g. google</source>
        <translation>例: google</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>Domain regex</source>
        <translation>ドメイン正規表現</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>e.g. ^ads\..*</source>
        <translation>例: ^ads\..*</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>Geosite category</source>
        <translation>Geosite カテゴリ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>e.g. cn / netflix / telegram</source>
        <translation>例: cn / netflix / telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>IP country</source>
        <translation>IP 国コード</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>e.g. CN / US / JP</source>
        <translation>例: CN / US / JP</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>IPv4 CIDR</source>
        <translation>IPv4 CIDR</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>e.g. 10.0.0.0/8</source>
        <translation>例: 10.0.0.0/8</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>IPv6 CIDR</source>
        <translation>IPv6 CIDR</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>e.g. 2001:db8::/32</source>
        <translation>例: 2001:db8::/32</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>ASN</source>
        <translation>ASN</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>e.g. 13335</source>
        <translation>例: 13335</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>Destination port</source>
        <translation>宛先ポート</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>e.g. 443</source>
        <translation>例: 443</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>Source port</source>
        <translation>送信元ポート</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>e.g. 1234</source>
        <translation>例: 1234</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>Network type</source>
        <translation>ネットワーク種別</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>tcp or udp</source>
        <translation>tcp または udp</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>Process name</source>
        <translation>プロセス名</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>e.g. telegram</source>
        <translation>例: telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>Process path</source>
        <translation>プロセスパス</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>e.g. /usr/bin/curl</source>
        <translation>例: /usr/bin/curl</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Rule set</source>
        <translation>ルールセット</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Choose a rule set from the config</source>
        <translation>設定からルールセットを選択</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="129"/>
        <source>Catch-all (match all remaining traffic)</source>
        <translation>全捕捉（残りの通信をすべて一致）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="405"/>
        <source>Overrides</source>
        <translation>上書き</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="406"/>
        <source>Adds custom rules on top of the profile&apos;s rules. Pick a type, content and target when adding — no rule syntax needed. The list order is the match priority; changes are saved and applied automatically.</source>
        <translation>プロファイルのルールにカスタムルールを追加します。追加時に種別・内容・対象を選ぶだけで、ルール構文の知識は不要です。一覧の順序がそのまま一致の優先度になります。変更は自動的に保存・適用されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="408"/>
        <source>Custom rules</source>
        <translation>カスタムルール</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="418"/>
        <source>New rule</source>
        <translation>新規ルール</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Enabled</source>
        <translation>有効</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Rule type</source>
        <translation>ルール種別</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Content</source>
        <translation>内容</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Target</source>
        <translation>対象</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="432"/>
        <source>Actions</source>
        <translation>操作</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="452"/>
        <source>Double-click any row to edit; uncheck it to disable (content is kept but not matched). Changes are saved and applied automatically.</source>
        <translation>行をダブルクリックすると編集できます。チェックを外すと無効になり、内容は保持されますが一致しなくなります。変更は自動的に保存・適用されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="526"/>
        <source>No profile selected; custom rules cannot be saved</source>
        <translation>プロファイルが選択されていないため、カスタムルールを保存できません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="527"/>
        <source>Applies to profile: %1 (rules are stored per profile and differ between profiles)</source>
        <translation>適用先プロファイル: %1（ルールはプロファイルごとに保存され、内容もプロファイル間で異なります）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="553"/>
        <source>%1 rules, %2 enabled</source>
        <translation>ルール %1 件、有効 %2 件</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="575"/>
        <source>Enabled; uncheck to disable</source>
        <translation>有効。チェックを外すと無効になります</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="576"/>
        <source>Disabled: not matched after saving</source>
        <translation>無効: 保存後は一致しません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="601"/>
        <source>%1 (%2)
Rule: %3</source>
        <translation>%1（%2）
ルール: %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="612"/>
        <source>Move up (higher priority)</source>
        <translation>上へ移動（優先度を上げる）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="614"/>
        <source>Move down (lower priority)</source>
        <translation>下へ移動（優先度を下げる）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="615"/>
        <source>Edit this rule</source>
        <translation>このルールを編集</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="616"/>
        <source>Delete this rule</source>
        <translation>このルールを削除</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="726"/>
        <source>Save failed</source>
        <translation>保存に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="733"/>
        <source>Applied automatically: %1 rules, %2 disabled</source>
        <translation>自動適用しました: ルール %1 件、無効 %2 件</translation>
    </message>
</context>
<context>
    <name>clashqt::ProfilesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="38"/>
        <source>Expired</source>
        <translation>期限切れ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="40"/>
        <source> (%1 days)</source>
        <translation>（%1 日）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="235"/>
        <source>Profiles</source>
        <translation>プロファイル</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="236"/>
        <source>Manage config sources. The active profile is merged with &quot;Config overrides&quot; before use.</source>
        <translation>設定ソースを管理します。有効なプロファイルは使用前に「設定の上書き」と統合されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="247"/>
        <source>Add profile</source>
        <translation>プロファイルを追加</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="248"/>
        <source>Import local config</source>
        <translation>ローカル設定をインポート</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="249"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="250"/>
        <source>Update selected</source>
        <translation>選択項目を更新</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="251"/>
        <source>Update all</source>
        <translation>すべて更新</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="252"/>
        <source>Set active</source>
        <translation>有効に設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="253"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="263"/>
        <source>Edit the name or URL (or double-click a row)</source>
        <translation>名前または URL を編集（行のダブルクリックでも可）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="276"/>
        <source>No profiles yet. Click &quot;Add profile&quot; to enter a subscription URL, or &quot;Import local config&quot; to pick an existing Clash config file.</source>
        <translation>プロファイルがまだありません。「プロファイルを追加」でサブスクリプション URL を入力するか、「ローカル設定をインポート」で既存の Clash 設定ファイルを選択してください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Active</source>
        <translation>有効</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Name</source>
        <translation>名前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Type</source>
        <translation>種別</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Traffic</source>
        <translation>通信量</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Expires</source>
        <translation>有効期限</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Updated</source>
        <translation>更新日時</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="341"/>
        <source>Remote</source>
        <translation>リモート</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="342"/>
        <source>Local</source>
        <translation>ローカル</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="343"/>
        <source>Plugin</source>
        <translation>プラグイン</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="386"/>
        <source>Add failed</source>
        <translation>追加に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="404"/>
        <source>Choose a config file</source>
        <translation>設定ファイルを選択</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="405"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Clash 設定 (*.yaml *.yml);;すべてのファイル (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="410"/>
        <source>Import failed</source>
        <translation>インポートに失敗しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="420"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="471"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="505"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="521"/>
        <source>Select a profile first</source>
        <translation>先にプロファイルを選択してください</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Save failed</source>
        <translation>保存に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Failed to write profile config</source>
        <translation>プロファイル設定を書き込めませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="446"/>
        <source>Replace failed</source>
        <translation>置き換えに失敗しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="458"/>
        <source>URL saved</source>
        <translation>URL を保存しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="459"/>
        <source>URL saved, but fetching the new content failed:

%1

Try &quot;Update selected&quot; again later.</source>
        <translation>URL は保存しましたが、新しい内容の取得に失敗しました:

%1

後で「選択項目を更新」をもう一度お試しください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="477"/>
        <source>Update failed</source>
        <translation>更新に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="489"/>
        <source>Some profiles failed to update</source>
        <translation>一部のプロファイルを更新できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="490"/>
        <source>%1 succeeded, %2 failed:

%3</source>
        <translation>成功 %1 件、失敗 %2 件:

%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="496"/>
        <source>Updated %1 profile(s)</source>
        <translation>%1 件のプロファイルを更新しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="511"/>
        <source>Switch failed</source>
        <translation>切り替えに失敗しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="527"/>
        <source>Delete profile</source>
        <translation>プロファイルを削除</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="528"/>
        <source>Delete &quot;%1&quot;? Its config file will be removed as well.</source>
        <translation>「%1」を削除しますか？設定ファイルも削除されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Delete failed</source>
        <translation>削除に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Profile not found</source>
        <translation>プロファイルが見つかりません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="545"/>
        <source>Profile deleted, but reloading the config failed: %1</source>
        <translation>プロファイルは削除しましたが、設定の再読み込みに失敗しました: %1</translation>
    </message>
</context>
<context>
    <name>clashqt::ProxiesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="66"/>
        <source>Group</source>
        <translation>グループ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="90"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="334"/>
        <source>Nodes</source>
        <translation>ノード</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="110"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="149"/>
        <source>Testing</source>
        <translation>テスト中</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="111"/>
        <source>Test</source>
        <translation>テスト</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="112"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="151"/>
        <source>Timeout</source>
        <translation>タイムアウト</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="291"/>
        <source>Proxies</source>
        <translation>プロキシ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="292"/>
        <source>Pick a node from the proxy group: click a card to switch to it. The text on the right is the last measured latency; click it to test that node again, or click Test on a node that has not been measured yet. In global mode, the GLOBAL group decides the exit for all traffic.</source>
        <translation>プロキシグループからノードを選びます。カードをクリックするとそのノードに切り替わります。右側のテキストは最後に測定した遅延で、クリックすると再テストできます。未測定のノードは「テスト」をクリックしてください。グローバルモードでは GLOBAL グループがすべての通信の出口を決めます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="298"/>
        <source>Proxy group</source>
        <translation>プロキシグループ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="309"/>
        <source>Sort</source>
        <translation>並べ替え</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="311"/>
        <source>Default order</source>
        <translation>既定の順序</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="312"/>
        <source>Name</source>
        <translation>名前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="313"/>
        <source>Type</source>
        <translation>種別</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="314"/>
        <source>Latency</source>
        <translation>遅延</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="316"/>
        <source>Sort nodes by name, protocol type or latest measured latency.
Latency sorts ascending; untested and timed-out nodes go last. Sorting never changes the selected node.</source>
        <translation>名前、プロトコル種別、最後に測定した遅延でノードを並べ替えます。
遅延は昇順で、未テストとタイムアウトのノードは末尾になります。並べ替えても選択中のノードは変わりません。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="323"/>
        <source>Test this group</source>
        <translation>このグループをテスト</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="324"/>
        <source>Test all groups</source>
        <translation>すべてテスト</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="436"/>
        <source>The active profile has no proxy groups</source>
        <translation>有効なプロファイルにプロキシグループがありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="469"/>
        <source>Type %1 · %2 members</source>
        <translation>種別 %1 · メンバー %2 件</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="472"/>
        <source>Current node: %1</source>
        <translation>現在のノード: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="501"/>
        <source>Proxy group · %1</source>
        <translation>プロキシグループ · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="502"/>
        <source>Protocol · %1</source>
        <translation>プロトコル · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="503"/>
        <source>Unknown</source>
        <translation>不明</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="505"/>
        <source>This protocol is not supported yet; connections will fail explicitly</source>
        <translation>このプロトコルはまだ未対応です。接続は明示的に失敗します</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="511"/>
        <source>Last latency test timed out</source>
        <translation>前回の遅延テストはタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="516"/>
        <source>Last latency %1 ms</source>
        <translation>前回の遅延 %1 ms</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="517"/>
        <source>Last test timed out</source>
        <translation>前回のテストはタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="522"/>
        <source>Currently pointing at %1</source>
        <translation>現在 %1 を参照中</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="524"/>
        <source>Click the text on the right of a card to test that node</source>
        <translation>カード右側のテキストをクリックすると、そのノードをテストします</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="741"/>
        <source>Node no longer exists</source>
        <translation>ノードは既に存在しません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="772"/>
        <source>Latency test finished: %1/%2 nodes available</source>
        <translation>遅延テスト完了: %1/%2 件のノードが利用可能</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="776"/>
        <source>%1 latency test failed: %2</source>
        <translation>%1 の遅延テストに失敗しました: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="792"/>
        <source>These nodes are being tested, or none can be tested</source>
        <translation>これらのノードはテスト中か、テスト可能なノードがありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="805"/>
        <source>Testing %1 nodes (%2 at a time, results fill in as they finish)</source>
        <translation>%1 件のノードをテスト中（同時 %2 件、完了したものから順に表示）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="835"/>
        <source>Failed to switch node: %1</source>
        <translation>ノードを切り替えられませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="843"/>
        <source>%1 switched to %2</source>
        <translation>%1 を %2 に切り替えました</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="170"/>
        <source>New rule</source>
        <translation>新規ルール</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="171"/>
        <source>Edit rule</source>
        <translation>ルールを編集</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="180"/>
        <source>Follow the prompts; the rule syntax is assembled when you save.</source>
        <translation>画面の案内に従ってください。ルール構文は保存時に組み立てられます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="198"/>
        <source>%1 (cannot be edited structurally)</source>
        <translation>%1（構造の編集はできません）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="201"/>
        <source>Rule type</source>
        <translation>ルール種別</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="217"/>
        <source>Content</source>
        <translation>内容</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="223"/>
        <source>Choose a policy or proxy group</source>
        <translation>ポリシーまたはプロキシグループを選択</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="224"/>
        <source>Target</source>
        <translation>対象</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="227"/>
        <source>Default</source>
        <translation>既定</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="228"/>
        <source>Do not resolve domain (no-resolve)</source>
        <translation>ドメインを解決しない（no-resolve）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="229"/>
        <source>Match source address (src)</source>
        <translation>送信元アドレスで一致（src）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="230"/>
        <source>Options</source>
        <translation>オプション</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="253"/>
        <source>Cancel</source>
        <translation>キャンセル</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="254"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="302"/>
        <source>No rule-providers in the config</source>
        <translation>設定に rule-providers がありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="303"/>
        <source>Choose a rule set</source>
        <translation>ルールセットを選択</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="333"/>
        <source>A catch-all rule matches all remaining traffic; keep it last among your custom rules.</source>
        <translation>全捕捉ルールは残りの通信すべてに一致します。カスタムルールの最後に置いてください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="335"/>
        <source>example.com also matches all of its subdomains.</source>
        <translation>example.com はすべてのサブドメインにも一致します。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="337"/>
        <source>For LAN traffic, set Options to no-resolve.</source>
        <translation>LAN 内の通信では、オプションを no-resolve に設定してください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="339"/>
        <source>Rule sets come from rule-providers in the profile.</source>
        <translation>ルールセットはプロファイル内の rule-providers から取得されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="362"/>
        <source>Choose what handles this rule (policy / proxy group).</source>
        <translation>このルールを処理する対象（ポリシー / プロキシグループ）を選択します。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="367"/>
        <source>Enter the content to match; it is what the rule checks.</source>
        <translation>一致させる内容を入力します。ルールが照合する値になります。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="372"/>
        <source>IPv4 CIDR needs a prefix length, e.g. 10.0.0.0/8.</source>
        <translation>IPv4 CIDR にはプレフィックス長が必要です（例: 10.0.0.0/8）。</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleRowDelegate</name>
    <message>
        <location filename="../src/app/src/ui/RuleList.cpp" line="156"/>
        <source>%1 hits</source>
        <translation>%1 件一致</translation>
    </message>
</context>
<context>
    <name>clashqt::RulesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="43"/>
        <source>Rules</source>
        <translation>ルール</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="44"/>
        <source>Rules in match order, one card each. Turn the switch on the right off to disable a rule for this session; it is restored on restart.</source>
        <translation>一致順にルールを 1 件ずつカードで表示します。右側のスイッチをオフにすると、このセッション中そのルールが無効になります（再起動時に元に戻ります）。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="64"/>
        <source>No rules loaded</source>
        <translation>ルールが読み込まれていません</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="90"/>
        <source>Failed to update rule state</source>
        <translation>ルールの状態を更新できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="132"/>
        <source>%1 hits in this session</source>
        <translation>このセッションで %1 件一致</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="133"/>
        <source>Disabled until restart</source>
        <translation>再起動まで無効</translation>
    </message>
</context>
<context>
    <name>clashqt::SettingsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="42"/>
        <source>Settings</source>
        <translation>設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="43"/>
        <source>Logs, DNS overrides, ports and system integration. Changes are saved and applied immediately.</source>
        <translation>ログ、DNS 上書き、ポート、システム連携の設定です。変更は保存後すぐに適用されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="46"/>
        <source>Appearance</source>
        <translation>外観</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="50"/>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="63"/>
        <source>Follow system</source>
        <translation>システムに従う</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="51"/>
        <source>Light</source>
        <translation>ライト</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="52"/>
        <source>Dark</source>
        <translation>ダーク</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="58"/>
        <source>Theme</source>
        <translation>テーマ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="68"/>
        <source>Language</source>
        <translation>言語</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="69"/>
        <source>Interface text is loaded at startup; restart the app after switching.</source>
        <translation>インターフェースのテキストは起動時に読み込まれます。切り替え後はアプリを再起動してください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="74"/>
        <source>Logs</source>
        <translation>ログ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="81"/>
        <source>Log level</source>
        <translation>ログレベル</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="86"/>
        <source> days</source>
        <translation> 日</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="89"/>
        <source>Log retention (days)</source>
        <translation>ログ保存期間（日）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="90"/>
        <source>Log files older than the retention period are removed at startup.</source>
        <translation>保存期間を超えたログファイルは起動時に削除されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="93"/>
        <source>Open data folder</source>
        <translation>データフォルダを開く</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="102"/>
        <source>Config overrides</source>
        <translation>設定の上書き</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="105"/>
        <source>Manage DNS settings in this app</source>
        <translation>このアプリで DNS 設定を管理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="108"/>
        <source>When on, the DNS section configured here is used; when off, the profile&apos;s own DNS settings apply.</source>
        <translation>オンの場合、ここで設定した DNS セクションが使われます。オフの場合はプロファイル自身の DNS 設定が適用されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="111"/>
        <source>Manage domain sniffing in this app</source>
        <translation>このアプリでドメインスニッフィングを管理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="117"/>
        <source>Inbound ports</source>
        <translation>インバウンドポート</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="131"/>
        <source>Mixed port (HTTP + SOCKS5)</source>
        <translation>混合ポート（HTTP + SOCKS5）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="133"/>
        <source>SOCKS5 port</source>
        <translation>SOCKS5 ポート</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="134"/>
        <source>Set it equal to the mixed port to reuse the mixed port.</source>
        <translation>混合ポートと同じ値にすると、混合ポートを併用できます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="136"/>
        <source>HTTP port</source>
        <translation>HTTP ポート</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="141"/>
        <source>System integration</source>
        <translation>システム連携</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="144"/>
        <source>Start at login</source>
        <translation>ログイン時に起動</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="146"/>
        <source>Implemented via an XDG autostart entry (~/.config/autostart/clash-qt.desktop).</source>
        <translation>XDG 自動起動エントリ（~/.config/autostart/clash-qt.desktop）で実装しています。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="149"/>
        <source>System proxy</source>
        <translation>システムプロキシ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="152"/>
        <source>Points the system HTTP/HTTPS/SOCKS proxy at this app&apos;s mixed port. Current backend: %1</source>
        <translation>システムの HTTP/HTTPS/SOCKS プロキシをこのアプリの混合ポートに向けます。現在のバックエンド: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="156"/>
        <source>TUN mode</source>
        <translation>TUN モード</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="159"/>
        <source>Creates a virtual network interface to capture all traffic, including apps that ignore the system proxy (games, CLI tools, some Electron apps). The first enable asks for your password once; it only takes effect together with &quot;Route domain traffic&quot;.</source>
        <translation>システムプロキシを無視するアプリ（ゲーム、CLI ツール、一部の Electron アプリ）も含め、すべての通信を捕捉する仮想ネットワークインターフェースを作成します。初回の有効化時に一度だけパスワードを求めます。「ドメイン通信をルーティング」と併用したときのみ有効になります。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="162"/>
        <source>Route domain traffic (fake-IP)</source>
        <translation>ドメイン通信をルーティング（fake-IP）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="165"/>
        <source>Routes the fake-IP range to the TUN interface (removed together with the interface). The default route is untouched, so connectivity is unaffected; with this off, TUN receives no traffic.</source>
        <translation>fake-IP の範囲を TUN インターフェースへルーティングします（インターフェース削除時に一緒に削除されます）。既定ルートは変更しないため通信には影響しませんが、オフにすると TUN には通信が流れません。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="171"/>
        <source>About</source>
        <translation>情報</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="177"/>
        <source>Check for updates when the app starts</source>
        <translation>アプリ起動時にアップデートを確認</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="187"/>
        <source>Version v%1</source>
        <translation>バージョン v%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="192"/>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="481"/>
        <source>Check for updates</source>
        <translation>アップデートを確認</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="198"/>
        <source>Reads the update manifest attached to the latest release, fetched through public GitHub mirrors. When an update exists, its changelog is shown and the release page can be opened in your browser; downloading and installing is up to you.</source>
        <translation>最新リリースに添付されたアップデートマニフェストを、公開 GitHub ミラー経由で取得して確認します。アップデートがある場合は変更履歴を表示し、ブラウザでリリースページを開けます。ダウンロードとインストールはご自身で行ってください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="252"/>
        <source>Already up to date (v%1)</source>
        <translation>最新版です（v%1）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="260"/>
        <source> (the kernel is not running; start the proxy and try again)</source>
        <translation>（カーネルが実行されていません。プロキシを起動してから再試行してください）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="262"/>
        <source>Update check failed: %1%2</source>
        <translation>アップデートの確認に失敗しました: %1%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="300"/>
        <source>Data folder: %1</source>
        <translation>データフォルダ: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="359"/>
        <source>Reload failed after port change: %1</source>
        <translation>ポート変更後の再読み込みに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="372"/>
        <source>Failed to change startup setting: %1</source>
        <translation>起動設定を変更できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="456"/>
        <source>Language saved; restart the app to apply.</source>
        <translation>言語を保存しました。適用するにはアプリを再起動してください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="481"/>
        <source>Checking…</source>
        <translation>確認中…</translation>
    </message>
</context>
<context>
    <name>clashqt::SubscriptionDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Add profile</source>
        <translation>プロファイルを追加</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Edit profile</source>
        <translation>プロファイルを編集</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="84"/>
        <source>Enter the subscription URL; it will be downloaded and activated immediately.</source>
        <translation>サブスクリプション URL を入力してください。すぐにダウンロードして有効化します。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="86"/>
        <source>Change the name or subscription URL. If this is the active profile, a changed URL is fetched again immediately.</source>
        <translation>名前またはサブスクリプション URL を変更します。これが有効なプロファイルの場合、URL を変更するとすぐに再取得します。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="87"/>
        <source>A local profile uses an imported config file, which you can replace below.</source>
        <translation>ローカルプロファイルはインポートした設定ファイルを使用します。以下から置き換えられます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="90"/>
        <source>Profile info</source>
        <translation>プロファイル情報</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="97"/>
        <source>Optional; defaults to the subscription&apos;s domain</source>
        <translation>任意。既定ではサブスクリプションのドメインを使用</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="99"/>
        <source>Name</source>
        <translation>名前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="108"/>
        <source>Subscription URL</source>
        <translation>サブスクリプション URL</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="110"/>
        <source>Update this profile on startup</source>
        <translation>起動時にこのプロファイルを更新</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="115"/>
        <source>When enabled, the profile is refreshed automatically at startup.</source>
        <translation>有効にすると、起動時にプロファイルが自動で更新されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="127"/>
        <source>Choose another file</source>
        <translation>別のファイルを選択</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="132"/>
        <source>Config file</source>
        <translation>設定ファイル</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="133"/>
        <source>Picking a new file replaces this profile&apos;s content; its id and active state are preserved.</source>
        <translation>新しいファイルを選ぶとこのプロファイルの内容が置き換わります。ID と有効状態は保持されます。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="138"/>
        <source>Choose a config file</source>
        <translation>設定ファイルを選択</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="139"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Clash 設定 (*.yaml *.yml);;すべてのファイル (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="153"/>
        <source>Cancel</source>
        <translation>キャンセル</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Add</source>
        <translation>追加</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="193"/>
        <source>Name required</source>
        <translation>名前が必要です</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="194"/>
        <source>Give this profile a name.</source>
        <translation>このプロファイルに名前を付けてください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="203"/>
        <source>Invalid subscription URL</source>
        <translation>無効なサブスクリプション URL</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="205"/>
        <source>Enter a subscription URL starting with http/https.</source>
        <translation>http/https で始まるサブスクリプション URL を入力してください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="206"/>
        <source>The subscription URL must start with http/https.</source>
        <translation>サブスクリプション URL は http/https で始まる必要があります。</translation>
    </message>
</context>
<context>
    <name>clashqt::TrafficPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="63"/>
        <source>Traffic</source>
        <translation>通信量</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="64"/>
        <source>Live upload/download speed and cumulative traffic; the chart shows 1-second samples from the last 2 minutes.</source>
        <translation>リアルタイムのアップロード／ダウンロード速度と累計通信量です。グラフは直近 2 分間を 1 秒ごとにサンプリングして表示します。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="67"/>
        <source>Status</source>
        <translation>状態</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="72"/>
        <source>Total 0 B / 0 B</source>
        <translation>合計 0 B / 0 B</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="73"/>
        <source>0 active connections</source>
        <translation>アクティブな接続 0 件</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="82"/>
        <source>Speed chart</source>
        <translation>速度グラフ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="96"/>
        <source>s</source>
        <translation>秒</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="188"/>
        <source>Session total ↑ %1 · ↓ %2</source>
        <translation>セッション合計 ↑ %1 · ↓ %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="193"/>
        <source>%1 active connection(s) · Memory %2</source>
        <translation>アクティブな接続 %1 件 · メモリ %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="242"/>
        <source>Upload (%1)</source>
        <translation>アップロード（%1）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="243"/>
        <source>Download (%1)</source>
        <translation>ダウンロード（%1）</translation>
    </message>
</context>
<context>
    <name>clashqt::UpdateDialog</name>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="20"/>
        <source>Update available</source>
        <translation>アップデートがあります</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="30"/>
        <source>Version %1 is available</source>
        <translation>バージョン %1 が利用可能です</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="31"/>
        <source>Version v%1. The changelog comes from the GitHub release; downloading and installing is up to you — this app never replaces itself.</source>
        <translation>バージョン v%1 です。変更履歴は GitHub のリリースから取得しています。ダウンロードとインストールはご自身で行ってください。このアプリが自動で置き換わることはありません。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="51"/>
        <source>(No release notes for this version; see the release page.)</source>
        <translation>（このバージョンのリリースノートはありません。リリースページをご覧ください）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="97"/>
        <source>No package has been attached to this release yet; it may still be uploading.</source>
        <translation>このリリースにはまだパッケージが添付されていません。アップロード中の可能性があります。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="100"/>
        <source>Published on %1. Download the package from the release page.</source>
        <translation>%1 に公開されました。パッケージはリリースページからダウンロードしてください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="103"/>
        <source>Download the package from the release page.</source>
        <translation>パッケージはリリースページからダウンロードしてください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="112"/>
        <source>Later</source>
        <translation>後で</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="113"/>
        <source>Open download page</source>
        <translation>ダウンロードページを開く</translation>
    </message>
</context>
<context>
    <name>clashqt::app</name>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="130"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable.</source>
        <translation>wintun.dll が見つかりません。wintun.dll を実行ファイルと同じ場所に置いてください。</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="145"/>
        <source>Could not create the Wintun adapter</source>
        <translation>Wintun アダプタを作成できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="175"/>
        <source>Wintun is unavailable</source>
        <translation>Wintun は利用できません</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="182"/>
        <source>Could not start the Wintun session</source>
        <translation>Wintun セッションを開始できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="65"/>
        <source>Clash Qt - a desktop client for Clash-style subscriptions</source>
        <translation>Clash Qt - Clash 形式のサブスクリプション向けデスクトップクライアント</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="71"/>
        <source>Start without showing the main window (for autostart)</source>
        <translation>メインウィンドウを表示せずに起動（自動起動用）</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="73"/>
        <source>Use the data directory next to the executable</source>
        <translation>実行ファイルと同じ場所のデータディレクトリを使用</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="97"/>
        <source>Another instance is already running: asked it to show its window, exiting</source>
        <translation>別のインスタンスが既に実行中のため、ウィンドウ表示を要求して終了します</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="108"/>
        <source>Starting %1 %2 (%3, silent=%4)</source>
        <translation>%1 %2 を起動します（%3、サイレント=%4）</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="121"/>
        <source>Initialization failed: %1</source>
        <translation>初期化に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="128"/>
        <source>UI language: %1 (configured value %2)</source>
        <translation>UI 言語: %1（設定値 %2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="148"/>
        <source>Start failed: %1</source>
        <translation>起動に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="177"/>
        <source>Silent start: not showing the main window</source>
        <translation>サイレント起動: メインウィンドウを表示しません</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="188"/>
        <source>%1 exited with code %2</source>
        <translation>%1 がコード %2 で終了しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="72"/>
        <source>Could not read file %1: %2</source>
        <translation>ファイル %1 を読み込めませんでした: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="82"/>
        <source>Not a valid Clash config: %1</source>
        <translation>有効な Clash 設定ではありません: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/AppLogger.cpp" line="152"/>
        <source>Removed %1 expired log file(s) (retention %2 days)</source>
        <translation>期限切れのログファイルを %1 件削除しました（保存期間 %2 日）</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="18"/>
        <source>Could not create the directory: %1</source>
        <translation>ディレクトリを作成できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="26"/>
        <source>Could not write %1: %2</source>
        <translation>%1 を書き込めませんでした: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="32"/>
        <source>Incomplete write to %1: %2</source>
        <translation>%1 への書き込みが完了しませんでした: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="38"/>
        <source>Failed to commit %1: %2</source>
        <translation>%1 の書き込みを確定できませんでした: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="70"/>
        <source>Used up</source>
        <translation>使用済み</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="77"/>
        <source>%1 d %2 h</source>
        <translation>%1 日 %2 時間</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="78"/>
        <source>%1 h %2 min</source>
        <translation>%1 時間 %2 分</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="79"/>
        <source>%1 min</source>
        <translation>%1 分</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="208"/>
        <source>Failed to parse the app config; using defaults: %1</source>
        <translation>アプリ設定を解析できなかったため、既定値を使用します: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="228"/>
        <source>Failed to write back the app config: %1</source>
        <translation>アプリ設定を書き戻せませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="238"/>
        <source>Failed to save the app config: %1</source>
        <translation>アプリ設定を保存できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="226"/>
        <source>Failed to parse the legacy managed config (%1): %2</source>
        <translation>旧形式の管理設定を解析できませんでした（%1）: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="232"/>
        <source>Merged the legacy managed config into config.yaml and deleted it: %1</source>
        <translation>旧形式の管理設定を config.yaml に統合し、元のファイルを削除しました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="134"/>
        <source>Failed to parse the profile list: %1</source>
        <translation>プロファイル一覧を解析できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="157"/>
        <source>Failed to save the profile list: %1</source>
        <translation>プロファイル一覧を保存できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="242"/>
        <source>Failed to write profile content (%1): %2</source>
        <translation>プロファイルの内容を書き込めませんでした（%1）: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="74"/>
        <source>Failed to parse the override list: %1</source>
        <translation>上書き一覧を解析できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="92"/>
        <source>Failed to save the override list: %1</source>
        <translation>上書き一覧を保存できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="156"/>
        <source>Failed to write override content (%1.%2): %3</source>
        <translation>上書きの内容を書き込めませんでした（%1.%2）: %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="98"/>
        <source>References an override that does not exist: %1</source>
        <translation>存在しない上書きを参照しています: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="107"/>
        <source>Override %1 has no content file</source>
        <translation>上書き %1 に内容ファイルがありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="116"/>
        <source>Override &quot;%1&quot; is a JS script, and the JS override engine is not supported yet; it was skipped (the config will not apply it)</source>
        <translation>上書き「%1」は JS スクリプトですが、JS 上書きエンジンはまだ未対応のため、スキップしました（設定には適用されません）</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="126"/>
        <source>Failed to parse override &quot;%1&quot;: %2</source>
        <translation>上書き「%1」を解析できませんでした: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="133"/>
        <source>The root of override &quot;%1&quot; must be a mapping</source>
        <translation>上書き「%1」のルートはマッピングである必要があります</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="159"/>
        <source>Failed to parse the rule-override file: %1</source>
        <translation>ルール上書きファイルを解析できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="228"/>
        <source>No profile selected yet. Add one on the Profiles page and select it first.</source>
        <translation>プロファイルがまだ選択されていません。プロファイルページで追加し、先に選択してください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="239"/>
        <source>Profile &quot;%1&quot; is empty or missing</source>
        <translation>プロファイル「%1」が空か存在しません</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="249"/>
        <source>Failed to parse the profile content: %1</source>
        <translation>プロファイルの内容を解析できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="356"/>
        <source>Failed to write the config: %1</source>
        <translation>設定を書き込めませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="361"/>
        <source>Generated the runtime config: %1 (profile %2)</source>
        <translation>実行時設定を生成しました: %1（プロファイル %2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="279"/>
        <source>Request timed out</source>
        <translation>リクエストがタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="281"/>
        <source>The release has no update manifest</source>
        <translation>リリースにアップデートマニフェストがありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="341"/>
        <source>No update manifest address available</source>
        <translation>アップデートマニフェストのアドレスがありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="354"/>
        <source>Could not parse the manifest: %1</source>
        <translation>マニフェストを解析できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="363"/>
        <source>The manifest has no usable version field</source>
        <translation>マニフェストに使用可能なバージョンフィールドがありません</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="234"/>
        <source>System-proxy backend: %1</source>
        <translation>システムプロキシのバックエンド: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="258"/>
        <source>System-proxy backend: %1 (XDG_CURRENT_DESKTOP=%2)</source>
        <translation>システムプロキシのバックエンド: %1（XDG_CURRENT_DESKTOP=%2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="273"/>
        <source>unavailable</source>
        <translation>利用不可</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="327"/>
        <source>Could not write ProxyServer to the registry</source>
        <translation>レジストリに ProxyServer を書き込めませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="335"/>
        <source>Failed to write the system-proxy ignore list</source>
        <translation>システムプロキシの除外リストを書き込めませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="339"/>
        <source>Could not enable the system proxy in the registry</source>
        <translation>レジストリでシステムプロキシを有効にできませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="350"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="378"/>
        <source>System proxy enabled (%1): %2:%3</source>
        <translation>システムプロキシを有効にしました（%1）: %2:%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="356"/>
        <source>No usable system-proxy backend detected (gsettings or kwriteconfig5/6 is required)</source>
        <translation>使用可能なシステムプロキシのバックエンドが見つかりません（gsettings または kwriteconfig5/6 が必要です）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="372"/>
        <source>Failed to enable the system proxy: %1</source>
        <translation>システムプロキシを有効にできませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="387"/>
        <source>Could not turn off the system proxy in the registry</source>
        <translation>レジストリでシステムプロキシを無効にできませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="398"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="423"/>
        <source>System proxy disabled</source>
        <translation>システムプロキシ無効</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="402"/>
        <source>No usable system-proxy backend detected</source>
        <translation>使用可能なシステムプロキシのバックエンドが見つかりません</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="418"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>システムプロキシを無効にできませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="438"/>
        <source>Could not start %1</source>
        <translation>%1 を起動できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="443"/>
        <source>%1 timed out</source>
        <translation>%1 がタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="448"/>
        <source>%1 failed: %2</source>
        <translation>%1 に失敗しました: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="515"/>
        <source>Failed to write the system-proxy ignore list: %1</source>
        <translation>システムプロキシの除外リストを書き込めませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="577"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="620"/>
        <source>kwriteconfig5/6 not found</source>
        <translation>kwriteconfig5/6 が見つかりません</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="53"/>
        <source>Could not open the autostart registry key</source>
        <translation>自動起動のレジストリキーを開けませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="68"/>
        <source>Could not write the autostart entry (error %1)</source>
        <translation>自動起動のエントリを書き込めませんでした（エラー %1）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="109"/>
        <source>Could not create the autostart directory</source>
        <translation>自動起動ディレクトリを作成できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="161"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="193"/>
        <source>Failed to enable autostart: %1</source>
        <translation>自動起動を有効にできませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="167"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="196"/>
        <source>Autostart enabled: %1</source>
        <translation>自動起動を有効にしました: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="215"/>
        <source>Could not remove the autostart entry (error %1)</source>
        <translation>自動起動のエントリを削除できませんでした（エラー %1）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="224"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="235"/>
        <source>Autostart disabled</source>
        <translation>自動起動を無効にしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="231"/>
        <source>Could not remove the autostart entry: %1</source>
        <translation>自動起動のエントリを削除できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SingleInstance.cpp" line="53"/>
        <source>The single-instance guard failed to start (%1); running as multiple instances this time: %2</source>
        <translation>単一インスタンスのガードを起動できませんでした（%1）。今回は複数インスタンスで実行します: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="117"/>
        <source>Could not set the TUN adapter address</source>
        <translation>TUN アダプタのアドレスを設定できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="134"/>
        <source>Could not read the TUN adapter settings</source>
        <translation>TUN アダプタの設定を読み取れませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="150"/>
        <source>Could not set the TUN adapter MTU/metric</source>
        <translation>TUN アダプタの MTU/メトリックを設定できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="175"/>
        <source>Could not add the TUN route</source>
        <translation>TUN ルートを追加できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="203"/>
        <source>Could not start netsh</source>
        <translation>netsh を起動できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="208"/>
        <source>netsh timed out</source>
        <translation>netsh がタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="312"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="383"/>
        <source>TUN mode on Windows requires administrator privileges. Close the app and start it again with &quot;Run as administrator&quot;.</source>
        <translation>Windows の TUN モードには管理者権限が必要です。アプリを閉じて「管理者として実行」で起動し直してください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="321"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="513"/>
        <source>Helper not found: %1</source>
        <translation>ヘルパーが見つかりません: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="337"/>
        <source>Could not start pkexec: %1</source>
        <translation>pkexec を起動できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="344"/>
        <source>Authorization timed out (the password prompt may have been cancelled)</source>
        <translation>認証がタイムアウトしました（パスワード入力がキャンセルされた可能性があります）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="349"/>
        <source>%1 failed (exit code %2): %3</source>
        <translation>%1 に失敗しました（終了コード %2）: %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="359"/>
        <source>TUN permissions granted (chown root:root + chmod +sx): %1</source>
        <translation>TUN の権限を付与しました（chown root:root + chmod +sx）: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="369"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="509"/>
        <source>TUN is starting or already running</source>
        <translation>TUN は起動中か、すでに実行中です</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="389"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable and try again.</source>
        <translation>wintun.dll が見つかりません。wintun.dll を実行ファイルと同じ場所に置いて再試行してください。</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="402"/>
        <source>Invalid TUN address: %1</source>
        <translation>無効な TUN アドレス: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="410"/>
        <source>Could not create the TUN adapter</source>
        <translation>TUN アダプタを作成できませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="415"/>
        <source>Could not read the TUN adapter LUID</source>
        <translation>TUN アダプタの LUID を読み取れませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="448"/>
        <source>Invalid fake-ip CIDR: %1</source>
        <translation>無効な fake-ip CIDR: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="468"/>
        <source>Could not point system DNS at the TUN address: %1</source>
        <translation>システム DNS を TUN アドレスに向けられませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="481"/>
        <source>TUN adapter ready: %1 %2/%3, MTU %4, routes %5, Wintun %6</source>
        <translation>TUN アダプタの準備完了: %1 %2/%3、MTU %4、ルート %5、Wintun %6</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="487"/>
        <source>taken over</source>
        <translation>引き継ぎ済み</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="488"/>
        <source>not taken over</source>
        <translation>未引き継ぎ</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="505"/>
        <source>Found a stale TUN-enabled state; cleaned it up and retried</source>
        <translation>古い TUN 有効状態を検出したため、クリーンアップして再試行しました</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="517"/>
        <source>TUN permission not granted yet (enter the password in the prompt that appears)</source>
        <translation>TUN の権限がまだ付与されていません（表示されるダイアログでパスワードを入力してください）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="542"/>
        <source>Socket path too long</source>
        <translation>ソケットのパスが長すぎます</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="553"/>
        <source>Failed to listen on the socket: %1</source>
        <translation>ソケットで待ち受けできませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="583"/>
        <source>Could not start the helper: %1</source>
        <translation>ヘルパーを起動できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="595"/>
        <source>Timed out waiting for the helper (20 s)</source>
        <translation>ヘルパーの応答がタイムアウトしました（20 秒）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="611"/>
        <source>The helper exited unexpectedly (exit code %1)</source>
        <translation>ヘルパーが予期せず終了しました（終了コード %1）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="616"/>
        <source>Started the TUN helper; waiting for the device: %1</source>
        <translation>TUN ヘルパーを起動しました。デバイスを待っています: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="618"/>
        <source>TUN handshake details: socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</source>
        <translation>TUN ハンドシェイクの詳細: socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="700"/>
        <source>The TUN helper disconnected</source>
        <translation>TUN ヘルパーが切断されました</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="749"/>
        <source>The helper did not send a TUN fd</source>
        <translation>ヘルパーが TUN の fd を送信しませんでした</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="760"/>
        <source>TUN ready: %1 (%2)</source>
        <translation>TUN の準備完了: %1（%2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="762"/>
        <source>real device</source>
        <translation>実デバイス</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="815"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="868"/>
        <source>TUN is closed</source>
        <translation>TUN は閉じられています</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="778"/>
        <source>System tray unavailable; skipping tray setup</source>
        <translation>システムトレイが利用できないため、トレイの設定をスキップします</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="767"/>
        <source>UI style: Fusion (%1) style=%2 field=%3 accent=%4 desktop dark=%5</source>
        <translation>UI スタイル: Fusion（%1）style=%2 field=%3 accent=%4 desktop dark=%5</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>dark</source>
        <translation>ダーク</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>light</source>
        <translation>ライト</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>yes</source>
        <translation>はい</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>no</source>
        <translation>いいえ</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/Icons.cpp" line="53"/>
        <source>Icon resource missing: %1 (check resources/clash-qt.qrc)</source>
        <translation>アイコンのリソースが見つかりません: %1（resources/clash-qt.qrc を確認してください）</translation>
    </message>
</context>
<context>
    <name>clashqt::core</name>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="141"/>
        <source>YAML parse failed: %1</source>
        <translation>YAML の解析に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="165"/>
        <source>YAML emit failed: %1</source>
        <translation>YAML の出力に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="312"/>
        <source>Unknown error</source>
        <translation>不明なエラー</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="549"/>
        <source>Stream closed</source>
        <translation>ストリームが閉じられました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="566"/>
        <source>Write timed out (flow-control window stayed closed)</source>
        <translation>書き込みがタイムアウトしました（フロー制御ウィンドウが開きませんでした）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="570"/>
        <source>Session released</source>
        <translation>セッションを解放しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="627"/>
        <source>Session release</source>
        <translation>セッションの解放</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="656"/>
        <source>Could not parse server address %1 (%2)</source>
        <translation>サーバーアドレス %1 を解析できませんでした（%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="676"/>
        <source>Failed to create the UDP socket: %1</source>
        <translation>UDP ソケットを作成できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="689"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="700"/>
        <source>UDP connect failed: %1</source>
        <translation>UDP 接続に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="714"/>
        <source>Failed to create the QUIC context</source>
        <translation>QUIC コンテキストを作成できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="728"/>
        <source>Failed to create the QUIC connection</source>
        <translation>QUIC 接続を作成できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="741"/>
        <source>Failed to bind the UDP socket</source>
        <translation>UDP ソケットをバインドできませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="756"/>
        <source>Failed to read the peer address: %1</source>
        <translation>相手先アドレスを読み取れませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="776"/>
        <source>Failed to build the initial QUIC peer address</source>
        <translation>QUIC の初期相手先アドレスを構築できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="791"/>
        <source>QUIC handshake failed: %1</source>
        <translation>QUIC ハンドシェイクに失敗しました: %1</translation>
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
        <translation>QUIC イベント処理に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="810"/>
        <source>QUIC handshake timed out (port %1)</source>
        <translation>QUIC ハンドシェイクがタイムアウトしました（ポート %1）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="820"/>
        <source>The server provided no certificate</source>
        <translation>サーバーが証明書を提示しませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="831"/>
        <source>Server certificate fingerprint does not match the config (fingerprint check failed)</source>
        <translation>サーバー証明書のフィンガープリントが設定と一致しません（フィンガープリント検証に失敗）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="852"/>
        <source>Failed to create a QUIC stream: %1</source>
        <translation>QUIC ストリームを作成できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="856"/>
        <source>Timed out creating a QUIC stream</source>
        <translation>QUIC ストリームの作成がタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="876"/>
        <source>Failed to write to a QUIC stream: %1</source>
        <translation>QUIC ストリームへの書き込みに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="881"/>
        <source>Timed out writing to a QUIC stream</source>
        <translation>QUIC ストリームへの書き込みがタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="971"/>
        <source>No response to authentication (timeout or connection reset, port %1)</source>
        <translation>認証に応答がありません（タイムアウトまたは接続リセット、ポート %1）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="979"/>
        <source>Authentication rejected (HTTP status %1; check the password)</source>
        <translation>認証が拒否されました（HTTP ステータス %1。パスワードを確認してください）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1008"/>
        <source>Node is missing server</source>
        <translation>ノードにサーバーが指定されていません</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1012"/>
        <source>Node is missing password</source>
        <translation>ノードにパスワードが指定されていません</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1019"/>
        <source>Socket setup</source>
        <translation>ソケットの準備</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1020"/>
        <source>Handshake</source>
        <translation>ハンドシェイク</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1023"/>
        <source>Authentication</source>
        <translation>認証</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1034"/>
        <source>Session unavailable</source>
        <translation>セッションを利用できません</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1038"/>
        <source>Invalid target address</source>
        <translation>宛先アドレスが無効です</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1106"/>
        <source>Target %1 did not respond (timeout)</source>
        <translation>宛先 %1 が応答しません（タイムアウト）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1114"/>
        <source>Connection to target %1 refused</source>
        <translation>宛先 %1 への接続が拒否されました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1115"/>
        <source>Connection to target %1 refused: %2</source>
        <translation>宛先 %1 への接続が拒否されました: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="29"/>
        <source>Read more than %1 bytes without finding the delimiter</source>
        <translation>区切り文字が見つからないまま %1 バイト以上読み取りました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="42"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="87"/>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="247"/>
        <source>Connection closed</source>
        <translation>接続が閉じられました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="48"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="93"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="329"/>
        <source>Read timed out</source>
        <translation>読み取りがタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="62"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="104"/>
        <source>Read failed: %1</source>
        <translation>読み取りに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="126"/>
        <source>Write failed: %1</source>
        <translation>書き込みに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="133"/>
        <source>Write timed out</source>
        <translation>書き込みがタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="145"/>
        <source>Connection closed by the peer</source>
        <translation>接続が相手側から閉じられました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="461"/>
        <source>REALITY connection to %1:%2 failed: %3</source>
        <translation>REALITY で %1:%2 への接続に失敗しました: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="487"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="508"/>
        <source>X25519 key generation failed</source>
        <translation>X25519 鍵の生成に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="514"/>
        <source>REALITY static ECDH failed</source>
        <translation>REALITY の静的 ECDH に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="521"/>
        <source>Failed to build ClientHello</source>
        <translation>ClientHello の構築に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="552"/>
        <source>Failed to generate REALITY authentication data</source>
        <translation>REALITY の認証データを生成できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="571"/>
        <source>REALITY received no ServerHello (record type=%1 length=%2 content=%3)</source>
        <translation>REALITY が ServerHello を受信できませんでした（record type=%1 length=%2 content=%3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="582"/>
        <source>Truncated ServerHello</source>
        <translation>ServerHello が途中で切れています</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="618"/>
        <source>ServerHello contains no usable key_share</source>
        <translation>ServerHello に有効な key_share が含まれていません</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="624"/>
        <source>TLS negotiated: cipher=0x%1 key-exchange group=%2</source>
        <translation>TLS ネゴシエーション結果: cipher=0x%1 key-exchange group=%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="636"/>
        <source>The REALITY server negotiated an unsupported cipher 0x%1</source>
        <translation>REALITY サーバーが未対応の暗号 0x%1 を選択しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="704"/>
        <source>Unexpected record type %1 during the REALITY handshake</source>
        <translation>REALITY ハンドシェイク中に予期しないレコード種別 %1 を受信しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="713"/>
        <source>Failed to decrypt a REALITY handshake record (type=%1 length=%2 seq=%3)</source>
        <translation>REALITY ハンドシェイクのレコードを復号できませんでした（type=%1 length=%2 seq=%3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="717"/>
        <source>Failed to decrypt a REALITY handshake record</source>
        <translation>REALITY ハンドシェイクのレコードを復号できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="768"/>
        <source>The server returned no certificate</source>
        <translation>サーバーが証明書を返しませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="799"/>
        <source>TLS certificate verification failed (chain=%1 hostname=%2)</source>
        <translation>TLS 証明書の検証に失敗しました（chain=%1 hostname=%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="800"/>
        <source>Pass</source>
        <translation>合格</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="801"/>
        <source>Fail</source>
        <translation>不合格</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="802"/>
        <source>Match</source>
        <translation>一致</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="803"/>
        <source>No match</source>
        <translation>不一致</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="809"/>
        <source>Node %1 skips TLS certificate verification</source>
        <translation>ノード %1 は TLS 証明書の検証をスキップします</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="818"/>
        <source>REALITY authentication failed (the peer is the borrowed site itself rather than a REALITY server, or public-key / short-id is misconfigured)</source>
        <translation>REALITY 認証に失敗しました（相手が REALITY サーバーではなく借用先のサイトであるか、公開鍵／short-id の設定が誤っています）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="830"/>
        <source>Server Finished verification failed</source>
        <translation>Server Finished の検証に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="993"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1008"/>
        <source>REALITY send failed: %1</source>
        <translation>REALITY の送信に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1022"/>
        <source>Timed out reading a REALITY record</source>
        <translation>REALITY レコードの読み取りがタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1028"/>
        <source>REALITY failed to read a record: %1</source>
        <translation>REALITY レコードを読み取れませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1044"/>
        <source>Invalid REALITY record length %1</source>
        <translation>REALITY レコードの長さ %1 が無効です</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1079"/>
        <source>Invalid REALITY application-record length</source>
        <translation>REALITY のアプリケーションレコードの長さが無効です</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1099"/>
        <source>Failed to decrypt a REALITY record</source>
        <translation>REALITY レコードを復号できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1251"/>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="390"/>
        <source>Timed out reading the response</source>
        <translation>応答の読み取りがタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1256"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="324"/>
        <source>Connection closed while reading</source>
        <translation>読み取り中に接続が閉じられました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="73"/>
        <source>Failed to write the upgrade request: %1</source>
        <translation>アップグレードリクエストを書き込めませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="77"/>
        <source>WS upgrade request: sent %1 bytes to %2:%3 (%4 pending, state %5)</source>
        <translation>WS アップグレードリクエスト: %2:%3 へ %1 バイト送信（未送信 %4、状態 %5）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="87"/>
        <source>%1 (received %2 bytes: %3)</source>
        <translation>%1（%2 バイト受信: %3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="101"/>
        <source>WebSocket handshake rejected: %1
%2</source>
        <translation>WebSocket ハンドシェイクが拒否されました: %1
%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="116"/>
        <source>WebSocket handshake verification failed (Sec-WebSocket-Accept mismatch)</source>
        <translation>WebSocket ハンドシェイクの検証に失敗しました（Sec-WebSocket-Accept が一致しません）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="129"/>
        <source>The WebSocket handshake response carried extra data, which is not supported</source>
        <translation>WebSocket ハンドシェイクの応答に余分なデータが含まれていました（未対応です）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="227"/>
        <source>The peer sent a WebSocket close frame</source>
        <translation>相手側から WebSocket のクローズフレームが送信されました</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="236"/>
        <source>Fragmented WebSocket messages are not supported</source>
        <translation>分割された WebSocket メッセージには対応していません</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="262"/>
        <source>Timed out reading a WebSocket frame</source>
        <translation>WebSocket フレームの読み取りがタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/config/RawConfig.cpp" line="81"/>
        <source>The config root must be a mapping</source>
        <translation>設定のルートはマッピングである必要があります</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/cidr/IpCidrSet.cpp" line="12"/>
        <source>Ignoring an invalid CIDR rule entry: %1</source>
        <translation>無効な CIDR ルール項目を無視します: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="68"/>
        <source>DNS listener not started: %1</source>
        <translation>DNS リスナーが起動していません: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="77"/>
        <source>Not listening</source>
        <translation>待ち受けていません</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="86"/>
        <source>dns.listen is not configured</source>
        <translation>dns.listen が設定されていません</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="95"/>
        <source>Failed to bind UDP %1:%2: %3</source>
        <translation>UDP %1:%2 をバインドできませんでした: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="139"/>
        <source>DNS listener started: %1 (enhanced-mode=%2)</source>
        <translation>DNS リスナーを起動しました: %1（enhanced-mode=%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="199"/>
        <source>DNS fake-ip: AAAA query for %1 returned an empty answer</source>
        <translation>DNS fake-ip: %1 の AAAA クエリが空の応答を返しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="222"/>
        <source>DNS real resolution: %1 -&gt; %2 record(s)</source>
        <translation>DNS 実解決: %1 -&gt; %2 件のレコード</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="285"/>
        <source>Invalid or non-IPv4 fake-ip-range: %1; falling back to 198.18.0.1/16</source>
        <translation>fake-ip-range が無効か IPv4 ではありません: %1。198.18.0.1/16 にフォールバックします</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="545"/>
        <source>DNS configured: enhanced-mode=%1, nameserver=%2, fake-ip=%3</source>
        <translation>DNS を設定しました: enhanced-mode=%1、nameserver=%2、fake-ip=%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="915"/>
        <source>DoT (tls://) currently connects over plain TCP to %1</source>
        <translation>DoT（tls://）は現在 %1 へ平文 TCP で接続します</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="979"/>
        <source>Could not resolve the DNS upstream host through the bootstrap server: %1</source>
        <translation>bootstrap サーバー経由で DNS 上流ホストを解決できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1004"/>
        <source>Could not parse the DNS server config: %1</source>
        <translation>DNS サーバー設定を解析できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1045"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="487"/>
        <source>DNS resolution failed: %1</source>
        <translation>DNS 解決に失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="92"/>
        <source>Malformed logical-rule sub-item: %1</source>
        <translation>論理ルールのサブ項目の形式が不正です: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="103"/>
        <source>Failed to parse a logical-rule sub-item: %1 (%2)</source>
        <translation>論理ルールのサブ項目を解析できませんでした: %1（%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="119"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="125"/>
        <source>Empty rule</source>
        <translation>空のルール</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="133"/>
        <source>Unknown rule type: %1</source>
        <translation>不明なルール種別: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="141"/>
        <source>MATCH rule is missing the target policy</source>
        <translation>MATCH ルールに対象ポリシーがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="149"/>
        <source>Rule has too few fields (expected type,value,policy): %1</source>
        <translation>ルールのフィールドが不足しています（type,value,policy が必要です）: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="197"/>
        <source>Invalid domain regular expression: %1</source>
        <translation>無効なドメイン正規表現: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="213"/>
        <source>Invalid IP CIDR: %1</source>
        <translation>無効な IP CIDR: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="225"/>
        <source>Invalid source IP CIDR: %1</source>
        <translation>無効な送信元 IP CIDR: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="251"/>
        <source>Invalid port: %1</source>
        <translation>無効なポート: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="261"/>
        <source>Invalid source port: %1</source>
        <translation>無効な送信元ポート: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="271"/>
        <source>Invalid inbound port: %1</source>
        <translation>無効なインバウンドポート: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="288"/>
        <source>Invalid process-match expression: %1</source>
        <translation>無効なプロセス一致式: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="298"/>
        <source>Invalid UID: %1</source>
        <translation>無効な UID: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="310"/>
        <source>References an undefined rule set: %1</source>
        <translation>未定義のルールセットを参照しています: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="325"/>
        <source>Logical rule has no usable sub-rules: %1</source>
        <translation>論理ルールに使用可能なサブルールがありません: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="340"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="346"/>
        <source>Unsupported rule type: %1</source>
        <translation>未対応のルール種別: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="352"/>
        <source>Failed to build the rule: %1</source>
        <translation>ルールを構築できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="369"/>
        <source>Skipped a non-string rule entry</source>
        <translation>文字列でないルール項目をスキップしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="420"/>
        <source>Could not parse an entry in rule set %1: %2</source>
        <translation>ルールセット %1 の項目を解析できませんでした: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="93"/>
        <source>Failed to connect to %1:%2: %3 (error=%4)</source>
        <translation>%1:%2 への接続に失敗しました: %3（error=%4）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="177"/>
        <source>Invalid DIRECT target address</source>
        <translation>DIRECT の宛先アドレスが無効です</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="186"/>
        <source>DIRECT UDP requires a literal IP target</source>
        <translation>DIRECT の UDP では IP アドレスを直接指定する必要があります</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="191"/>
        <source>UDP bind failed: %1</source>
        <translation>UDP のバインドに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="224"/>
        <source>Connection rejected by a rule (REJECT)</source>
        <translation>ルールにより接続が拒否されました（REJECT）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="242"/>
        <source>HTTP proxy %1 claims https; connecting with plaintext CONNECT for now</source>
        <translation>HTTP プロキシ %1 が https を要求しています。当面は平文の CONNECT で接続します</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="250"/>
        <source>HTTP proxy %1 is missing server/port</source>
        <translation>HTTP プロキシ %1 にサーバー/ポートがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="261"/>
        <source>The HTTP proxy does not support UDP forwarding</source>
        <translation>HTTP プロキシは UDP 転送に対応していません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="286"/>
        <source>HTTP proxy %1 did not respond, or its response headers are too long</source>
        <translation>HTTP プロキシ %1 が応答しないか、応答ヘッダーが長すぎます</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="300"/>
        <source>HTTP proxy returned %1</source>
        <translation>HTTP プロキシが %1 を返しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="335"/>
        <source>SOCKS5 proxy %1 is missing server/port</source>
        <translation>SOCKS5 プロキシ %1 にサーバー/ポートがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="356"/>
        <source>No response to SOCKS5 negotiation</source>
        <translation>SOCKS5 ネゴシエーションに応答がありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="362"/>
        <source>SOCKS5 version mismatch</source>
        <translation>SOCKS5 のバージョンが一致しません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="368"/>
        <source>The SOCKS5 server rejected all authentication methods</source>
        <translation>SOCKS5 サーバーがすべての認証方式を拒否しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="377"/>
        <source>SOCKS5 username or password too long</source>
        <translation>SOCKS5 のユーザー名またはパスワードが長すぎます</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="391"/>
        <source>No response to SOCKS5 authentication</source>
        <translation>SOCKS5 認証に応答がありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="396"/>
        <source>SOCKS5 authentication failed</source>
        <translation>SOCKS5 認証に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="401"/>
        <source>The SOCKS5 server requires an unsupported auth method 0x%1</source>
        <translation>SOCKS5 サーバーが未対応の認証方式 0x%1 を要求しています</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="425"/>
        <source>SOCKS5 target domain name too long</source>
        <translation>SOCKS5 の宛先ドメイン名が長すぎます</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="442"/>
        <source>Could not resolve the SOCKS5 target address: %1</source>
        <translation>SOCKS5 の宛先アドレスを解決できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="455"/>
        <source>No response to SOCKS5 CONNECT</source>
        <translation>SOCKS5 CONNECT に応答がありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="464"/>
        <source>Succeeded</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="465"/>
        <source>General SOCKS server failure</source>
        <translation>SOCKS サーバーの一般的なエラー</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="466"/>
        <source>Connection not allowed by ruleset</source>
        <translation>ルールセットで許可されていない接続です</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="467"/>
        <source>Network unreachable</source>
        <translation>ネットワークに到達できません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="468"/>
        <source>Host unreachable</source>
        <translation>ホストに到達できません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="469"/>
        <source>Connection refused</source>
        <translation>接続が拒否されました</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="470"/>
        <source>TTL expired</source>
        <translation>TTL が期限切れです</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="471"/>
        <source>Command not supported</source>
        <translation>コマンドが未対応です</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="472"/>
        <source>Address type not supported</source>
        <translation>アドレス種別が未対応です</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="475"/>
        <source>Unknown error %1</source>
        <translation>不明なエラー %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="477"/>
        <source>SOCKS5 connection refused: %1 (%2)</source>
        <translation>SOCKS5 接続が拒否されました: %1（%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="491"/>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="503"/>
        <source>Truncated SOCKS5 response</source>
        <translation>SOCKS5 の応答が途中で切れています</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="498"/>
        <source>The SOCKS5 response contains an unknown address type %1</source>
        <translation>SOCKS5 の応答に不明なアドレス種別 %1 が含まれています</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="521"/>
        <source>SOCKS5 UDP ASSOCIATE forwarding is not implemented yet (Phase 2)</source>
        <translation>SOCKS5 の UDP ASSOCIATE 転送はまだ未実装です（Phase 2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="560"/>
        <source>Node %1 (%2) is not supported yet: %3</source>
        <translation>ノード %1（%2）はまだ未対応です: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="632"/>
        <source>Group %1 switched to %2</source>
        <translation>グループ %1 を %2 に切り替えました</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="645"/>
        <source>Proxy group %1 has no available members</source>
        <translation>プロキシグループ %1 に利用可能なメンバーがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="658"/>
        <source>Group %1 member %2 failed to connect: %3</source>
        <translation>グループ %1 のメンバー %2 が接続に失敗しました: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="67"/>
        <source>This protocol is not implemented yet</source>
        <translation>このプロトコルはまだ未実装です</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="82"/>
        <source>Skipped non-mapping entries in proxies</source>
        <translation>proxies 内のマッピングでない項目をスキップしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="91"/>
        <source>Unrecognized node: %1 (type=%2)</source>
        <translation>認識できないノード: %1（type=%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="92"/>
        <source>&lt;unnamed&gt;</source>
        <translation>&lt;名前なし&gt;</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="96"/>
        <source>Duplicate node name; ignoring the later definition: %1</source>
        <translation>ノード名が重複しています。後から定義されたものを無視します: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="115"/>
        <source>Unrecognized proxy group: %1 (type=%2)</source>
        <translation>認識できないプロキシグループ: %1（type=%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="154"/>
        <source>Proxy group %1 has no members</source>
        <translation>プロキシグループ %1 にメンバーがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="168"/>
        <source>Proxy group %1 has members that cannot be resolved (a missing node or a reference cycle); skipped. Members: %2</source>
        <translation>プロキシグループ %1 に解決できないメンバー（存在しないノードまたは循環参照）があるため、スキップしました。メンバー: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="180"/>
        <source>Adapters loaded: %1 nodes/groups, %2 unimplemented protocols, %3 warnings</source>
        <translation>アダプタを読み込みました: ノード/グループ %1 件、未実装プロトコル %2 件、警告 %3 件</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="197"/>
        <source>The config defines its own GLOBAL group; using it as-is</source>
        <translation>設定が独自の GLOBAL グループを定義しているため、そのまま使用します</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="231"/>
        <source>Built the built-in GLOBAL group with %1 member(s)</source>
        <translation>組み込みの GLOBAL グループを %1 件のメンバーで構築しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="234"/>
        <source>The profile has no usable node; the GLOBAL group only contains DIRECT</source>
        <translation>プロファイルに使用可能なノードがないため、GLOBAL グループには DIRECT のみが含まれます</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="52"/>
        <source>fingerprint is not a valid SHA-256 hex string</source>
        <translation>fingerprint が有効な SHA-256 の 16 進文字列ではありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="59"/>
        <source>obfs=%1 is set (Salamander obfuscation is not implemented yet)</source>
        <translation>obfs=%1 が設定されています（Salamander 難読化はまだ未実装です）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="63"/>
        <source>realm-opts is set (realm is not implemented yet)</source>
        <translation>realm-opts が設定されています（realm はまだ未実装です）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="67"/>
        <source>Missing password</source>
        <translation>パスワードがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="78"/>
        <source>Node %1 does not support UDP relay yet (Hysteria2 needs QUIC DATAGRAM, which OpenSSL 3.2 does not provide)</source>
        <translation>ノード %1 はまだ UDP リレーに対応していません（Hysteria2 には QUIC DATAGRAM が必要ですが、OpenSSL 3.2 では提供されません）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="84"/>
        <source>Node %1 is not supported yet: %2</source>
        <translation>ノード %1 はまだ未対応です: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="92"/>
        <source>Node %1 has an invalid target address</source>
        <translation>ノード %1 の宛先アドレスが無効です</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="133"/>
        <source>Node %1 failed to connect (attempt %2); retrying on another port: %3</source>
        <translation>ノード %1 の接続に失敗しました（試行 %2）。別のポートで再試行します: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="140"/>
        <source>Node %1 failed to connect: %2</source>
        <translation>ノード %1 の接続に失敗しました: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="149"/>
        <source>Node %1 failed to establish the tunnel: %2</source>
        <translation>ノード %1 のトンネル確立に失敗しました: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="158"/>
        <source>Node %1: the server has UDP disabled; UDP traffic will not work</source>
        <translation>ノード %1: サーバー側で UDP が無効なため、UDP 通信は利用できません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="61"/>
        <source>Target domain name too long</source>
        <translation>宛先ドメイン名が長すぎます</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="78"/>
        <source>Could not resolve the target address: %1</source>
        <translation>宛先アドレスを解決できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="121"/>
        <source>Node %1 is missing server/port</source>
        <translation>ノード %1 にサーバー/ポートがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="138"/>
        <source>Failed to connect to %1:%2: %3</source>
        <translation>%1:%2 への接続に失敗しました: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="157"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="442"/>
        <source>Node %1 is missing uuid</source>
        <translation>ノード %1 に uuid がありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="163"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="448"/>
        <source>Node %1 has an invalid uuid: %2</source>
        <translation>ノード %1 の uuid が無効です: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="176"/>
        <source>Node %1 uses an unsupported flow: %2</source>
        <translation>ノード %1 が未対応の flow を使用しています: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="181"/>
        <source>Node %1 uses vision but has no reality-opts configured; this combination is not implemented yet</source>
        <translation>ノード %1 は vision を使用していますが reality-opts が設定されていません。この組み合わせはまだ未実装です</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="196"/>
        <source>Node %1 has an invalid reality-opts.public-key (it should decode to 32 bytes)</source>
        <translation>ノード %1 の reality-opts.public-key が無効です（32 バイトにデコードされる必要があります）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="200"/>
        <source>Node %1 has a reality-opts.short-id that is too long</source>
        <translation>ノード %1 の reality-opts.short-id が長すぎます</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="208"/>
        <source>Node %1 REALITY handshake failed: %2</source>
        <translation>ノード %1 の REALITY ハンドシェイクに失敗しました: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="236"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="293"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="322"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="541"/>
        <source>Node %1 failed to send the VLESS request</source>
        <translation>ノード %1 で VLESS リクエストを送信できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="252"/>
        <source>Node %1 transport %2 is not implemented yet (ws, REALITY and plain TCP are supported)</source>
        <translation>ノード %1 のトランスポート %2 はまだ未実装です（ws、REALITY、平文 TCP に対応しています）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="280"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="317"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="497"/>
        <source>Node %1 TLS connection failed: %2</source>
        <translation>ノード %1 の TLS 接続に失敗しました: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="288"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="340"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="507"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="525"/>
        <source>Node %1 WebSocket handshake failed: %2</source>
        <translation>ノード %1 の WebSocket ハンドシェイクに失敗しました: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="350"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="355"/>
        <source>Failed to send the VLESS request: %1</source>
        <translation>VLESS リクエストを送信できませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="373"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="397"/>
        <source>Failed to read the VLESS response: %1</source>
        <translation>VLESS の応答を読み取れませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="377"/>
        <source>Truncated VLESS response</source>
        <translation>VLESS の応答が途中で切れています</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="382"/>
        <source>Truncated VLESS response addons</source>
        <translation>VLESS 応答のアドオンが途中で切れています</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="404"/>
        <source>Failed to read the VLESS response addons: %1</source>
        <translation>VLESS 応答のアドオンを読み取れませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="454"/>
        <source>VLESS UDP is missing the target address</source>
        <translation>VLESS の UDP に宛先アドレスがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="488"/>
        <source>Node %1 has an invalid reality-opts.public-key</source>
        <translation>ノード %1 の reality-opts.public-key が無効です</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="515"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="533"/>
        <source>Node %1 transport %2 does not support UDP</source>
        <translation>ノード %1 のトランスポート %2 は UDP に対応していません</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="54"/>
        <source>%1 has an invalid listen address: %2</source>
        <translation>%1 の待ち受けアドレスが無効です: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="62"/>
        <source>%1 failed to listen on %2:%3: %4</source>
        <translation>%1 が %2:%3 で待ち受けできませんでした: %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="76"/>
        <source>%1 inbound started: %2:%3 (concurrency limit %4)</source>
        <translation>%1 インバウンドを起動しました: %2:%3（同時接続上限 %4）</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="126"/>
        <source>%1 still has %2 open connection(s)</source>
        <translation>%1 にまだ %2 件の接続が開いています</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="154"/>
        <source>Concurrency limit %1 reached; rejecting new connections</source>
        <translation>同時接続数の上限 %1 に達したため、新しい接続を拒否します</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="182"/>
        <source>Failed to duplicate the connection descriptor; connection dropped</source>
        <translation>接続ディスクリプタの複製に失敗したため、接続を切断しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="216"/>
        <source>%1 accepted connection %2:%3</source>
        <translation>%1 が接続 %2:%3 を受け付けました</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="231"/>
        <source>Could not rebuild a socket from descriptor %1</source>
        <translation>ディスクリプタ %1 からソケットを再構築できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="61"/>
        <source>Missing HTTP request line</source>
        <translation>HTTP リクエスト行がありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="68"/>
        <source>Malformed HTTP request line: %1</source>
        <translation>HTTP リクエスト行の形式が不正です: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="135"/>
        <source>HTTP inbound read failed: %1</source>
        <translation>HTTP インバウンドの読み取りに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="58"/>
        <source>Unsupported SOCKS version: %1</source>
        <translation>未対応の SOCKS バージョン: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="75"/>
        <source>The client does not support username/password authentication</source>
        <translation>クライアントがユーザー名/パスワード認証に対応していません</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="81"/>
        <source>The client offered no acceptable authentication method</source>
        <translation>クライアントが受け入れ可能な認証方式を提示しませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="89"/>
        <source>Failed to write the SOCKS5 method selection</source>
        <translation>SOCKS5 の認証方式選択を書き込めませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="100"/>
        <source>Unsupported username/password auth version</source>
        <translation>未対応のユーザー名/パスワード認証バージョンです</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="125"/>
        <source>SOCKS5 authentication failed (user %1)</source>
        <translation>SOCKS5 認証に失敗しました（ユーザー %1）</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="142"/>
        <source>Wrong SOCKS5 request version</source>
        <translation>SOCKS5 リクエストのバージョンが正しくありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="152"/>
        <source>Unsupported SOCKS5 command 0x%1</source>
        <translation>未対応の SOCKS5 コマンド 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="180"/>
        <source>Unsupported SOCKS5 address type 0x%1</source>
        <translation>未対応の SOCKS5 アドレス種別 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="219"/>
        <source>SOCKS5 negotiation failed: %1</source>
        <translation>SOCKS5 ネゴシエーションに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>SOCKS5 request failed: %1</source>
        <translation>SOCKS5 リクエストに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>Invalid target</source>
        <translation>宛先が無効です</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/MixedListener.cpp" line="37"/>
        <source>MIXED inbound timed out while sniffing the protocol</source>
        <translation>MIXED インバウンドでプロトコル判定がタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="26"/>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="24"/>
        <source>No dialer implementation available</source>
        <translation>利用可能なダイヤラ実装がありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="32"/>
        <source>Dial failed</source>
        <translation>ダイヤルに失敗しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="71"/>
        <source>UDP socket closed</source>
        <translation>UDP ソケットが閉じられました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="160"/>
        <source>UDP tunnel closed</source>
        <translation>UDP トンネルが閉じられました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="30"/>
        <source>UDP dial failed</source>
        <translation>UDP のダイヤルに失敗しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="97"/>
        <source>Could not resolve the TUN adapter from its LUID</source>
        <translation>LUID から TUN アダプタを特定できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="116"/>
        <source>Could not get the Wintun read event</source>
        <translation>Wintun の読み取りイベントを取得できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="161"/>
        <source>The Wintun session was closed</source>
        <translation>Wintun セッションが閉じられました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="199"/>
        <source>DNS is not taken over on the TUN address (%1): %2</source>
        <translation>TUN アドレス（%1）で DNS を引き継いでいません: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="282"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="427"/>
        <source>Invalid TUN adapter</source>
        <translation>無効な TUN アダプタです</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="287"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="432"/>
        <source>Invalid TUN device fd</source>
        <translation>無効な TUN デバイス fd です</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="306"/>
        <source>Failed to send a packet to the TUN adapter</source>
        <translation>TUN アダプタへのパケット送信に失敗しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="314"/>
        <source>Failed to write to the TUN device: %1</source>
        <translation>TUN デバイスへの書き込みに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="342"/>
        <source>Could not open the TUN adapter</source>
        <translation>TUN アダプタを開けませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="349"/>
        <source>TUN data plane ready (worker thread %1, Wintun %2)</source>
        <translation>TUN データプレーンの準備完了（ワーカースレッド %1、Wintun %2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="382"/>
        <source>TUN data plane ready (worker thread %1, DNS 53 %2)</source>
        <translation>TUN データプレーンの準備完了（ワーカースレッド %1、DNS 53 %2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Taken over</source>
        <translation>引き継ぎ済み</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Not taken over</source>
        <translation>未引き継ぎ</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="412"/>
        <source>Failed to read the TUN device: %1</source>
        <translation>TUN デバイスの読み取りに失敗しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="414"/>
        <source>Failed to read the TUN device</source>
        <translation>TUN デバイスの読み取りに失敗しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="185"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="193"/>
        <source>Idle timeout</source>
        <translation>アイドルタイムアウト</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="216"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="224"/>
        <source>Closing TUN</source>
        <translation>TUN を終了しています</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="248"/>
        <source>Re-entered TUN packet handling; dropped this packet to avoid concurrent flow-table writes</source>
        <translation>TUN パケット処理に再入したため、フローテーブルの同時書き込みを避けるためこのパケットを破棄しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="268"/>
        <source>Dropped a non-IPv4 packet from TUN (IPv6 is not supported yet)</source>
        <translation>TUN から届いた IPv4 以外のパケットを破棄しました（IPv6 はまだ未対応です）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="321"/>
        <source>Dropped an IP fragment from TUN (no reassembly)</source>
        <translation>TUN から届いた IP フラグメントを破棄しました（再構成は行いません）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="421"/>
        <source>TUN: dropped link-local/multicast UDP to %1</source>
        <translation>TUN: リンクローカル/マルチキャストの UDP（%1 宛）を破棄しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="535"/>
        <source>TUN: answered ICMP echo locally (%1, fake-ip)</source>
        <translation>TUN: ICMP エコーにローカルで応答しました（%1、fake-ip）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="565"/>
        <source>TUN TCP segment %1:%2 -&gt; %3 flags=0x%4 seq=%5 len=%6 known flows=%7</source>
        <translation>TUN TCP セグメント %1:%2 -&gt; %3 flags=0x%4 seq=%5 len=%6 既知フロー=%7</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="590"/>
        <source>TUN: TCP port 53 is not supported yet; replying RST so the client falls back to UDP</source>
        <translation>TUN: TCP のポート 53 はまだ未対応です。クライアントが UDP にフォールバックできるよう RST を返します</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="640"/>
        <source>Client RST</source>
        <translation>クライアント RST</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="668"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1227"/>
        <source>Client closed and upstream already disconnected</source>
        <translation>クライアントが切断し、上流もすでに切断済みです</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="674"/>
        <source>Four-way handshake complete</source>
        <translation>4-way ハンドシェイク完了</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="778"/>
        <source>TUN: UDP target failed recently; replying with ICMP: %1</source>
        <translation>TUN: UDP の宛先が直前に失敗したため、ICMP で応答します: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="785"/>
        <source>TUN: UDP sessions exceed %1; rejecting new sessions</source>
        <translation>TUN: UDP セッションが %1 を超えたため、新しいセッションを拒否します</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="869"/>
        <source>TUN UDP session established: %1:%2 -&gt; %3</source>
        <translation>TUN UDP セッションを確立しました: %1:%2 -&gt; %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="899"/>
        <source>TUN UDP dial failed for %1: %2</source>
        <translation>TUN の UDP ダイヤルに失敗しました（%1）: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="928"/>
        <source>TUN UDP channel closed: %1</source>
        <translation>TUN の UDP チャネルが閉じられました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="930"/>
        <source>Upstream closed</source>
        <translation>上流が切断されました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="950"/>
        <source>TUN closed UDP session %1:%2 (%3)</source>
        <translation>TUN が UDP セッション %1:%2 を閉じました（%3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1044"/>
        <source>TUN: target failed recently; sending RST: %1</source>
        <translation>TUN: 宛先が直前に失敗したため、RST を送信します: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1098"/>
        <source>TUN: concurrent connections exceed %1; rejecting new connections</source>
        <translation>TUN: 同時接続数が %1 を超えたため、新しい接続を拒否します</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1149"/>
        <source>TUN new connection %1:%2 -&gt; %3:%4</source>
        <translation>TUN 新規接続 %1:%2 -&gt; %3:%4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1179"/>
        <source>TUN dial failed for %1:%2: %3</source>
        <translation>TUN のダイヤルに失敗しました（%1:%2）: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1204"/>
        <source>TUN upstream data: %1 bytes (%2 pending)</source>
        <translation>TUN 上流データ: %1 バイト（未送信 %2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1336"/>
        <source>TUN closed connection %1:%2 (%3)</source>
        <translation>TUN が接続 %1:%2 を閉じました（%3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="61"/>
        <source>Connection %1: client disconnected (up %2 / down %3 bytes)</source>
        <translation>接続 %1: クライアントが切断しました（上り %2 / 下り %3 バイト）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="70"/>
        <source>Connection %1: upstream closed (up %2 / down %3 bytes) %4</source>
        <translation>接続 %1: 上流が切断しました（上り %2 / 下り %3 バイト）%4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="111"/>
        <source>Connection %1: short upstream write, %2 / %3 bytes</source>
        <translation>接続 %1: 上流への書き込みが不完全です（%2 / %3 バイト）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="224"/>
        <source>Config not loaded yet</source>
        <translation>設定がまだ読み込まれていません</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="232"/>
        <source>GLOBAL mode is on, but the config has no GLOBAL group</source>
        <translation>GLOBAL モードが有効ですが、設定に GLOBAL グループがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="266"/>
        <source>No MATCH fallback rule in the rule list; treating as DIRECT</source>
        <translation>ルール一覧に MATCH のフォールバックルールがないため、DIRECT として扱います</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="277"/>
        <source>Rule %1 points at a policy that does not exist: %2</source>
        <translation>ルール %1 が存在しないポリシーを参照しています: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="311"/>
        <source>Closed an inbound connection with no target address</source>
        <translation>宛先アドレスのないインバウンド接続を切断しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="332"/>
        <source>Could not determine an outbound policy (%1 -&gt; %2); connection closed</source>
        <translation>アウトバウンドポリシーを決定できなかったため、接続を切断しました（%1 -&gt; %2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="333"/>
        <source>Unknown reason</source>
        <translation>不明な理由</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="351"/>
        <source>The DIRECT outbound needs DNS resolution, but no resolver is available</source>
        <translation>DIRECT のアウトバウンドには DNS 解決が必要ですが、利用可能なリゾルバがありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="360"/>
        <source>DNS resolution failed; connection closed: %1</source>
        <translation>DNS 解決に失敗したため、接続を切断しました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="381"/>
        <source>Rule %1 dropped the connection: %2</source>
        <translation>ルール %1 が接続を破棄しました: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="394"/>
        <source>Outbound %1 failed to connect to %2:%3: %4</source>
        <translation>アウトバウンド %1 が %2:%3 への接続に失敗しました: %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="404"/>
        <source>UDP relay is not implemented yet; connection closed</source>
        <translation>UDP リレーはまだ未実装のため、接続を切断しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="430"/>
        <source>%1 -&gt; %2:%3 via %4 (rule %5)</source>
        <translation>%1 -&gt; %2:%3（経由 %4、ルール %5）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="309"/>
        <source>Vision: the server echoed an unknown UUID</source>
        <translation>Vision: サーバーから不明な UUID が返されました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="338"/>
        <source>Vision: read an unknown command %1</source>
        <translation>Vision: 不明なコマンド %1 を受信しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="385"/>
        <source>Connection closed while reading the response</source>
        <translation>応答の読み取り中に接続が閉じられました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="88"/>
        <source>Failed to send the WebSocket upgrade request</source>
        <translation>WebSocket アップグレードリクエストを送信できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="104"/>
        <source>Connection closed during the WebSocket upgrade (received %1 bytes: %2)</source>
        <translation>WebSocket アップグレード中に接続が閉じられました（%1 バイト受信: %2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="113"/>
        <source>WebSocket upgrade handshake timed out</source>
        <translation>WebSocket アップグレードのハンドシェイクがタイムアウトしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="127"/>
        <source>WebSocket handshake rejected: %1</source>
        <translation>WebSocket ハンドシェイクが拒否されました: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="154"/>
        <source>Server-sent WebSocket frames must not be masked</source>
        <translation>サーバーから送信される WebSocket フレームはマスクされていない必要があります</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="179"/>
        <source>WebSocket frame too long</source>
        <translation>WebSocket フレームが長すぎます</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="127"/>
        <source>DNS override is off; using system resolution</source>
        <translation>DNS 上書きがオフのため、システムの解決を使用します</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="152"/>
        <source>Rule set %1 has an invalid behavior: %2</source>
        <translation>ルールセット %1 の behavior が無効です: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="178"/>
        <source>Could not read the rule-set file: %1</source>
        <translation>ルールセットファイルを読み込めませんでした: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="181"/>
        <source>Remote rule sets (type=http) are not implemented yet; skipped</source>
        <translation>リモートルールセット（type=http）はまだ未実装のため、スキップしました</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="207"/>
        <source>Config hot-reloaded (inbound listeners kept)</source>
        <translation>設定をホットリロードしました（インバウンドリスナーは維持）</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="248"/>
        <source>Mixed port %1 failed to start</source>
        <translation>混合ポート %1 を起動できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="265"/>
        <source>SOCKS5 port %1 failed to start</source>
        <translation>SOCKS5 ポート %1 を起動できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="280"/>
        <source>HTTP port %1 failed to start</source>
        <translation>HTTP ポート %1 を起動できませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="290"/>
        <source>redir-port / tproxy-port transparent proxying is not implemented yet (Phase 2); those ports are not listening</source>
        <translation>redir-port / tproxy-port の透過プロキシはまだ未実装です（Phase 2）。これらのポートは待ち受けていません</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="295"/>
        <source>No inbound port is enabled in the config (mixed-port / socks-port / port are all 0)</source>
        <translation>設定でインバウンドポートが有効になっていません（mixed-port / socks-port / port がすべて 0 です）</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="335"/>
        <source>No config has been applied yet</source>
        <translation>設定がまだ適用されていません</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="349"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="350"/>
        <source>Started</source>
        <translation>起動しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="367"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="384"/>
        <source>Stopped</source>
        <translation>停止しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="403"/>
        <source>Outbound mode switched to %1</source>
        <translation>アウトバウンドモードを %1 に切り替えました</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="437"/>
        <source>TUN is already running</source>
        <translation>TUN はすでに実行中です</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="447"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="593"/>
        <source>TUN is closed</source>
        <translation>TUN は閉じられています</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="456"/>
        <source>No usable outbound matched</source>
        <translation>使用可能なアウトバウンドが見つかりませんでした</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="530"/>
        <source>TUN data-plane error (%1); TUN stopped</source>
        <translation>TUN データプレーンのエラー（%1）。TUN を停止しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="545"/>
        <source>TUN taken over (device=%1); data plane running on a worker thread</source>
        <translation>TUN を引き継ぎました（device=%1）。データプレーンはワーカースレッドで動作しています</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="586"/>
        <source>The TUN data-plane thread is still winding down; it will be released after exit</source>
        <translation>TUN データプレーンのスレッドはまだ終了処理中です。終了後に解放されます</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="633"/>
        <source>Group %1 does not exist</source>
        <translation>グループ %1 は存在しません</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="638"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="766"/>
        <source>%1 is not a proxy group</source>
        <translation>%1 はプロキシグループではありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="642"/>
        <source>Group %1 has no member %2</source>
        <translation>グループ %1 にメンバー %2 がありません</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="678"/>
        <source>Group %1 references member %2, which no longer exists; ignored</source>
        <translation>グループ %1 が存在しないメンバー %2 を参照しているため、無視しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="688"/>
        <source>Restored manual selections for %1 proxy group(s)</source>
        <translation>%1 件のプロキシグループで手動選択を復元しました</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="703"/>
        <source>Node does not exist: %1</source>
        <translation>ノードが存在しません: %1</translation>
    </message>
</context>
</TS>
