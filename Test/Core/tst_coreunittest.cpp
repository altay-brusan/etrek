#include <QtTest>

// add necessary includes here

class CoreUnitTest : public QObject
{
    Q_OBJECT

public:
    CoreUnitTest();
    ~CoreUnitTest();

private slots:
    void test_case1();
};

CoreUnitTest::CoreUnitTest() {}

CoreUnitTest::~CoreUnitTest() {}

void CoreUnitTest::test_case1() {}

QTEST_APPLESS_MAIN(CoreUnitTest)

#include "tst_coreunittest.moc"
