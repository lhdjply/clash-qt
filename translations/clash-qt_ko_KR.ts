<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ko_KR">
<context>
    <name>clashqt::AppContext</name>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="188"/>
        <source>Failed to turn off the system proxy on exit: %1 (the desktop still points at the local port)</source>
        <translation>종료 시 시스템 프록시를 해제하지 못했습니다: %1 (바탕화면이 여전히 로컬 포트를 가리킵니다)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="193"/>
        <source>System proxy turned off before exit, so the desktop stops pointing at a stopped local port</source>
        <translation>종료 전에 시스템 프록시를 해제하여 바탕화면이 중지된 로컬 포트를 가리키지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="208"/>
        <source>Failed to load the config. Check that the files in %1 are writable</source>
        <translation>설정을 불러오지 못했습니다. %1의 파일이 쓰기 가능한지 확인하세요</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="224"/>
        <source>Data directory: %1 (portable: %2)</source>
        <translation>데이터 디렉터리: %1 (포터블: %2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="225"/>
        <source>yes</source>
        <translation>예</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="226"/>
        <source>no</source>
        <translation>아니요</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="239"/>
        <source>Config generation failed: %1</source>
        <translation>설정 생성 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="268"/>
        <source>Start failed: %1</source>
        <translation>시작 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="269"/>
        <source>Startup failed: %1</source>
        <translation>시작 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="304"/>
        <source>Kernel failed to restart after config reload: %1</source>
        <translation>설정을 다시 불러온 후 커널을 재시작하지 못했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="384"/>
        <location filename="../src/app/src/AppContext.cpp" line="411"/>
        <source>Config not loaded yet</source>
        <translation>설정이 아직 로드되지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="423"/>
        <source>TUN enable request: enabled=%1 running=%2 starting(old)=%3</source>
        <translation>TUN 활성화 요청: enabled=%1 running=%2 starting(old)=%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="434"/>
        <source>TUN disabled; system routes restored</source>
        <translation>TUN이 비활성화되어 시스템 경로를 복원했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="446"/>
        <source>TUN helper not found (it should sit next to the main executable): %1</source>
        <translation>TUN 헬퍼를 찾을 수 없습니다 (실행 파일 옆에 있어야 합니다): %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="485"/>
        <source>Removed the wildcard &quot;*&quot; from fake-ip-filter (it makes every domain skip fake-ip)</source>
        <translation>fake-ip-filter에서 와일드카드 &amp;quot;*&amp;quot;를 제거했습니다 (모든 도메인이 fake-ip를 건너뛰게 됩니다)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="487"/>
        <source>Fixed the DNS config: the &quot;*&quot; in fake-ip-filter made every domain skip fake-ip, so it was removed</source>
        <translation>DNS 설정을 수정했습니다: fake-ip-filter의 &amp;quot;*&amp;quot; 때문에 모든 도메인이 fake-ip를 건너뛰어 제거했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="501"/>
        <source>TUN device name changed from Mihomo to %1</source>
        <translation>TUN 장치 이름이 Mihomo에서 %1(으)로 변경되었습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="515"/>
        <source>TUN mode needs kernel DNS (fake-ip); DNS override was enabled automatically</source>
        <translation>TUN 모드에는 커널 DNS(fake-ip)가 필요하여 DNS 재정의를 자동으로 활성화했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="517"/>
        <source>TUN mode needs kernel DNS; &quot;DNS override&quot; was enabled automatically</source>
        <translation>TUN 모드에는 커널 DNS가 필요하여 &amp;quot;DNS 재정의&amp;quot;를 자동으로 활성화했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="532"/>
        <source>The kernel failed to start, so TUN cannot be enabled</source>
        <translation>커널이 시작되지 않아 TUN을 활성화할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="555"/>
        <location filename="../src/app/src/AppContext.cpp" line="607"/>
        <source>Failed to enable TUN: %1</source>
        <translation>TUN 활성화 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="576"/>
        <source>Enabling TUN timed out; state was force-reset</source>
        <translation>TUN 활성화 시간이 초과되어 상태를 강제 초기화했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="579"/>
        <source>Timed out enabling TUN; state was reset, please try again</source>
        <translation>TUN 활성화 시간이 초과되어 상태를 초기화했습니다. 다시 시도하세요</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="583"/>
        <source>Enabling TUN…</source>
        <translation>TUN 활성화 중…</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="608"/>
        <source>Could not take over the device</source>
        <translation>장치를 점유할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="620"/>
        <source>TUN enabled (system routes taken over)</source>
        <translation>TUN 활성화됨 (시스템 경로 점유)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="621"/>
        <source>TUN enabled (routes not taken over)</source>
        <translation>TUN 활성화됨 (경로 미점유)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="630"/>
        <source>Kernel is not running; skipping switch-state restore</source>
        <translation>커널이 실행 중이 아니므로 스위치 상태 복원을 건너뜁니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="640"/>
        <source>Restored TUN to its previous state</source>
        <translation>TUN을 이전 상태로 복원했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="643"/>
        <source>Failed to restore TUN: %1</source>
        <translation>TUN 복원 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="645"/>
        <source>Failed to restore TUN mode: %1</source>
        <translation>TUN 모드 복원 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="654"/>
        <source>Restored the system proxy to its previous state</source>
        <translation>시스템 프록시를 이전 상태로 복원했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="657"/>
        <location filename="../src/app/src/AppContext.cpp" line="659"/>
        <source>Failed to restore the system proxy: %1</source>
        <translation>시스템 프록시 복원 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="674"/>
        <source>Start failed; system proxy not enabled: %1</source>
        <translation>시작에 실패하여 시스템 프록시를 활성화하지 않았습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="686"/>
        <source>No inbound port available; cannot set the system proxy</source>
        <translation>사용 가능한 인바운드 포트가 없어 시스템 프록시를 설정할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="696"/>
        <source>127.0.0.1:%1 is not listening locally; refusing to enable the system proxy (the whole desktop would lose connectivity by pointing at an empty port)</source>
        <translation>127.0.0.1:%1이(가) 로컬에서 수신 중이 아니어서 시스템 프록시 활성화를 거부합니다 (빈 포트를 가리키면 바탕화면 전체가 인터넷에 연결되지 않습니다)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="708"/>
        <source>Failed to set the system proxy: %1</source>
        <translation>시스템 프록시 설정 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="716"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>시스템 프록시 해제 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="749"/>
        <source>Unknown outbound mode: %1</source>
        <translation>알 수 없는 아웃바운드 모드: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="788"/>
        <location filename="../src/app/src/AppContext.cpp" line="813"/>
        <location filename="../src/app/src/AppContext.cpp" line="901"/>
        <location filename="../src/app/src/AppContext.cpp" line="979"/>
        <source>Failed to write the profile contents</source>
        <translation>프로필 내용을 쓰지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="793"/>
        <location filename="../src/app/src/AppContext.cpp" line="905"/>
        <source>Failed to write the profile list</source>
        <translation>프로필 목록을 쓰지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="797"/>
        <source>Added local config: %1</source>
        <translation>로컬 설정을 추가했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="805"/>
        <location filename="../src/app/src/AppContext.cpp" line="917"/>
        <location filename="../src/app/src/AppContext.cpp" line="1020"/>
        <source>Profile does not exist: %1</source>
        <translation>프로필이 존재하지 않습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="819"/>
        <source>Replaced the config file of &quot;%1&quot;</source>
        <translation>&amp;quot;%1&amp;quot;의 설정 파일을 교체했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="831"/>
        <source>Invalid profile URL: %1</source>
        <translation>잘못된 프로필 URL: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="858"/>
        <source>Profile download timed out (%1 ms)</source>
        <translation>프로필 다운로드 시간 초과 (%1 ms)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="865"/>
        <source>Profile download failed: %1</source>
        <translation>프로필 다운로드 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="884"/>
        <source>The profile is not a valid Clash config: %1</source>
        <translation>프로필이 올바른 Clash 설정이 아닙니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="909"/>
        <source>Added profile: %1</source>
        <translation>프로필을 추가했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="921"/>
        <source>Profile &quot;%1&quot; is not a remote profile; nothing to update</source>
        <translation>프로필 &amp;quot;%1&amp;quot;은(는) 원격 프로필이 아니어서 업데이트할 내용이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="948"/>
        <source>Updating &quot;%1&quot; timed out</source>
        <translation>&amp;quot;%1&amp;quot; 업데이트 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="954"/>
        <source>Failed to update &quot;%1&quot;: %2</source>
        <translation>&amp;quot;%1&amp;quot; 업데이트 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="972"/>
        <source>The update for &quot;%1&quot; returned invalid content; the old config was kept: %2</source>
        <translation>&amp;quot;%1&amp;quot; 업데이트가 잘못된 내용을 반환하여 이전 설정을 유지했습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="993"/>
        <source>Profile updated: %1</source>
        <translation>프로필 업데이트됨: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1025"/>
        <source>Switched to profile: %1</source>
        <translation>프로필 전환됨: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1040"/>
        <source>Override content must be a YAML mapping: %1</source>
        <translation>재정의 내용은 YAML 매핑이어야 합니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1047"/>
        <source>Unnamed override</source>
        <translation>이름 없는 재정의</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1054"/>
        <source>Failed to write the override contents</source>
        <translation>재정의 내용을 쓰지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1058"/>
        <source>Failed to write the override list</source>
        <translation>재정의 목록을 쓰지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1062"/>
        <source>Added override: %1</source>
        <translation>재정의를 추가했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1093"/>
        <source>Failed to parse the rule-override file; treating it as empty: %1</source>
        <translation>규칙 재정의 파일을 파싱하지 못해 빈 것으로 처리합니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1117"/>
        <source>No profile selected; cannot save custom rules</source>
        <translation>선택된 프로필이 없어 사용자 규칙을 저장할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1129"/>
        <source>%1 entry %2 contains a line break (each rule must stay on one line)</source>
        <translation>%1 항목 %2에 줄바꿈이 있습니다 (각 규칙은 한 줄이어야 합니다)</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1137"/>
        <source>%1 entry %2 is not a valid rule (expected at least &quot;type,target&quot;): %3</source>
        <translation>%1 항목 %2이(가) 올바른 규칙이 아닙니다 (최소 &amp;quot;type,target&amp;quot; 필요): %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1147"/>
        <source>Custom rules</source>
        <translation>사용자 규칙</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1148"/>
        <source>Fallback rules</source>
        <translation>폴백 규칙</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1149"/>
        <source>Removal rules</source>
        <translation>제거 규칙</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1182"/>
        <source>Failed to write custom rules: %1</source>
        <translation>사용자 규칙을 쓰지 못했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1187"/>
        <source>Custom rules saved (profile %1): %2 total, %3 disabled, %4 fallback / %5 removal</source>
        <translation>사용자 규칙 저장됨 (프로필 %1): 총 %2개, 비활성 %3개, 폴백 %4개 / 제거 %5개</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="1196"/>
        <source>Custom rules applied</source>
        <translation>사용자 규칙 적용됨</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionListModel</name>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="67"/>
        <source>Rule: %1</source>
        <translation>규칙: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="68"/>
        <source>Outbound: %1</source>
        <translation>아웃바운드: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/ConnectionList.cpp" line="69"/>
        <source>Up %1 · Down %2 · %3</source>
        <translation>업로드 %1 · 다운로드 %2 · %3</translation>
    </message>
</context>
<context>
    <name>clashqt::ConnectionsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="63"/>
        <source>Connections</source>
        <translation>연결</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="64"/>
        <source>Active connections, one card each. Double-click a card, or use the × on its right, to close that connection.</source>
        <translation>활성 연결을 카드 하나씩 표시합니다. 카드를 더블클릭하거나 오른쪽 ×를 눌러 해당 연결을 닫으세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="73"/>
        <source>Auto refresh (1 s)</source>
        <translation>자동 새로고침 (1초)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="77"/>
        <source>Close selected</source>
        <translation>선택 항목 닫기</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="78"/>
        <source>Close all</source>
        <translation>모두 닫기</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="104"/>
        <source>No active connections</source>
        <translation>활성 연결 없음</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="145"/>
        <source>just now</source>
        <translation>방금</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="146"/>
        <source>%1 min ago</source>
        <translation>%1분 전</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="147"/>
        <source>%1 h ago</source>
        <translation>%1시간 전</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="148"/>
        <source>%1 d ago</source>
        <translation>%1일 전</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="157"/>
        <source>%1 active connection(s) · Total up %2 · Total down %3</source>
        <translation>활성 연결 %1개 · 총 업로드 %2 · 총 다운로드 %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="239"/>
        <source>Select a connection first</source>
        <translation>먼저 연결을 선택하세요</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ConnectionsPage.cpp" line="253"/>
        <source>Closed %1 connection(s)</source>
        <translation>연결 %1개를 닫았습니다</translation>
    </message>
</context>
<context>
    <name>clashqt::LogsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="48"/>
        <source>Logs</source>
        <translation>로그</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="49"/>
        <source>Application log. Logs are also written to the logs/ folder in the data directory.</source>
        <translation>애플리케이션 로그입니다. 로그는 데이터 디렉터리의 logs/ 폴더에도 기록됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="54"/>
        <source>Level</source>
        <translation>레벨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="61"/>
        <source>Pause scrolling</source>
        <translation>스크롤 일시정지</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/LogsPage.cpp" line="64"/>
        <source>Clear view</source>
        <translation>보기 지우기</translation>
    </message>
</context>
<context>
    <name>clashqt::MainWindow</name>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="236"/>
        <source>Rule</source>
        <translation>규칙</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="237"/>
        <source>Global</source>
        <translation>전역</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="238"/>
        <source>Direct</source>
        <translation>직접</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="272"/>
        <source>System proxy</source>
        <translation>시스템 프록시</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="282"/>
        <source>No system proxy backend found (gsettings / kwriteconfig); system proxy is unavailable</source>
        <translation>시스템 프록시 백엔드를 찾을 수 없습니다 (gsettings / kwriteconfig). 시스템 프록시를 사용할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="288"/>
        <source>TUN mode</source>
        <translation>TUN 모드</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="295"/>
        <source>Captures all local traffic, including apps that ignore the system proxy. The first enable requires a one-time password prompt.</source>
        <translation>시스템 프록시를 무시하는 앱까지 모든 로컬 트래픽을 캡처합니다. 처음 활성화할 때 비밀번호를 한 번 입력해야 합니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="343"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="708"/>
        <source>Update this subscription</source>
        <translation>이 구독 업데이트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="412"/>
        <source>Proxies</source>
        <translation>프록시</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="414"/>
        <source>Rules</source>
        <translation>규칙</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="416"/>
        <source>Overrides</source>
        <translation>재정의</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="418"/>
        <source>Logs</source>
        <translation>로그</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="420"/>
        <source>Connections</source>
        <translation>연결</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="423"/>
        <source>Settings</source>
        <translation>설정</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="484"/>
        <source>Traffic</source>
        <translation>트래픽</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="694"/>
        <source>No profile selected</source>
        <translation>선택된 프로필 없음</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="709"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="757"/>
        <source>Local profiles have no URL to update from</source>
        <translation>로컬 프로필은 업데이트할 URL이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="721"/>
        <source>%1 / %2</source>
        <translation>%1 / %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="730"/>
        <source>Updated %1</source>
        <translation>%1 업데이트됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="745"/>
        <source>Open the profiles page</source>
        <translation>프로필 페이지 열기</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="752"/>
        <source>Select a profile first</source>
        <translation>먼저 프로필을 선택하세요</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="765"/>
        <source>Update failed: %1</source>
        <translation>업데이트 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="767"/>
        <source>Subscription updated</source>
        <translation>구독 업데이트됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="792"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="871"/>
        <source>Disable system proxy</source>
        <translation>시스템 프록시 해제</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="800"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="907"/>
        <source>Enable TUN mode</source>
        <translation>TUN 모드 활성화</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="807"/>
        <source>Show main window</source>
        <translation>메인 창 표시</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="815"/>
        <source>Quit</source>
        <translation>종료</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="872"/>
        <source>Enable system proxy</source>
        <translation>시스템 프록시 활성화</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="877"/>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="905"/>
        <source>Enabling TUN…</source>
        <translation>TUN 활성화 중…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="881"/>
        <source>System proxy + TUN enabled</source>
        <translation>시스템 프록시 + TUN 활성화됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="882"/>
        <source>TUN enabled</source>
        <translation>TUN 활성화됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="883"/>
        <source>System proxy enabled</source>
        <translation>시스템 프록시 활성화됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="884"/>
        <source>System proxy disabled</source>
        <translation>시스템 프록시 해제됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="900"/>
        <source>enabling…</source>
        <translation>활성화 중…</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="906"/>
        <source>Disable TUN mode</source>
        <translation>TUN 모드 비활성화</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="916"/>
        <source>Mixed port %1</source>
        <translation>혼합 포트 %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="918"/>
        <source>Not running</source>
        <translation>실행 중 아님</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="946"/>
        <source>Failed to switch outbound mode: %1</source>
        <translation>아웃바운드 모드 전환 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="952"/>
        <source>Outbound mode: %1</source>
        <translation>아웃바운드 모드: %1</translation>
    </message>
</context>
<context>
    <name>clashqt::OverridePage</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>Domain suffix (incl. subdomains)</source>
        <translation>도메인 접미사 (하위 도메인 포함)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="114"/>
        <source>e.g. example.com</source>
        <translation>예: example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>Domain (exact)</source>
        <translation>도메인 (정확히 일치)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="115"/>
        <source>e.g. www.example.com</source>
        <translation>예: www.example.com</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>Domain keyword</source>
        <translation>도메인 키워드</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="116"/>
        <source>e.g. google</source>
        <translation>예: google</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>Domain regex</source>
        <translation>도메인 정규식</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="117"/>
        <source>e.g. ^ads\..*</source>
        <translation>예: ^ads\..*</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>Geosite category</source>
        <translation>Geosite 카테고리</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="118"/>
        <source>e.g. cn / netflix / telegram</source>
        <translation>예: cn / netflix / telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>IP country</source>
        <translation>IP 국가</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="119"/>
        <source>e.g. CN / US / JP</source>
        <translation>예: CN / US / JP</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>IPv4 CIDR</source>
        <translation>IPv4 CIDR</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="120"/>
        <source>e.g. 10.0.0.0/8</source>
        <translation>예: 10.0.0.0/8</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>IPv6 CIDR</source>
        <translation>IPv6 CIDR</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="121"/>
        <source>e.g. 2001:db8::/32</source>
        <translation>예: 2001:db8::/32</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>ASN</source>
        <translation>ASN</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="122"/>
        <source>e.g. 13335</source>
        <translation>예: 13335</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>Destination port</source>
        <translation>대상 포트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="123"/>
        <source>e.g. 443</source>
        <translation>예: 443</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>Source port</source>
        <translation>소스 포트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="124"/>
        <source>e.g. 1234</source>
        <translation>예: 1234</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>Network type</source>
        <translation>네트워크 유형</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="125"/>
        <source>tcp or udp</source>
        <translation>tcp 또는 udp</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>Process name</source>
        <translation>프로세스 이름</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="126"/>
        <source>e.g. telegram</source>
        <translation>예: telegram</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>Process path</source>
        <translation>프로세스 경로</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="127"/>
        <source>e.g. /usr/bin/curl</source>
        <translation>예: /usr/bin/curl</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Rule set</source>
        <translation>규칙 집합</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="128"/>
        <source>Choose a rule set from the config</source>
        <translation>설정에서 규칙 집합 선택</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="129"/>
        <source>Catch-all (match all remaining traffic)</source>
        <translation>전체 매칭 (남은 모든 트래픽과 일치)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="405"/>
        <source>Overrides</source>
        <translation>재정의</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="406"/>
        <source>Adds custom rules on top of the profile&apos;s rules. Pick a type, content and target when adding — no rule syntax needed. The list order is the match priority; changes are saved and applied automatically.</source>
        <translation>프로필의 규칙 위에 사용자 규칙을 추가합니다. 추가할 때 유형, 내용, 대상을 고르면 되고 규칙 문법은 필요 없습니다. 목록 순서가 매칭 우선순위이며 변경 사항은 자동으로 저장·적용됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="408"/>
        <source>Custom rules</source>
        <translation>사용자 규칙</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="418"/>
        <source>New rule</source>
        <translation>새 규칙</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Enabled</source>
        <translation>활성화</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="430"/>
        <source>Rule type</source>
        <translation>규칙 유형</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Content</source>
        <translation>내용</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="431"/>
        <source>Target</source>
        <translation>대상</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="432"/>
        <source>Actions</source>
        <translation>작업</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="452"/>
        <source>Double-click any row to edit; uncheck it to disable (content is kept but not matched). Changes are saved and applied automatically.</source>
        <translation>행을 더블클릭하면 편집되고, 체크를 해제하면 비활성화됩니다 (내용은 유지되지만 매칭되지 않습니다). 변경 사항은 자동으로 저장·적용됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="526"/>
        <source>No profile selected; custom rules cannot be saved</source>
        <translation>선택된 프로필이 없어 사용자 규칙을 저장할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="527"/>
        <source>Applies to profile: %1 (rules are stored per profile and differ between profiles)</source>
        <translation>적용 프로필: %1 (규칙은 프로필별로 저장되어 프로필마다 다릅니다)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="553"/>
        <source>%1 rules, %2 enabled</source>
        <translation>규칙 %1개, 활성 %2개</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="575"/>
        <source>Enabled; uncheck to disable</source>
        <translation>활성화됨. 해제하면 비활성화됩니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="576"/>
        <source>Disabled: not matched after saving</source>
        <translation>비활성화됨: 저장 후 매칭되지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="601"/>
        <source>%1 (%2)
Rule: %3</source>
        <translation>%1 (%2)
규칙: %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="612"/>
        <source>Move up (higher priority)</source>
        <translation>위로 이동 (우선순위 높임)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="614"/>
        <source>Move down (lower priority)</source>
        <translation>아래로 이동 (우선순위 낮춤)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="615"/>
        <source>Edit this rule</source>
        <translation>이 규칙 편집</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="616"/>
        <source>Delete this rule</source>
        <translation>이 규칙 삭제</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="726"/>
        <source>Save failed</source>
        <translation>저장 실패</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="733"/>
        <source>Applied automatically: %1 rules, %2 disabled</source>
        <translation>자동 적용됨: 규칙 %1개, 비활성 %2개</translation>
    </message>
</context>
<context>
    <name>clashqt::ProfilesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="38"/>
        <source>Expired</source>
        <translation>만료됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="40"/>
        <source> (%1 days)</source>
        <translation> (%1일)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="235"/>
        <source>Profiles</source>
        <translation>프로필</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="236"/>
        <source>Manage config sources. The active profile is merged with &quot;Config overrides&quot; before use.</source>
        <translation>설정 소스를 관리합니다. 활성 프로필은 사용 전에 &amp;quot;설정 재정의&amp;quot;와 병합됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="247"/>
        <source>Add profile</source>
        <translation>프로필 추가</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="248"/>
        <source>Import local config</source>
        <translation>로컬 설정 가져오기</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="249"/>
        <source>Edit</source>
        <translation>편집</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="250"/>
        <source>Update selected</source>
        <translation>선택 항목 업데이트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="251"/>
        <source>Update all</source>
        <translation>모두 업데이트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="252"/>
        <source>Set active</source>
        <translation>활성으로 설정</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="253"/>
        <source>Delete</source>
        <translation>삭제</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="263"/>
        <source>Edit the name or URL (or double-click a row)</source>
        <translation>이름 또는 URL 편집 (또는 행 더블클릭)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="276"/>
        <source>No profiles yet. Click &quot;Add profile&quot; to enter a subscription URL, or &quot;Import local config&quot; to pick an existing Clash config file.</source>
        <translation>프로필이 없습니다. &amp;quot;프로필 추가&amp;quot;를 눌러 구독 URL을 입력하거나 &amp;quot;로컬 설정 가져오기&amp;quot;로 기존 Clash 설정 파일을 선택하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Active</source>
        <translation>활성</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="282"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Type</source>
        <translation>유형</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="283"/>
        <source>Traffic</source>
        <translation>트래픽</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Expires</source>
        <translation>만료</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="284"/>
        <source>Updated</source>
        <translation>업데이트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="341"/>
        <source>Remote</source>
        <translation>원격</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="342"/>
        <source>Local</source>
        <translation>로컬</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="343"/>
        <source>Plugin</source>
        <translation>플러그인</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="386"/>
        <source>Add failed</source>
        <translation>추가 실패</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="404"/>
        <source>Choose a config file</source>
        <translation>설정 파일 선택</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="405"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Clash 설정 (*.yaml *.yml);;모든 파일 (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="410"/>
        <source>Import failed</source>
        <translation>가져오기 실패</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="420"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="471"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="505"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="521"/>
        <source>Select a profile first</source>
        <translation>먼저 프로필을 선택하세요</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Save failed</source>
        <translation>저장 실패</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="438"/>
        <source>Failed to write profile config</source>
        <translation>프로필 설정을 쓰지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="446"/>
        <source>Replace failed</source>
        <translation>교체 실패</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="458"/>
        <source>URL saved</source>
        <translation>URL 저장됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="459"/>
        <source>URL saved, but fetching the new content failed:

%1

Try &quot;Update selected&quot; again later.</source>
        <translation>URL은 저장했지만 새 내용을 가져오지 못했습니다:

%1

나중에 &amp;quot;선택 항목 업데이트&amp;quot;를 다시 시도하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="477"/>
        <source>Update failed</source>
        <translation>업데이트 실패</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="489"/>
        <source>Some profiles failed to update</source>
        <translation>일부 프로필을 업데이트하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="490"/>
        <source>%1 succeeded, %2 failed:

%3</source>
        <translation>%1개 성공, %2개 실패:

%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="496"/>
        <source>Updated %1 profile(s)</source>
        <translation>프로필 %1개 업데이트됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="511"/>
        <source>Switch failed</source>
        <translation>전환 실패</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="527"/>
        <source>Delete profile</source>
        <translation>프로필 삭제</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="528"/>
        <source>Delete &quot;%1&quot;? Its config file will be removed as well.</source>
        <translation>&amp;quot;%1&amp;quot;을(를) 삭제할까요? 설정 파일도 함께 삭제됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Delete failed</source>
        <translation>삭제 실패</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="532"/>
        <source>Profile not found</source>
        <translation>프로필을 찾을 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="545"/>
        <source>Profile deleted, but reloading the config failed: %1</source>
        <translation>프로필은 삭제했지만 설정을 다시 불러오지 못했습니다: %1</translation>
    </message>
</context>
<context>
    <name>clashqt::ProxiesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="66"/>
        <source>Group</source>
        <translation>그룹</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="90"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="334"/>
        <source>Nodes</source>
        <translation>노드</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="110"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="149"/>
        <source>Testing</source>
        <translation>테스트 중</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="111"/>
        <source>Test</source>
        <translation>테스트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="112"/>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="151"/>
        <source>Timeout</source>
        <translation>시간 초과</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="291"/>
        <source>Proxies</source>
        <translation>프록시</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="292"/>
        <source>Pick a node from the proxy group: click a card to switch to it. The text on the right is the last measured latency; click it to test that node again, or click Test on a node that has not been measured yet. In global mode, the GLOBAL group decides the exit for all traffic.</source>
        <translation>프록시 그룹에서 노드를 고르세요: 카드를 클릭하면 해당 노드로 전환됩니다. 오른쪽 텍스트는 마지막으로 측정한 지연 시간이며, 클릭하면 해당 노드를 다시 테스트합니다. 아직 측정하지 않은 노드는 테스트를 클릭하세요. 전역 모드에서는 GLOBAL 그룹이 모든 트래픽의 출구를 결정합니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="298"/>
        <source>Proxy group</source>
        <translation>프록시 그룹</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="309"/>
        <source>Sort</source>
        <translation>정렬</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="311"/>
        <source>Default order</source>
        <translation>기본 순서</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="312"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="313"/>
        <source>Type</source>
        <translation>유형</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="314"/>
        <source>Latency</source>
        <translation>지연 시간</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="316"/>
        <source>Sort nodes by name, protocol type or latest measured latency.
Latency sorts ascending; untested and timed-out nodes go last. Sorting never changes the selected node.</source>
        <translation>이름, 프로토콜 유형 또는 최근 측정된 지연 시간으로 노드를 정렬합니다.
지연 시간은 오름차순이며, 미측정 및 시간 초과 노드는 맨 뒤로 갑니다. 정렬은 선택된 노드를 바꾸지 않습니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="323"/>
        <source>Test this group</source>
        <translation>이 그룹 테스트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="324"/>
        <source>Test all groups</source>
        <translation>모든 그룹 테스트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="436"/>
        <source>The active profile has no proxy groups</source>
        <translation>활성 프로필에 프록시 그룹이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="469"/>
        <source>Type %1 · %2 members</source>
        <translation>유형 %1 · 멤버 %2개</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="472"/>
        <source>Current node: %1</source>
        <translation>현재 노드: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="501"/>
        <source>Proxy group · %1</source>
        <translation>프록시 그룹 · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="502"/>
        <source>Protocol · %1</source>
        <translation>프로토콜 · %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="503"/>
        <source>Unknown</source>
        <translation>알 수 없음</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="505"/>
        <source>This protocol is not supported yet; connections will fail explicitly</source>
        <translation>아직 지원하지 않는 프로토콜이라 연결이 명시적으로 실패합니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="511"/>
        <source>Last latency test timed out</source>
        <translation>마지막 지연 시간 테스트가 시간 초과되었습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="516"/>
        <source>Last latency %1 ms</source>
        <translation>마지막 지연 시간 %1 ms</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="517"/>
        <source>Last test timed out</source>
        <translation>마지막 테스트 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="522"/>
        <source>Currently pointing at %1</source>
        <translation>현재 %1을(를) 가리킴</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="524"/>
        <source>Click the text on the right of a card to test that node</source>
        <translation>카드 오른쪽 텍스트를 클릭하면 해당 노드를 테스트합니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="741"/>
        <source>Node no longer exists</source>
        <translation>노드가 더 이상 존재하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="772"/>
        <source>Latency test finished: %1/%2 nodes available</source>
        <translation>지연 시간 테스트 완료: %1/%2개 노드 사용 가능</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="776"/>
        <source>%1 latency test failed: %2</source>
        <translation>%1 지연 시간 테스트 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="792"/>
        <source>These nodes are being tested, or none can be tested</source>
        <translation>이 노드들은 테스트 중이거나 테스트할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="805"/>
        <source>Testing %1 nodes (%2 at a time, results fill in as they finish)</source>
        <translation>%1개 노드 테스트 중 (동시 %2개, 완료되는 대로 결과 표시)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="835"/>
        <source>Failed to switch node: %1</source>
        <translation>노드 전환 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProxiesPage.cpp" line="843"/>
        <source>%1 switched to %2</source>
        <translation>%1이(가) %2(으)로 전환되었습니다</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="170"/>
        <source>New rule</source>
        <translation>새 규칙</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="171"/>
        <source>Edit rule</source>
        <translation>규칙 편집</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="180"/>
        <source>Follow the prompts; the rule syntax is assembled when you save.</source>
        <translation>안내에 따라 입력하세요. 규칙 문법은 저장할 때 조합됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="198"/>
        <source>%1 (cannot be edited structurally)</source>
        <translation>%1 (구조적으로 편집할 수 없음)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="201"/>
        <source>Rule type</source>
        <translation>규칙 유형</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="217"/>
        <source>Content</source>
        <translation>내용</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="223"/>
        <source>Choose a policy or proxy group</source>
        <translation>정책 또는 프록시 그룹 선택</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="224"/>
        <source>Target</source>
        <translation>대상</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="227"/>
        <source>Default</source>
        <translation>기본값</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="228"/>
        <source>Do not resolve domain (no-resolve)</source>
        <translation>도메인 확인 안 함 (no-resolve)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="229"/>
        <source>Match source address (src)</source>
        <translation>소스 주소 매칭 (src)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="230"/>
        <source>Options</source>
        <translation>옵션</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="253"/>
        <source>Cancel</source>
        <translation>취소</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="254"/>
        <source>Save</source>
        <translation>저장</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="302"/>
        <source>No rule-providers in the config</source>
        <translation>설정에 rule-providers가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="303"/>
        <source>Choose a rule set</source>
        <translation>규칙 집합 선택</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="333"/>
        <source>A catch-all rule matches all remaining traffic; keep it last among your custom rules.</source>
        <translation>전체 매칭 규칙은 남은 모든 트래픽과 일치하므로 사용자 규칙의 맨 뒤에 두세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="335"/>
        <source>example.com also matches all of its subdomains.</source>
        <translation>example.com은 모든 하위 도메인도 함께 매칭합니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="337"/>
        <source>For LAN traffic, set Options to no-resolve.</source>
        <translation>LAN 트래픽의 경우 옵션을 no-resolve로 설정하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="339"/>
        <source>Rule sets come from rule-providers in the profile.</source>
        <translation>규칙 집합은 프로필의 rule-providers에서 가져옵니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="362"/>
        <source>Choose what handles this rule (policy / proxy group).</source>
        <translation>이 규칙을 처리할 대상(정책 / 프록시 그룹)을 선택하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="367"/>
        <source>Enter the content to match; it is what the rule checks.</source>
        <translation>매칭할 내용을 입력하세요. 규칙이 검사하는 값입니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/OverridePage.cpp" line="372"/>
        <source>IPv4 CIDR needs a prefix length, e.g. 10.0.0.0/8.</source>
        <translation>IPv4 CIDR에는 프리픽스 길이가 필요합니다. 예: 10.0.0.0/8.</translation>
    </message>
</context>
<context>
    <name>clashqt::RuleRowDelegate</name>
    <message>
        <location filename="../src/app/src/ui/RuleList.cpp" line="156"/>
        <source>%1 hits</source>
        <translation>%1회 일치</translation>
    </message>
</context>
<context>
    <name>clashqt::RulesPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="43"/>
        <source>Rules</source>
        <translation>규칙</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="44"/>
        <source>Rules in match order, one card each. Turn the switch on the right off to disable a rule for this session; it is restored on restart.</source>
        <translation>매칭 순서대로 규칙을 카드 하나씩 표시합니다. 오른쪽 스위치를 끄면 이 세션에서만 규칙이 비활성화되며 재시작하면 복원됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="64"/>
        <source>No rules loaded</source>
        <translation>불러온 규칙 없음</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="90"/>
        <source>Failed to update rule state</source>
        <translation>규칙 상태 업데이트 실패</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="132"/>
        <source>%1 hits in this session</source>
        <translation>이 세션에서 %1회 일치</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/RulesPage.cpp" line="133"/>
        <source>Disabled until restart</source>
        <translation>재시작 전까지 비활성화됨</translation>
    </message>
</context>
<context>
    <name>clashqt::SettingsPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="42"/>
        <source>Settings</source>
        <translation>설정</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="43"/>
        <source>Logs, DNS overrides, ports and system integration. Changes are saved and applied immediately.</source>
        <translation>로그, DNS 재정의, 포트 및 시스템 통합 설정입니다. 변경 사항은 즉시 저장·적용됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="46"/>
        <source>Appearance</source>
        <translation>모양</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="50"/>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="63"/>
        <source>Follow system</source>
        <translation>시스템 설정 따르기</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="51"/>
        <source>Light</source>
        <translation>라이트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="52"/>
        <source>Dark</source>
        <translation>다크</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="58"/>
        <source>Theme</source>
        <translation>테마</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="68"/>
        <source>Language</source>
        <translation>언어</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="69"/>
        <source>Interface text is loaded at startup; restart the app after switching.</source>
        <translation>인터페이스 텍스트는 시작 시 로드됩니다. 변경 후 앱을 재시작하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="74"/>
        <source>Logs</source>
        <translation>로그</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="81"/>
        <source>Log level</source>
        <translation>로그 레벨</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="86"/>
        <source> days</source>
        <translation>일</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="89"/>
        <source>Log retention (days)</source>
        <translation>로그 보관 기간 (일)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="90"/>
        <source>Log files older than the retention period are removed at startup.</source>
        <translation>보관 기간이 지난 로그 파일은 시작 시 삭제됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="93"/>
        <source>Open data folder</source>
        <translation>데이터 폴더 열기</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="102"/>
        <source>Config overrides</source>
        <translation>설정 재정의</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="105"/>
        <source>Manage DNS settings in this app</source>
        <translation>이 앱에서 DNS 설정 관리</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="108"/>
        <source>When on, the DNS section configured here is used; when off, the profile&apos;s own DNS settings apply.</source>
        <translation>켜면 여기서 설정한 DNS 섹션이 사용되고, 끄면 프로필 자체의 DNS 설정이 적용됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="111"/>
        <source>Manage domain sniffing in this app</source>
        <translation>이 앱에서 도메인 스니핑 관리</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="117"/>
        <source>Inbound ports</source>
        <translation>인바운드 포트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="131"/>
        <source>Mixed port (HTTP + SOCKS5)</source>
        <translation>혼합 포트 (HTTP + SOCKS5)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="133"/>
        <source>SOCKS5 port</source>
        <translation>SOCKS5 포트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="134"/>
        <source>Set it equal to the mixed port to reuse the mixed port.</source>
        <translation>혼합 포트를 재사용하려면 혼합 포트와 같게 설정하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="136"/>
        <source>HTTP port</source>
        <translation>HTTP 포트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="141"/>
        <source>System integration</source>
        <translation>시스템 통합</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="144"/>
        <source>Start at login</source>
        <translation>로그인 시 자동 시작</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="146"/>
        <source>Implemented via an XDG autostart entry (~/.config/autostart/clash-qt.desktop).</source>
        <translation>XDG 자동 시작 항목(~/.config/autostart/clash-qt.desktop)으로 구현됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="149"/>
        <source>System proxy</source>
        <translation>시스템 프록시</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="152"/>
        <source>Points the system HTTP/HTTPS/SOCKS proxy at this app&apos;s mixed port. Current backend: %1</source>
        <translation>시스템 HTTP/HTTPS/SOCKS 프록시를 이 앱의 혼합 포트로 지정합니다. 현재 백엔드: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="156"/>
        <source>TUN mode</source>
        <translation>TUN 모드</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="159"/>
        <source>Creates a virtual network interface to capture all traffic, including apps that ignore the system proxy (games, CLI tools, some Electron apps). The first enable asks for your password once; it only takes effect together with &quot;Route domain traffic&quot;.</source>
        <translation>시스템 프록시를 무시하는 앱(게임, CLI 도구, 일부 Electron 앱)까지 모든 트래픽을 캡처하는 가상 네트워크 인터페이스를 만듭니다. 처음 활성화할 때 비밀번호를 한 번 묻습니다. &amp;quot;도메인 트래픽 라우팅&amp;quot;과 함께 켜야 동작합니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="162"/>
        <source>Route domain traffic (fake-IP)</source>
        <translation>도메인 트래픽 라우팅 (fake-IP)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="165"/>
        <source>Routes the fake-IP range to the TUN interface (removed together with the interface). The default route is untouched, so connectivity is unaffected; with this off, TUN receives no traffic.</source>
        <translation>fake-IP 대역을 TUN 인터페이스로 라우팅합니다 (인터페이스와 함께 제거됩니다). 기본 경로는 건드리지 않아 연결에 영향이 없으며, 끄면 TUN으로 트래픽이 들어오지 않습니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="171"/>
        <source>About</source>
        <translation>정보</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="177"/>
        <source>Check for updates when the app starts</source>
        <translation>앱 시작 시 업데이트 확인</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="187"/>
        <source>Version v%1</source>
        <translation>버전 v%1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="192"/>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="481"/>
        <source>Check for updates</source>
        <translation>업데이트 확인</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="198"/>
        <source>Reads the update manifest attached to the latest release, fetched through public GitHub mirrors. When an update exists, its changelog is shown and the release page can be opened in your browser; downloading and installing is up to you.</source>
        <translation>최신 릴리스에 첨부된 업데이트 매니페스트를 공개 GitHub 미러를 통해 읽습니다. 업데이트가 있으면 변경 내역이 표시되고 브라우저에서 릴리스 페이지를 열 수 있습니다. 다운로드와 설치는 직접 진행하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="252"/>
        <source>Already up to date (v%1)</source>
        <translation>최신 상태입니다 (v%1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="260"/>
        <source> (the kernel is not running; start the proxy and try again)</source>
        <translation> (커널이 실행 중이 아닙니다. 프록시를 시작한 후 다시 시도하세요)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="262"/>
        <source>Update check failed: %1%2</source>
        <translation>업데이트 확인 실패: %1%2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="300"/>
        <source>Data folder: %1</source>
        <translation>데이터 폴더: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="359"/>
        <source>Reload failed after port change: %1</source>
        <translation>포트 변경 후 다시 불러오기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="372"/>
        <source>Failed to change startup setting: %1</source>
        <translation>시작 설정 변경 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="456"/>
        <source>Language saved; restart the app to apply.</source>
        <translation>언어가 저장되었습니다. 적용하려면 앱을 재시작하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/SettingsPage.cpp" line="481"/>
        <source>Checking…</source>
        <translation>확인 중…</translation>
    </message>
</context>
<context>
    <name>clashqt::SubscriptionDialog</name>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Add profile</source>
        <translation>프로필 추가</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="74"/>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="83"/>
        <source>Edit profile</source>
        <translation>프로필 편집</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="84"/>
        <source>Enter the subscription URL; it will be downloaded and activated immediately.</source>
        <translation>구독 URL을 입력하면 즉시 다운로드되어 활성화됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="86"/>
        <source>Change the name or subscription URL. If this is the active profile, a changed URL is fetched again immediately.</source>
        <translation>이름 또는 구독 URL을 변경합니다. 활성 프로필인 경우 URL이 변경되면 즉시 다시 가져옵니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="87"/>
        <source>A local profile uses an imported config file, which you can replace below.</source>
        <translation>로컬 프로필은 가져온 설정 파일을 사용하며, 아래에서 교체할 수 있습니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="90"/>
        <source>Profile info</source>
        <translation>프로필 정보</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="97"/>
        <source>Optional; defaults to the subscription&apos;s domain</source>
        <translation>선택 사항이며, 기본값은 구독 도메인입니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="99"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="108"/>
        <source>Subscription URL</source>
        <translation>구독 URL</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="110"/>
        <source>Update this profile on startup</source>
        <translation>시작 시 이 프로필 업데이트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="115"/>
        <source>When enabled, the profile is refreshed automatically at startup.</source>
        <translation>활성화하면 시작 시 프로필이 자동으로 갱신됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="127"/>
        <source>Choose another file</source>
        <translation>다른 파일 선택</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="132"/>
        <source>Config file</source>
        <translation>설정 파일</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="133"/>
        <source>Picking a new file replaces this profile&apos;s content; its id and active state are preserved.</source>
        <translation>새 파일을 선택하면 이 프로필의 내용이 교체되며, id와 활성 상태는 유지됩니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="138"/>
        <source>Choose a config file</source>
        <translation>설정 파일 선택</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="139"/>
        <source>Clash config (*.yaml *.yml);;All files (*)</source>
        <translation>Clash 설정 (*.yaml *.yml);;모든 파일 (*)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="153"/>
        <source>Cancel</source>
        <translation>취소</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Add</source>
        <translation>추가</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="156"/>
        <source>Save</source>
        <translation>저장</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="193"/>
        <source>Name required</source>
        <translation>이름이 필요합니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="194"/>
        <source>Give this profile a name.</source>
        <translation>이 프로필에 이름을 지정하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="203"/>
        <source>Invalid subscription URL</source>
        <translation>잘못된 구독 URL</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="205"/>
        <source>Enter a subscription URL starting with http/https.</source>
        <translation>http/https로 시작하는 구독 URL을 입력하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/ProfilesPage.cpp" line="206"/>
        <source>The subscription URL must start with http/https.</source>
        <translation>구독 URL은 http/https로 시작해야 합니다.</translation>
    </message>
</context>
<context>
    <name>clashqt::TrafficPage</name>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="63"/>
        <source>Traffic</source>
        <translation>트래픽</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="64"/>
        <source>Live upload/download speed and cumulative traffic; the chart shows 1-second samples from the last 2 minutes.</source>
        <translation>실시간 업로드/다운로드 속도와 누적 트래픽입니다. 차트는 최근 2분간 1초 단위 샘플을 표시합니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="67"/>
        <source>Status</source>
        <translation>상태</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="72"/>
        <source>Total 0 B / 0 B</source>
        <translation>총 0 B / 0 B</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="73"/>
        <source>0 active connections</source>
        <translation>활성 연결 0개</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="82"/>
        <source>Speed chart</source>
        <translation>속도 차트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="96"/>
        <source>s</source>
        <translation>초</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="188"/>
        <source>Session total ↑ %1 · ↓ %2</source>
        <translation>세션 총계 ↑ %1 · ↓ %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="193"/>
        <source>%1 active connection(s) · Memory %2</source>
        <translation>활성 연결 %1개 · 메모리 %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="242"/>
        <source>Upload (%1)</source>
        <translation>업로드 (%1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/pages/TrafficPage.cpp" line="243"/>
        <source>Download (%1)</source>
        <translation>다운로드 (%1)</translation>
    </message>
</context>
<context>
    <name>clashqt::UpdateDialog</name>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="20"/>
        <source>Update available</source>
        <translation>업데이트 있음</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="30"/>
        <source>Version %1 is available</source>
        <translation>버전 %1을(를) 사용할 수 있습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="31"/>
        <source>Version v%1. The changelog comes from the GitHub release; downloading and installing is up to you — this app never replaces itself.</source>
        <translation>버전 v%1. 변경 내역은 GitHub 릴리스에서 가져옵니다. 다운로드와 설치는 직접 진행하세요. 이 앱은 스스로를 교체하지 않습니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="51"/>
        <source>(No release notes for this version; see the release page.)</source>
        <translation>(이 버전의 릴리스 노트가 없습니다. 릴리스 페이지를 확인하세요.)</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="97"/>
        <source>No package has been attached to this release yet; it may still be uploading.</source>
        <translation>이 릴리스에는 아직 패키지가 첨부되지 않았습니다. 업로드 중일 수 있습니다.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="100"/>
        <source>Published on %1. Download the package from the release page.</source>
        <translation>%1에 공개되었습니다. 릴리스 페이지에서 패키지를 다운로드하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="103"/>
        <source>Download the package from the release page.</source>
        <translation>릴리스 페이지에서 패키지를 다운로드하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="112"/>
        <source>Later</source>
        <translation>나중에</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UpdateDialog.cpp" line="113"/>
        <source>Open download page</source>
        <translation>다운로드 페이지 열기</translation>
    </message>
</context>
<context>
    <name>clashqt::app</name>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="130"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable.</source>
        <translation>wintun.dll을 찾을 수 없습니다. 실행 파일 옆에 wintun.dll을 두세요.</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="145"/>
        <source>Could not create the Wintun adapter</source>
        <translation>Wintun 어댑터를 만들 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="175"/>
        <source>Wintun is unavailable</source>
        <translation>Wintun을 사용할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Wintun.cpp" line="182"/>
        <source>Could not start the Wintun session</source>
        <translation>Wintun 세션을 시작할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="65"/>
        <source>Clash Qt - a desktop client for Clash-style subscriptions</source>
        <translation>Clash Qt - Clash 방식 구독을 위한 데스크톱 클라이언트</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="71"/>
        <source>Start without showing the main window (for autostart)</source>
        <translation>메인 창을 표시하지 않고 시작 (자동 시작용)</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="73"/>
        <source>Use the data directory next to the executable</source>
        <translation>실행 파일 옆의 데이터 디렉터리 사용</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="97"/>
        <source>Another instance is already running: asked it to show its window, exiting</source>
        <translation>다른 인스턴스가 이미 실행 중이라 창 표시를 요청하고 종료합니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="108"/>
        <source>Starting %1 %2 (%3, silent=%4)</source>
        <translation>%1 %2 시작 중 (%3, silent=%4)</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="121"/>
        <source>Initialization failed: %1</source>
        <translation>초기화 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="128"/>
        <source>UI language: %1 (configured value %2)</source>
        <translation>UI 언어: %1 (설정값 %2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="148"/>
        <source>Start failed: %1</source>
        <translation>시작 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="177"/>
        <source>Silent start: not showing the main window</source>
        <translation>자동 시작: 메인 창을 표시하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/main.cpp" line="188"/>
        <source>%1 exited with code %2</source>
        <translation>%1이(가) 코드 %2(으)로 종료되었습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="72"/>
        <source>Could not read file %1: %2</source>
        <translation>파일 %1을(를) 읽을 수 없습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/AppContext.cpp" line="82"/>
        <source>Not a valid Clash config: %1</source>
        <translation>올바른 Clash 설정이 아닙니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/AppLogger.cpp" line="152"/>
        <source>Removed %1 expired log file(s) (retention %2 days)</source>
        <translation>만료된 로그 파일 %1개를 삭제했습니다 (보관 %2일)</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="18"/>
        <source>Could not create the directory: %1</source>
        <translation>디렉터리를 만들 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="26"/>
        <source>Could not write %1: %2</source>
        <translation>%1을(를) 쓸 수 없습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="32"/>
        <source>Incomplete write to %1: %2</source>
        <translation>%1에 대한 쓰기가 완료되지 않았습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/SafeFile.cpp" line="38"/>
        <source>Failed to commit %1: %2</source>
        <translation>%1 커밋 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="70"/>
        <source>Used up</source>
        <translation>소진됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="77"/>
        <source>%1 d %2 h</source>
        <translation>%1일 %2시간</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="78"/>
        <source>%1 h %2 min</source>
        <translation>%1시간 %2분</translation>
    </message>
    <message>
        <location filename="../src/app/src/utils/Calc.cpp" line="79"/>
        <source>%1 min</source>
        <translation>%1분</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="208"/>
        <source>Failed to parse the app config; using defaults: %1</source>
        <translation>앱 설정을 파싱하지 못해 기본값을 사용합니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="228"/>
        <source>Failed to write back the app config: %1</source>
        <translation>앱 설정을 되돌려 쓰지 못했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/AppConfig.cpp" line="238"/>
        <source>Failed to save the app config: %1</source>
        <translation>앱 설정 저장 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="226"/>
        <source>Failed to parse the legacy managed config (%1): %2</source>
        <translation>레거시 관리 설정 파싱 실패 (%1): %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ControledKernelConfig.cpp" line="232"/>
        <source>Merged the legacy managed config into config.yaml and deleted it: %1</source>
        <translation>레거시 관리 설정을 config.yaml에 병합하고 삭제했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="134"/>
        <source>Failed to parse the profile list: %1</source>
        <translation>프로필 목록 파싱 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="157"/>
        <source>Failed to save the profile list: %1</source>
        <translation>프로필 목록 저장 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/ProfileConfig.cpp" line="242"/>
        <source>Failed to write profile content (%1): %2</source>
        <translation>프로필 내용 쓰기 실패 (%1): %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="74"/>
        <source>Failed to parse the override list: %1</source>
        <translation>재정의 목록 파싱 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="92"/>
        <source>Failed to save the override list: %1</source>
        <translation>재정의 목록 저장 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/OverrideConfig.cpp" line="156"/>
        <source>Failed to write override content (%1.%2): %3</source>
        <translation>재정의 내용 쓰기 실패 (%1.%2): %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="98"/>
        <source>References an override that does not exist: %1</source>
        <translation>존재하지 않는 재정의를 참조합니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="107"/>
        <source>Override %1 has no content file</source>
        <translation>재정의 %1에 내용 파일이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="116"/>
        <source>Override &quot;%1&quot; is a JS script, and the JS override engine is not supported yet; it was skipped (the config will not apply it)</source>
        <translation>재정의 &amp;quot;%1&amp;quot;은(는) JS 스크립트이며 JS 재정의 엔진은 아직 지원하지 않아 건너뛰었습니다 (설정에 적용되지 않습니다)</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="126"/>
        <source>Failed to parse override &quot;%1&quot;: %2</source>
        <translation>재정의 &amp;quot;%1&amp;quot; 파싱 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="133"/>
        <source>The root of override &quot;%1&quot; must be a mapping</source>
        <translation>재정의 &amp;quot;%1&amp;quot;의 최상위는 매핑이어야 합니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="159"/>
        <source>Failed to parse the rule-override file: %1</source>
        <translation>규칙 재정의 파일 파싱 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="228"/>
        <source>No profile selected yet. Add one on the Profiles page and select it first.</source>
        <translation>아직 선택된 프로필이 없습니다. 프로필 페이지에서 프로필을 추가하고 먼저 선택하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="239"/>
        <source>Profile &quot;%1&quot; is empty or missing</source>
        <translation>프로필 &amp;quot;%1&amp;quot;이(가) 비어 있거나 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="249"/>
        <source>Failed to parse the profile content: %1</source>
        <translation>프로필 내용 파싱 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="356"/>
        <source>Failed to write the config: %1</source>
        <translation>설정 쓰기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/config/Factory.cpp" line="361"/>
        <source>Generated the runtime config: %1 (profile %2)</source>
        <translation>런타임 설정을 생성했습니다: %1 (프로필 %2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="279"/>
        <source>Request timed out</source>
        <translation>요청 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="281"/>
        <source>The release has no update manifest</source>
        <translation>릴리스에 업데이트 매니페스트가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="341"/>
        <source>No update manifest address available</source>
        <translation>사용 가능한 업데이트 매니페스트 주소가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="354"/>
        <source>Could not parse the manifest: %1</source>
        <translation>매니페스트를 파싱할 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/updater/UpdateChecker.cpp" line="363"/>
        <source>The manifest has no usable version field</source>
        <translation>매니페스트에 사용 가능한 버전 필드가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="234"/>
        <source>System-proxy backend: %1</source>
        <translation>시스템 프록시 백엔드: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="258"/>
        <source>System-proxy backend: %1 (XDG_CURRENT_DESKTOP=%2)</source>
        <translation>시스템 프록시 백엔드: %1 (XDG_CURRENT_DESKTOP=%2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="273"/>
        <source>unavailable</source>
        <translation>사용 불가</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="327"/>
        <source>Could not write ProxyServer to the registry</source>
        <translation>레지스트리에 ProxyServer를 쓸 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="335"/>
        <source>Failed to write the system-proxy ignore list</source>
        <translation>시스템 프록시 무시 목록을 쓰지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="339"/>
        <source>Could not enable the system proxy in the registry</source>
        <translation>레지스트리에서 시스템 프록시를 활성화할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="350"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="378"/>
        <source>System proxy enabled (%1): %2:%3</source>
        <translation>시스템 프록시 활성화됨 (%1): %2:%3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="356"/>
        <source>No usable system-proxy backend detected (gsettings or kwriteconfig5/6 is required)</source>
        <translation>사용 가능한 시스템 프록시 백엔드가 없습니다 (gsettings 또는 kwriteconfig5/6이 필요합니다)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="372"/>
        <source>Failed to enable the system proxy: %1</source>
        <translation>시스템 프록시 활성화 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="387"/>
        <source>Could not turn off the system proxy in the registry</source>
        <translation>레지스트리에서 시스템 프록시를 해제할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="398"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="423"/>
        <source>System proxy disabled</source>
        <translation>시스템 프록시 해제됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="402"/>
        <source>No usable system-proxy backend detected</source>
        <translation>사용 가능한 시스템 프록시 백엔드가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="418"/>
        <source>Failed to turn off the system proxy: %1</source>
        <translation>시스템 프록시 해제 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="438"/>
        <source>Could not start %1</source>
        <translation>%1을(를) 시작할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="443"/>
        <source>%1 timed out</source>
        <translation>%1 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="448"/>
        <source>%1 failed: %2</source>
        <translation>%1 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="515"/>
        <source>Failed to write the system-proxy ignore list: %1</source>
        <translation>시스템 프록시 무시 목록 쓰기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="577"/>
        <location filename="../src/app/src/sys/SysProxy.cpp" line="620"/>
        <source>kwriteconfig5/6 not found</source>
        <translation>kwriteconfig5/6을 찾을 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="53"/>
        <source>Could not open the autostart registry key</source>
        <translation>자동 시작 레지스트리 키를 열 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="68"/>
        <source>Could not write the autostart entry (error %1)</source>
        <translation>자동 시작 항목을 쓸 수 없습니다 (오류 %1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="109"/>
        <source>Could not create the autostart directory</source>
        <translation>자동 시작 디렉터리를 만들 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="161"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="193"/>
        <source>Failed to enable autostart: %1</source>
        <translation>자동 시작 활성화 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="167"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="196"/>
        <source>Autostart enabled: %1</source>
        <translation>자동 시작 활성화됨: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="215"/>
        <source>Could not remove the autostart entry (error %1)</source>
        <translation>자동 시작 항목을 제거할 수 없습니다 (오류 %1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="224"/>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="235"/>
        <source>Autostart disabled</source>
        <translation>자동 시작 해제됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/AutoRun.cpp" line="231"/>
        <source>Could not remove the autostart entry: %1</source>
        <translation>자동 시작 항목을 제거할 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/SingleInstance.cpp" line="53"/>
        <source>The single-instance guard failed to start (%1); running as multiple instances this time: %2</source>
        <translation>단일 인스턴스 가드가 시작되지 않아 (%1) 이번에는 여러 인스턴스로 실행합니다: %2</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="117"/>
        <source>Could not set the TUN adapter address</source>
        <translation>TUN 어댑터 주소를 설정할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="134"/>
        <source>Could not read the TUN adapter settings</source>
        <translation>TUN 어댑터 설정을 읽을 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="150"/>
        <source>Could not set the TUN adapter MTU/metric</source>
        <translation>TUN 어댑터 MTU/metric을 설정할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="175"/>
        <source>Could not add the TUN route</source>
        <translation>TUN 경로를 추가할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="203"/>
        <source>Could not start netsh</source>
        <translation>netsh를 시작할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="208"/>
        <source>netsh timed out</source>
        <translation>netsh 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="312"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="383"/>
        <source>TUN mode on Windows requires administrator privileges. Close the app and start it again with &quot;Run as administrator&quot;.</source>
        <translation>Windows에서 TUN 모드는 관리자 권한이 필요합니다. 앱을 종료하고 &amp;quot;관리자로 실행&amp;quot;으로 다시 시작하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="321"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="513"/>
        <source>Helper not found: %1</source>
        <translation>헬퍼를 찾을 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="337"/>
        <source>Could not start pkexec: %1</source>
        <translation>pkexec를 시작할 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="344"/>
        <source>Authorization timed out (the password prompt may have been cancelled)</source>
        <translation>권한 부여 시간 초과 (비밀번호 입력이 취소되었을 수 있습니다)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="349"/>
        <source>%1 failed (exit code %2): %3</source>
        <translation>%1 실패 (종료 코드 %2): %3</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="359"/>
        <source>TUN permissions granted (chown root:root + chmod +sx): %1</source>
        <translation>TUN 권한이 부여되었습니다 (chown root:root + chmod +sx): %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="369"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="509"/>
        <source>TUN is starting or already running</source>
        <translation>TUN이 시작 중이거나 이미 실행 중입니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="389"/>
        <source>wintun.dll was not found. Put wintun.dll next to the executable and try again.</source>
        <translation>wintun.dll을 찾을 수 없습니다. 실행 파일 옆에 wintun.dll을 두고 다시 시도하세요.</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="402"/>
        <source>Invalid TUN address: %1</source>
        <translation>잘못된 TUN 주소: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="410"/>
        <source>Could not create the TUN adapter</source>
        <translation>TUN 어댑터를 만들 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="415"/>
        <source>Could not read the TUN adapter LUID</source>
        <translation>TUN 어댑터 LUID를 읽을 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="448"/>
        <source>Invalid fake-ip CIDR: %1</source>
        <translation>잘못된 fake-ip CIDR: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="468"/>
        <source>Could not point system DNS at the TUN address: %1</source>
        <translation>시스템 DNS를 TUN 주소로 지정할 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="481"/>
        <source>TUN adapter ready: %1 %2/%3, MTU %4, routes %5, Wintun %6</source>
        <translation>TUN 어댑터 준비됨: %1 %2/%3, MTU %4, 경로 %5, Wintun %6</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="487"/>
        <source>taken over</source>
        <translation>점유됨</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="488"/>
        <source>not taken over</source>
        <translation>미점유</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="505"/>
        <source>Found a stale TUN-enabled state; cleaned it up and retried</source>
        <translation>남아 있던 TUN 활성 상태를 발견하여 정리하고 다시 시도했습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="517"/>
        <source>TUN permission not granted yet (enter the password in the prompt that appears)</source>
        <translation>TUN 권한이 아직 부여되지 않았습니다 (표시되는 창에 비밀번호를 입력하세요)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="542"/>
        <source>Socket path too long</source>
        <translation>소켓 경로가 너무 깁니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="553"/>
        <source>Failed to listen on the socket: %1</source>
        <translation>소켓에서 수신하지 못했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="583"/>
        <source>Could not start the helper: %1</source>
        <translation>헬퍼를 시작할 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="595"/>
        <source>Timed out waiting for the helper (20 s)</source>
        <translation>헬퍼 대기 시간 초과 (20초)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="611"/>
        <source>The helper exited unexpectedly (exit code %1)</source>
        <translation>헬퍼가 예기치 않게 종료되었습니다 (종료 코드 %1)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="616"/>
        <source>Started the TUN helper; waiting for the device: %1</source>
        <translation>TUN 헬퍼를 시작했습니다. 장치를 기다리는 중: %1</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="618"/>
        <source>TUN handshake details: socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</source>
        <translation>TUN 핸드셰이크 정보: socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="700"/>
        <source>The TUN helper disconnected</source>
        <translation>TUN 헬퍼 연결이 끊어졌습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="749"/>
        <source>The helper did not send a TUN fd</source>
        <translation>헬퍼가 TUN fd를 보내지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="760"/>
        <source>TUN ready: %1 (%2)</source>
        <translation>TUN 준비됨: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="762"/>
        <source>real device</source>
        <translation>실제 장치</translation>
    </message>
    <message>
        <location filename="../src/app/src/sys/TunController.cpp" line="815"/>
        <location filename="../src/app/src/sys/TunController.cpp" line="868"/>
        <source>TUN is closed</source>
        <translation>TUN이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/MainWindow.cpp" line="778"/>
        <source>System tray unavailable; skipping tray setup</source>
        <translation>시스템 트레이를 사용할 수 없어 트레이 설정을 건너뜁니다</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="767"/>
        <source>UI style: Fusion (%1) style=%2 field=%3 accent=%4 desktop dark=%5</source>
        <translation>UI 스타일: Fusion (%1) style=%2 field=%3 accent=%4 desktop dark=%5</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>dark</source>
        <translation>다크</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="768"/>
        <source>light</source>
        <translation>라이트</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>yes</source>
        <translation>예</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/UiFactory.cpp" line="773"/>
        <source>no</source>
        <translation>아니요</translation>
    </message>
    <message>
        <location filename="../src/app/src/ui/Icons.cpp" line="53"/>
        <source>Icon resource missing: %1 (check resources/clash-qt.qrc)</source>
        <translation>아이콘 리소스가 없습니다: %1 (resources/clash-qt.qrc 확인)</translation>
    </message>
</context>
<context>
    <name>clashqt::core</name>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="141"/>
        <source>YAML parse failed: %1</source>
        <translation>YAML 파싱 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Yaml.cpp" line="165"/>
        <source>YAML emit failed: %1</source>
        <translation>YAML 출력 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="312"/>
        <source>Unknown error</source>
        <translation>알 수 없는 오류</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="549"/>
        <source>Stream closed</source>
        <translation>스트림이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="566"/>
        <source>Write timed out (flow-control window stayed closed)</source>
        <translation>쓰기 시간 초과 (흐름 제어 윈도우가 계속 닫혀 있음)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="570"/>
        <source>Session released</source>
        <translation>세션 해제됨</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="627"/>
        <source>Session release</source>
        <translation>세션 해제</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="656"/>
        <source>Could not parse server address %1 (%2)</source>
        <translation>서버 주소 %1을(를) 파싱할 수 없습니다 (%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="676"/>
        <source>Failed to create the UDP socket: %1</source>
        <translation>UDP 소켓 생성 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="689"/>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="700"/>
        <source>UDP connect failed: %1</source>
        <translation>UDP 연결 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="714"/>
        <source>Failed to create the QUIC context</source>
        <translation>QUIC 컨텍스트를 만들지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="728"/>
        <source>Failed to create the QUIC connection</source>
        <translation>QUIC 연결을 만들지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="741"/>
        <source>Failed to bind the UDP socket</source>
        <translation>UDP 소켓을 바인드하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="756"/>
        <source>Failed to read the peer address: %1</source>
        <translation>피어 주소를 읽지 못했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="776"/>
        <source>Failed to build the initial QUIC peer address</source>
        <translation>초기 QUIC 피어 주소를 구성하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="791"/>
        <source>QUIC handshake failed: %1</source>
        <translation>QUIC 핸드셰이크 실패: %1</translation>
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
        <translation>QUIC 이벤트 처리 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="810"/>
        <source>QUIC handshake timed out (port %1)</source>
        <translation>QUIC 핸드셰이크 시간 초과 (포트 %1)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="820"/>
        <source>The server provided no certificate</source>
        <translation>서버가 인증서를 제공하지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="831"/>
        <source>Server certificate fingerprint does not match the config (fingerprint check failed)</source>
        <translation>서버 인증서 지문이 설정과 일치하지 않습니다 (지문 검사 실패)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="852"/>
        <source>Failed to create a QUIC stream: %1</source>
        <translation>QUIC 스트림 생성 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="856"/>
        <source>Timed out creating a QUIC stream</source>
        <translation>QUIC 스트림 생성 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="876"/>
        <source>Failed to write to a QUIC stream: %1</source>
        <translation>QUIC 스트림 쓰기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="881"/>
        <source>Timed out writing to a QUIC stream</source>
        <translation>QUIC 스트림 쓰기 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="971"/>
        <source>No response to authentication (timeout or connection reset, port %1)</source>
        <translation>인증 응답 없음 (시간 초과 또는 연결 재설정, 포트 %1)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="979"/>
        <source>Authentication rejected (HTTP status %1; check the password)</source>
        <translation>인증이 거부되었습니다 (HTTP 상태 %1, 비밀번호를 확인하세요)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1008"/>
        <source>Node is missing server</source>
        <translation>노드에 서버가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1012"/>
        <source>Node is missing password</source>
        <translation>노드에 비밀번호가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1019"/>
        <source>Socket setup</source>
        <translation>소켓 설정</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1020"/>
        <source>Handshake</source>
        <translation>핸드셰이크</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1023"/>
        <source>Authentication</source>
        <translation>인증</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1034"/>
        <source>Session unavailable</source>
        <translation>세션을 사용할 수 없음</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1038"/>
        <source>Invalid target address</source>
        <translation>잘못된 대상 주소</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1106"/>
        <source>Target %1 did not respond (timeout)</source>
        <translation>대상 %1이(가) 응답하지 않았습니다 (시간 초과)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1114"/>
        <source>Connection to target %1 refused</source>
        <translation>대상 %1에 대한 연결이 거부되었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Hysteria2Client.cpp" line="1115"/>
        <source>Connection to target %1 refused: %2</source>
        <translation>대상 %1 연결 거부됨: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="29"/>
        <source>Read more than %1 bytes without finding the delimiter</source>
        <translation>구분자를 찾기 전에 %1바이트를 초과하여 읽었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="42"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="87"/>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="247"/>
        <source>Connection closed</source>
        <translation>연결이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="48"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="93"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="329"/>
        <source>Read timed out</source>
        <translation>읽기 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="62"/>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="104"/>
        <source>Read failed: %1</source>
        <translation>읽기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="126"/>
        <source>Write failed: %1</source>
        <translation>쓰기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="133"/>
        <source>Write timed out</source>
        <translation>쓰기 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/SocketUtil.cpp" line="145"/>
        <source>Connection closed by the peer</source>
        <translation>피어가 연결을 닫았습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="461"/>
        <source>REALITY connection to %1:%2 failed: %3</source>
        <translation>%1:%2에 대한 REALITY 연결 실패: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="487"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="508"/>
        <source>X25519 key generation failed</source>
        <translation>X25519 키 생성 실패</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="514"/>
        <source>REALITY static ECDH failed</source>
        <translation>REALITY 정적 ECDH 실패</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="521"/>
        <source>Failed to build ClientHello</source>
        <translation>ClientHello를 구성하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="552"/>
        <source>Failed to generate REALITY authentication data</source>
        <translation>REALITY 인증 데이터를 생성하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="571"/>
        <source>REALITY received no ServerHello (record type=%1 length=%2 content=%3)</source>
        <translation>REALITY가 ServerHello를 받지 못했습니다 (record type=%1 length=%2 content=%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="582"/>
        <source>Truncated ServerHello</source>
        <translation>ServerHello가 잘렸습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="618"/>
        <source>ServerHello contains no usable key_share</source>
        <translation>ServerHello에 사용 가능한 key_share가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="624"/>
        <source>TLS negotiated: cipher=0x%1 key-exchange group=%2</source>
        <translation>TLS 협상됨: cipher=0x%1 key-exchange group=%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="636"/>
        <source>The REALITY server negotiated an unsupported cipher 0x%1</source>
        <translation>REALITY 서버가 지원하지 않는 암호 0x%1을(를) 협상했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="704"/>
        <source>Unexpected record type %1 during the REALITY handshake</source>
        <translation>REALITY 핸드셰이크 중 예기치 않은 레코드 유형 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="713"/>
        <source>Failed to decrypt a REALITY handshake record (type=%1 length=%2 seq=%3)</source>
        <translation>REALITY 핸드셰이크 레코드 복호화 실패 (type=%1 length=%2 seq=%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="717"/>
        <source>Failed to decrypt a REALITY handshake record</source>
        <translation>REALITY 핸드셰이크 레코드를 복호화하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="768"/>
        <source>The server returned no certificate</source>
        <translation>서버가 인증서를 반환하지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="799"/>
        <source>TLS certificate verification failed (chain=%1 hostname=%2)</source>
        <translation>TLS 인증서 검증 실패 (chain=%1 hostname=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="800"/>
        <source>Pass</source>
        <translation>통과</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="801"/>
        <source>Fail</source>
        <translation>실패</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="802"/>
        <source>Match</source>
        <translation>일치</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="803"/>
        <source>No match</source>
        <translation>일치하지 않음</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="809"/>
        <source>Node %1 skips TLS certificate verification</source>
        <translation>노드 %1이(가) TLS 인증서 검증을 건너뜁니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="818"/>
        <source>REALITY authentication failed (the peer is the borrowed site itself rather than a REALITY server, or public-key / short-id is misconfigured)</source>
        <translation>REALITY 인증 실패 (피어가 REALITY 서버가 아니라 위장 대상 사이트 자체이거나 public-key / short-id 설정이 잘못되었습니다)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="830"/>
        <source>Server Finished verification failed</source>
        <translation>Server Finished 검증 실패</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="993"/>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1008"/>
        <source>REALITY send failed: %1</source>
        <translation>REALITY 전송 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1022"/>
        <source>Timed out reading a REALITY record</source>
        <translation>REALITY 레코드 읽기 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1028"/>
        <source>REALITY failed to read a record: %1</source>
        <translation>REALITY 레코드 읽기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1044"/>
        <source>Invalid REALITY record length %1</source>
        <translation>잘못된 REALITY 레코드 길이 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1079"/>
        <source>Invalid REALITY application-record length</source>
        <translation>잘못된 REALITY 애플리케이션 레코드 길이</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1099"/>
        <source>Failed to decrypt a REALITY record</source>
        <translation>REALITY 레코드를 복호화하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1251"/>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="390"/>
        <source>Timed out reading the response</source>
        <translation>응답 읽기 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/Tls13Client.cpp" line="1256"/>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="324"/>
        <source>Connection closed while reading</source>
        <translation>읽는 중 연결이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="73"/>
        <source>Failed to write the upgrade request: %1</source>
        <translation>업그레이드 요청 쓰기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="77"/>
        <source>WS upgrade request: sent %1 bytes to %2:%3 (%4 pending, state %5)</source>
        <translation>WS 업그레이드 요청: %2:%3에 %1바이트 전송 (%4 대기 중, 상태 %5)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="87"/>
        <source>%1 (received %2 bytes: %3)</source>
        <translation>%1 (%2바이트 수신: %3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="101"/>
        <source>WebSocket handshake rejected: %1
%2</source>
        <translation>WebSocket 핸드셰이크 거부됨: %1
%2</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="116"/>
        <source>WebSocket handshake verification failed (Sec-WebSocket-Accept mismatch)</source>
        <translation>WebSocket 핸드셰이크 검증 실패 (Sec-WebSocket-Accept 불일치)</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="129"/>
        <source>The WebSocket handshake response carried extra data, which is not supported</source>
        <translation>WebSocket 핸드셰이크 응답에 추가 데이터가 있어 지원하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="227"/>
        <source>The peer sent a WebSocket close frame</source>
        <translation>피어가 WebSocket close 프레임을 보냈습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="236"/>
        <source>Fragmented WebSocket messages are not supported</source>
        <translation>분할된 WebSocket 메시지는 지원하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/common/WebSocketClient.cpp" line="262"/>
        <source>Timed out reading a WebSocket frame</source>
        <translation>WebSocket 프레임 읽기 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/core/src/config/RawConfig.cpp" line="81"/>
        <source>The config root must be a mapping</source>
        <translation>설정의 최상위는 매핑이어야 합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/cidr/IpCidrSet.cpp" line="12"/>
        <source>Ignoring an invalid CIDR rule entry: %1</source>
        <translation>잘못된 CIDR 규칙 항목을 무시합니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="68"/>
        <source>DNS listener not started: %1</source>
        <translation>DNS 리스너가 시작되지 않았습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="77"/>
        <source>Not listening</source>
        <translation>수신 중 아님</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="86"/>
        <source>dns.listen is not configured</source>
        <translation>dns.listen이 설정되지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="95"/>
        <source>Failed to bind UDP %1:%2: %3</source>
        <translation>UDP %1:%2 바인드 실패: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="139"/>
        <source>DNS listener started: %1 (enhanced-mode=%2)</source>
        <translation>DNS 리스너 시작됨: %1 (enhanced-mode=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="199"/>
        <source>DNS fake-ip: AAAA query for %1 returned an empty answer</source>
        <translation>DNS fake-ip: %1에 대한 AAAA 쿼리가 빈 응답을 반환했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/DnsServer.cpp" line="222"/>
        <source>DNS real resolution: %1 -&gt; %2 record(s)</source>
        <translation>DNS 실제 조회: %1 -&gt; 레코드 %2개</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="285"/>
        <source>Invalid or non-IPv4 fake-ip-range: %1; falling back to 198.18.0.1/16</source>
        <translation>잘못되었거나 IPv4가 아닌 fake-ip-range: %1. 198.18.0.1/16으로 대체합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="545"/>
        <source>DNS configured: enhanced-mode=%1, nameserver=%2, fake-ip=%3</source>
        <translation>DNS 설정됨: enhanced-mode=%1, nameserver=%2, fake-ip=%3</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="915"/>
        <source>DoT (tls://) currently connects over plain TCP to %1</source>
        <translation>DoT (tls://)는 현재 %1에 평문 TCP로 연결합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="979"/>
        <source>Could not resolve the DNS upstream host through the bootstrap server: %1</source>
        <translation>부트스트랩 서버로 DNS 업스트림 호스트를 확인할 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1004"/>
        <source>Could not parse the DNS server config: %1</source>
        <translation>DNS 서버 설정을 파싱할 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/component/dns/Resolver.cpp" line="1045"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="487"/>
        <source>DNS resolution failed: %1</source>
        <translation>DNS 조회 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="92"/>
        <source>Malformed logical-rule sub-item: %1</source>
        <translation>논리 규칙 하위 항목 형식이 잘못되었습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="103"/>
        <source>Failed to parse a logical-rule sub-item: %1 (%2)</source>
        <translation>논리 규칙 하위 항목 파싱 실패: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="119"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="125"/>
        <source>Empty rule</source>
        <translation>빈 규칙</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="133"/>
        <source>Unknown rule type: %1</source>
        <translation>알 수 없는 규칙 유형: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="141"/>
        <source>MATCH rule is missing the target policy</source>
        <translation>MATCH 규칙에 대상 정책이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="149"/>
        <source>Rule has too few fields (expected type,value,policy): %1</source>
        <translation>규칙의 필드가 너무 적습니다 (type,value,policy 필요): %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="197"/>
        <source>Invalid domain regular expression: %1</source>
        <translation>잘못된 도메인 정규식: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="213"/>
        <source>Invalid IP CIDR: %1</source>
        <translation>잘못된 IP CIDR: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="225"/>
        <source>Invalid source IP CIDR: %1</source>
        <translation>잘못된 소스 IP CIDR: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="251"/>
        <source>Invalid port: %1</source>
        <translation>잘못된 포트: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="261"/>
        <source>Invalid source port: %1</source>
        <translation>잘못된 소스 포트: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="271"/>
        <source>Invalid inbound port: %1</source>
        <translation>잘못된 인바운드 포트: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="288"/>
        <source>Invalid process-match expression: %1</source>
        <translation>잘못된 프로세스 매칭식: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="298"/>
        <source>Invalid UID: %1</source>
        <translation>잘못된 UID: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="310"/>
        <source>References an undefined rule set: %1</source>
        <translation>정의되지 않은 규칙 집합을 참조합니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="325"/>
        <source>Logical rule has no usable sub-rules: %1</source>
        <translation>논리 규칙에 사용 가능한 하위 규칙이 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="340"/>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="346"/>
        <source>Unsupported rule type: %1</source>
        <translation>지원하지 않는 규칙 유형: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="352"/>
        <source>Failed to build the rule: %1</source>
        <translation>규칙 구성 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="369"/>
        <source>Skipped a non-string rule entry</source>
        <translation>문자열이 아닌 규칙 항목을 건너뛰었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/rules/RuleParser.cpp" line="420"/>
        <source>Could not parse an entry in rule set %1: %2</source>
        <translation>규칙 집합 %1의 항목을 파싱할 수 없습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="93"/>
        <source>Failed to connect to %1:%2: %3 (error=%4)</source>
        <translation>%1:%2 연결 실패: %3 (error=%4)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="177"/>
        <source>Invalid DIRECT target address</source>
        <translation>잘못된 DIRECT 대상 주소입니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="186"/>
        <source>DIRECT UDP requires a literal IP target</source>
        <translation>DIRECT UDP는 대상이 IP 리터럴이어야 합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="191"/>
        <source>UDP bind failed: %1</source>
        <translation>UDP 바인드 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="224"/>
        <source>Connection rejected by a rule (REJECT)</source>
        <translation>규칙에 의해 연결이 거부되었습니다 (REJECT)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="242"/>
        <source>HTTP proxy %1 claims https; connecting with plaintext CONNECT for now</source>
        <translation>HTTP 프록시 %1이(가) https를 주장하여 일단 평문 CONNECT로 연결합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="250"/>
        <source>HTTP proxy %1 is missing server/port</source>
        <translation>HTTP 프록시 %1에 서버/포트가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="261"/>
        <source>The HTTP proxy does not support UDP forwarding</source>
        <translation>HTTP 프록시는 UDP 포워딩을 지원하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="286"/>
        <source>HTTP proxy %1 did not respond, or its response headers are too long</source>
        <translation>HTTP 프록시 %1이(가) 응답하지 않았거나 응답 헤더가 너무 깁니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="300"/>
        <source>HTTP proxy returned %1</source>
        <translation>HTTP 프록시가 %1을(를) 반환했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="335"/>
        <source>SOCKS5 proxy %1 is missing server/port</source>
        <translation>SOCKS5 프록시 %1에 서버/포트가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="356"/>
        <source>No response to SOCKS5 negotiation</source>
        <translation>SOCKS5 협상에 응답이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="362"/>
        <source>SOCKS5 version mismatch</source>
        <translation>SOCKS5 버전이 일치하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="368"/>
        <source>The SOCKS5 server rejected all authentication methods</source>
        <translation>SOCKS5 서버가 모든 인증 방식을 거부했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="377"/>
        <source>SOCKS5 username or password too long</source>
        <translation>SOCKS5 사용자 이름 또는 비밀번호가 너무 깁니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="391"/>
        <source>No response to SOCKS5 authentication</source>
        <translation>SOCKS5 인증에 응답이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="396"/>
        <source>SOCKS5 authentication failed</source>
        <translation>SOCKS5 인증 실패</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="401"/>
        <source>The SOCKS5 server requires an unsupported auth method 0x%1</source>
        <translation>SOCKS5 서버가 지원하지 않는 인증 방식 0x%1을(를) 요구합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="425"/>
        <source>SOCKS5 target domain name too long</source>
        <translation>SOCKS5 대상 도메인 이름이 너무 깁니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="442"/>
        <source>Could not resolve the SOCKS5 target address: %1</source>
        <translation>SOCKS5 대상 주소를 확인할 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="455"/>
        <source>No response to SOCKS5 CONNECT</source>
        <translation>SOCKS5 CONNECT에 응답이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="464"/>
        <source>Succeeded</source>
        <translation>성공</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="465"/>
        <source>General SOCKS server failure</source>
        <translation>일반 SOCKS 서버 오류</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="466"/>
        <source>Connection not allowed by ruleset</source>
        <translation>규칙에 의해 연결이 허용되지 않음</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="467"/>
        <source>Network unreachable</source>
        <translation>네트워크에 연결할 수 없음</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="468"/>
        <source>Host unreachable</source>
        <translation>호스트에 연결할 수 없음</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="469"/>
        <source>Connection refused</source>
        <translation>연결이 거부됨</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="470"/>
        <source>TTL expired</source>
        <translation>TTL 만료</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="471"/>
        <source>Command not supported</source>
        <translation>지원하지 않는 명령</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="472"/>
        <source>Address type not supported</source>
        <translation>지원하지 않는 주소 유형</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="475"/>
        <source>Unknown error %1</source>
        <translation>알 수 없는 오류 %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="477"/>
        <source>SOCKS5 connection refused: %1 (%2)</source>
        <translation>SOCKS5 연결 거부됨: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="491"/>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="503"/>
        <source>Truncated SOCKS5 response</source>
        <translation>SOCKS5 응답이 잘렸습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="498"/>
        <source>The SOCKS5 response contains an unknown address type %1</source>
        <translation>SOCKS5 응답에 알 수 없는 주소 유형 %1이(가) 있습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="521"/>
        <source>SOCKS5 UDP ASSOCIATE forwarding is not implemented yet (Phase 2)</source>
        <translation>SOCKS5 UDP ASSOCIATE 포워딩은 아직 구현되지 않았습니다 (Phase 2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="560"/>
        <source>Node %1 (%2) is not supported yet: %3</source>
        <translation>노드 %1 (%2)은(는) 아직 지원하지 않습니다: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="632"/>
        <source>Group %1 switched to %2</source>
        <translation>그룹 %1이(가) %2(으)로 전환되었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="645"/>
        <source>Proxy group %1 has no available members</source>
        <translation>프록시 그룹 %1에 사용 가능한 멤버가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Adapter.cpp" line="658"/>
        <source>Group %1 member %2 failed to connect: %3</source>
        <translation>그룹 %1의 멤버 %2 연결 실패: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="67"/>
        <source>This protocol is not implemented yet</source>
        <translation>이 프로토콜은 아직 구현되지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="82"/>
        <source>Skipped non-mapping entries in proxies</source>
        <translation>proxies의 매핑이 아닌 항목을 건너뛰었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="91"/>
        <source>Unrecognized node: %1 (type=%2)</source>
        <translation>인식할 수 없는 노드: %1 (type=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="92"/>
        <source>&lt;unnamed&gt;</source>
        <translation>&lt;이름 없음&gt;</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="96"/>
        <source>Duplicate node name; ignoring the later definition: %1</source>
        <translation>노드 이름이 중복되어 이후 정의를 무시합니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="115"/>
        <source>Unrecognized proxy group: %1 (type=%2)</source>
        <translation>인식할 수 없는 프록시 그룹: %1 (type=%2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="154"/>
        <source>Proxy group %1 has no members</source>
        <translation>프록시 그룹 %1에 멤버가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="168"/>
        <source>Proxy group %1 has members that cannot be resolved (a missing node or a reference cycle); skipped. Members: %2</source>
        <translation>프록시 그룹 %1에 확인할 수 없는 멤버(없는 노드 또는 순환 참조)가 있어 건너뛰었습니다. 멤버: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="180"/>
        <source>Adapters loaded: %1 nodes/groups, %2 unimplemented protocols, %3 warnings</source>
        <translation>어댑터 로드됨: 노드/그룹 %1개, 미구현 프로토콜 %2개, 경고 %3개</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="197"/>
        <source>The config defines its own GLOBAL group; using it as-is</source>
        <translation>설정에 자체 GLOBAL 그룹이 정의되어 있어 그대로 사용합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="231"/>
        <source>Built the built-in GLOBAL group with %1 member(s)</source>
        <translation>내장 GLOBAL 그룹을 멤버 %1개로 구성했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/AdapterRegistry.cpp" line="234"/>
        <source>The profile has no usable node; the GLOBAL group only contains DIRECT</source>
        <translation>프로필에 사용 가능한 노드가 없어 GLOBAL 그룹에 DIRECT만 포함됩니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="52"/>
        <source>fingerprint is not a valid SHA-256 hex string</source>
        <translation>fingerprint가 올바른 SHA-256 16진수 문자열이 아닙니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="59"/>
        <source>obfs=%1 is set (Salamander obfuscation is not implemented yet)</source>
        <translation>obfs=%1이(가) 설정되었습니다 (Salamander 난독화는 아직 구현되지 않았습니다)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="63"/>
        <source>realm-opts is set (realm is not implemented yet)</source>
        <translation>realm-opts가 설정되었습니다 (realm은 아직 구현되지 않았습니다)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="67"/>
        <source>Missing password</source>
        <translation>비밀번호가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="78"/>
        <source>Node %1 does not support UDP relay yet (Hysteria2 needs QUIC DATAGRAM, which OpenSSL 3.2 does not provide)</source>
        <translation>노드 %1은(는) 아직 UDP 릴레이를 지원하지 않습니다 (Hysteria2는 QUIC DATAGRAM이 필요한데 OpenSSL 3.2는 이를 제공하지 않습니다)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="84"/>
        <source>Node %1 is not supported yet: %2</source>
        <translation>노드 %1은(는) 아직 지원하지 않습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="92"/>
        <source>Node %1 has an invalid target address</source>
        <translation>노드 %1의 대상 주소가 잘못되었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="133"/>
        <source>Node %1 failed to connect (attempt %2); retrying on another port: %3</source>
        <translation>노드 %1 연결 실패 (시도 %2). 다른 포트로 재시도합니다: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="140"/>
        <source>Node %1 failed to connect: %2</source>
        <translation>노드 %1 연결 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="149"/>
        <source>Node %1 failed to establish the tunnel: %2</source>
        <translation>노드 %1이(가) 터널을 설정하지 못했습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/Hysteria2Adapter.cpp" line="158"/>
        <source>Node %1: the server has UDP disabled; UDP traffic will not work</source>
        <translation>노드 %1: 서버에서 UDP가 비활성화되어 UDP 트래픽이 동작하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="61"/>
        <source>Target domain name too long</source>
        <translation>대상 도메인 이름이 너무 깁니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="78"/>
        <source>Could not resolve the target address: %1</source>
        <translation>대상 주소를 확인할 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="121"/>
        <source>Node %1 is missing server/port</source>
        <translation>노드 %1에 서버/포트가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="138"/>
        <source>Failed to connect to %1:%2: %3</source>
        <translation>%1:%2 연결 실패: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="157"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="442"/>
        <source>Node %1 is missing uuid</source>
        <translation>노드 %1에 uuid가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="163"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="448"/>
        <source>Node %1 has an invalid uuid: %2</source>
        <translation>노드 %1의 uuid가 잘못되었습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="176"/>
        <source>Node %1 uses an unsupported flow: %2</source>
        <translation>노드 %1이(가) 지원하지 않는 flow를 사용합니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="181"/>
        <source>Node %1 uses vision but has no reality-opts configured; this combination is not implemented yet</source>
        <translation>노드 %1이(가) vision을 사용하지만 reality-opts가 설정되지 않았습니다. 이 조합은 아직 구현되지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="196"/>
        <source>Node %1 has an invalid reality-opts.public-key (it should decode to 32 bytes)</source>
        <translation>노드 %1의 reality-opts.public-key가 잘못되었습니다 (32바이트로 디코딩되어야 합니다)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="200"/>
        <source>Node %1 has a reality-opts.short-id that is too long</source>
        <translation>노드 %1의 reality-opts.short-id가 너무 깁니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="208"/>
        <source>Node %1 REALITY handshake failed: %2</source>
        <translation>노드 %1 REALITY 핸드셰이크 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="236"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="293"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="322"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="541"/>
        <source>Node %1 failed to send the VLESS request</source>
        <translation>노드 %1이(가) VLESS 요청을 보내지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="252"/>
        <source>Node %1 transport %2 is not implemented yet (ws, REALITY and plain TCP are supported)</source>
        <translation>노드 %1의 전송 방식 %2은(는) 아직 구현되지 않았습니다 (ws, REALITY, 일반 TCP를 지원합니다)</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="280"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="317"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="497"/>
        <source>Node %1 TLS connection failed: %2</source>
        <translation>노드 %1 TLS 연결 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="288"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="340"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="507"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="525"/>
        <source>Node %1 WebSocket handshake failed: %2</source>
        <translation>노드 %1 WebSocket 핸드셰이크 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="350"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="355"/>
        <source>Failed to send the VLESS request: %1</source>
        <translation>VLESS 요청 전송 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="373"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="397"/>
        <source>Failed to read the VLESS response: %1</source>
        <translation>VLESS 응답 읽기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="377"/>
        <source>Truncated VLESS response</source>
        <translation>VLESS 응답이 잘렸습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="382"/>
        <source>Truncated VLESS response addons</source>
        <translation>VLESS 응답 addons가 잘렸습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="404"/>
        <source>Failed to read the VLESS response addons: %1</source>
        <translation>VLESS 응답 addons 읽기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="454"/>
        <source>VLESS UDP is missing the target address</source>
        <translation>VLESS UDP에 대상 주소가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="488"/>
        <source>Node %1 has an invalid reality-opts.public-key</source>
        <translation>노드 %1의 reality-opts.public-key가 잘못되었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="515"/>
        <location filename="../src/core/src/adapter/VlessAdapter.cpp" line="533"/>
        <source>Node %1 transport %2 does not support UDP</source>
        <translation>노드 %1의 전송 방식 %2은(는) UDP를 지원하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="54"/>
        <source>%1 has an invalid listen address: %2</source>
        <translation>%1의 수신 주소가 잘못되었습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="62"/>
        <source>%1 failed to listen on %2:%3: %4</source>
        <translation>%1이(가) %2:%3에서 수신하지 못했습니다: %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="76"/>
        <source>%1 inbound started: %2:%3 (concurrency limit %4)</source>
        <translation>%1 인바운드 시작됨: %2:%3 (동시 연결 제한 %4)</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="126"/>
        <source>%1 still has %2 open connection(s)</source>
        <translation>%1에 아직 열린 연결이 %2개 있습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="154"/>
        <source>Concurrency limit %1 reached; rejecting new connections</source>
        <translation>동시 연결 제한 %1에 도달하여 새 연결을 거부합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="182"/>
        <source>Failed to duplicate the connection descriptor; connection dropped</source>
        <translation>연결 디스크립터를 복제하지 못해 연결을 종료했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="216"/>
        <source>%1 accepted connection %2:%3</source>
        <translation>%1이(가) %2:%3 연결을 수락했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Listener.cpp" line="231"/>
        <source>Could not rebuild a socket from descriptor %1</source>
        <translation>디스크립터 %1에서 소켓을 다시 만들 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="61"/>
        <source>Missing HTTP request line</source>
        <translation>HTTP 요청 라인이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="68"/>
        <source>Malformed HTTP request line: %1</source>
        <translation>HTTP 요청 라인 형식이 잘못되었습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/HttpListener.cpp" line="135"/>
        <source>HTTP inbound read failed: %1</source>
        <translation>HTTP 인바운드 읽기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="58"/>
        <source>Unsupported SOCKS version: %1</source>
        <translation>지원하지 않는 SOCKS 버전: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="75"/>
        <source>The client does not support username/password authentication</source>
        <translation>클라이언트가 사용자 이름/비밀번호 인증을 지원하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="81"/>
        <source>The client offered no acceptable authentication method</source>
        <translation>클라이언트가 허용 가능한 인증 방식을 제시하지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="89"/>
        <source>Failed to write the SOCKS5 method selection</source>
        <translation>SOCKS5 방식 선택을 쓰지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="100"/>
        <source>Unsupported username/password auth version</source>
        <translation>지원하지 않는 사용자 이름/비밀번호 인증 버전</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="125"/>
        <source>SOCKS5 authentication failed (user %1)</source>
        <translation>SOCKS5 인증 실패 (사용자 %1)</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="142"/>
        <source>Wrong SOCKS5 request version</source>
        <translation>SOCKS5 요청 버전이 잘못되었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="152"/>
        <source>Unsupported SOCKS5 command 0x%1</source>
        <translation>지원하지 않는 SOCKS5 명령 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="180"/>
        <source>Unsupported SOCKS5 address type 0x%1</source>
        <translation>지원하지 않는 SOCKS5 주소 유형 0x%1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="219"/>
        <source>SOCKS5 negotiation failed: %1</source>
        <translation>SOCKS5 협상 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>SOCKS5 request failed: %1</source>
        <translation>SOCKS5 요청 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/Socks5Listener.cpp" line="250"/>
        <source>Invalid target</source>
        <translation>잘못된 대상</translation>
    </message>
    <message>
        <location filename="../src/core/src/listener/MixedListener.cpp" line="37"/>
        <source>MIXED inbound timed out while sniffing the protocol</source>
        <translation>MIXED 인바운드가 프로토콜 스니핑 중 시간 초과되었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="26"/>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="24"/>
        <source>No dialer implementation available</source>
        <translation>사용 가능한 다이얼러 구현이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/FlowWorker.cpp" line="32"/>
        <source>Dial failed</source>
        <translation>다이얼 실패</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="71"/>
        <source>UDP socket closed</source>
        <translation>UDP 소켓이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpChannel.cpp" line="160"/>
        <source>UDP tunnel closed</source>
        <translation>UDP 터널이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/UdpWorker.cpp" line="30"/>
        <source>UDP dial failed</source>
        <translation>UDP 다이얼 실패</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="97"/>
        <source>Could not resolve the TUN adapter from its LUID</source>
        <translation>LUID로 TUN 어댑터를 확인할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="116"/>
        <source>Could not get the Wintun read event</source>
        <translation>Wintun 읽기 이벤트를 가져올 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="161"/>
        <source>The Wintun session was closed</source>
        <translation>Wintun 세션이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="199"/>
        <source>DNS is not taken over on the TUN address (%1): %2</source>
        <translation>TUN 주소(%1)에서 DNS를 점유하지 않았습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="282"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="427"/>
        <source>Invalid TUN adapter</source>
        <translation>잘못된 TUN 어댑터</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="287"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="432"/>
        <source>Invalid TUN device fd</source>
        <translation>잘못된 TUN 장치 fd</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="306"/>
        <source>Failed to send a packet to the TUN adapter</source>
        <translation>TUN 어댑터로 패킷을 보내지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="314"/>
        <source>Failed to write to the TUN device: %1</source>
        <translation>TUN 장치 쓰기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="342"/>
        <source>Could not open the TUN adapter</source>
        <translation>TUN 어댑터를 열 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="349"/>
        <source>TUN data plane ready (worker thread %1, Wintun %2)</source>
        <translation>TUN 데이터 플레인 준비됨 (워커 스레드 %1, Wintun %2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="382"/>
        <source>TUN data plane ready (worker thread %1, DNS 53 %2)</source>
        <translation>TUN 데이터 플레인 준비됨 (워커 스레드 %1, DNS 53 %2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Taken over</source>
        <translation>점유됨</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="384"/>
        <source>Not taken over</source>
        <translation>미점유</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="412"/>
        <source>Failed to read the TUN device: %1</source>
        <translation>TUN 장치 읽기 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunDevice.cpp" line="414"/>
        <source>Failed to read the TUN device</source>
        <translation>TUN 장치를 읽지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="185"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="193"/>
        <source>Idle timeout</source>
        <translation>유휴 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="216"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="224"/>
        <source>Closing TUN</source>
        <translation>TUN 닫는 중</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="248"/>
        <source>Re-entered TUN packet handling; dropped this packet to avoid concurrent flow-table writes</source>
        <translation>TUN 패킷 처리에 재진입하여 흐름 테이블 동시 쓰기를 피하려고 이 패킷을 버렸습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="268"/>
        <source>Dropped a non-IPv4 packet from TUN (IPv6 is not supported yet)</source>
        <translation>TUN에서 IPv4가 아닌 패킷을 버렸습니다 (IPv6는 아직 지원하지 않습니다)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="321"/>
        <source>Dropped an IP fragment from TUN (no reassembly)</source>
        <translation>TUN에서 IP 단편을 버렸습니다 (재조립하지 않음)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="421"/>
        <source>TUN: dropped link-local/multicast UDP to %1</source>
        <translation>TUN: %1에 대한 링크 로컬/멀티캐스트 UDP를 버렸습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="535"/>
        <source>TUN: answered ICMP echo locally (%1, fake-ip)</source>
        <translation>TUN: ICMP echo에 로컬로 응답했습니다 (%1, fake-ip)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="565"/>
        <source>TUN TCP segment %1:%2 -&gt; %3 flags=0x%4 seq=%5 len=%6 known flows=%7</source>
        <translation>TUN TCP 세그먼트 %1:%2 -&gt; %3 flags=0x%4 seq=%5 len=%6 known flows=%7</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="590"/>
        <source>TUN: TCP port 53 is not supported yet; replying RST so the client falls back to UDP</source>
        <translation>TUN: TCP 포트 53은 아직 지원하지 않아 RST를 보내 클라이언트가 UDP로 전환하도록 합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="640"/>
        <source>Client RST</source>
        <translation>클라이언트 RST</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="668"/>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1227"/>
        <source>Client closed and upstream already disconnected</source>
        <translation>클라이언트가 닫았고 업스트림도 이미 끊어졌습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="674"/>
        <source>Four-way handshake complete</source>
        <translation>4방향 핸드셰이크 완료</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="778"/>
        <source>TUN: UDP target failed recently; replying with ICMP: %1</source>
        <translation>TUN: UDP 대상이 최근 실패하여 ICMP로 응답합니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="785"/>
        <source>TUN: UDP sessions exceed %1; rejecting new sessions</source>
        <translation>TUN: UDP 세션이 %1개를 초과하여 새 세션을 거부합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="869"/>
        <source>TUN UDP session established: %1:%2 -&gt; %3</source>
        <translation>TUN UDP 세션 설정됨: %1:%2 -&gt; %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="899"/>
        <source>TUN UDP dial failed for %1: %2</source>
        <translation>%1에 대한 TUN UDP 다이얼 실패: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="928"/>
        <source>TUN UDP channel closed: %1</source>
        <translation>TUN UDP 채널이 닫혔습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="930"/>
        <source>Upstream closed</source>
        <translation>업스트림이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="950"/>
        <source>TUN closed UDP session %1:%2 (%3)</source>
        <translation>TUN UDP 세션 종료 %1:%2 (%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1044"/>
        <source>TUN: target failed recently; sending RST: %1</source>
        <translation>TUN: 대상이 최근 실패하여 RST를 보냅니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1098"/>
        <source>TUN: concurrent connections exceed %1; rejecting new connections</source>
        <translation>TUN: 동시 연결이 %1개를 초과하여 새 연결을 거부합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1149"/>
        <source>TUN new connection %1:%2 -&gt; %3:%4</source>
        <translation>TUN 새 연결 %1:%2 -&gt; %3:%4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1179"/>
        <source>TUN dial failed for %1:%2: %3</source>
        <translation>%1:%2에 대한 TUN 다이얼 실패: %3</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1204"/>
        <source>TUN upstream data: %1 bytes (%2 pending)</source>
        <translation>TUN 업스트림 데이터: %1바이트 (%2 대기 중)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/TunIpStack.cpp" line="1336"/>
        <source>TUN closed connection %1:%2 (%3)</source>
        <translation>TUN 연결 종료 %1:%2 (%3)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="61"/>
        <source>Connection %1: client disconnected (up %2 / down %3 bytes)</source>
        <translation>연결 %1: 클라이언트 연결 끊김 (업로드 %2 / 다운로드 %3바이트)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="70"/>
        <source>Connection %1: upstream closed (up %2 / down %3 bytes) %4</source>
        <translation>연결 %1: 업스트림 닫힘 (업로드 %2 / 다운로드 %3바이트) %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="111"/>
        <source>Connection %1: short upstream write, %2 / %3 bytes</source>
        <translation>연결 %1: 업스트림 쓰기 부족, %2 / %3바이트</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="224"/>
        <source>Config not loaded yet</source>
        <translation>설정이 아직 로드되지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="232"/>
        <source>GLOBAL mode is on, but the config has no GLOBAL group</source>
        <translation>GLOBAL 모드가 켜져 있지만 설정에 GLOBAL 그룹이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="266"/>
        <source>No MATCH fallback rule in the rule list; treating as DIRECT</source>
        <translation>규칙 목록에 MATCH 폴백 규칙이 없어 DIRECT로 처리합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="277"/>
        <source>Rule %1 points at a policy that does not exist: %2</source>
        <translation>규칙 %1이(가) 존재하지 않는 정책을 가리킵니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="311"/>
        <source>Closed an inbound connection with no target address</source>
        <translation>대상 주소가 없는 인바운드 연결을 종료했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="332"/>
        <source>Could not determine an outbound policy (%1 -&gt; %2); connection closed</source>
        <translation>아웃바운드 정책을 결정할 수 없습니다 (%1 -&gt; %2). 연결을 종료했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="333"/>
        <source>Unknown reason</source>
        <translation>알 수 없는 이유</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="351"/>
        <source>The DIRECT outbound needs DNS resolution, but no resolver is available</source>
        <translation>DIRECT 아웃바운드에 DNS 조회가 필요하지만 사용 가능한 리졸버가 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="360"/>
        <source>DNS resolution failed; connection closed: %1</source>
        <translation>DNS 조회 실패로 연결을 종료했습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="381"/>
        <source>Rule %1 dropped the connection: %2</source>
        <translation>규칙 %1이(가) 연결을 차단했습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="394"/>
        <source>Outbound %1 failed to connect to %2:%3: %4</source>
        <translation>아웃바운드 %1이(가) %2:%3에 연결하지 못했습니다: %4</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="404"/>
        <source>UDP relay is not implemented yet; connection closed</source>
        <translation>UDP 릴레이는 아직 구현되지 않아 연결을 종료했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/Tunnel.cpp" line="430"/>
        <source>%1 -&gt; %2:%3 via %4 (rule %5)</source>
        <translation>%1 -&gt; %2:%3, 경유 %4 (규칙 %5)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="309"/>
        <source>Vision: the server echoed an unknown UUID</source>
        <translation>Vision: 서버가 알 수 없는 UUID를 반환했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="338"/>
        <source>Vision: read an unknown command %1</source>
        <translation>Vision: 알 수 없는 명령 %1을(를) 읽었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/VisionStream.cpp" line="385"/>
        <source>Connection closed while reading the response</source>
        <translation>응답을 읽는 중 연결이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="88"/>
        <source>Failed to send the WebSocket upgrade request</source>
        <translation>WebSocket 업그레이드 요청을 보내지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="104"/>
        <source>Connection closed during the WebSocket upgrade (received %1 bytes: %2)</source>
        <translation>WebSocket 업그레이드 중 연결이 닫혔습니다 (%1바이트 수신: %2)</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="113"/>
        <source>WebSocket upgrade handshake timed out</source>
        <translation>WebSocket 업그레이드 핸드셰이크 시간 초과</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="127"/>
        <source>WebSocket handshake rejected: %1</source>
        <translation>WebSocket 핸드셰이크 거부됨: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="154"/>
        <source>Server-sent WebSocket frames must not be masked</source>
        <translation>서버가 보내는 WebSocket 프레임은 마스킹되면 안 됩니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/tunnel/WsStream.cpp" line="179"/>
        <source>WebSocket frame too long</source>
        <translation>WebSocket 프레임이 너무 깁니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="127"/>
        <source>DNS override is off; using system resolution</source>
        <translation>DNS 재정의가 꺼져 있어 시스템 설정으로 조회합니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="152"/>
        <source>Rule set %1 has an invalid behavior: %2</source>
        <translation>규칙 집합 %1의 behavior가 잘못되었습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="178"/>
        <source>Could not read the rule-set file: %1</source>
        <translation>규칙 집합 파일을 읽을 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="181"/>
        <source>Remote rule sets (type=http) are not implemented yet; skipped</source>
        <translation>원격 규칙 집합(type=http)은 아직 구현되지 않아 건너뛰었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="207"/>
        <source>Config hot-reloaded (inbound listeners kept)</source>
        <translation>설정이 핫 리로드되었습니다 (인바운드 리스너 유지)</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="248"/>
        <source>Mixed port %1 failed to start</source>
        <translation>혼합 포트 %1을(를) 시작하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="265"/>
        <source>SOCKS5 port %1 failed to start</source>
        <translation>SOCKS5 포트 %1을(를) 시작하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="280"/>
        <source>HTTP port %1 failed to start</source>
        <translation>HTTP 포트 %1을(를) 시작하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="290"/>
        <source>redir-port / tproxy-port transparent proxying is not implemented yet (Phase 2); those ports are not listening</source>
        <translation>redir-port / tproxy-port 투명 프록시는 아직 구현되지 않았습니다 (Phase 2). 해당 포트는 수신하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="295"/>
        <source>No inbound port is enabled in the config (mixed-port / socks-port / port are all 0)</source>
        <translation>설정에서 활성화된 인바운드 포트가 없습니다 (mixed-port / socks-port / port가 모두 0)</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="335"/>
        <source>No config has been applied yet</source>
        <translation>아직 적용된 설정이 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="349"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="350"/>
        <source>Started</source>
        <translation>시작됨</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="367"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="384"/>
        <source>Stopped</source>
        <translation>중지됨</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="403"/>
        <source>Outbound mode switched to %1</source>
        <translation>아웃바운드 모드가 %1(으)로 전환되었습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="437"/>
        <source>TUN is already running</source>
        <translation>TUN이 이미 실행 중입니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="447"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="593"/>
        <source>TUN is closed</source>
        <translation>TUN이 닫혔습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="456"/>
        <source>No usable outbound matched</source>
        <translation>사용 가능한 아웃바운드가 매칭되지 않았습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="530"/>
        <source>TUN data-plane error (%1); TUN stopped</source>
        <translation>TUN 데이터 플레인 오류 (%1). TUN을 중지했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="545"/>
        <source>TUN taken over (device=%1); data plane running on a worker thread</source>
        <translation>TUN 점유됨 (device=%1). 데이터 플레인은 워커 스레드에서 실행 중입니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="586"/>
        <source>The TUN data-plane thread is still winding down; it will be released after exit</source>
        <translation>TUN 데이터 플레인 스레드가 아직 정리 중이며 종료 후 해제됩니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="633"/>
        <source>Group %1 does not exist</source>
        <translation>그룹 %1이(가) 존재하지 않습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="638"/>
        <location filename="../src/core/src/hub/Hub.cpp" line="766"/>
        <source>%1 is not a proxy group</source>
        <translation>%1은(는) 프록시 그룹이 아닙니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="642"/>
        <source>Group %1 has no member %2</source>
        <translation>그룹 %1에 멤버 %2이(가) 없습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="678"/>
        <source>Group %1 references member %2, which no longer exists; ignored</source>
        <translation>그룹 %1이(가) 더 이상 존재하지 않는 멤버 %2을(를) 참조하여 무시했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="688"/>
        <source>Restored manual selections for %1 proxy group(s)</source>
        <translation>프록시 그룹 %1개의 수동 선택을 복원했습니다</translation>
    </message>
    <message>
        <location filename="../src/core/src/hub/Hub.cpp" line="703"/>
        <source>Node does not exist: %1</source>
        <translation>노드가 존재하지 않습니다: %1</translation>
    </message>
</context>
</TS>
