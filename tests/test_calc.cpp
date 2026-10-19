// ---------------------------------------------------------------------------
//  流量数值格式化
//
//  这是界面上每一秒都在刷新的文本，也是用户判断「有没有跑流量」的直接依据。
//  重点覆盖 1024 进位边界、单位后缀、以及「窄列里不溢出」的长度收缩规则：
//  数字变长时自动从 2 位小数降到 1 位、再降到取整，否则会把列宽撑开。
// ---------------------------------------------------------------------------

#include <QtTest>

#include "utils/Calc.h"

using namespace clashqt::calc;

class TestCalc : public QObject
{
    Q_OBJECT

private slots:
    void trafficBytes();
    void trafficScales();
    void trafficNeverWritesDecimalsForBytes();
    void speedHasSuffix();
    void speedUnitBoundaries();
    void numberShrinksWhenLong();
    void timeLeftVariants();
};

void TestCalc::trafficBytes()
{
    QCOMPARE(traffic(0), QStringLiteral("0 B"));
    QCOMPARE(traffic(1), QStringLiteral("1 B"));
    QCOMPARE(traffic(1023), QStringLiteral("1023 B"));
}

void TestCalc::trafficScales()
{
    QCOMPARE(traffic(1024), QStringLiteral("1.00 KB"));
    QCOMPARE(traffic(1536), QStringLiteral("1.50 KB"));
    QCOMPARE(traffic(1024 * 1024), QStringLiteral("1.00 MB"));
    QCOMPARE(traffic(qint64(1024) * 1024 * 1024), QStringLiteral("1.00 GB"));
    QCOMPARE(traffic(qint64(1024) * 1024 * 1024 * 1024), QStringLiteral("1.00 TB"));
}

void TestCalc::trafficNeverWritesDecimalsForBytes()
{
    // 字节是最小单位，显示 "1.00 B" 没有意义（原实现同样取整）
    QCOMPARE(traffic(999), QStringLiteral("999 B"));
    QVERIFY(!traffic(999).contains(QLatin1Char('.')));
}

void TestCalc::speedHasSuffix()
{
    QCOMPARE(speed(0), QStringLiteral("0 B/s"));
    QCOMPARE(speed(1024), QStringLiteral("1.00 KB/s"));
    QCOMPARE(speed(1024 * 1024), QStringLiteral("1.00 MB/s"));
}

void TestCalc::speedUnitBoundaries()
{
    // 曲线 Y 轴用它决定「除以多少」和轴标题，1024 的分界必须精确
    const SpeedUnit bytes = speedUnit(1023.0);
    QCOMPARE(bytes.divisor, 1.0);
    QCOMPARE(bytes.name, QStringLiteral("B/s"));

    const SpeedUnit kilo = speedUnit(1024.0);
    QCOMPARE(kilo.divisor, 1024.0);
    QCOMPARE(kilo.name, QStringLiteral("KB/s"));

    const SpeedUnit mega = speedUnit(1024.0 * 1024);
    QCOMPARE(mega.divisor, 1024.0 * 1024);
    QCOMPARE(mega.name, QStringLiteral("MB/s"));

    const SpeedUnit giga = speedUnit(1024.0 * 1024 * 1024);
    QCOMPARE(giga.divisor, 1024.0 * 1024 * 1024);
    QCOMPARE(giga.name, QStringLiteral("GB/s"));

    // 单位封顶在 TB/s，不能越界读到数组外
    QCOMPARE(speedUnit(1e18).name, QStringLiteral("TB/s"));
}

void TestCalc::numberShrinksWhenLong()
{
    QCOMPARE(number(0.0), QStringLiteral("0.00"));
    QCOMPARE(number(1.5), QStringLiteral("1.50"));

    // "12.35" 正好 5 个字符，仍在 2 位小数档
    QCOMPARE(number(12.345), QStringLiteral("12.35"));
    // 6 个字符 -> 1 位小数（"123.46" 是 6 个字符，保持不变）
    QCOMPARE(number(123.456), QStringLiteral("123.5"));
    // 更长 -> 直接取整
    QCOMPARE(number(1234.567), QStringLiteral("1235"));
}

void TestCalc::timeLeftVariants()
{
    // 没有速率时不能算出「0 分」这种误导性结果
    QCOMPARE(timeLeft(0, 100, 0), QStringLiteral("--"));

    // 已用完
    QCOMPARE(timeLeft(100, 100, 1024), QStringLiteral("已用完"));
    QCOMPARE(timeLeft(200, 100, 1024), QStringLiteral("已用完"));

    // 分钟级
    QCOMPARE(timeLeft(0, 1024.0 * 60 * 10, 1024), QStringLiteral("10 分"));

    // 小时级
    QCOMPARE(timeLeft(0, 1024.0 * 3600 * 2, 1024), QStringLiteral("2 小时 0 分"));

    // 天级
    QCOMPARE(timeLeft(0, 1024.0 * 86400 * 3, 1024), QStringLiteral("3 天 0 小时"));
}

QTEST_GUILESS_MAIN(TestCalc)
#include "test_calc.moc"
