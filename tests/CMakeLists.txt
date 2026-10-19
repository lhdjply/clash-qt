# ---------------------------------------------------------------------------
#  单元测试
#
#  只测「纯逻辑」：不碰网络、不需要 root、不弹窗口。这些都是曾经真实出过
#  故障的地方（CIDR 字节序、规则覆写合并、fake-ip 分配、DNS 编解码），
#  回归成本最低，收益最直接。
#
#  用法：
#      cmake -S . -B build -DCLASHQT_BUILD_TESTS=ON
#      cmake --build build
#      ctest --test-dir build --output-on-failure
#  或只跑一个：build/bin/tests/test_rules
#
#  内核是 OBJECT 库，直接 target_link_libraries 即可把它的目标文件带进来，
#  不需要额外产出静态库（与主程序的链接方式一致）。
# ---------------------------------------------------------------------------

find_package(Qt6 6.2 REQUIRED COMPONENTS Test)

set(CLASHQT_TEST_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/tests")

function(clashqt_add_test name)
    add_executable(${name} ${name}.cpp ${ARGN})

    target_link_libraries(${name} PRIVATE clashqt_core Qt6::Test)
    # 应用侧源码（Calc / Factory 等）与主程序用同样的 include 根
    target_include_directories(${name} PRIVATE "${CMAKE_SOURCE_DIR}/src/app/src")

    target_compile_options(${name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wno-unused-parameter>
    )

    set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CLASHQT_TEST_OUTPUT_DIR}"
    )

    add_test(NAME ${name} COMMAND ${name})

    # 测试不写用户真实配置：统一的 XDG_CONFIG_HOME / 日志级别隔离
    set_tests_properties(${name} PROPERTIES
        ENVIRONMENT "XDG_CONFIG_HOME=${CMAKE_BINARY_DIR}/test-home;QT_QPA_PLATFORM=offscreen"
        TIMEOUT 120
    )
endfunction()

# ---- 内核：基础设施 -------------------------------------------------------
clashqt_add_test(test_common)        # IpNet 解析/包含、字符串工具
clashqt_add_test(test_domain_trie)   # 域名后缀树的精确/后缀匹配
clashqt_add_test(test_cidr)          # CIDR 集合与最长前缀优先
clashqt_add_test(test_rules)         # 规则解析、匹配、逻辑规则、停用
clashqt_add_test(test_dns_codec)     # DNS 报文编解码（含名字压缩指针）
clashqt_add_test(test_fakeip)        # fake-ip 池的分配/复用/反查
clashqt_add_test(test_yaml)          # YAML <-> JSON 往返
clashqt_add_test(test_json_util)     # 深合并与 <key> / key! / +key 修饰语法
clashqt_add_test(test_tun_icmp)      # TUN 数据面的 ICMP：fake-ip 应答 / 真实 IP 不可达

# ---- 应用：纯逻辑 ---------------------------------------------------------
# Calc 与冲突检测等工具函数只依赖 Qt Core，单独把 .cpp 编进测试目标即可，
# 不必为了测试去重构主程序的构建图。
clashqt_add_test(test_calc
    "${CMAKE_SOURCE_DIR}/src/app/src/utils/Calc.cpp")

# 数据目录约定：<XDG_CONFIG_HOME>/clash-qt（不能再多一层），以及旧布局的迁移
clashqt_add_test(test_paths
    "${CMAKE_SOURCE_DIR}/src/app/src/utils/Paths.cpp")

# 规则覆写合并：需要配置层（AppConfig / ProfileConfig / Factory）。
# 这些 .cpp 都不依赖 Widgets，直接编进测试目标即可；测试用临时
# XDG_CONFIG_HOME 隔离，不会碰到用户真实配置。
clashqt_add_test(test_rule_override
    "${CMAKE_SOURCE_DIR}/src/app/src/config/AppConfig.cpp"
    "${CMAKE_SOURCE_DIR}/src/app/src/config/ControledKernelConfig.cpp"
    "${CMAKE_SOURCE_DIR}/src/app/src/config/ProfileConfig.cpp"
    "${CMAKE_SOURCE_DIR}/src/app/src/config/OverrideConfig.cpp"
    "${CMAKE_SOURCE_DIR}/src/app/src/config/Factory.cpp"
    "${CMAKE_SOURCE_DIR}/src/app/src/utils/Paths.cpp"
    "${CMAKE_SOURCE_DIR}/src/app/src/utils/AppLogger.cpp"
    "${CMAKE_SOURCE_DIR}/src/app/src/utils/SafeFile.cpp")
