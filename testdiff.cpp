#include <string>
#include "gtest/gtest.h"
#include "diffconfig.hpp"
using namespace std;

//extern "C" {
  bool parse_line(const char *line, Config *config);
  bool compare_configs(const Config *a, const Config *b);
//}

TEST(parse_line, normal_ynm) {
    auto str = "CONFIG_CC_IS_GCC=y";
    struct Config expect_config = {
        {.name = "CONFIG_CC_IS_GCC"},
        {.setting_str = ""},
        .setting_ynm = 'y',
        .setting_type = TYPE_YNM,
        .setting_int = 0,
        .is_not_set = false,
    };
    Config config;
    EXPECT_TRUE(parse_line(str, &config));
    EXPECT_EQ(expect_config, config);
}

TEST(parse_line, normal_string) {
    auto str = "CONFIG_DEFAULT_HOSTNAME=\"(none)\"";
    struct Config expect_config = {
        {.name = "CONFIG_DEFAULT_HOSTNAME"},
        {.setting_str = "(none)"},
        .setting_ynm = '\0',
        .setting_type = TYPE_STRING,
        .setting_int = 0,
        .is_not_set = false,
    };
    Config config;
    EXPECT_TRUE(parse_line(str, &config));
    EXPECT_EQ(expect_config, config);
}

TEST(parse_line, normal_integer) {
    auto str = "CONFIG_HZ=250";
    struct Config expect_config = {
        {.name = "CONFIG_HZ"},
        {.setting_str = ""},
        .setting_ynm = '\0',
        .setting_type = TYPE_INTEGER,
        .setting_int = 250,
        .is_not_set = false,
    };
    Config config;
    EXPECT_TRUE(parse_line(str, &config));
    EXPECT_EQ(expect_config, config);
}

TEST(parse_line, normal_integer_hex) {
    auto str = "CONFIG_ILLEGAL_POINTER_VALUE=0xdead000000000000";
    struct Config expect_config = {
        {.name = "CONFIG_ILLEGAL_POINTER_VALUE"},
        {.setting_str = ""},
        .setting_ynm = '\0',
        .setting_type = TYPE_INTEGER,
        .setting_int = 0xdead000000000000,
        .is_not_set = false,
    };
    Config config;
    EXPECT_TRUE(parse_line(str, &config));
    EXPECT_EQ(expect_config, config);
}

TEST(parse_line, comment_is_not_set) {
    auto str = "# CONFIG_VIRT_CPU_ACCOUNTING_GEN is not set";
    struct Config expect_config = {
        {.name = "CONFIG_VIRT_CPU_ACCOUNTING_GEN"},
        {.setting_str = ""},
        .setting_ynm = '\0',
        .setting_type = TYPE_COMMENT_IS_NOT_SET,
        .setting_int = 0,
        .is_not_set = true,
    };
    Config config;
    EXPECT_TRUE(parse_line(str, &config));
    EXPECT_EQ(expect_config, config);
}

TEST(parse_line, comment_regular) {
    auto str = "# Automatically generated file; DO NOT EDIT.";
    Config config;
    EXPECT_TRUE(parse_line(str, &config));
    EXPECT_EQ(config.setting_type, TYPE_COMMENT_REGULAR);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
