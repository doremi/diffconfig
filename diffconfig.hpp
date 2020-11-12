#pragma once

#include <stdint.h>

enum SettingType {
    TYPE_STRING = 0,
    TYPE_INTEGER,
    TYPE_YNM,
    TYPE_COMMENT_IS_NOT_SET,
    TYPE_COMMENT_REGULAR
};

struct Config {
    char name[128];
    char setting_str[128];
    char setting_ynm;
    int setting_type;
    uint64_t setting_int;
    bool is_not_set;
    Config& operator=(const Config* other) {
        memcpy(this, other, sizeof(Config));
        return *this;
    }
    bool operator==(const Config& other) const {
        return (memcmp(this, &other, sizeof(Config)) == 0);
    }
    bool operator!=(const Config& other) const {
        return !(*this == other);
    }

};

