#pragma once

#include <QString>
#include <QtGlobal>

// ---------------------------------------------------------------------------
//  wintun.dll 的动态封装（Windows）
//
//  wintun.dll 不随系统提供，必须与程序一起分发，所以这里动态加载而不是链接
//  导入库：缺 dll 时给出「找不到 wintun.dll」的明确提示，而不是双击就报
//  「缺少 DLL」起不来。
//
//  适配器 / 会话句柄都是指针，统一用 quintptr 传递，避免在头文件里引入
//  windows.h。适配器 LUID 用于 IP Helper 配地址与路由。
// ---------------------------------------------------------------------------

namespace core {
namespace wintun {

// dll 是否可用（与 exe 同目录或系统搜索路径）
bool    available();
QString versionText();

// 打开已存在的适配器，没有则创建；返回 0 表示失败
quintptr openAdapter(const QString &name, QString *error);
void     closeAdapter(quintptr adapter);
quint64  adapterLuid(quintptr adapter);

quintptr startSession(quintptr adapter, QString *error);
void     endSession(quintptr session);
quintptr readWaitEvent(quintptr session);

// >0 为收到的字节数；0 表示暂时没有包；<0 表示会话已结束
int  receivePacket(quintptr session, char *buffer, int capacity);
bool sendPacket(quintptr session, const char *data, int size);

} // namespace wintun
} // namespace core
