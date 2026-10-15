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

project(clash-qt
    VERSION 0.1.0
    DESCRIPTION "Clash Party rewritten in Qt6 / C++"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

find_package(yaml-cpp QUIET)

add_library(clashqt_thirdparty INTERFACE)
target_include_directories(clashqt_thirdparty INTERFACE "${CLASHQT_THIRD_PARTY}")

if(yaml-cpp_FOUND)
    message(STATUS "yaml-cpp：使用系统安装的版本 (${yaml-cpp_VERSION})")
    target_link_libraries(clashqt_thirdparty INTERFACE yaml-cpp::yaml-cpp)
elseif(EXISTS "${CLASHQT_YAML_CPP_DIR}/CMakeLists.txt")
    message(STATUS "yaml-cpp：使用内置子模块 third_party/yaml-cpp")

    set(YAML_CPP_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_TOOLS   OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_INSTALL       OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_FORMAT_SOURCE OFF CACHE BOOL "" FORCE)
    set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/yaml-cpp EXCLUDE_FROM_ALL)

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
    src/core/include/common/JsonUtil.h
    src/core/include/common/SocketUtil.h
    src/core/include/common/Tls13Client.h
    src/core/include/common/WebSocketClient.h
    src/core/include/config/RawConfig.h
    src/core/include/config/ProxyConfig.h
    src/core/include/component/trie/DomainTrie.h
    src/core/include/component/cidr/IpCidrSet.h
    src/core/include/component/dns/Resolver.h
    src/core/include/rules/Rule.h
    src/core/include/rules/RuleParser.h
    src/core/include/adapter/Adapter.h
    src/core/include/adapter/Adapters.h
    src/core/include/adapter/VlessAdapter.h
    src/core/include/adapter/AdapterRegistry.h
    src/core/include/listener/Listener.h
    src/core/include/listener/HttpListener.h
    src/core/include/listener/Socks5Listener.h
    src/core/include/listener/MixedListener.h
    src/core/include/tunnel/ConnectionTracker.h
    src/core/include/tunnel/OutboundStream.h
    src/core/include/tunnel/VisionStream.h
    src/core/include/tunnel/WsStream.h
    src/core/include/tunnel/Tunnel.h
    src/core/include/hub/Hub.h
)

set(CORE_SOURCES
    src/core/src/base/Common.cpp
    src/core/src/constant/Constants.cpp
    src/core/src/common/Yaml.cpp
    src/core/src/common/JsonUtil.cpp
    src/core/src/common/SocketUtil.cpp
    src/core/src/common/Tls13Client.cpp
    src/core/src/common/WebSocketClient.cpp
    src/core/src/config/RawConfig.cpp
    src/core/src/config/ProxyConfig.cpp
    src/core/src/component/trie/DomainTrie.cpp
    src/core/src/component/cidr/IpCidrSet.cpp
    src/core/src/component/dns/Resolver.cpp
    src/core/src/rules/Rule.cpp
    src/core/src/rules/RuleParser.cpp
    src/core/src/adapter/Adapter.cpp
    src/core/src/adapter/AdapterRegistry.cpp
    src/core/src/adapter/VlessAdapter.cpp
    src/core/src/listener/Listener.cpp
    src/core/src/listener/HttpListener.cpp
    src/core/src/listener/Socks5Listener.cpp
    src/core/src/listener/MixedListener.cpp
    src/core/src/tunnel/ConnectionTracker.cpp
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
        # yaml-cpp 在 Yaml.cpp 中被 include，头文件搜索路径需对翻译单元可见
        yaml-cpp::yaml-cpp
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

    src/app/src/config/AppConfig.cpp
    src/app/src/config/ControledKernelConfig.cpp
    src/app/src/config/ProfileConfig.cpp
    src/app/src/config/OverrideConfig.cpp
    src/app/src/config/Factory.cpp

    src/app/src/sys/SysProxy.cpp
    src/app/src/sys/AutoRun.cpp
    src/app/src/sys/SingleInstance.cpp

    src/app/src/ui/MainWindow.cpp
    src/app/src/ui/UiFactory.cpp
    src/app/src/ui/pages/ProxiesPage.cpp
    src/app/src/ui/pages/ProfilesPage.cpp
    src/app/src/ui/pages/ConnectionsPage.cpp
    src/app/src/ui/pages/RulesPage.cpp
    src/app/src/ui/pages/LogsPage.cpp
    src/app/src/ui/pages/TrafficPage.cpp
    src/app/src/ui/pages/SettingsPage.cpp
)

set(APP_HEADERS
    src/app/src/AppContext.h

    src/app/src/utils/Paths.h
    src/app/src/utils/AppLogger.h
    src/app/src/utils/SafeFile.h
    src/app/src/utils/Calc.h

    src/app/src/config/AppConfig.h
    src/app/src/config/ControledKernelConfig.h
    src/app/src/config/ProfileConfig.h
    src/app/src/config/OverrideConfig.h
    src/app/src/config/Factory.h

    src/app/src/sys/SysProxy.h
    src/app/src/sys/AutoRun.h
    src/app/src/sys/SingleInstance.h

    src/app/src/ui/MainWindow.h
    src/app/src/ui/UiFactory.h
    src/app/src/ui/pages/ProxiesPage.h
    src/app/src/ui/pages/ProfilesPage.h
    src/app/src/ui/pages/ConnectionsPage.h
    src/app/src/ui/pages/RulesPage.h
    src/app/src/ui/pages/LogsPage.h
    src/app/src/ui/pages/TrafficPage.h
    src/app/src/ui/pages/SettingsPage.h
)

qt_add_executable(clash-qt ${APP_SOURCES} ${APP_HEADERS})

target_include_directories(clash-qt PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/app/src")

target_link_libraries(clash-qt
    PRIVATE
        clashqt_core
        Qt6::Widgets
        Qt6::Network
        Qt6::Sql
        Qt6::Svg
        Qt6::Charts
)

target_compile_definitions(clash-qt PRIVATE
    CLASHQT_VERSION="${PROJECT_VERSION}"
    CLASHQT_APP_NAME="clash-qt"
)

target_compile_options(clash-qt PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wno-unused-parameter>
)

set_target_properties(clash-qt PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 资源（图标等）后续阶段以 .qrc 引入；此处保证目录存在时被识别
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/app/resources")
    target_compile_definitions(clash-qt PRIVATE
        CLASHQT_RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/app/resources")
endif()

endif() # CLASHQT_BUILD_APP

message(STATUS "----------------------------------------------------------")
message(STATUS "clash-qt ${PROJECT_VERSION}")
message(STATUS "  Build type      : ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard    : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Qt version      : ${Qt6_VERSION}")
message(STATUS "  GUI application : ${CLASHQT_BUILD_APP}")
message(STATUS "  Output dir      : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "----------------------------------------------------------")
