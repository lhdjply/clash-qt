#pragma once

// ---------------------------------------------------------------------------
//  内核配置生成
//
//  对应参照实现的 src/main/core/factory.ts。
//
//  生成链路（顺序不可调换）：
//     订阅原始内容
//       → 全局覆写（yaml 合并）
//       → 订阅级覆写（yaml 合并）
//       → 规则覆写（rules/<profileId>.yaml 的 prepend / append / delete）
//       → 合并受控内核配置（端口 / DNS / TUN / 嗅探 等）
//       → 写出 work/config.yaml
//
//  为什么受控配置放在最后合并：订阅内容可能自带同名段落（如 dns），
//  而用户在本应用里对端口、DNS 开关的设置必须拥有最终决定权，
//  否则会出现「界面上关了 DNS，实际仍在用订阅里的 DNS」这类问题。
// ---------------------------------------------------------------------------

#include <QString>
#include <QStringList>

#include <optional>

#include "config/RawConfig.h"

namespace clashqt {

class AppConfig;
class ControledKernelConfig;
class ProfileConfig;
class OverrideConfig;

class Factory {
public:
    struct Result {
        bool    success = false;
        QString error;
        // 生成过程中的非致命问题（例如遇到尚未支持的 js 覆写）
        QStringList warnings;
        QString     profileId;
    };

    Factory(AppConfig *appConfig, ControledKernelConfig *controledConfig,
            ProfileConfig *profileConfig, OverrideConfig *overrideConfig);

    // 生成内核配置并写入 work/config.yaml
    Result generate();

    // 仅生成不落盘（用于配置预览）
    std::optional<core::RawConfig> generateInMemory(Result *result = nullptr);

    // 最近一次生成出的运行时配置（含被内核忽略的应用层内容）
    const core::RawConfig &runtimeConfig() const { return m_runtimeConfig; }

    // 规则覆写文件路径（供界面编辑）
    QString ruleOverridePath() const;

private:
    // 应用一组 yaml 覆写；js 覆写会被记录到 warnings 并跳过
    bool applyOverrides(core::RawConfig *config, const QStringList &overrideIds,
                        QStringList *warnings);

    // 应用规则覆写（prepend / append / delete）
    void applyRuleOverride(core::RawConfig *config, const QString &profileId,
                           QStringList *warnings);

    AppConfig             *m_appConfig = nullptr;
    ControledKernelConfig *m_controledConfig = nullptr;
    ProfileConfig         *m_profileConfig = nullptr;
    OverrideConfig        *m_overrideConfig = nullptr;

    core::RawConfig m_runtimeConfig;
};

} // namespace clashqt
