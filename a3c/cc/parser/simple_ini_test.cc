#include "simple_ini.h"
#include "gtest/gtest.h"

TEST(IniTest, ReadValue) {

    CSimpleIniA ini(1, 0, 0);
    SI_Error rc = ini.LoadFile("/Users/sebastianlettner/GoogleDrive/MachineLearning/a3c-framework/a3c/cc/parser/test.ini");
    if (rc < 0) {
        throw std::runtime_error("Failed to load file");
    }

    const char * v = ini.GetValue("TestSection",
        "test_value_2", NULL);
    int v_int = atoi(v);
    ASSERT_EQ(v_int, 3);

}

