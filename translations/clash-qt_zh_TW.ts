<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_TW">
<context>
    <name>clashqt::AppContext</name>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="188"/>
        <source>Failed to turn off the system proxy on exit: %1 (the desktop still points at the local port)</source>
        <translation>結束時關閉系統代理失敗：%1（桌面仍指向本機連接埠）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="193"/>
        <source>System proxy turned off before exit, so the desktop stops pointing at a stopped local port</source>
        <translation>結束前已關閉系統代理：避免桌面繼續指向已停止的本機連接埠</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="208"/>
        <source>Failed to load the config. Check that the files in %1 are writable</source>
        <translation>設定載入失敗，請檢查 %1 目錄下的檔案是否可寫</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="224"/>
        <source>Data directory: %1 (portable: %2)</source>
        <translation>資料目錄：%1（便攜模式：%2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="225"/>
        <source>yes</source>
        <translation>是</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="226"/>
        <source>no</source>
        <translation>否</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="239"/>
        <source>Config generation failed: %1</source>
        <translation>設定產生失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="268"/>
        <source>Start failed: %1</source>
        <translation>啟動失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="269"/>
        <source>Startup failed: %1</source>
        <translation>啟動失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="304"/>
        <source>Kernel failed to restart after config reload: %1</source>
        <translation>設定重新載入後核心重新啟動失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="384"/>
        <location filename="../src/app/src/AppContext.cpp" line="411"/>
        <source>Config not loaded yet</source>
        <translation>尚未載入設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="423"/>
        <source>TUN enable request: enabled=%1 running=%2 starting(old)=%3</source>
        <translation>TUN 啟用請求：enabled=%1 running=%2 starting(舊)=%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="434"/>
        <source>TUN disabled; system routes restored</source>
        <translation>TUN 已關閉，系統路由已還原</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="446"/>
        <source>TUN helper not found (it should sit next to the main executable): %1</source>
        <translation>找不到 TUN 輔助程式（應與主程式同目錄）：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="485"/>
        <source>Removed the wildcard &quot;*&quot; from fake-ip-filter (it makes every domain skip fake-ip)</source>
        <translation>已從 fake-ip-filter 移除通配 &amp;quot;*&amp;quot;（它會讓所有域名都跳過 fake-ip）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="487"/>
        <source>Fixed the DNS config: the &quot;*&quot; in fake-ip-filter made every domain skip fake-ip, so it was removed</source>
        <translation>已修正 DNS 設定：fake-ip-filter 裡的 &amp;quot;*&amp;quot; 會導致所有域名都不走 fake-ip，已移除</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="501"/>
        <source>TUN device name changed from Mihomo to %1</source>
        <translation>TUN 裝置名已從 Mihomo 改為 %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="515"/>
        <source>TUN mode needs kernel DNS (fake-ip); DNS override was enabled automatically</source>
        <translation>TUN 模式需要核心 DNS（fake-ip），已自動開啟 DNS 覆寫</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="517"/>
        <source>TUN mode needs kernel DNS; &quot;DNS override&quot; was enabled automatically</source>
        <translation>TUN 模式需要核心 DNS，已自動開啟「DNS 覆寫」</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="532"/>
        <source>The kernel failed to start, so TUN cannot be enabled</source>
        <translation>核心啟動失敗，無法啟用 TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="555"/>
        <location filename="../src/app/src/AppContext.cpp" line="607"/>
        <source>Failed to enable TUN: %1</source>
        <translation>TUN 啟用失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="576"/>
        <source>Enabling TUN timed out; state was force-reset</source>
        <translation>TUN 啟用超時，已強制復位</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="579"/>
        <source>Timed out enabling TUN; state was reset, please try again</source>
        <translation>TUN 啟用超時，已復位，請重試</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="583"/>
        <source>Enabling TUN…</source>
        <translation>TUN 啟用中…</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="608"/>
        <source>Could not take over the device</source>
        <translation>無法接管裝置</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="620"/>
        <source>TUN enabled (system routes taken over)</source>
        <translation>TUN 已啟用（已接管系統路由）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="621"/>
        <source>TUN enabled (routes not taken over)</source>
        <translation>TUN 已啟用（未接管路由）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="630"/>
        <source>Kernel is not running; skipping switch-state restore</source>
        <translation>核心未執行，跳過開關狀態恢復</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="640"/>
        <source>Restored TUN to its previous state</source>
        <translation>已按上次的狀態恢復 TUN</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="643"/>
        <source>Failed to restore TUN: %1</source>
        <translation>恢復 TUN 失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="645"/>
        <source>Failed to restore TUN mode: %1</source>
        <translation>恢復 TUN 模式失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="654"/>
        <source>Restored the system proxy to its previous state</source>
        <translation>已按上次的狀態恢復系統代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="657"/>
        <location filename="../src/app/src/AppContext.cpp" line="659"/>
        <source>Failed to restore the system proxy: %1</source>
        <translation>恢復系統代理失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="674"/>
        <source>Start failed; system proxy not enabled: %1</source>
        <translation>啟動失敗，未啟用系統代理：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="686"/>
        <source>No inbound port available; cannot set the system proxy</source>
        <translation>沒有可用的入站連接埠，無法設定系統代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="696"/>
        <source>127.0.0.1:%1 is not listening locally; refusing to enable the system proxy (the whole desktop would lose connectivity by pointing at an empty port)</source>
        <translation>本機 127.0.0.1:%1 未在監聽，已拒絕啟用系統代理（否則整個桌面會因指向空連接埠而斷網）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="708"/>
        <source>Failed to set the system proxy: %1</source>
        <translation>設定系統代理失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="716"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>關閉系統代理失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="749"/>
        <source>Unknown outbound mode: %1</source>
        <translation>未知的出站模式：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="788"/>
        <location filename="../src/app/src/AppContext.cpp" line="813"/>
        <location filename="../src/app/src/AppContext.cpp" line="901"/>
        <location filename="../src/app/src/AppContext.cpp" line="979"/>
        <source>Failed to write the profile contents</source>
        <translation>訂閱內容寫入失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="793"/>
        <location filename="../src/app/src/AppContext.cpp" line="905"/>
        <source>Failed to write the profile list</source>
        <translation>訂閱列表寫入失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="797"/>
        <source>Added local config: %1</source>
        <translation>已新增本地設定：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="805"/>
        <location filename="../src/app/src/AppContext.cpp" line="917"/>
        <location filename="../src/app/src/AppContext.cpp" line="1020"/>
        <source>Profile does not exist: %1</source>
        <translation>訂閱不存在：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="819"/>
        <source>Replaced the config file of &quot;%1&quot;</source>
        <translation>已替換「%1」的設定檔</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="831"/>
        <source>Invalid profile URL: %1</source>
        <translation>訂閱地址無效：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="858"/>
        <source>Profile download timed out (%1 ms)</source>
        <translation>訂閱下載超時（%1 ms）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="865"/>
        <source>Profile download failed: %1</source>
        <translation>訂閱下載失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="884"/>
        <source>The profile is not a valid Clash config: %1</source>
        <translation>訂閱內容不是合法的 Clash 設定：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="909"/>
        <source>Added profile: %1</source>
        <translation>已新增訂閱：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="921"/>
        <source>Profile &quot;%1&quot; is not a remote profile; nothing to update</source>
        <translation>訂閱「%1」不是遠端訂閱，無需更新</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="948"/>
        <source>Updating &quot;%1&quot; timed out</source>
        <translation>更新「%1」超時</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="954"/>
        <source>Failed to update &quot;%1&quot;: %2</source>
        <translation>更新「%1」失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="972"/>
        <source>The update for &quot;%1&quot; returned invalid content; the old config was kept: %2</source>
        <translation>更新「%1」返回的內容不合法，已保留原設定：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="993"/>
        <source>Profile updated: %1</source>
        <translation>訂閱已更新：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1025"/>
        <source>Switched to profile: %1</source>
        <translation>已切換到訂閱：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1040"/>
        <source>Override content must be a YAML mapping: %1</source>
        <translation>覆寫內容必須是 YAML 對映：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1047"/>
        <source>Unnamed override</source>
        <translation>未命名覆寫</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1054"/>
        <source>Failed to write the override contents</source>
        <translation>覆寫內容寫入失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1058"/>
        <source>Failed to write the override list</source>
        <translation>覆寫列表寫入失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1062"/>
        <source>Added override: %1</source>
        <translation>已新增覆寫：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1093"/>
        <source>Failed to parse the rule-override file; treating it as empty: %1</source>
        <translation>規則覆寫檔案解析失敗，已按空處理：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1117"/>
        <source>No profile selected; cannot save custom rules</source>
        <translation>尚未選擇訂閱，無法儲存自定義規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1129"/>
        <source>%1 entry %2 contains a line break (each rule must stay on one line)</source>
        <translation>%1 第 %2 條包含換行（每條規則必須寫在一行內）</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1137"/>
        <source>%1 entry %2 is not a valid rule (expected at least &quot;type,target&quot;): %3</source>
        <translation>%1 第 %2 條不是合法規則（至少需要「類型,目標」）：%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1147"/>
        <source>Custom rules</source>
        <translation>自定義規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1148"/>
        <source>Fallback rules</source>
        <translation>兜底規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1149"/>
        <source>Removal rules</source>
        <translation>刪除規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1182"/>
        <source>Failed to write custom rules: %1</source>
        <translation>自定義規則寫入失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1187"/>
        <source>Custom rules saved (profile %1): %2 total, %3 disabled, %4 fallback / %5 removal</source>
        <translation>自定義規則已儲存（訂閱 %1）：共 %2 條，其中停用 %3 條，兜底 %4 條 / 刪除 %5 條</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1196"/>
        <source>Custom rules applied</source>
        <translation>自定義規則已生效</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionListModel</name>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="67"/>
        <source>Rule: %1</source>
        <translation>規則：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="68"/>
        <source>Outbound: %1</source>
        <translation>出站：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="69"/>
        <source>Up %1 · Down %2 · %3</source>
        <translation>上傳 %1 · 下載 %2 · %3</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="63"/>
        <source>Connections</source>
        <translation>連線</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="64"/>
        <source>Active connections, one card each. Double-click a card, or use the × on its right, to close that connection.</source>
        <translation>目前的作用中連線，一條一張卡片。按兩下卡片，或點右側的 ×，即可關閉該連線。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="73"/>
        <source>Auto refresh (1 s)</source>
        <translation>自動重新整理（1 秒）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="77"/>
        <source>Close selected</source>
        <translation>關閉選中</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="78"/>
        <source>Close all</source>
        <translation>關閉全部</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="104"/>
        <source>No active connections</source>
        <translation>暫無作用中連線</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="145"/>
        <source>just now</source>
        <translation>剛剛</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="146"/>
        <source>%1 min ago</source>
        <translation>%1 分鐘前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="147"/>
        <source>%1 h ago</source>
        <translation>%1 小時前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="148"/>
        <source>%1 d ago</source>
        <translation>%1 天前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="157"/>
        <source>%1 active connection(s) · Total up %2 · Total down %3</source>
        <translation>作用中連線 %1 · 累計上傳 %2 · 累計下載 %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="239"/>
        <source>Select a connection first</source>
        <translation>請先選擇一條連線</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="253"/>
        <source>Closed %1 connection(s)</source>
        <translation>已關閉 %1 條連線</translation>
    </message>
</context>
<context>
    <name>clashqt::LogsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="48"/>
        <source>Logs</source>
        <translation>日誌</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="49"/>
        <source>Application log. Logs are also written to the logs/ folder in the data directory.</source>
        <translation>應用執行日誌。日誌同時寫入資料目錄下的 logs/ 檔案。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="54"/>
        <source>Level</source>
        <translation>等級</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="61"/>
        <source>Pause scrolling</source>
        <translation>暫停滾動</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="64"/>
        <source>Clear view</source>
        <translation>清空顯示</translation>
    </message>
</context>
<context>
    <name>clashqt::MainWindow</name>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="236"/>
        <source>Rule</source>
        <translation>規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="237"/>
        <source>Global</source>
        <translation>全域性</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="238"/>
        <source>Direct</source>
        <translation>直連</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="272"/>
        <source>System proxy</source>
        <translation>系統代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="282"/>
        <source>No system proxy backend found (gsettings / kwriteconfig); system proxy is unavailable</source>
        <translation>未偵測到系統代理後端（gsettings / kwriteconfig），系統代理不可用</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="288"/>
        <source>TUN mode</source>
        <translation>TUN 模式</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="295"/>
        <source>Captures all local traffic, including apps that ignore the system proxy. The first enable requires a one-time password prompt.</source>
        <translation>接管本機全部流量（含不讀系統代理的程式）。首次啟用需要輸入一次密碼授權。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="343"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="708"/>
        <source>Update this subscription</source>
        <translation>更新此訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="412"/>
        <source>Proxies</source>
        <translation>代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="414"/>
        <source>Rules</source>
        <translation>規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="416"/>
        <source>Overrides</source>
        <translation>覆寫</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="418"/>
        <source>Logs</source>
        <translation>日誌</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="420"/>
        <source>Connections</source>
        <translation>連線</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="423"/>
        <source>Settings</source>
        <translation>設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="484"/>
        <source>Traffic</source>
        <translation>流量</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="694"/>
        <source>No profile selected</source>
        <translation>未選擇訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="709"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="757"/>
        <source>Local profiles have no URL to update from</source>
        <translation>本地訂閱沒有可更新的連結</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="721"/>
        <source>%1 / %2</source>
        <translation>%1 / %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="730"/>
        <source>Updated %1</source>
        <translation>更新於 %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="745"/>
        <source>Open the profiles page</source>
        <translation>開啟訂閱頁</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="752"/>
        <source>Select a profile first</source>
        <translation>請先選擇一個訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="765"/>
        <source>Update failed: %1</source>
        <translation>更新失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="767"/>
        <source>Subscription updated</source>
        <translation>訂閱已更新</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="792"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="871"/>
        <source>Disable system proxy</source>
        <translation>關閉系統代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="800"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="907"/>
        <source>Enable TUN mode</source>
        <translation>啟用 TUN 模式</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="807"/>
        <source>Show main window</source>
        <translation>顯示主視窗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="815"/>
        <source>Quit</source>
        <translation>結束</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="872"/>
        <source>Enable system proxy</source>
        <translation>啟用系統代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="877"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="905"/>
        <source>Enabling TUN…</source>
        <translation>TUN 啟用中…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="881"/>
        <source>System proxy + TUN enabled</source>
        <translation>系統代理 + TUN 已開啟</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="882"/>
        <source>TUN enabled</source>
        <translation>TUN 已開啟</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="883"/>
        <source>System proxy enabled</source>
        <translation>系統代理已開啟</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="884"/>
        <source>System proxy disabled</source>
        <translation>系統代理已關閉</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="900"/>
        <source>enabling…</source>
        <translation>啟用中…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="906"/>
        <source>Disable TUN mode</source>
        <translation>關閉 TUN 模式</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="916"/>
        <source>Mixed port %1</source>
        <translation>混合連接埠 %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="918"/>
        <source>Not running</source>
        <translation>未啟動</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="946"/>
        <source>Failed to switch outbound mode: %1</source>
        <translation>切換出站模式失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="952"/>
        <source>Outbound mode: %1</source>
        <translation>出站模式：%1</translation>
    </message>
</context>
<context>
    <name>clashqt::OverridePage</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>Domain suffix (incl. subdomains)</source>
        <translation>域名字尾（含所有子域）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>e.g. example.com</source>
        <translation>例如 example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>Domain (exact)</source>
        <translation>域名（完全一致）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>e.g. www.example.com</source>
        <translation>例如 www.example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>Domain keyword</source>
        <translation>域名關鍵字</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>e.g. google</source>
        <translation>例如 google</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>Domain regex</source>
        <translation>域名正則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>e.g. ^ads\..*</source>
        <translation>例如 ^ads\..*</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>Geosite category</source>
        <translation>預置站點庫</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>e.g. cn / netflix / telegram</source>
        <translation>例如 cn / netflix / telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>IP country</source>
        <translation>IP 歸屬地</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>e.g. CN / US / JP</source>
        <translation>例如 CN / US / JP</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>IPv4 CIDR</source>
        <translation>IPv4 網段</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>e.g. 10.0.0.0/8</source>
        <translation>例如 10.0.0.0/8</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>IPv6 CIDR</source>
        <translation>IPv6 網段</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>e.g. 2001:db8::/32</source>
        <translation>例如 2001:db8::/32</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>ASN</source>
        <translation>ASN 編號</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>e.g. 13335</source>
        <translation>例如 13335</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>Destination port</source>
        <translation>目標連接埠</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>e.g. 443</source>
        <translation>例如 443</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>Source port</source>
        <translation>來源連接埠</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>e.g. 1234</source>
        <translation>例如 1234</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>Network type</source>
        <translation>網路類型</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>tcp or udp</source>
        <translation>tcp 或 udp</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>Process name</source>
        <translation>處理程序名</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>e.g. telegram</source>
        <translation>例如 telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>Process path</source>
        <translation>處理程序路徑</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>e.g. /usr/bin/curl</source>
        <translation>例如 /usr/bin/curl</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Rule set</source>
        <translation>規則集</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Choose a rule set from the config</source>
        <translation>選擇設定裡的規則集</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="129"/>
        <source>Catch-all (match all remaining traffic)</source>
        <translation>兜底（匹配所有剩餘流量）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="405"/>
        <source>Overrides</source>
        <translation>覆寫</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="406"/>
        <source>Adds custom rules on top of the profile&apos;s rules. Pick a type, content and target when adding — no rule syntax needed. The list order is the match priority; changes are saved and applied automatically.</source>
        <translation>在訂閱規則之上疊加自定義規則。新增時按提示選擇類型、內容和策略即可，無需瞭解規則語法；列表從上到下即匹配優先順序，改動自動儲存並生效。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="408"/>
        <source>Custom rules</source>
        <translation>自定義規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="418"/>
        <source>New rule</source>
        <translation>新增規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Enabled</source>
        <translation>啟用</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Rule type</source>
        <translation>規則類型</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Content</source>
        <translation>匹配內容</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Target</source>
        <translation>交給誰處理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="432"/>
        <source>Actions</source>
        <translation>操作</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="452"/>
        <source>Double-click any row to edit; uncheck it to disable (content is kept but not matched). Changes are saved and applied automatically.</source>
        <translation>按兩下任意一行可以修改；取消勾選即停用（內容保留、不參與匹配）。改動會自動儲存並立即生效，無需手動提交。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="526"/>
        <source>No profile selected; custom rules cannot be saved</source>
        <translation>目前沒有選中訂閱，自定義規則無法儲存</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="527"/>
        <source>Applies to profile: %1 (rules are stored per profile and differ between profiles)</source>
        <translation>作用於訂閱：%1（規則按訂閱儲存，切換訂閱後內容不同）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="553"/>
        <source>%1 rules, %2 enabled</source>
        <translation>共 %1 條，啟用 %2 條</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="575"/>
        <source>Enabled; uncheck to disable</source>
        <translation>已啟用，取消勾選即停用</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="576"/>
        <source>Disabled: not matched after saving</source>
        <translation>已停用：儲存後不參與匹配</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="601"/>
        <source>%1 (%2)
Rule: %3</source>
        <translation>%1（%2）
規則：%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="612"/>
        <source>Move up (higher priority)</source>
        <translation>上移（提高優先順序）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="614"/>
        <source>Move down (lower priority)</source>
        <translation>下移（降低優先順序）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="615"/>
        <source>Edit this rule</source>
        <translation>修改這條規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="616"/>
        <source>Delete this rule</source>
        <translation>刪除這條規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="726"/>
        <source>Save failed</source>
        <translation>儲存失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="733"/>
        <source>Applied automatically: %1 rules, %2 disabled</source>
        <translation>已自動生效：%1 條規則，其中停用 %2 條</translation>
    </message>
</context>
<context>
    <name>clashqt::ProfilesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="38"/>
        <source>Expired</source>
        <translation>已過期</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="40"/>
        <source> (%1 days)</source>
        <translation>（%1 天）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="235"/>
        <source>Profiles</source>
        <translation>訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="236"/>
        <source>Manage config sources. The active profile is merged with &quot;Config overrides&quot; before use.</source>
        <translation>管理設定來源。目前選中的訂閱會與「設定覆寫」合併後生效。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="247"/>
        <source>Add profile</source>
        <translation>新增訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="248"/>
        <source>Import local config</source>
        <translation>匯入本地設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="249"/>
        <source>Edit</source>
        <translation>編輯</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="250"/>
        <source>Update selected</source>
        <translation>更新選中</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="251"/>
        <source>Update all</source>
        <translation>更新全部</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="252"/>
        <source>Set active</source>
        <translation>設為目前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="253"/>
        <source>Delete</source>
        <translation>刪除</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="263"/>
        <source>Edit the name or URL (or double-click a row)</source>
        <translation>修改名稱 / 訂閱連結（按兩下某一行也可以）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="276"/>
        <source>No profiles yet. Click &quot;Add profile&quot; to enter a subscription URL, or &quot;Import local config&quot; to pick an existing Clash config file.</source>
        <translation>還沒有任何訂閱。點選「新增訂閱」填寫訂閱連結，或「匯入本地設定」選擇一個已有的 Clash 設定檔。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Active</source>
        <translation>目前</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Type</source>
        <translation>類型</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Traffic</source>
        <translation>流量</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Expires</source>
        <translation>到期</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Updated</source>
        <translation>更新時間</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="341"/>
        <source>Remote</source>
        <translation>遠端</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="342"/>
        <source>Local</source>
        <translation>本地</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="343"/>
        <source>Plugin</source>
        <translation>外掛</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="386"/>
        <source>Add failed</source>
        <translation>新增失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="404"/>
        <source>Choose a config file</source>
        <translation>選擇設定檔</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="405"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Clash 設定 (*.yaml *.yml);;所有檔案 (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="410"/>
        <source>Import failed</source>
        <translation>匯入失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="420"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="471"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="505"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="521"/>
        <source>Select a profile first</source>
        <translation>請先選擇一個訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Save failed</source>
        <translation>儲存失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Failed to write profile config</source>
        <translation>寫入訂閱設定失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="446"/>
        <source>Replace failed</source>
        <translation>替換失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="458"/>
        <source>URL saved</source>
        <translation>連結已儲存</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="459"/>
        <source>URL saved, but fetching the new content failed:

%1

Try &quot;Update selected&quot; again later.</source>
        <translation>連結已儲存，但拉取新訂閱內容失敗：

%1

可稍後用「更新選中」重試。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="477"/>
        <source>Update failed</source>
        <translation>更新失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="489"/>
        <source>Some profiles failed to update</source>
        <translation>部分訂閱更新失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="490"/>
        <source>%1 succeeded, %2 failed:

%3</source>
        <translation>成功 %1 個，失敗 %2 個：

%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="496"/>
        <source>Updated %1 profile(s)</source>
        <translation>已更新 %1 個訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="511"/>
        <source>Switch failed</source>
        <translation>切換失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="527"/>
        <source>Delete profile</source>
        <translation>刪除訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="528"/>
        <source>Delete &quot;%1&quot;? Its config file will be removed as well.</source>
        <translation>確定要刪除「%1」嗎？該訂閱的設定檔也會被刪除。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Delete failed</source>
        <translation>刪除失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Profile not found</source>
        <translation>訂閱不存在</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="545"/>
        <source>Profile deleted, but reloading the config failed: %1</source>
        <translation>訂閱已刪除，但設定重新載入失敗：%1</translation>
    </message>
</context>
<context>
    <name>clashqt::ProxiesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="66"/>
        <source>Group</source>
        <translation>組</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="90"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="334"/>
        <source>Nodes</source>
        <translation>節點</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="110"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="149"/>
        <source>Testing</source>
        <translation>測試中</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="111"/>
        <source>Test</source>
        <translation>測試</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="112"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="151"/>
        <source>Timeout</source>
        <translation>超時</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="291"/>
        <source>Proxies</source>
        <translation>代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="292"/>
        <source>Pick a node from the proxy group: click a card to switch to it. The text on the right is the last measured latency; click it to test that node again, or click Test on a node that has not been measured yet. In global mode, the GLOBAL group decides the exit for all traffic.</source>
        <translation>從代理組裡選一個節點：點卡片即切換。右側的文字是上次測得的延遲，點它會重新測速；還沒測過的節點顯示「測試」，點它測一次。全域性模式下，GLOBAL 組決定所有流量的出口。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="298"/>
        <source>Proxy group</source>
        <translation>代理組</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="309"/>
        <source>Sort</source>
        <translation>排序</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="311"/>
        <source>Default order</source>
        <translation>預設順序</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="312"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="313"/>
        <source>Type</source>
        <translation>類型</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="314"/>
        <source>Latency</source>
        <translation>延遲</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="316"/>
        <source>Sort nodes by name, protocol type or latest measured latency.
Latency sorts ascending; untested and timed-out nodes go last. Sorting never changes the selected node.</source>
        <translation>按名稱、協議類型或最近一次測速延遲排列節點。
延遲按升序，未測與超時排在最後；切換排序不會改變目前選中的節點。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="323"/>
        <source>Test this group</source>
        <translation>測試本組</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="324"/>
        <source>Test all groups</source>
        <translation>測試全部組</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="436"/>
        <source>The active profile has no proxy groups</source>
        <translation>目前訂閱沒有任何代理組</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="469"/>
        <source>Type %1 · %2 members</source>
        <translation>類型 %1 · 共 %2 個成員</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="472"/>
        <source>Current node: %1</source>
        <translation>目前節點：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="501"/>
        <source>Proxy group · %1</source>
        <translation>代理組 · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="502"/>
        <source>Protocol · %1</source>
        <translation>協議 · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="503"/>
        <source>Unknown</source>
        <translation>未知</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="505"/>
        <source>This protocol is not supported yet; connections will fail explicitly</source>
        <translation>目前實現尚不支援該協議，選中後連線會明確失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="511"/>
        <source>Last latency test timed out</source>
        <translation>最近一次測速超時</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="516"/>
        <source>Last latency %1 ms</source>
        <translation>上次延遲 %1 ms</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="517"/>
        <source>Last test timed out</source>
        <translation>上次測試超時</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="522"/>
        <source>Currently pointing at %1</source>
        <translation>目前指向 %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="524"/>
        <source>Click the text on the right of a card to test that node</source>
        <translation>點卡片右側的文字可測速</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="741"/>
        <source>Node no longer exists</source>
        <translation>節點已不存在</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="772"/>
        <source>Latency test finished: %1/%2 nodes available</source>
        <translation>測速完成：%1/%2 個節點可用</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="776"/>
        <source>%1 latency test failed: %2</source>
        <translation>%1 測速失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="792"/>
        <source>These nodes are being tested, or none can be tested</source>
        <translation>這些節點正在測速，或沒有可測的節點</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="805"/>
        <source>Testing %1 nodes (%2 at a time, results fill in as they finish)</source>
        <translation>開始測試 %1 個節點（同時 %2 個，結果逐個回填）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="835"/>
        <source>Failed to switch node: %1</source>
        <translation>切換節點失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="843"/>
        <source>%1 switched to %2</source>
        <translation>%1 已切換到 %2</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="170"/>
        <source>New rule</source>
        <translation>新增規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="171"/>
        <source>Edit rule</source>
        <translation>編輯規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="180"/>
        <source>Follow the prompts; the rule syntax is assembled when you save.</source>
        <translation>按提示選擇即可，儲存時會自動拼成規則語法。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="198"/>
        <source>%1 (cannot be edited structurally)</source>
        <translation>%1（無法結構化編輯）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="201"/>
        <source>Rule type</source>
        <translation>規則類型</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="217"/>
        <source>Content</source>
        <translation>匹配內容</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="223"/>
        <source>Choose a policy or proxy group</source>
        <translation>選擇策略或代理組</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="224"/>
        <source>Target</source>
        <translation>交給誰處理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="227"/>
        <source>Default</source>
        <translation>預設</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="228"/>
        <source>Do not resolve domain (no-resolve)</source>
        <translation>不解析域名（no-resolve）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="229"/>
        <source>Match source address (src)</source>
        <translation>匹配來源地址（src）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="230"/>
        <source>Options</source>
        <translation>匹配方式</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="253"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="254"/>
        <source>Save</source>
        <translation>儲存</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="302"/>
        <source>No rule-providers in the config</source>
        <translation>設定裡沒有 rule-providers</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="303"/>
        <source>Choose a rule set</source>
        <translation>選擇規則集</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="333"/>
        <source>A catch-all rule matches all remaining traffic; keep it last among your custom rules.</source>
        <translation>兜底規則會匹配所有未命中的流量，請務必排在自定義規則的最後。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="335"/>
        <source>example.com also matches all of its subdomains.</source>
        <translation>填 example.com 會同時匹配它自己與它的所有子域。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="337"/>
        <source>For LAN traffic, set Options to no-resolve.</source>
        <translation>直連區域網路時，建議把「匹配方式」設為不解析。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="339"/>
        <source>Rule sets come from rule-providers in the profile.</source>
        <translation>規則集來自訂閱設定的 rule-providers。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="362"/>
        <source>Choose what handles this rule (policy / proxy group).</source>
        <translation>請選擇這條規則交給誰處理（策略 / 代理組）。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="367"/>
        <source>Enter the content to match; it is what the rule checks.</source>
        <translation>請填寫「匹配內容」，它是這條規則判斷的依據。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="372"/>
        <source>IPv4 CIDR needs a prefix length, e.g. 10.0.0.0/8.</source>
        <translation>IPv4 網段要帶掩碼長度，例如 10.0.0.0/8。</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleRowDelegate</name>
    <message>
        <location filename="../src/app/src/ui/RuleList.cpp" line="156"/>
        <source>%1 hits</source>
        <translation>命中 %1 次</translation>
    </message>
</context>
<context>
    <name>clashqt::RulesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="43"/>
        <source>Rules</source>
        <translation>規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="44"/>
        <source>Rules in match order, one card each. Turn the switch on the right off to disable a rule for this session; it is restored on restart.</source>
        <translation>按匹配順序排列全部規則，一條一張卡片。關掉右側開關可臨時停用該規則，重新啟動後恢復。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="64"/>
        <source>No rules loaded</source>
        <translation>尚未載入規則</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="90"/>
        <source>Failed to update rule state</source>
        <translation>規則狀態更新失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="132"/>
        <source>%1 hits in this session</source>
        <translation>本次執行命中 %1 次</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="133"/>
        <source>Disabled until restart</source>
        <translation>已停用，重新啟動後恢復</translation>
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
        <translation>日誌、DNS 覆寫、連接埠與系統整合。修改後會自動儲存並即時生效。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="46"/>
        <source>Appearance</source>
        <translation>介面</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="50"/>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="63"/>
        <source>Follow system</source>
        <translation>跟隨系統</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="51"/>
        <source>Light</source>
        <translation>淺色</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="52"/>
        <source>Dark</source>
        <translation>深色</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="58"/>
        <source>Theme</source>
        <translation>主題</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="68"/>
        <source>Language</source>
        <translation>語言</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="69"/>
        <source>Interface text is loaded at startup; restart the app after switching.</source>
        <translation>介面文案在啟動時載入，切換後需要重新啟動應用。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="74"/>
        <source>Logs</source>
        <translation>日誌</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="81"/>
        <source>Log level</source>
        <translation>日誌等級</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="86"/>
        <source> days</source>
        <translation> 天</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="89"/>
        <source>Log retention (days)</source>
        <translation>日誌保留天數</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="90"/>
        <source>Log files older than the retention period are removed at startup.</source>
        <translation>超過保留天數的日誌檔案會在啟動時清理。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="93"/>
        <source>Open data folder</source>
        <translation>開啟資料目錄</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="102"/>
        <source>Config overrides</source>
        <translation>設定覆寫</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="105"/>
        <source>Manage DNS settings in this app</source>
        <translation>由本應用管理 DNS 設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="108"/>
        <source>When on, the DNS section configured here is used; when off, the profile&apos;s own DNS settings apply.</source>
        <translation>開啟後使用本應用設定的 DNS 段；關閉則沿用訂閱自帶的 DNS 設定。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="111"/>
        <source>Manage domain sniffing in this app</source>
        <translation>由本應用管理域名嗅探</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="117"/>
        <source>Inbound ports</source>
        <translation>入站連接埠</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="131"/>
        <source>Mixed port (HTTP + SOCKS5)</source>
        <translation>混合連接埠（HTTP + SOCKS5）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="133"/>
        <source>SOCKS5 port</source>
        <translation>SOCKS5 連接埠</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="134"/>
        <source>Set it equal to the mixed port to reuse the mixed port.</source>
        <translation>設為與混合連接埠相同則複用混合連接埠。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="136"/>
        <source>HTTP port</source>
        <translation>HTTP 連接埠</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="141"/>
        <source>System integration</source>
        <translation>系統整合</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="144"/>
        <source>Start at login</source>
        <translation>開機自啟動</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="146"/>
        <source>Implemented via an XDG autostart entry (~/.config/autostart/clash-qt.desktop).</source>
        <translation>透過 XDG 自啟動項實現（~/.config/autostart/clash-qt.desktop）。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="149"/>
        <source>System proxy</source>
        <translation>系統代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="152"/>
        <source>Points the system HTTP/HTTPS/SOCKS proxy at this app&apos;s mixed port. Current backend: %1</source>
        <translation>將系統 HTTP/HTTPS/SOCKS 代理指向本應用的混合連接埠。目前後端：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="156"/>
        <source>TUN mode</source>
        <translation>TUN 模式</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="159"/>
        <source>Creates a virtual network interface to capture all traffic, including apps that ignore the system proxy (games, CLI tools, some Electron apps). The first enable asks for your password once; it only takes effect together with &quot;Route domain traffic&quot;.</source>
        <translation>建立虛擬網路卡接管流量，連不認系統代理的程式（遊戲 / 命令列 / 部分 Electron）也能走代理。首次開啟會彈系統密碼框授權一次；必須與「接管域名流量」配合才真正生效。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="162"/>
        <source>Route domain traffic (fake-IP)</source>
        <translation>接管域名流量（fake-ip 路由）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="165"/>
        <source>Routes the fake-IP range to the TUN interface (removed together with the interface). The default route is untouched, so connectivity is unaffected; with this off, TUN receives no traffic.</source>
        <translation>把假 IP 網段的路由指向 TUN（關閉 TUN 時隨介面一起消失）。不改動系統預設路由，因此不會影響網路連通性；關掉這一項，TUN 將收不到任何流量。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="171"/>
        <source>About</source>
        <translation>關於</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="177"/>
        <source>Check for updates when the app starts</source>
        <translation>軟體啟動時檢查更新</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="187"/>
        <source>Version v%1</source>
        <translation>目前版本 v%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="192"/>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="481"/>
        <source>Check for updates</source>
        <translation>檢查更新</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="198"/>
        <source>Reads the update manifest attached to the latest release, fetched through public GitHub mirrors. When an update exists, its changelog is shown and the release page can be opened in your browser; downloading and installing is up to you.</source>
        <translation>讀取最新版本附帶的更新清單（經公共 GitHub 映象獲取）。有更新時顯示更新日誌，並可開啟發布頁在瀏覽器裡檢視；下載與安裝由你自己完成。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="252"/>
        <source>Already up to date (v%1)</source>
        <translation>已是最新版本 v%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="260"/>
        <source> (the kernel is not running; start the proxy and try again)</source>
        <translation>（目前核心未執行，請先開啟代理再試）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="262"/>
        <source>Update check failed: %1%2</source>
        <translation>檢查更新失敗：%1%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="300"/>
        <source>Data folder: %1</source>
        <translation>資料目錄：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="359"/>
        <source>Reload failed after port change: %1</source>
        <translation>連接埠修改後過載失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="372"/>
        <source>Failed to change startup setting: %1</source>
        <translation>自啟動設定失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="456"/>
        <source>Language saved; restart the app to apply.</source>
        <translation>介面語言已儲存，重新啟動應用後生效。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="481"/>
        <source>Checking…</source>
        <translation>檢查中…</translation>
    </message>
</context>
<context>
    <name>clashqt::SubscriptionDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Add profile</source>
        <translation>新增訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Edit profile</source>
        <translation>編輯訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="84"/>
        <source>Enter the subscription URL; it will be downloaded and activated immediately.</source>
        <translation>填寫訂閱連結，程式會立即下載並切換到這個訂閱。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="86"/>
        <source>Change the name or subscription URL. If this is the active profile, a changed URL is fetched again immediately.</source>
        <translation>修改名稱或訂閱連結。連結變更後，若它是目前訂閱，會立即按新連結重新拉取。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="87"/>
        <source>A local profile uses an imported config file, which you can replace below.</source>
        <translation>本地訂閱的內容來自匯入的設定檔，可在下面替換。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="90"/>
        <source>Profile info</source>
        <translation>訂閱資訊</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="97"/>
        <source>Optional; defaults to the subscription&apos;s domain</source>
        <translation>可留空，預設按訂閱域名命名</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="99"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="108"/>
        <source>Subscription URL</source>
        <translation>訂閱連結</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="110"/>
        <source>Update this profile on startup</source>
        <translation>啟動時自動更新該訂閱</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="115"/>
        <source>When enabled, the profile is refreshed automatically at startup.</source>
        <translation>開啟後，程式啟動時會自動下載該訂閱的最新設定。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="127"/>
        <source>Choose another file</source>
        <translation>重新選擇設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="132"/>
        <source>Config file</source>
        <translation>設定檔</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="133"/>
        <source>Picking a new file replaces this profile&apos;s content; its id and active state are preserved.</source>
        <translation>選擇新檔案後會替換該訂閱的內容，id 與「目前訂閱」狀態保持不變。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="138"/>
        <source>Choose a config file</source>
        <translation>選擇設定檔</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="139"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Clash 設定 (*.yaml *.yml);;所有檔案 (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="153"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Add</source>
        <translation>新增</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Save</source>
        <translation>儲存</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="193"/>
        <source>Name required</source>
        <translation>名稱不能為空</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="194"/>
        <source>Give this profile a name.</source>
        <translation>請給這個訂閱起一個名稱。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="203"/>
        <source>Invalid subscription URL</source>
        <translation>訂閱連結無效</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="205"/>
        <source>Enter a subscription URL starting with http/https.</source>
        <translation>請填寫以 http/https 開頭的訂閱連結。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="206"/>
        <source>The subscription URL must start with http/https.</source>
        <translation>訂閱連結需要以 http/https 開頭，請檢查後重試。</translation>
    </message>
</context>
<context>
    <name>clashqt::TrafficPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="63"/>
        <source>Traffic</source>
        <translation>流量</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="64"/>
        <source>Live upload/download speed and cumulative traffic; the chart shows 1-second samples from the last 2 minutes.</source>
        <translation>實時上下行速度與累計流量統計，曲線為最近 2 分鐘的每秒取樣。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="67"/>
        <source>Status</source>
        <translation>目前狀態</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="72"/>
        <source>Total 0 B / 0 B</source>
        <translation>累計 0 B / 0 B</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="73"/>
        <source>0 active connections</source>
        <translation>作用中連線 0</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="82"/>
        <source>Speed chart</source>
        <translation>速度曲線</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="96"/>
        <source>s</source>
        <translation>秒</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="188"/>
        <source>Session total ↑ %1 · ↓ %2</source>
        <translation>視窗累計 ↑ %1 · ↓ %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="193"/>
        <source>%1 active connection(s) · Memory %2</source>
        <translation>作用中連線 %1 · 記憶體 %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="242"/>
        <source>Upload (%1)</source>
        <translation>上傳 (%1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="243"/>
        <source>Download (%1)</source>
        <translation>下載 (%1)</translation>
    </message>
</context>
<context>
    <name>clashqt::UpdateDialog</name>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="20"/>
        <source>Update available</source>
        <translation>發現新版本</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="30"/>
        <source>Version %1 is available</source>
        <translation>發現新版本 %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="31"/>
        <source>Version v%1. The changelog comes from the GitHub release; downloading and installing is up to you — this app never replaces itself.</source>
        <translation>目前版本 v%1。更新日誌來自 GitHub Release；下載與安裝由你自己決定，本程式不會自動替換自身。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="51"/>
        <source>(No release notes for this version; see the release page.)</source>
        <translation>（這次釋出沒有填寫更新說明，可直接到釋出頁檢視。）</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="97"/>
        <source>No package has been attached to this release yet; it may still be uploading.</source>
        <translation>該版本還沒有附帶安裝包，可能仍在上傳。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="100"/>
        <source>Published on %1. Download the package from the release page.</source>
        <translation>釋出於 %1。安裝包請到釋出頁下載。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="103"/>
        <source>Download the package from the release page.</source>
        <translation>安裝包請到釋出頁下載。</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="112"/>
        <source>Later</source>
        <translation>稍後再說</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="113"/>
        <source>Open download page</source>
        <translation>開啟下載頁</translation>
    </message>
</context>
<context>
    <name>clashqt::app</name>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="130"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable.</source>
        <translation>未找到 wintun.dll。請把它放到可執行檔案同目錄。</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="145"/>
        <source>Could not create the Wintun adapter</source>
        <translation>無法建立 Wintun 介面卡</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="175"/>
        <source>Wintun is unavailable</source>
        <translation>Wintun 不可用</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="182"/>
        <source>Could not start the Wintun session</source>
        <translation>無法啟動 Wintun 會話</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="65"/>
        <source>Clash Qt - a desktop client for Clash-style subscriptions</source>
        <translation>Clash Qt —— Clash 訂閱客戶端（Qt6 / C++）</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="71"/>
        <source>Start without showing the main window (for autostart)</source>
        <translation>啟動後不顯示主視窗（配合自啟動使用）</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="73"/>
        <source>Use the data directory next to the executable</source>
        <translation>使用可執行檔案旁的 data 目錄作為資料目錄</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="97"/>
        <source>Another instance is already running: asked it to show its window, exiting</source>
        <translation>偵測到已有執行個體在執行：已請求其顯示視窗，本處理程序結束</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="108"/>
        <source>Starting %1 %2 (%3, silent=%4)</source>
        <translation>啟動 %1 %2（%3，silent=%4）</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="121"/>
        <source>Initialization failed: %1</source>
        <translation>初始化失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="128"/>
        <source>UI language: %1 (configured value %2)</source>
        <translation>介面語言：%1（設定值 %2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="148"/>
        <source>Start failed: %1</source>
        <translation>啟動失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="177"/>
        <source>Silent start: not showing the main window</source>
        <translation>靜默啟動：不顯示主視窗</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="188"/>
        <source>%1 exited with code %2</source>
        <translation>%1 已結束，結束碼 %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="72"/>
        <source>Could not read file %1: %2</source>
        <translation>無法讀取檔案 %1：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="82"/>
        <source>Not a valid Clash config: %1</source>
        <translation>不是合法的 Clash 設定：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/AppLogger.cpp" line="152"/>
        <source>Removed %1 expired log file(s) (retention %2 days)</source>
        <translation>已清理 %1 個過期日誌檔案（保留 %2 天）</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="18"/>
        <source>Could not create the directory: %1</source>
        <translation>無法建立目錄：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="26"/>
        <source>Could not write %1: %2</source>
        <translation>無法寫入 %1：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="32"/>
        <source>Incomplete write to %1: %2</source>
        <translation>寫入 %1 不完整：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="38"/>
        <source>Failed to commit %1: %2</source>
        <translation>提交 %1 失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="70"/>
        <source>Used up</source>
        <translation>已用完</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="77"/>
        <source>%1 d %2 h</source>
        <translation>%1 天 %2 小時</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="78"/>
        <source>%1 h %2 min</source>
        <translation>%1 小時 %2 分</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="79"/>
        <source>%1 min</source>
        <translation>%1 分</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="208"/>
        <source>Failed to parse the app config; using defaults: %1</source>
        <translation>應用設定解析失敗，已使用預設設定：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="228"/>
        <source>Failed to write back the app config: %1</source>
        <translation>應用設定回寫失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="238"/>
        <source>Failed to save the app config: %1</source>
        <translation>應用設定儲存失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="226"/>
        <source>Failed to parse the legacy managed config (%1): %2</source>
        <translation>舊受控設定解析失敗（%1）：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="232"/>
        <source>Merged the legacy managed config into config.yaml and deleted it: %1</source>
        <translation>已將舊受控設定併入 config.yaml 並刪除：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="134"/>
        <source>Failed to parse the profile list: %1</source>
        <translation>訂閱列表解析失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="157"/>
        <source>Failed to save the profile list: %1</source>
        <translation>訂閱列表儲存失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="242"/>
        <source>Failed to write profile content (%1): %2</source>
        <translation>訂閱內容寫入失敗（%1）：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="74"/>
        <source>Failed to parse the override list: %1</source>
        <translation>覆寫列表解析失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="92"/>
        <source>Failed to save the override list: %1</source>
        <translation>覆寫列表儲存失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="156"/>
        <source>Failed to write override content (%1.%2): %3</source>
        <translation>覆寫內容寫入失敗（%1.%2）：%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="98"/>
        <source>References an override that does not exist: %1</source>
        <translation>引用了不存在的覆寫：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="107"/>
        <source>Override %1 has no content file</source>
        <translation>覆寫 %1 的內容檔案缺失</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="116"/>
        <source>Override &quot;%1&quot; is a JS script, and the JS override engine is not supported yet; it was skipped (the config will not apply it)</source>
        <translation>覆寫「%1」是 js 指令碼，目前尚未支援 JS 覆寫引擎，已跳過（設定不會應用該覆寫）</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="126"/>
        <source>Failed to parse override &quot;%1&quot;: %2</source>
        <translation>覆寫「%1」解析失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="133"/>
        <source>The root of override &quot;%1&quot; must be a mapping</source>
        <translation>覆寫「%1」的根節點必須是對映</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="159"/>
        <source>Failed to parse the rule-override file: %1</source>
        <translation>規則覆寫檔案解析失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="228"/>
        <source>No profile selected yet. Add one on the Profiles page and select it first.</source>
        <translation>尚未選擇任何訂閱，請先在「訂閱」頁新增並選中一個設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="239"/>
        <source>Profile &quot;%1&quot; is empty or missing</source>
        <translation>訂閱「%1」的內容為空或不存在</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="249"/>
        <source>Failed to parse the profile content: %1</source>
        <translation>訂閱內容解析失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="356"/>
        <source>Failed to write the config: %1</source>
        <translation>設定寫入失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="361"/>
        <source>Generated the runtime config: %1 (profile %2)</source>
        <translation>已產生執行設定：%1（訂閱 %2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="279"/>
        <source>Request timed out</source>
        <translation>請求超時</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="281"/>
        <source>The release has no update manifest</source>
        <translation>這個版本沒有附帶更新清單</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="341"/>
        <source>No update manifest address available</source>
        <translation>沒有可用的更新清單地址</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="354"/>
        <source>Could not parse the manifest: %1</source>
        <translation>更新清單解析失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="363"/>
        <source>The manifest has no usable version field</source>
        <translation>更新清單裡沒有可用的版本號</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="234"/>
        <source>System-proxy backend: %1</source>
        <translation>系統代理後端：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="258"/>
        <source>System-proxy backend: %1 (XDG_CURRENT_DESKTOP=%2)</source>
        <translation>系統代理後端：%1（XDG_CURRENT_DESKTOP=%2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="273"/>
        <source>unavailable</source>
        <translation>不可用</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="327"/>
        <source>Could not write ProxyServer to the registry</source>
        <translation>無法把 ProxyServer 寫入登錄檔</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="335"/>
        <source>Failed to write the system-proxy ignore list</source>
        <translation>寫入系統代理忽略列表失敗</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="339"/>
        <source>Could not enable the system proxy in the registry</source>
        <translation>無法在登錄檔中啟用系統代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="350"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="378"/>
        <source>System proxy enabled (%1): %2:%3</source>
        <translation>系統代理已啟用（%1）：%2:%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="356"/>
        <source>No usable system-proxy backend detected (gsettings or kwriteconfig5/6 is required)</source>
        <translation>未偵測到可用的系統代理後端（需要 gsettings 或 kwriteconfig5/6）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="372"/>
        <source>Failed to enable the system proxy: %1</source>
        <translation>啟用系統代理失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="387"/>
        <source>Could not turn off the system proxy in the registry</source>
        <translation>無法在登錄檔中關閉系統代理</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="398"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="423"/>
        <source>System proxy disabled</source>
        <translation>系統代理已關閉</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="402"/>
        <source>No usable system-proxy backend detected</source>
        <translation>未偵測到可用的系統代理後端</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="418"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>關閉系統代理失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="438"/>
        <source>Could not start %1</source>
        <translation>無法啟動 %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="443"/>
        <source>%1 timed out</source>
        <translation>%1 執行超時</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="448"/>
        <source>%1 failed: %2</source>
        <translation>%1 執行失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="515"/>
        <source>Failed to write the system-proxy ignore list: %1</source>
        <translation>寫入系統代理忽略列表失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="577"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="620"/>
        <source>kwriteconfig5/6 not found</source>
        <translation>未找到 kwriteconfig5/6</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="53"/>
        <source>Could not open the autostart registry key</source>
        <translation>無法開啟開機自啟動的登錄檔項</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="68"/>
        <source>Could not write the autostart entry (error %1)</source>
        <translation>無法寫入開機自啟動項（錯誤 %1）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="109"/>
        <source>Could not create the autostart directory</source>
        <translation>無法建立自啟動目錄</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="161"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="193"/>
        <source>Failed to enable autostart: %1</source>
        <translation>啟用自啟動失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="167"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="196"/>
        <source>Autostart enabled: %1</source>
        <translation>已啟用開機自啟動：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="215"/>
        <source>Could not remove the autostart entry (error %1)</source>
        <translation>無法移除開機自啟動項（錯誤 %1）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="224"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="235"/>
        <source>Autostart disabled</source>
        <translation>已關閉開機自啟動</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="231"/>
        <source>Could not remove the autostart entry: %1</source>
        <translation>無法刪除自啟動項：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SingleInstance.cpp" line="53"/>
        <source>The single-instance guard failed to start (%1); running as multiple instances this time: %2</source>
        <translation>單執行個體守衛啟動失敗（%1），本次將以多執行個體方式執行：%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="117"/>
        <source>Could not set the TUN adapter address</source>
        <translation>無法設定 TUN 介面卡地址</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="134"/>
        <source>Could not read the TUN adapter settings</source>
        <translation>無法讀取 TUN 介面卡設定</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="150"/>
        <source>Could not set the TUN adapter MTU/metric</source>
        <translation>無法設定 TUN 介面卡的 MTU / metric</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="175"/>
        <source>Could not add the TUN route</source>
        <translation>無法新增 TUN 路由</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="203"/>
        <source>Could not start netsh</source>
        <translation>無法啟動 netsh</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="208"/>
        <source>netsh timed out</source>
        <translation>netsh 執行超時</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="312"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="383"/>
        <source>TUN mode on Windows requires administrator privileges. Close the app and start it again with &quot;Run as administrator&quot;.</source>
        <translation>Windows 上的 TUN 模式需要管理員許可權。請關閉程式，以「以管理員身份執行」重新啟動。</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="321"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="513"/>
        <source>Helper not found: %1</source>
        <translation>找不到輔助程式：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="337"/>
        <source>Could not start pkexec: %1</source>
        <translation>無法啟動 pkexec：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="344"/>
        <source>Authorization timed out (the password prompt may have been cancelled)</source>
        <translation>授權超時（可能取消了密碼框）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="349"/>
        <source>%1 failed (exit code %2): %3</source>
        <translation>%1 失敗（結束碼 %2）：%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="359"/>
        <source>TUN permissions granted (chown root:root + chmod +sx): %1</source>
        <translation>已授權 TUN 許可權（chown root:root + chmod +sx）：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="369"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="509"/>
        <source>TUN is starting or already running</source>
        <translation>TUN 正在啟用或已在執行</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="389"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable and try again.</source>
        <translation>未找到 wintun.dll。請把 wintun.dll 放到可執行檔案同目錄後重試。</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="402"/>
        <source>Invalid TUN address: %1</source>
        <translation>TUN 地址無效：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="410"/>
        <source>Could not create the TUN adapter</source>
        <translation>無法建立 TUN 介面卡</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="415"/>
        <source>Could not read the TUN adapter LUID</source>
        <translation>無法讀取 TUN 介面卡的 LUID</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="448"/>
        <source>Invalid fake-ip CIDR: %1</source>
        <translation>fake-ip 網段無效：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="468"/>
        <source>Could not point system DNS at the TUN address: %1</source>
        <translation>無法把系統 DNS 指向 TUN 地址：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="481"/>
        <source>TUN adapter ready: %1 %2/%3, MTU %4, routes %5, Wintun %6</source>
        <translation>TUN 介面卡就緒：%1 %2/%3，MTU %4，路由 %5，Wintun %6</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="487"/>
        <source>taken over</source>
        <translation>已接管</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="488"/>
        <source>not taken over</source>
        <translation>未接管</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="505"/>
        <source>Found a stale TUN-enabled state; cleaned it up and retried</source>
        <translation>偵測到殘留的 TUN 啟用狀態，已自動清理後重試</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="517"/>
        <source>TUN permission not granted yet (enter the password in the prompt that appears)</source>
        <translation>尚未授權 TUN 許可權（需要先在彈出的視窗裡輸入密碼）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="542"/>
        <source>Socket path too long</source>
        <translation>socket 路徑過長</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="553"/>
        <source>Failed to listen on the socket: %1</source>
        <translation>監聽 socket 失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="583"/>
        <source>Could not start the helper: %1</source>
        <translation>無法啟動輔助程式：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="595"/>
        <source>Timed out waiting for the helper (20 s)</source>
        <translation>等待輔助程式超時（20 秒）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="611"/>
        <source>The helper exited unexpectedly (exit code %1)</source>
        <translation>輔助程式意外結束（結束碼 %1）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="616"/>
        <source>Started the TUN helper; waiting for the device: %1</source>
        <translation>已拉起 TUN 輔助程式，等待裝置就緒：%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="618"/>
        <source>TUN handshake details: socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</source>
        <translation>TUN 握手詳情：socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="700"/>
        <source>The TUN helper disconnected</source>
        <translation>TUN 輔助程式連線已斷開</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="749"/>
        <source>The helper did not send a TUN fd</source>
        <translation>輔助程式未傳來 TUN fd</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="760"/>
        <source>TUN ready: %1 (%2)</source>
        <translation>TUN 已就緒：%1（%2）</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="762"/>
        <source>real device</source>
        <translation>真實裝置</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="815"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="868"/>
        <source>TUN is closed</source>
        <translation>TUN 已關閉</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="778"/>
        <source>System tray unavailable; skipping tray setup</source>
        <translation>系統托盤不可用，跳過托盤初始化</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="767"/>
        <source>UI style: Fusion (%1) style=%2 field=%3 accent=%4 desktop dark=%5</source>
        <translation>介面樣式：Fusion（%1）style=%2 控制元件底色=%3 強調色=%4 桌面深色=%5</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>dark</source>
        <translation>深色</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>light</source>
        <translation>淺色</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>yes</source>
        <translation>是</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>no</source>
        <translation>否</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/Icons.cpp" line="53"/>
        <source>Icon resource missing: %1 (check resources/clash-qt.qrc)</source>
        <translation>圖示資源缺失：%1（檢查 resources/clash-qt.qrc）</translation>
    </message>
</context>
<context>
    <name>clashqt::core</name>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="141"/>
        <source>YAML parse failed: %1</source>
        <translation>YAML 解析失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="165"/>
        <source>YAML emit failed: %1</source>
        <translation>YAML 發射失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="312"/>
        <source>Unknown error</source>
        <translation>未知錯誤</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="549"/>
        <source>Stream closed</source>
        <translation>流已關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="566"/>
        <source>Write timed out (flow-control window stayed closed)</source>
        <translation>寫入超時（流控視窗未放開）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="570"/>
        <source>Session released</source>
        <translation>會話已釋放</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="627"/>
        <source>Session release</source>
        <translation>會話釋放</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="656"/>
        <source>Could not parse server address %1 (%2)</source>
        <translation>無法解析伺服器地址 %1（%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="676"/>
        <source>Failed to create the UDP socket: %1</source>
        <translation>建立 UDP 套接字失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="689"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="700"/>
        <source>UDP connect failed: %1</source>
        <translation>連線 UDP 失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="714"/>
        <source>Failed to create the QUIC context</source>
        <translation>建立 QUIC 上下文失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="728"/>
        <source>Failed to create the QUIC connection</source>
        <translation>建立 QUIC 連線物件失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="741"/>
        <source>Failed to bind the UDP socket</source>
        <translation>繫結 UDP 套接字失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="756"/>
        <source>Failed to read the peer address: %1</source>
        <translation>讀取對端地址失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="776"/>
        <source>Failed to build the initial QUIC peer address</source>
        <translation>構造 QUIC 初始對端地址失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="791"/>
        <source>QUIC handshake failed: %1</source>
        <translation>QUIC 握手失敗：%1</translation>
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
        <translation>QUIC 事件處理失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="810"/>
        <source>QUIC handshake timed out (port %1)</source>
        <translation>QUIC 握手超時（連接埠 %1）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="820"/>
        <source>The server provided no certificate</source>
        <translation>伺服器未提供證書</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="831"/>
        <source>Server certificate fingerprint does not match the config (fingerprint check failed)</source>
        <translation>伺服器證書指紋與設定不符（fingerprint 驗證失敗）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="852"/>
        <source>Failed to create a QUIC stream: %1</source>
        <translation>建立 QUIC 流失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="856"/>
        <source>Timed out creating a QUIC stream</source>
        <translation>建立 QUIC 流超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="876"/>
        <source>Failed to write to a QUIC stream: %1</source>
        <translation>寫入 QUIC 流失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="881"/>
        <source>Timed out writing to a QUIC stream</source>
        <translation>寫入 QUIC 流超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="971"/>
        <source>No response to authentication (timeout or connection reset, port %1)</source>
        <translation>鑑權無應答（超時或連線被重置，連接埠 %1）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="979"/>
        <source>Authentication rejected (HTTP status %1; check the password)</source>
        <translation>鑑權被拒絕（HTTP 狀態 %1，請檢查 password）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1008"/>
        <source>Node is missing server</source>
        <translation>節點缺少 server</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1012"/>
        <source>Node is missing password</source>
        <translation>節點缺少 password</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1019"/>
        <source>Socket setup</source>
        <translation>套接字準備</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1020"/>
        <source>Handshake</source>
        <translation>握手</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1023"/>
        <source>Authentication</source>
        <translation>鑑權</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1034"/>
        <source>Session unavailable</source>
        <translation>會話不可用</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1038"/>
        <source>Invalid target address</source>
        <translation>目標地址非法</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1106"/>
        <source>Target %1 did not respond (timeout)</source>
        <translation>目標 %1 無應答（超時）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1114"/>
        <source>Connection to target %1 refused</source>
        <translation>目標 %1 連線被拒絕</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1115"/>
        <source>Connection to target %1 refused: %2</source>
        <translation>目標 %1 連線被拒絕：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="29"/>
        <source>Read more than %1 bytes without finding the delimiter</source>
        <translation>讀取資料超過上限 %1 位元組且未找到分隔符</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="42"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="87"/>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="247"/>
        <source>Connection closed</source>
        <translation>連線已關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="48"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="93"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="329"/>
        <source>Read timed out</source>
        <translation>讀取超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="62"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="104"/>
        <source>Read failed: %1</source>
        <translation>讀取失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="126"/>
        <source>Write failed: %1</source>
        <translation>寫入失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="133"/>
        <source>Write timed out</source>
        <translation>寫入超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="145"/>
        <source>Connection closed by the peer</source>
        <translation>連線已被對端關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="461"/>
        <source>REALITY connection to %1:%2 failed: %3</source>
        <translation>REALITY 連線 %1:%2 失敗：%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="487"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="508"/>
        <source>X25519 key generation failed</source>
        <translation>X25519 金鑰產生失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="514"/>
        <source>REALITY static ECDH failed</source>
        <translation>REALITY 靜態 ECDH 失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="521"/>
        <source>Failed to build ClientHello</source>
        <translation>ClientHello 構造失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="552"/>
        <source>Failed to generate REALITY authentication data</source>
        <translation>REALITY 認證資料產生失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="571"/>
        <source>REALITY received no ServerHello (record type=%1 length=%2 content=%3)</source>
        <translation>REALITY 未收到 ServerHello（記錄類型=%1 長度=%2 內容=%3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="582"/>
        <source>Truncated ServerHello</source>
        <translation>ServerHello 截斷</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="618"/>
        <source>ServerHello contains no usable key_share</source>
        <translation>ServerHello 未包含可用的 key_share</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="624"/>
        <source>TLS negotiated: cipher=0x%1 key-exchange group=%2</source>
        <translation>TLS 協商：密碼套件=0x%1 金鑰交換組=%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="636"/>
        <source>The REALITY server negotiated an unsupported cipher 0x%1</source>
        <translation>REALITY 伺服器協商了不支援的套件 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="704"/>
        <source>Unexpected record type %1 during the REALITY handshake</source>
        <translation>REALITY 握手中出現意外記錄類型 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="713"/>
        <source>Failed to decrypt a REALITY handshake record (type=%1 length=%2 seq=%3)</source>
        <translation>REALITY 握手記錄解密失敗（記錄類型=%1 長度=%2 序號=%3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="717"/>
        <source>Failed to decrypt a REALITY handshake record</source>
        <translation>REALITY 握手記錄解密失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="768"/>
        <source>The server returned no certificate</source>
        <translation>伺服器未返回證書</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="799"/>
        <source>TLS certificate verification failed (chain=%1 hostname=%2)</source>
        <translation>TLS 證書驗證失敗（鏈=%1 主機名=%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="800"/>
        <source>Pass</source>
        <translation>透過</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="801"/>
        <source>Fail</source>
        <translation>不透過</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="802"/>
        <source>Match</source>
        <translation>匹配</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="803"/>
        <source>No match</source>
        <translation>不匹配</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="809"/>
        <source>Node %1 skips TLS certificate verification</source>
        <translation>節點 %1 跳過 TLS 證書驗證</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="818"/>
        <source>REALITY authentication failed (the peer is the borrowed site itself rather than a REALITY server, or public-key / short-id is misconfigured)</source>
        <translation>REALITY 認證失敗（對端是借用站點本身而非 REALITY 伺服器，或 public-key / short-id 設定錯誤）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="830"/>
        <source>Server Finished verification failed</source>
        <translation>伺服器 Finished 驗證失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="993"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1008"/>
        <source>REALITY send failed: %1</source>
        <translation>REALITY 傳送失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1022"/>
        <source>Timed out reading a REALITY record</source>
        <translation>REALITY 讀取記錄超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1028"/>
        <source>REALITY failed to read a record: %1</source>
        <translation>REALITY 讀取記錄失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1044"/>
        <source>Invalid REALITY record length %1</source>
        <translation>REALITY 記錄長度非法 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1079"/>
        <source>Invalid REALITY application-record length</source>
        <translation>REALITY 應用記錄長度非法</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1099"/>
        <source>Failed to decrypt a REALITY record</source>
        <translation>REALITY 記錄解密失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1251"/>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="390"/>
        <source>Timed out reading the response</source>
        <translation>讀取應答超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1256"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="324"/>
        <source>Connection closed while reading</source>
        <translation>連線在讀取時關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="73"/>
        <source>Failed to write the upgrade request: %1</source>
        <translation>寫入升級請求失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="77"/>
        <source>WS upgrade request: sent %1 bytes to %2:%3 (%4 pending, state %5)</source>
        <translation>WS 升級請求已傳送 %1 位元組到 %2:%3（待寫 %4，狀態 %5）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="87"/>
        <source>%1 (received %2 bytes: %3)</source>
        <translation>%1（已收到 %2 位元組：%3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="101"/>
        <source>WebSocket handshake rejected: %1
%2</source>
        <translation>WebSocket 握手被拒絕：%1
%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="116"/>
        <source>WebSocket handshake verification failed (Sec-WebSocket-Accept mismatch)</source>
        <translation>WebSocket 握手驗證失敗（Sec-WebSocket-Accept 不匹配）</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="129"/>
        <source>The WebSocket handshake response carried extra data, which is not supported</source>
        <translation>WebSocket 握手響應中夾帶了資料，目前不支援該形態</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="227"/>
        <source>The peer sent a WebSocket close frame</source>
        <translation>對端傳送了 WebSocket 關閉幀</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="236"/>
        <source>Fragmented WebSocket messages are not supported</source>
        <translation>收到分片的 WebSocket 訊息，目前不支援</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="262"/>
        <source>Timed out reading a WebSocket frame</source>
        <translation>讀取 WebSocket 幀超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/config/RawConfig.cpp" line="81"/>
        <source>The config root must be a mapping</source>
        <translation>設定根節點必須是對映（mapping）</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/cidr/IpCidrSet.cpp" line="12"/>
        <source>Ignoring an invalid CIDR rule entry: %1</source>
        <translation>忽略非法的 CIDR 規則項：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="68"/>
        <source>DNS listener not started: %1</source>
        <translation>DNS 監聽未開啟：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="77"/>
        <source>Not listening</source>
        <translation>未監聽</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="86"/>
        <source>dns.listen is not configured</source>
        <translation>未設定 dns.listen</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="95"/>
        <source>Failed to bind UDP %1:%2: %3</source>
        <translation>UDP %1:%2 繫結失敗：%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="139"/>
        <source>DNS listener started: %1 (enhanced-mode=%2)</source>
        <translation>DNS 監聽已開啟：%1（enhanced-mode=%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="199"/>
        <source>DNS fake-ip: AAAA query for %1 returned an empty answer</source>
        <translation>DNS fake-ip：%1 的 AAAA 查詢返回空應答</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="222"/>
        <source>DNS real resolution: %1 -&gt; %2 record(s)</source>
        <translation>DNS 真實解析：%1 -&gt; %2 條記錄</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="285"/>
        <source>Invalid or non-IPv4 fake-ip-range: %1; falling back to 198.18.0.1/16</source>
        <translation>非法或非 IPv4 的 fake-ip-range：%1，回退到 198.18.0.1/16</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="545"/>
        <source>DNS configured: enhanced-mode=%1, nameserver=%2, fake-ip=%3</source>
        <translation>DNS 已設定：enhanced-mode=%1，nameserver=%2，fake-ip=%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="915"/>
        <source>DoT (tls://) currently connects over plain TCP to %1</source>
        <translation>DoT（tls://）暫以 TCP 方式連線 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="979"/>
        <source>Could not resolve the DNS upstream host through the bootstrap server: %1</source>
        <translation>無法透過引導伺服器解析 DNS 上游域名：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1004"/>
        <source>Could not parse the DNS server config: %1</source>
        <translation>無法解析 DNS 伺服器設定：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1045"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="487"/>
        <source>DNS resolution failed: %1</source>
        <translation>域名解析失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="92"/>
        <source>Malformed logical-rule sub-item: %1</source>
        <translation>邏輯規則子項格式不正確：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="103"/>
        <source>Failed to parse a logical-rule sub-item: %1 (%2)</source>
        <translation>邏輯規則子項解析失敗：%1（%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="119"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="125"/>
        <source>Empty rule</source>
        <translation>規則為空</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="133"/>
        <source>Unknown rule type: %1</source>
        <translation>未知規則類型：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="141"/>
        <source>MATCH rule is missing the target policy</source>
        <translation>MATCH 規則缺少目標策略</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="149"/>
        <source>Rule has too few fields (expected type,value,policy): %1</source>
        <translation>規則欄位不足（至少需要 類型,匹配值,策略）：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="197"/>
        <source>Invalid domain regular expression: %1</source>
        <translation>非法的域名正則：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="213"/>
        <source>Invalid IP CIDR: %1</source>
        <translation>非法的 IP 段：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="225"/>
        <source>Invalid source IP CIDR: %1</source>
        <translation>非法的源 IP 段：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="251"/>
        <source>Invalid port: %1</source>
        <translation>非法連接埠：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="261"/>
        <source>Invalid source port: %1</source>
        <translation>非法源連接埠：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="271"/>
        <source>Invalid inbound port: %1</source>
        <translation>非法入站連接埠：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="288"/>
        <source>Invalid process-match expression: %1</source>
        <translation>非法的處理程序匹配表示式：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="298"/>
        <source>Invalid UID: %1</source>
        <translation>非法的 UID：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="310"/>
        <source>References an undefined rule set: %1</source>
        <translation>引用了未定義的規則集：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="325"/>
        <source>Logical rule has no usable sub-rules: %1</source>
        <translation>邏輯規則沒有可用的子規則：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="340"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="346"/>
        <source>Unsupported rule type: %1</source>
        <translation>暫不支援的規則類型：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="352"/>
        <source>Failed to build the rule: %1</source>
        <translation>規則構建失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="369"/>
        <source>Skipped a non-string rule entry</source>
        <translation>規則項不是字串，已跳過</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="420"/>
        <source>Could not parse an entry in rule set %1: %2</source>
        <translation>規則集 %1 中的條目無法解析：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="93"/>
        <source>Failed to connect to %1:%2: %3 (error=%4)</source>
        <translation>連線 %1:%2 失敗：%3（error=%4）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="177"/>
        <source>Invalid DIRECT target address</source>
        <translation>DIRECT 目標地址無效</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="186"/>
        <source>DIRECT UDP requires a literal IP target</source>
        <translation>DIRECT UDP 需要字面 IP 目標</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="191"/>
        <source>UDP bind failed: %1</source>
        <translation>UDP 繫結失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="224"/>
        <source>Connection rejected by a rule (REJECT)</source>
        <translation>連線被規則拒絕（REJECT）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="242"/>
        <source>HTTP proxy %1 claims https; connecting with plaintext CONNECT for now</source>
        <translation>HTTP 代理 %1 宣告為 https，目前以明文 CONNECT 方式連線</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="250"/>
        <source>HTTP proxy %1 is missing server/port</source>
        <translation>HTTP 代理 %1 缺少 server/port</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="261"/>
        <source>The HTTP proxy does not support UDP forwarding</source>
        <translation>HTTP 代理不支援 UDP 轉發</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="286"/>
        <source>HTTP proxy %1 did not respond, or its response headers are too long</source>
        <translation>HTTP 代理 %1 無響應或響應頭超長</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="300"/>
        <source>HTTP proxy returned %1</source>
        <translation>HTTP 代理返回 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="335"/>
        <source>SOCKS5 proxy %1 is missing server/port</source>
        <translation>SOCKS5 代理 %1 缺少 server/port</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="356"/>
        <source>No response to SOCKS5 negotiation</source>
        <translation>SOCKS5 協商無響應</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="362"/>
        <source>SOCKS5 version mismatch</source>
        <translation>SOCKS5 版本不匹配</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="368"/>
        <source>The SOCKS5 server rejected all authentication methods</source>
        <translation>SOCKS5 伺服器拒絕了所有認證方式</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="377"/>
        <source>SOCKS5 username or password too long</source>
        <translation>SOCKS5 使用者名稱或密碼過長</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="391"/>
        <source>No response to SOCKS5 authentication</source>
        <translation>SOCKS5 認證無響應</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="396"/>
        <source>SOCKS5 authentication failed</source>
        <translation>SOCKS5 認證失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="401"/>
        <source>The SOCKS5 server requires an unsupported auth method 0x%1</source>
        <translation>SOCKS5 伺服器要求不支援的認證方式 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="425"/>
        <source>SOCKS5 target domain name too long</source>
        <translation>SOCKS5 目標域名過長</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="442"/>
        <source>Could not resolve the SOCKS5 target address: %1</source>
        <translation>SOCKS5 目標地址無法解析：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="455"/>
        <source>No response to SOCKS5 CONNECT</source>
        <translation>SOCKS5 CONNECT 無響應</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="464"/>
        <source>Succeeded</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="465"/>
        <source>General SOCKS server failure</source>
        <translation>一般性失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="466"/>
        <source>Connection not allowed by ruleset</source>
        <translation>連線不被允許</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="467"/>
        <source>Network unreachable</source>
        <translation>網路不可達</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="468"/>
        <source>Host unreachable</source>
        <translation>主機不可達</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="469"/>
        <source>Connection refused</source>
        <translation>連線被拒絕</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="470"/>
        <source>TTL expired</source>
        <translation>TTL 過期</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="471"/>
        <source>Command not supported</source>
        <translation>不支援的命令</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="472"/>
        <source>Address type not supported</source>
        <translation>不支援的地址類型</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="475"/>
        <source>Unknown error %1</source>
        <translation>未知錯誤 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="477"/>
        <source>SOCKS5 connection refused: %1 (%2)</source>
        <translation>SOCKS5 連線被拒：%1（%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="491"/>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="503"/>
        <source>Truncated SOCKS5 response</source>
        <translation>SOCKS5 應答被截斷</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="498"/>
        <source>The SOCKS5 response contains an unknown address type %1</source>
        <translation>SOCKS5 應答包含未知地址類型 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="521"/>
        <source>SOCKS5 UDP ASSOCIATE forwarding is not implemented yet (Phase 2)</source>
        <translation>SOCKS5 UDP ASSOCIATE 轉發尚未實現（Phase 2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="560"/>
        <source>Node %1 (%2) is not supported yet: %3</source>
        <translation>節點 %1（%2）暫不支援：%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="632"/>
        <source>Group %1 switched to %2</source>
        <translation>組 %1 切換到 %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="645"/>
        <source>Proxy group %1 has no available members</source>
        <translation>代理組 %1 沒有可用成員</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="658"/>
        <source>Group %1 member %2 failed to connect: %3</source>
        <translation>組 %1 的成員 %2 連線失敗：%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="67"/>
        <source>This protocol is not implemented yet</source>
        <translation>該協議尚未實現</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="82"/>
        <source>Skipped non-mapping entries in proxies</source>
        <translation>proxies 中存在非對映條目，已跳過</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="91"/>
        <source>Unrecognized node: %1 (type=%2)</source>
        <translation>無法識別的節點：%1（type=%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="92"/>
        <source>&lt;unnamed&gt;</source>
        <translation>&lt;未命名&gt;</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="96"/>
        <source>Duplicate node name; ignoring the later definition: %1</source>
        <translation>節點名重複，後出現的定義被忽略：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="115"/>
        <source>Unrecognized proxy group: %1 (type=%2)</source>
        <translation>無法識別的代理組：%1（type=%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="154"/>
        <source>Proxy group %1 has no members</source>
        <translation>代理組 %1 沒有任何成員</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="168"/>
        <source>Proxy group %1 has members that cannot be resolved (a missing node or a reference cycle); skipped. Members: %2</source>
        <translation>代理組 %1 的成員無法解析（引用了不存在的節點，或存在迴圈引用），已跳過；成員：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="180"/>
        <source>Adapters loaded: %1 nodes/groups, %2 unimplemented protocols, %3 warnings</source>
        <translation>介面卡載入完成：節點/組共 %1 個，未實現協議 %2 個，告警 %3 條</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="197"/>
        <source>The config defines its own GLOBAL group; using it as-is</source>
        <translation>設定裡已定義 GLOBAL 組，直接沿用</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="231"/>
        <source>Built the built-in GLOBAL group with %1 member(s)</source>
        <translation>已合成內建 GLOBAL 組，共 %1 個成員</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="234"/>
        <source>The profile has no usable node; the GLOBAL group only contains DIRECT</source>
        <translation>訂閱中沒有可用節點，GLOBAL 組只包含 DIRECT</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="52"/>
        <source>fingerprint is not a valid SHA-256 hex string</source>
        <translation>fingerprint 不是合法的 SHA-256 十六進位制串</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="59"/>
        <source>obfs=%1 is set (Salamander obfuscation is not implemented yet)</source>
        <translation>設定了 obfs=%1（Salamander 混淆尚未實現）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="63"/>
        <source>realm-opts is set (realm is not implemented yet)</source>
        <translation>設定了 realm-opts（realm 尚未實現）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="67"/>
        <source>Missing password</source>
        <translation>缺少 password</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="78"/>
        <source>Node %1 does not support UDP relay yet (Hysteria2 needs QUIC DATAGRAM, which OpenSSL 3.2 does not provide)</source>
        <translation>節點 %1 暫不支援 UDP 中繼（Hysteria2 需要 QUIC DATAGRAM，OpenSSL 3.2 未提供）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="84"/>
        <source>Node %1 is not supported yet: %2</source>
        <translation>節點 %1 暫不支援：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="92"/>
        <source>Node %1 has an invalid target address</source>
        <translation>節點 %1 的目標地址無效</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="133"/>
        <source>Node %1 failed to connect (attempt %2); retrying on another port: %3</source>
        <translation>節點 %1 第 %2 次連線失敗，換連接埠重試：%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="140"/>
        <source>Node %1 failed to connect: %2</source>
        <translation>節點 %1 連線失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="149"/>
        <source>Node %1 failed to establish the tunnel: %2</source>
        <translation>節點 %1 建立隧道失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="158"/>
        <source>Node %1: the server has UDP disabled; UDP traffic will not work</source>
        <translation>節點 %1：服務端未啟用 UDP，UDP 流量將不受支援</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="61"/>
        <source>Target domain name too long</source>
        <translation>目標域名過長</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="78"/>
        <source>Could not resolve the target address: %1</source>
        <translation>目標地址無法解析：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="121"/>
        <source>Node %1 is missing server/port</source>
        <translation>節點 %1 缺少 server/port</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="138"/>
        <source>Failed to connect to %1:%2: %3</source>
        <translation>連線 %1:%2 失敗：%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="157"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="442"/>
        <source>Node %1 is missing uuid</source>
        <translation>節點 %1 缺少 uuid</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="163"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="448"/>
        <source>Node %1 has an invalid uuid: %2</source>
        <translation>節點 %1 的 uuid 非法：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="176"/>
        <source>Node %1 uses an unsupported flow: %2</source>
        <translation>節點 %1 使用不支援的 flow：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="181"/>
        <source>Node %1 uses vision but has no reality-opts configured; this combination is not implemented yet</source>
        <translation>節點 %1 使用 vision 但未設定 reality-opts，該組合尚未實現</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="196"/>
        <source>Node %1 has an invalid reality-opts.public-key (it should decode to 32 bytes)</source>
        <translation>節點 %1 的 reality-opts.public-key 非法（解碼後應為 32 位元組）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="200"/>
        <source>Node %1 has a reality-opts.short-id that is too long</source>
        <translation>節點 %1 的 reality-opts.short-id 過長</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="208"/>
        <source>Node %1 REALITY handshake failed: %2</source>
        <translation>節點 %1 REALITY 握手失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="236"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="293"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="322"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="541"/>
        <source>Node %1 failed to send the VLESS request</source>
        <translation>節點 %1 VLESS 請求傳送失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="252"/>
        <source>Node %1 transport %2 is not implemented yet (ws, REALITY and plain TCP are supported)</source>
        <translation>節點 %1 的傳輸 %2 尚未實現（目前支援 ws、REALITY 與裸 TCP）</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="280"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="317"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="497"/>
        <source>Node %1 TLS connection failed: %2</source>
        <translation>節點 %1 TLS 建連失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="288"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="340"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="507"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="525"/>
        <source>Node %1 WebSocket handshake failed: %2</source>
        <translation>節點 %1 WebSocket 握手失敗：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="350"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="355"/>
        <source>Failed to send the VLESS request: %1</source>
        <translation>VLESS 請求傳送失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="373"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="397"/>
        <source>Failed to read the VLESS response: %1</source>
        <translation>VLESS 應答讀取失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="377"/>
        <source>Truncated VLESS response</source>
        <translation>VLESS 應答被截斷</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="382"/>
        <source>Truncated VLESS response addons</source>
        <translation>VLESS 應答 addons 被截斷</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="404"/>
        <source>Failed to read the VLESS response addons: %1</source>
        <translation>VLESS 應答 addons 讀取失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="454"/>
        <source>VLESS UDP is missing the target address</source>
        <translation>VLESS UDP 缺少目標地址</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="488"/>
        <source>Node %1 has an invalid reality-opts.public-key</source>
        <translation>節點 %1 的 reality-opts.public-key 非法</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="515"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="533"/>
        <source>Node %1 transport %2 does not support UDP</source>
        <translation>節點 %1 的傳輸 %2 尚不支援 UDP</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="54"/>
        <source>%1 has an invalid listen address: %2</source>
        <translation>%1 監聽地址無效：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="62"/>
        <source>%1 failed to listen on %2:%3: %4</source>
        <translation>%1 監聽 %2:%3 失敗：%4</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="76"/>
        <source>%1 inbound started: %2:%3 (concurrency limit %4)</source>
        <translation>%1 入站已啟動：%2:%3（併發上限 %4）</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="126"/>
        <source>%1 still has %2 open connection(s)</source>
        <translation>%1 仍有 %2 條連線未結束</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="154"/>
        <source>Concurrency limit %1 reached; rejecting new connections</source>
        <translation>併發連線已達上限 %1，拒絕新連線</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="182"/>
        <source>Failed to duplicate the connection descriptor; connection dropped</source>
        <translation>複製連線描述符失敗，連線已丟棄</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="216"/>
        <source>%1 accepted connection %2:%3</source>
        <translation>%1 接受連線 %2:%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="231"/>
        <source>Could not rebuild a socket from descriptor %1</source>
        <translation>無法從描述符 %1 重建套接字</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="61"/>
        <source>Missing HTTP request line</source>
        <translation>HTTP 請求行缺失</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="68"/>
        <source>Malformed HTTP request line: %1</source>
        <translation>HTTP 請求行格式錯誤：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="135"/>
        <source>HTTP inbound read failed: %1</source>
        <translation>HTTP 入站讀取失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="58"/>
        <source>Unsupported SOCKS version: %1</source>
        <translation>不支援的 SOCKS 版本：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="75"/>
        <source>The client does not support username/password authentication</source>
        <translation>客戶端不支援使用者名稱/密碼認證</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="81"/>
        <source>The client offered no acceptable authentication method</source>
        <translation>客戶端未提供可接受的認證方式</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="89"/>
        <source>Failed to write the SOCKS5 method selection</source>
        <translation>寫入 SOCKS5 方法選擇失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="100"/>
        <source>Unsupported username/password auth version</source>
        <translation>不支援的使用者名稱/密碼認證版本</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="125"/>
        <source>SOCKS5 authentication failed (user %1)</source>
        <translation>SOCKS5 認證失敗（使用者名稱 %1）</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="142"/>
        <source>Wrong SOCKS5 request version</source>
        <translation>SOCKS5 請求版本錯誤</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="152"/>
        <source>Unsupported SOCKS5 command 0x%1</source>
        <translation>不支援的 SOCKS5 命令 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="180"/>
        <source>Unsupported SOCKS5 address type 0x%1</source>
        <translation>SOCKS5 不支援的地址類型 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="219"/>
        <source>SOCKS5 negotiation failed: %1</source>
        <translation>SOCKS5 協商失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>SOCKS5 request failed: %1</source>
        <translation>SOCKS5 請求失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>Invalid target</source>
        <translation>目標無效</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/MixedListener.cpp" line="37"/>
        <source>MIXED inbound timed out while sniffing the protocol</source>
        <translation>MIXED 入站嗅探協議超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="26"/>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="24"/>
        <source>No dialer implementation available</source>
        <translation>沒有可用的撥號實現</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="32"/>
        <source>Dial failed</source>
        <translation>撥號失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="71"/>
        <source>UDP socket closed</source>
        <translation>UDP 套接字已關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="160"/>
        <source>UDP tunnel closed</source>
        <translation>UDP 隧道已關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="30"/>
        <source>UDP dial failed</source>
        <translation>UDP 撥號失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="97"/>
        <source>Could not resolve the TUN adapter from its LUID</source>
        <translation>無法根據 LUID 找到 TUN 介面卡</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="116"/>
        <source>Could not get the Wintun read event</source>
        <translation>無法獲取 Wintun 的讀取事件</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="161"/>
        <source>The Wintun session was closed</source>
        <translation>Wintun 會話已關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="199"/>
        <source>DNS is not taken over on the TUN address (%1): %2</source>
        <translation>TUN 地址上的 DNS 未被接管（%1）：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="282"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="427"/>
        <source>Invalid TUN adapter</source>
        <translation>TUN 介面卡無效</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="287"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="432"/>
        <source>Invalid TUN device fd</source>
        <translation>無效的 TUN 裝置 fd</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="306"/>
        <source>Failed to send a packet to the TUN adapter</source>
        <translation>向 TUN 介面卡傳送資料包失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="314"/>
        <source>Failed to write to the TUN device: %1</source>
        <translation>寫 TUN 裝置失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="342"/>
        <source>Could not open the TUN adapter</source>
        <translation>無法開啟 TUN 介面卡</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="349"/>
        <source>TUN data plane ready (worker thread %1, Wintun %2)</source>
        <translation>TUN 資料面就緒（工作執行緒 %1，Wintun %2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="382"/>
        <source>TUN data plane ready (worker thread %1, DNS 53 %2)</source>
        <translation>TUN 資料面已就緒（工作執行緒 %1，DNS 53 %2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Taken over</source>
        <translation>已接管</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Not taken over</source>
        <translation>未接管</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="412"/>
        <source>Failed to read the TUN device: %1</source>
        <translation>讀 TUN 裝置失敗：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="414"/>
        <source>Failed to read the TUN device</source>
        <translation>TUN 裝置讀取失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="185"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="193"/>
        <source>Idle timeout</source>
        <translation>空閒超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="216"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="224"/>
        <source>Closing TUN</source>
        <translation>關閉 TUN</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="248"/>
        <source>Re-entered TUN packet handling; dropped this packet to avoid concurrent flow-table writes</source>
        <translation>TUN 收包重入，本次丟棄（避免流表被併發改寫）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="268"/>
        <source>Dropped a non-IPv4 packet from TUN (IPv6 is not supported yet)</source>
        <translation>TUN 收到非 IPv4 包，已丟棄（IPv6 尚未支援）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="321"/>
        <source>Dropped an IP fragment from TUN (no reassembly)</source>
        <translation>TUN 收到 IP 分片，未做重組，已丟棄</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="421"/>
        <source>TUN: dropped link-local/multicast UDP to %1</source>
        <translation>TUN：本地鏈路/組播 UDP %1 已丟棄</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="535"/>
        <source>TUN: answered ICMP echo locally (%1, fake-ip)</source>
        <translation>TUN：ICMP echo 由本機應答（%1，fake-ip）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="565"/>
        <source>TUN TCP segment %1:%2 -&gt; %3 flags=0x%4 seq=%5 len=%6 known flows=%7</source>
        <translation>TUN TCP 段 %1:%2 → %3 標誌=0x%4 序號=%5 長度=%6 已知流=%7</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="590"/>
        <source>TUN: TCP port 53 is not supported yet; replying RST so the client falls back to UDP</source>
        <translation>TUN：TCP 53 連接埠暫不支援，回 RST 促其回落到 UDP</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="640"/>
        <source>Client RST</source>
        <translation>客戶端 RST</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="668"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1227"/>
        <source>Client closed and upstream already disconnected</source>
        <translation>客戶端關閉且上游已斷</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="674"/>
        <source>Four-way handshake complete</source>
        <translation>四次揮手完成</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="778"/>
        <source>TUN: UDP target failed recently; replying with ICMP: %1</source>
        <translation>TUN UDP 目標近期失敗過，直接回 ICMP：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="785"/>
        <source>TUN: UDP sessions exceed %1; rejecting new sessions</source>
        <translation>TUN UDP 會話數超過 %1，拒絕新會話</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="869"/>
        <source>TUN UDP session established: %1:%2 -&gt; %3</source>
        <translation>TUN UDP 會話建立：%1:%2 → %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="899"/>
        <source>TUN UDP dial failed for %1: %2</source>
        <translation>TUN UDP 撥號失敗 %1：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="928"/>
        <source>TUN UDP channel closed: %1</source>
        <translation>TUN UDP 通道關閉：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="930"/>
        <source>Upstream closed</source>
        <translation>上游關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="950"/>
        <source>TUN closed UDP session %1:%2 (%3)</source>
        <translation>TUN 關閉 UDP 會話 %1:%2（%3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1044"/>
        <source>TUN: target failed recently; sending RST: %1</source>
        <translation>TUN 目標近期失敗過，直接 RST：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1098"/>
        <source>TUN: concurrent connections exceed %1; rejecting new connections</source>
        <translation>TUN 併發連線超過 %1，拒絕新連線</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1149"/>
        <source>TUN new connection %1:%2 -&gt; %3:%4</source>
        <translation>TUN 新連線 %1:%2 → %3:%4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1179"/>
        <source>TUN dial failed for %1:%2: %3</source>
        <translation>TUN 撥號失敗 %1:%2：%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1204"/>
        <source>TUN upstream data: %1 bytes (%2 pending)</source>
        <translation>TUN 上游資料 %1 位元組（待發 %2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1336"/>
        <source>TUN closed connection %1:%2 (%3)</source>
        <translation>TUN 關閉連線 %1:%2（%3）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="61"/>
        <source>Connection %1: client disconnected (up %2 / down %3 bytes)</source>
        <translation>連線 %1：客戶端斷開（上行 %2 / 下行 %3 位元組）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="70"/>
        <source>Connection %1: upstream closed (up %2 / down %3 bytes) %4</source>
        <translation>連線 %1：上游關閉（上行 %2 / 下行 %3 位元組）%4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="111"/>
        <source>Connection %1: short upstream write, %2 / %3 bytes</source>
        <translation>連線 %1：上游寫入不完整，%2 / %3 位元組</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="224"/>
        <source>Config not loaded yet</source>
        <translation>尚未載入設定</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="232"/>
        <source>GLOBAL mode is on, but the config has no GLOBAL group</source>
        <translation>GLOBAL 模式已啟用，但設定中不存在 GLOBAL 組</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="266"/>
        <source>No MATCH fallback rule in the rule list; treating as DIRECT</source>
        <translation>規則列表中沒有 MATCH 兜底規則，已按直連處理</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="277"/>
        <source>Rule %1 points at a policy that does not exist: %2</source>
        <translation>規則 %1 指向了不存在的策略：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="311"/>
        <source>Closed an inbound connection with no target address</source>
        <translation>入站連線缺少目標地址，已關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="332"/>
        <source>Could not determine an outbound policy (%1 -&gt; %2); connection closed</source>
        <translation>無法確定出站策略（%1 -&gt; %2），連線已關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="333"/>
        <source>Unknown reason</source>
        <translation>未知原因</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="351"/>
        <source>The DIRECT outbound needs DNS resolution, but no resolver is available</source>
        <translation>DIRECT 出站需要 DNS 解析，但解析器不可用</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="360"/>
        <source>DNS resolution failed; connection closed: %1</source>
        <translation>域名解析失敗，連線已關閉：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="381"/>
        <source>Rule %1 dropped the connection: %2</source>
        <translation>規則 %1 丟棄連線：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="394"/>
        <source>Outbound %1 failed to connect to %2:%3: %4</source>
        <translation>出站 %1 連線 %2:%3 失敗：%4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="404"/>
        <source>UDP relay is not implemented yet; connection closed</source>
        <translation>UDP 中繼尚未實現，連線已關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="430"/>
        <source>%1 -&gt; %2:%3 via %4 (rule %5)</source>
        <translation>%1 -&gt; %2:%3 經由 %4（規則 %5）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="309"/>
        <source>Vision: the server echoed an unknown UUID</source>
        <translation>Vision 服務端回顯了未知 UUID</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="338"/>
        <source>Vision: read an unknown command %1</source>
        <translation>Vision 讀取到未知命令 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="385"/>
        <source>Connection closed while reading the response</source>
        <translation>連線在讀取應答時關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="88"/>
        <source>Failed to send the WebSocket upgrade request</source>
        <translation>WebSocket 升級請求傳送失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="104"/>
        <source>Connection closed during the WebSocket upgrade (received %1 bytes: %2)</source>
        <translation>WebSocket 升級時連線關閉（已收到 %1 位元組：%2）</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="113"/>
        <source>WebSocket upgrade handshake timed out</source>
        <translation>WebSocket 升級握手超時</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="127"/>
        <source>WebSocket handshake rejected: %1</source>
        <translation>WebSocket 握手被拒絕：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="154"/>
        <source>Server-sent WebSocket frames must not be masked</source>
        <translation>WebSocket 服務端幀帶掩碼</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="179"/>
        <source>WebSocket frame too long</source>
        <translation>WebSocket 幀過長</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="127"/>
        <source>DNS override is off; using system resolution</source>
        <translation>DNS 覆寫已關閉，使用系統解析</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="152"/>
        <source>Rule set %1 has an invalid behavior: %2</source>
        <translation>規則集 %1 的 behavior 非法：%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="178"/>
        <source>Could not read the rule-set file: %1</source>
        <translation>無法讀取規則集檔案：%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="181"/>
        <source>Remote rule sets (type=http) are not implemented yet; skipped</source>
        <translation>遠端規則集（type=http）尚未實現，已跳過</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="207"/>
        <source>Config hot-reloaded (inbound listeners kept)</source>
        <translation>設定已熱更新（不重新啟動入站監聽）</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="248"/>
        <source>Mixed port %1 failed to start</source>
        <translation>混合連接埠 %1 啟動失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="265"/>
        <source>SOCKS5 port %1 failed to start</source>
        <translation>SOCKS5 連接埠 %1 啟動失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="280"/>
        <source>HTTP port %1 failed to start</source>
        <translation>HTTP 連接埠 %1 啟動失敗</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="290"/>
        <source>redir-port / tproxy-port transparent proxying is not implemented yet (Phase 2); those ports are not listening</source>
        <translation>redir-port / tproxy-port 透明代理尚未實現（Phase 2），對應連接埠未監聽</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="295"/>
        <source>No inbound port is enabled in the config (mixed-port / socks-port / port are all 0)</source>
        <translation>設定中沒有任何啟用的入站連接埠（mixed-port / socks-port / port 均為 0）</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="335"/>
        <source>No config has been applied yet</source>
        <translation>尚未應用任何設定</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="349"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="350"/>
        <source>Started</source>
        <translation>已啟動</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="367"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="384"/>
        <source>Stopped</source>
        <translation>已停止</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="403"/>
        <source>Outbound mode switched to %1</source>
        <translation>出站模式切換為 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="437"/>
        <source>TUN is already running</source>
        <translation>TUN 已在執行</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="447"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="593"/>
        <source>TUN is closed</source>
        <translation>TUN 已關閉</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="456"/>
        <source>No usable outbound matched</source>
        <translation>沒有匹配到可用出站</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="530"/>
        <source>TUN data-plane error (%1); TUN stopped</source>
        <translation>TUN 資料面異常（%1），已停止 TUN</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="545"/>
        <source>TUN taken over (device=%1); data plane running on a worker thread</source>
        <translation>TUN 已接管（裝置 %1），資料面執行在工作執行緒</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="586"/>
        <source>The TUN data-plane thread is still winding down; it will be released after exit</source>
        <translation>TUN 資料面執行緒仍在收尾，結束後自動釋放</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="633"/>
        <source>Group %1 does not exist</source>
        <translation>組 %1 不存在</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="638"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="766"/>
        <source>%1 is not a proxy group</source>
        <translation>%1 不是代理組</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="642"/>
        <source>Group %1 has no member %2</source>
        <translation>組 %1 中不存在成員 %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="678"/>
        <source>Group %1 references member %2, which no longer exists; ignored</source>
        <translation>組 %1 儲存的成員 %2 已不存在，已忽略</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="688"/>
        <source>Restored manual selections for %1 proxy group(s)</source>
        <translation>已恢復 %1 個代理組的手動選擇</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="703"/>
        <source>Node does not exist: %1</source>
        <translation>節點不存在：%1</translation>
    </message>
</context>
</TS>
