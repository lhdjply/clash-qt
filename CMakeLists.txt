cmake_minimum_required(VERSION 3.21)

# ---------------------------------------------------------------------------
# Clash Qt —— clash-party 的 Qt6 / C++ 重写
#
# 源码统一收在 src/ 下，构建配置集中在**本文件**（src/core 与 src/app 不再各自
# 持有 CMakeLists）：
#   src/core/ : 自研 C++ 内核，目标 clashqt_core（OBJECT 库，直接编入应用）
#   src/app/  : clash-party 的 Qt6 Widgets 图形界面与内核管理链路
#
# 产物只有一个可执行文件 build/bin/clash-qt；内核不产出 .a，也不产出可执行文件。
#
# 设计约束（Phase 1 已确认）：
#   * 平台优先级：Linux 优先，其余平台保留接口占位
#   * 内核以 OBJECT 库方式编入应用（进程内直调），不驱动外部内核进程
#   * 内核目标只依赖 Qt6::Core / Qt6::Network，不含任何 GUI 依赖
#
# 离线构建：第三方依赖全部本地化（git 子模块），配置阶段不联网。
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# 版本号只有一个来源：仓库根目录的 VERSION 文件
#
#   file(STRINGS ... LIMIT_COUNT 1) 只取第一行；下面的 string(STRIP) 顺带吃掉
#   行尾的 \r —— Windows 上若按 CRLF 检出，版本号会带进一个回车，
#   project(VERSION ...) 会直接解析失败。
# ---------------------------------------------------------------------------
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" CLASHQT_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${CLASHQT_VERSION_STRING}" CLASHQT_VERSION_STRING)
if(CLASHQT_VERSION_STRING STREQUAL "")
    message(FATAL_ERROR
            "无法从 ${CMAKE_CURRENT_SOURCE_DIR}/VERSION 读取版本号（文件缺失或为空）")
endif()

# 文件里按 tag 的写法带 v 前缀（v1.0.0），但 CMake 的 project(VERSION) 只接受
# 纯数字；这里剥掉前缀给 CMake 用，原样保留一份给 `--version` 等展示用。
string(REGEX REPLACE "^[vV]" "" CLASHQT_VERSION_NUMBER "${CLASHQT_VERSION_STRING}")
if(NOT CLASHQT_VERSION_NUMBER MATCHES "^[0-9]+(\\.[0-9]+)*$")
    message(FATAL_ERROR
            "VERSION 里的版本号不合法：'${CLASHQT_VERSION_STRING}'（期望形如 v1.0.0 或 1.0.0）")
endif()

# file() 只在配置期读一次，CMake 不会自动把它登记成"重配置依赖"：少了这一行，
# 改完 VERSION 直接构建会继续用旧版本号（必须手工再 configure 一次）。
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
             "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")

project(clash-qt
    VERSION ${CLASHQT_VERSION_NUMBER}
    DESCRIPTION "Clash Party rewritten in Qt6 / C++"
    LANGUAGES C CXX)

# Debian 打包的版本号只能来自 debian/changelog（dpkg 的硬约束，不能动态生成）。
# 两份版本号不一致时给个提示：通常意味着改了 VERSION 却忘了 dch。
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog" CLASHQT_DEB_LINE
         LIMIT_COUNT 1)
    if(CLASHQT_DEB_LINE MATCHES "^[^(]+\\(([^)]+)\\)")
        set(CLASHQT_DEB_VERSION "${CMAKE_MATCH_1}")
        # 允许 Debian 修订号（0.1.0-1）：只比较上游版本部分
        string(REGEX REPLACE "-[0-9]+$" "" CLASHQT_DEB_UPSTREAM "${CLASHQT_DEB_VERSION}")
        if(NOT CLASHQT_DEB_UPSTREAM STREQUAL CLASHQT_VERSION_NUMBER)
            message(WARNING
                    "VERSION（${CLASHQT_VERSION_STRING}）与 debian/changelog"
                    "（${CLASHQT_DEB_VERSION}）不一致：安装包版本取 changelog，"
                    "发布前请用 `dch -v ${CLASHQT_VERSION_NUMBER}` 同步")
        endif()
    endif()
endif()

# ---------------------------------------------------------------------------
#  应用标识
#
#  源码里不再出现 "clash-qt" 字面量：名字、数据目录、辅助程序、桌面入口
#  全部由这里的四个变量派生。改软件名只需要改 project() 与显示名。
# ---------------------------------------------------------------------------
set(CLASHQT_TARGET          "${PROJECT_NAME}")   # 可执行文件 / CMake 目标名
set(CLASHQT_APP_NAME        "${PROJECT_NAME}")
set(CLASHQT_APP_DISPLAY_NAME "Clash Qt")
set(CLASHQT_TUN_HELPER      "${CLASHQT_APP_NAME}-tun")
set(CLASHQT_DESKTOP_FILE    "${CLASHQT_APP_NAME}.desktop")

# TUN 接口名受内核限制（IFNAMSIZ，最长 15 字符），不能直接等于软件名：
# 名字变了也不该把接口名顶到非法长度，所以单独派生、清洗、截断。
set(CLASHQT_TUN_DEVICE "${CLASHQT_TUN_HELPER}")
string(REGEX REPLACE "[^A-Za-z0-9_.-]" "-" CLASHQT_TUN_DEVICE "${CLASHQT_TUN_DEVICE}")
string(LENGTH "${CLASHQT_TUN_DEVICE}" CLASHQT_TUN_DEVICE_LEN)
if(CLASHQT_TUN_DEVICE_LEN GREATER 15)
    string(SUBSTRING "${CLASHQT_TUN_DEVICE}" 0 15 CLASHQT_TUN_DEVICE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 安装路径（CMAKE_INSTALL_BINDIR / DATADIR 等），供 `cmake --install`
# 与 Debian 打包共用；默认即 usr/bin、usr/share
include(GNUInstallDirs)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

option(CLASHQT_BUILD_APP "Build the Qt6 GUI application" ON)
option(CLASHQT_USE_CCACHE "Use ccache when available" ON)

find_package(OpenSSL REQUIRED)
find_package(Qt6 6.2 REQUIRED COMPONENTS Core Network)
find_package(Qt6 6.2 QUIET COMPONENTS Widgets Sql Svg Charts)

if(CLASHQT_BUILD_APP)
    foreach(_mod Widgets Sql Svg Charts)
        if(NOT Qt6${_mod}_FOUND)
            message(FATAL_ERROR
                "CLASHQT_BUILD_APP=ON 需要 Qt6 ${_mod} 组件，但未找到。"
                "请安装对应的 Qt6 开发包，或使用 -DCLASHQT_BUILD_APP=OFF 只构建内核。")
        endif()
    endforeach()
endif()

qt_standard_project_setup()

if(CLASHQT_USE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    endif()
endif()

# ---------------------------------------------------------------------------
# 第三方依赖：yaml-cpp
#
# 取用顺序（保证「有系统包就用系统包、离线也能构建」）：
#   1. 系统已安装的 yaml-cpp（find_package）—— 发行版打包时优先
#   2. git 子模块 third_party/yaml-cpp（推荐，版本已固定到 0.9.0）
#   3. 两者都不可用 → 明确报错并给出修复命令，不静默继续
#
# 子模块化说明：third_party/yaml-cpp 是 git submodule，指向
# https://github.com/jbeder/yaml-cpp.git 的 0.9.0（56e3bb55）。
# 首次克隆请使用：
#     git clone --recursive <repo>
#   或在已有克隆中执行：
#     git submodule update --init --recursive
# ---------------------------------------------------------------------------
set(CLASHQT_THIRD_PARTY "${CMAKE_CURRENT_SOURCE_DIR}/third_party")
set(CLASHQT_YAML_CPP_DIR "${CLASHQT_THIRD_PARTY}/yaml-cpp")

# 不限定版本：发行版提供的 0.7 与内置子模块的 0.9 都直接用。
# 两者只有 CMake 目标名不同（见各分支里的 TARGET 判断），代码侧用到的
# API 在 0.7 起就稳定。
find_package(yaml-cpp QUIET)

add_library(clashqt_thirdparty INTERFACE)
target_include_directories(clashqt_thirdparty INTERFACE "${CLASHQT_THIRD_PARTY}")

if(yaml-cpp_FOUND)
    message(STATUS "yaml-cpp：使用系统安装的版本 (${yaml-cpp_VERSION})")

    # 目标名随版本变化：0.8+ 导出带命名空间的 yaml-cpp::yaml-cpp，
    # 0.7 及更早的发行版包只导出 yaml-cpp。写死前者会让 0.7 在
    # Generate 阶段报 "target yaml-cpp::yaml-cpp not found"。
    if(TARGET yaml-cpp::yaml-cpp)
        target_link_libraries(clashqt_thirdparty INTERFACE yaml-cpp::yaml-cpp)
    elseif(TARGET yaml-cpp)
        target_link_libraries(clashqt_thirdparty INTERFACE yaml-cpp)
    else()
        message(FATAL_ERROR
            "已找到 yaml-cpp（${yaml-cpp_VERSION}），但没有可链接的 CMake 目标")
    endif()
elseif(EXISTS "${CLASHQT_YAML_CPP_DIR}/CMakeLists.txt")
    message(STATUS "yaml-cpp：使用内置子模块 third_party/yaml-cpp")

    set(YAML_CPP_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_TOOLS   OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_INSTALL       OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_FORMAT_SOURCE OFF CACHE BOOL "" FORCE)
    set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/yaml-cpp EXCLUDE_FROM_ALL)

    # 内置子模块固定是 0.9，导出带命名空间的目标
    target_link_libraries(clashqt_thirdparty INTERFACE yaml-cpp::yaml-cpp)
else()
    # 子模块目录存在但为空，通常是克隆时没有加 --recursive
    message(FATAL_ERROR
        "未找到 yaml-cpp。请任选一种方式解决：\n"
        "  A) 初始化子模块（推荐）：\n"
        "       git submodule update --init --recursive\n"
        "  B) 安装系统包：\n"
        "       Debian/Ubuntu/Deepin: sudo apt install libyaml-cpp-dev\n"
        "       Fedora/RHEL:          sudo dnf install yaml-cpp-devel\n"
        "       Arch:                 sudo pacman -S yaml-cpp")
endif()

# ---------------------------------------------------------------------------
#  ML-KEM-768（FIPS 203）—— REALITY 的 X25519MLKEM768 混合密钥分享所需
#
#  依赖四个 header-only 库，全部是 third_party/ 下的 git 子模块：
#      ml-kem / sha3 / randomshake / subtle
#
#  这里刻意**不调用 ml-kem 自带的 CMakeLists（它用 FetchContent 拉依赖）**：
#  那会让每次 cmake 配置都联网 clone 三个依赖，网络不通时表现为 `cmake ..`
#  长时间无输出（卡住），也无法离线编译。子模块只在首次
#  `git submodule update --init --recursive` 时联网一次，之后配置与编译完全
#  离线；CMake 仅挂载它们的 include 目录，不执行任何下游构建脚本。
# ---------------------------------------------------------------------------
set(CLASHQT_MLKEM_INCLUDES
    ${CLASHQT_THIRD_PARTY}/ml-kem/include
    ${CLASHQT_THIRD_PARTY}/sha3/include
    ${CLASHQT_THIRD_PARTY}/randomshake/include
    ${CLASHQT_THIRD_PARTY}/subtle/include)

foreach(_inc IN LISTS CLASHQT_MLKEM_INCLUDES)
    if(NOT IS_DIRECTORY "${_inc}")
        message(FATAL_ERROR
            "缺少内核所需的第三方依赖目录：${_inc}\n"
            "请先初始化子模块：\n"
            "    git submodule update --init --recursive")
    endif()
endforeach()

add_library(clashqt_mlkem INTERFACE)
target_include_directories(clashqt_mlkem INTERFACE ${CLASHQT_MLKEM_INCLUDES})

# ===========================================================================
#  内核（src/core）
#
#  该目标即项目自研内核，作为「内嵌内核」直接编入 Qt 应用；设计上只依赖
#  Qt6::Core 与 Qt6::Network，不引入任何 GUI 依赖。
#
#  当前阶段（Phase 1）已实现：
#    配置模型与 YAML 往返、域名树与 IP 集合、规则引擎（含逻辑规则与规则集）、
#    DNS 解析（hosts / UDP / TCP / DoH / fake-ip / 缓存）、
#    出站适配器（VLESS / DIRECT / REJECT / HTTP / SOCKS5 / 代理组）、
#    HTTP 与 SOCKS5 入站（含 MIXED 端口嗅探）、隧道分发与双向中继、
#    连接跟踪与流量统计、Hub 门面。
#
#  尚未实现（见 README 的阶段说明）：
#    外部控制器 REST/WebSocket、geoip/geosite 数据层、TUN/redir/tproxy 入站、
#    其余出站协议的传输层实现。
# ===========================================================================

set(CORE_PUBLIC_HEADERS
    src/core/include/base/Common.h
    src/core/include/constant/Constants.h
    src/core/include/common/Yaml.h
    src/core/include/common/Hysteria2Client.h
    src/core/include/common/JsonUtil.h
    src/core/include/common/SocketUtil.h
    src/core/include/common/Tls13Client.h
    src/core/include/common/WebSocketClient.h
    src/core/include/config/RawConfig.h
    src/core/include/config/ProxyConfig.h
    src/core/include/component/trie/DomainTrie.h
    src/core/include/component/cidr/IpCidrSet.h
    src/core/include/component/dns/DnsServer.h
    src/core/include/component/dns/Resolver.h
    src/core/include/rules/Rule.h
    src/core/include/rules/RuleParser.h
    src/core/include/adapter/Adapter.h
    src/core/include/adapter/Adapters.h
    src/core/include/adapter/VlessAdapter.h
    src/core/include/adapter/AdapterRegistry.h
    src/core/include/adapter/Hysteria2Adapter.h
    src/core/include/listener/Listener.h
    src/core/include/listener/HttpListener.h
    src/core/include/listener/Socks5Listener.h
    src/core/include/listener/MixedListener.h
    src/core/include/tunnel/ConnectionTracker.h
    src/core/include/tunnel/OutboundStream.h
    src/core/include/tunnel/VisionStream.h
    src/core/include/tunnel/WsStream.h
    src/core/include/tunnel/FlowWorker.h
    src/core/include/tunnel/UdpChannel.h
    src/core/include/tunnel/UdpWorker.h
    src/core/include/tunnel/TunDevice.h
    src/core/include/tunnel/TunIpStack.h
    src/core/include/tunnel/Tunnel.h
    src/core/include/hub/Hub.h
)

set(CORE_SOURCES
    src/core/src/base/Common.cpp
    src/core/src/constant/Constants.cpp
    src/core/src/common/Yaml.cpp
    src/core/src/common/Hysteria2Client.cpp
    src/core/src/common/JsonUtil.cpp
    src/core/src/common/SocketUtil.cpp
    src/core/src/common/Tls13Client.cpp
    src/core/src/common/WebSocketClient.cpp
    src/core/src/config/RawConfig.cpp
    src/core/src/config/ProxyConfig.cpp
    src/core/src/component/trie/DomainTrie.cpp
    src/core/src/component/cidr/IpCidrSet.cpp
    src/core/src/component/dns/DnsServer.cpp
    src/core/src/component/dns/Resolver.cpp
    src/core/src/rules/Rule.cpp
    src/core/src/rules/RuleParser.cpp
    src/core/src/adapter/Adapter.cpp
    src/core/src/adapter/AdapterRegistry.cpp
    src/core/src/adapter/Hysteria2Adapter.cpp
    src/core/src/adapter/VlessAdapter.cpp
    src/core/src/listener/Listener.cpp
    src/core/src/listener/HttpListener.cpp
    src/core/src/listener/Socks5Listener.cpp
    src/core/src/listener/MixedListener.cpp
    src/core/src/tunnel/ConnectionTracker.cpp
    src/core/src/tunnel/FlowWorker.cpp
    src/core/src/tunnel/UdpChannel.cpp
    src/core/src/tunnel/UdpWorker.cpp
    src/core/src/tunnel/TunDevice.cpp
    src/core/src/tunnel/TunIpStack.cpp
    src/core/src/tunnel/Tunnel.cpp
    src/core/src/tunnel/VisionStream.cpp
    src/core/src/tunnel/WsStream.cpp
    src/core/src/hub/Hub.cpp
)

# 内核编译为 OBJECT 库：目标文件直接编入 clash-qt，不产出 libclashqt_core.a。
# 内核不再作为独立库对外提供（也不单独产出可执行文件），无需中间归档文件。
add_library(clashqt_core OBJECT ${CORE_SOURCES} ${CORE_PUBLIC_HEADERS})
add_library(clashqt::core ALIAS clashqt_core)

target_include_directories(clashqt_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/include"
)

# 说明：OBJECT 库的 PRIVATE 依赖不会传递给链接方，而内核目标文件最终进入
# clash-qt，故这里统一用 PUBLIC 声明，让可执行文件把 Qt / OpenSSL / yaml-cpp
# 一并链上（仅有一个消费方，无依赖外溢顾虑）。
target_link_libraries(clashqt_core
    PUBLIC
        Qt6::Core
        Qt6::Network
        # yaml-cpp 的具体目标由 clashqt_thirdparty 按版本选择（系统 0.7 / 0.8
        # 或内置子模块）。这里不要再写死 yaml-cpp::yaml-cpp：带 0.7 的系统
        # 上该目标不存在，Generate 阶段会直接失败。
        clashqt_thirdparty
        # 手工 TLS 1.3（REALITY）使用 OpenSSL EVP 接口
        OpenSSL::SSL
        OpenSSL::Crypto
        # X25519MLKEM768 混合密钥交换（REALITY 新版服务器强制要求）
        clashqt_mlkem
)

target_compile_options(clashqt_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wno-unused-parameter>
)

set_target_properties(clashqt_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# 应用标识以宏的形式下发给所有使用者（内核、界面、测试）：PUBLIC 让链接
# clashqt_core 的目标自动获得，不必在每个 target 上重复一遍。
target_compile_definitions(clashqt_core PUBLIC
    CLASHQT_VERSION="${PROJECT_VERSION}"
    CLASHQT_VERSION_TAG="${CLASHQT_VERSION_STRING}"
    CLASHQT_APP_NAME="${CLASHQT_APP_NAME}"
    CLASHQT_APP_DISPLAY_NAME="${CLASHQT_APP_DISPLAY_NAME}"
    CLASHQT_TUN_HELPER_NAME="${CLASHQT_TUN_HELPER}"
    CLASHQT_TUN_DEVICE="${CLASHQT_TUN_DEVICE}"
    CLASHQT_DESKTOP_FILE="${CLASHQT_DESKTOP_FILE}"
)

# ===========================================================================
#  应用（src/app）
#
#  对应 clash-party 的 Electron 渲染层与主进程：本目标既承担界面，也承担
#  「应用侧内核管理链路」——配置持久化、配置生成（含覆写合并）、系统代理、
#  自启动、托盘。内核（clashqt_core）的目标文件直接编入本目标。
# ===========================================================================

if(CLASHQT_BUILD_APP)

set(APP_SOURCES
    src/app/src/main.cpp
    src/app/src/AppContext.cpp

    src/app/src/utils/Paths.cpp
    src/app/src/utils/AppLogger.cpp
    src/app/src/utils/SafeFile.cpp
    src/app/src/utils/Calc.cpp
    src/app/src/utils/CrashHandler.cpp

    src/app/src/config/AppConfig.cpp
    src/app/src/config/ControledKernelConfig.cpp
    src/app/src/config/ProfileConfig.cpp
    src/app/src/config/OverrideConfig.cpp
    src/app/src/config/Factory.cpp

    src/app/src/updater/UpdateChecker.cpp

    src/app/src/i18n/I18n.cpp

    src/app/src/sys/SysProxy.cpp
    src/app/src/sys/AutoRun.cpp
    src/app/src/sys/SingleInstance.cpp
    src/app/src/sys/TunController.cpp

    src/app/src/ui/MainWindow.cpp
    src/app/src/ui/UiFactory.cpp
    src/app/src/ui/Icons.cpp
    src/app/src/ui/UpdateDialog.cpp
    src/app/src/ui/pages/ProxiesPage.cpp
    src/app/src/ui/pages/ProfilesPage.cpp
    src/app/src/ui/pages/ConnectionsPage.cpp
    src/app/src/ui/pages/RulesPage.cpp
    src/app/src/ui/pages/LogsPage.cpp
    src/app/src/ui/pages/OverridePage.cpp
    src/app/src/ui/pages/TrafficPage.cpp
    src/app/src/ui/pages/SettingsPage.cpp
)

set(APP_HEADERS
    src/app/include/AppContext.h

    src/app/include/utils/Paths.h
    src/app/include/utils/AppLogger.h
    src/app/include/utils/SafeFile.h
    src/app/include/utils/Calc.h
    src/app/include/utils/CrashHandler.h

    src/app/include/config/AppConfig.h
    src/app/include/config/ControledKernelConfig.h
    src/app/include/config/ProfileConfig.h
    src/app/include/config/OverrideConfig.h
    src/app/include/config/Factory.h

    src/app/include/updater/UpdateChecker.h

    src/app/include/i18n/I18n.h

    src/app/include/sys/SysProxy.h
    src/app/include/sys/AutoRun.h
    src/app/include/sys/SingleInstance.h
    src/app/include/sys/TunController.h

    src/app/include/ui/MainWindow.h
    src/app/include/ui/UiFactory.h
    src/app/include/ui/UpdateDialog.h
    src/app/include/ui/Icons.h
    src/app/include/ui/pages/ProxiesPage.h
    src/app/include/ui/pages/ProfilesPage.h
    src/app/include/ui/pages/ConnectionsPage.h
    src/app/include/ui/pages/RulesPage.h
    src/app/include/ui/pages/LogsPage.h
    src/app/include/ui/pages/OverridePage.h
    src/app/include/ui/pages/TrafficPage.h
    src/app/include/ui/pages/SettingsPage.h
)

# 图标资源：项目根 resources/icons/ 下的 .svg 为真实文件，由 clash-qt.qrc 经 Qt 资源
# 系统编译进可执行文件 —— 保持「单个二进制、无外部文件依赖」的既有约定，
# 同时让图标能脱离 C++ 源码单独编辑与预览。
set(APP_RESOURCES
    resources/clash-qt.qrc
)

qt_add_executable(${CLASHQT_TARGET} ${APP_SOURCES} ${APP_HEADERS} ${APP_RESOURCES})
# .qrc 由 AUTORCC 编译出 qrc_*.cpp 参与链接
set_target_properties(${CLASHQT_TARGET} PROPERTIES AUTORCC ON)

# ---------------------------------------------------------------------------
#  界面翻译
#
#  源语言是英文：源码里的 tr("...") 就是英文原文。中文译文放在
#  translations/*.ts（可读、可 diff，是唯一的翻译来源），构建时由 lrelease
#  编译成 .qm 并作为资源嵌入（:/i18n/clash-qt_zh_CN.qm）。
#
#  仓库里不提交 .qm：它是编译产物，改了 .ts 重新构建即可，避免二进制文件
#  与 .ts 不一致。改了界面文案后跑 scripts/update-translations.py 同步 .ts
#  （它驱动下面的 update_translations 目标，源文件清单由 CMake 维护），
#  新增条目会标为 unfinished，需要人工补译文。
#
#  打包需要两个包，缺任一个都只是静默丢掉中文、不影响构建（只留一条警告）：
#    qt6-tools-dev    —— 提供 Qt6LinguistTools 的 CMake 包，
#                        find_package 找不到它就没有 Qt6::lrelease 这个目标
#    qt6-l10n-tools   —— 提供 lrelease / lupdate 可执行文件本身
#  注意别装成 qt6-tools-dev-tools：那是 designer/qdbusviewer/qtdiag 这类
#  开发期小工具，跟翻译流程无关（它会被 qt6-tools-dev 自动带进来）。
# ---------------------------------------------------------------------------
find_package(Qt6 QUIET COMPONENTS LinguistTools)
if(TARGET Qt6::lrelease)
    qt_add_translations(${CLASHQT_TARGET}
        TS_FILES
            translations/clash-qt_en.ts      # 英文是源语言，这份是同文对照表（校对/导出用）
            translations/clash-qt_zh_CN.ts
        RESOURCE_PREFIX "/i18n"
        LUPDATE_OPTIONS -no-obsolete
        LRELEASE_OPTIONS -removeidentical
    )
else()
    message(WARNING
        "未找到 Qt6 LinguistTools（lrelease）：界面只会有英文源文案。"
        "需要中文请安装 qt6-l10n-tools。")
endif()

target_include_directories(${CLASHQT_TARGET} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/app/include"   # 公开头文件（按模块分目录）
    "${CMAKE_CURRENT_SOURCE_DIR}/src/app/src"       # 仅 .cpp 自身所在目录
)

target_link_libraries(${CLASHQT_TARGET}
    PRIVATE
        clashqt_core
        Qt6::Widgets
        Qt6::Network
        Qt6::Sql
        Qt6::Svg
        Qt6::Charts
)

target_compile_options(${CLASHQT_TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wno-unused-parameter>
)

# -rdynamic：把可执行文件的符号导出到动态符号表，崩溃记录器（CrashHandler）
# 才能用 backtrace_symbols_fd 打印出函数名而不是裸地址
target_link_options(${CLASHQT_TARGET} PRIVATE
    $<$<PLATFORM_ID:Linux>:-rdynamic>
)

set_target_properties(${CLASHQT_TARGET} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 资源目录路径宏：图标已通过 clash-qt.qrc 内嵌，此宏供需要访问
# resources/ 原始路径的场景使用（当前无消费者，保留以便扩展）
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    target_compile_definitions(${CLASHQT_TARGET} PRIVATE
        CLASHQT_RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources")
endif()

# ---------------------------------------------------------------------------
# 安装规则
#
# `cmake --install` 与 Debian 打包（debian/rules 走 dh_auto_configure）共用
# 这一段，不另外维护 debian/install 清单，避免两处路径不一致。
#
# 注意：**不在这里设置 setuid 位**。普通安装（cmake --install 到用户目录）
# 不应该带 setuid；Debian 包由 debian/rules 的 override_dh_fixperms 处理，
# 运行期权限由应用自己的 pkexec 授权流程兜底（见 TunController）。
# ---------------------------------------------------------------------------
install(TARGETS ${CLASHQT_TARGET}
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

# 桌面入口与图标：使用 hicolor 主题的 scalable 目录，图标文件名必须与
# desktop 文件里的 Icon= 一致（源文件叫 app.svg，安装时改名）。
# desktop 文件是模板：Exec / Icon / StartupWMClass 与显示名都来自上面的
# 变量，避免改名时漏掉其中一处。
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources/clash-qt.desktop.in"
    "${CMAKE_BINARY_DIR}/${CLASHQT_DESKTOP_FILE}" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/${CLASHQT_DESKTOP_FILE}"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app.svg"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
    RENAME "${CLASHQT_APP_NAME}.svg"
)

endif() # CLASHQT_BUILD_APP

# ---------------------------------------------------------------------------
# TUN 特权辅助程序
#
# 建 TUN、配地址、日后接管路由都需要 CAP_NET_ADMIN。参照实现（clash-party）
# 的做法是把内核二进制 chown root:root + chmod +sx 做成 setuid；本项目内核是
# 进程内静态库，没法只给内核加 setuid，而让整个 GUI 以 root 跑是明确要避免的，
# 因此把「必须 root 的动作」单独放进这个极小、零依赖（不含 Qt）的程序：
#   src/tunhelper/main.cpp
# 授权由主程序在运行时用 pkexec 完成（chown/chmod），构建期不需要任何特权。
# 仅 Linux 有意义（用到 linux/if_tun.h）。
# ---------------------------------------------------------------------------
if(UNIX AND NOT APPLE AND CLASHQT_BUILD_APP)
    # 辅助程序名跟随软件名（<app>-tun），源码侧按 CLASHQT_TUN_HELPER_NAME 查找
    add_executable(${CLASHQT_TUN_HELPER} src/tunhelper/main.cpp)

    target_compile_options(${CLASHQT_TUN_HELPER} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wno-unused-parameter>
    )

    # 辅助程序不链接 Qt，拿不到 core 的 PUBLIC 宏，这里单独下发
    target_compile_definitions(${CLASHQT_TUN_HELPER} PRIVATE
        CLASHQT_TUN_HELPER_NAME="${CLASHQT_TUN_HELPER}"
        CLASHQT_APP_DISPLAY_NAME="${CLASHQT_APP_DISPLAY_NAME}"
    )

    set_target_properties(${CLASHQT_TUN_HELPER} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    # 与主程序同目录：TunController 按 applicationDirPath()/<app>-tun 查找，
    # 分开放会导致打包后找不到辅助程序
    install(TARGETS ${CLASHQT_TUN_HELPER}
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

    message(STATUS "TUN helper      : build/bin/${CLASHQT_TUN_HELPER}（运行时用 pkexec 授权）")
endif()


# ---------------------------------------------------------------------------
# 单元测试
#
#  默认关闭：日常构建不需要 Qt6::Test，也不该让 CTest 拖慢打包。
#  开启方式：
#      cmake -S . -B build -DCLASHQT_BUILD_TESTS=ON
#      cmake --build build --target all
#      ctest --test-dir build --output-on-failure
#  测试可执行文件在 build/bin/tests/，也可以单独运行某一个。
# ---------------------------------------------------------------------------
option(CLASHQT_BUILD_TESTS "Build unit tests (run with ctest)" OFF)

if(CLASHQT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


message(STATUS "----------------------------------------------------------")
message(STATUS "${CLASHQT_TARGET} ${PROJECT_VERSION}")
message(STATUS "  Build type      : ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard    : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Qt version      : ${Qt6_VERSION}")
message(STATUS "  GUI application : ${CLASHQT_BUILD_APP}")
message(STATUS "  Unit tests      : ${CLASHQT_BUILD_TESTS}")
message(STATUS "  Output dir      : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "----------------------------------------------------------")
