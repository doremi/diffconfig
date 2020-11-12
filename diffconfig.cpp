#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <list>
#include "diffconfig.hpp"

using namespace std;

// return false if parsing failed
bool parse_line(const char *line, Config *config) {
    const char *normal = "CONFIG_";
    const char *comment = "# CONFIG_";

    bzero(config, sizeof(Config));
    if (!strncmp(line, normal, strlen(normal))) {
        // normal, try to find '='
        const char *p = line;
        char *name = config->name;
        while (*p != '=') {
            *name++ = *p++;
        }
        *name = '\0';
        ++p;                    // '='
        if (*p == '"') {
            ++p;                // skip '"'
            config->setting_type = TYPE_STRING;
            strncpy(config->setting_str, p, strlen(p));
            config->setting_str[strlen(p) - 1] = '\0';
        } else if (*p == 'y' || *p == 'n' || *p == 'm') {
            config->setting_type = TYPE_YNM;
            config->setting_ynm = *p;
        } else { // integer type
            config->setting_type = TYPE_INTEGER;
            if (*p == '0' && *(p+1) == 'x') { // hex
                config->setting_int = strtoul(p, NULL, 16);
            } else {
                config->setting_int = atoi(p);
            }
        }
        config->is_not_set = false;
    } else if (!strncmp(line, comment, strlen(comment))) {
        // comment, try to find ' ', always is not set, otherwise reture false
        const char *p = &line[2];
        char *name = config->name;
        while (*p != ' ') {
            *name++ = *p++;
        }
        *name = '\0';
        ++p;                    // ' '
        if (strcmp(p, "is not set")) {
            return false;
        }
        config->setting_type = TYPE_COMMENT_IS_NOT_SET;
        config->is_not_set = true;
    } else {
        config->setting_type = TYPE_COMMENT_REGULAR;
    }
    return true;
}

unordered_map<string, Config> map;

bool create_table(const char* filename) {
    string line;
    Config config;
    size_t lines = 1;
    fstream file;

    file.open(filename, ios::in);
    if (!file.is_open()) {
        perror(filename);
        return false;
    }

    while (getline(file, line)) {
        if (!parse_line(line.c_str(), &config)) {
            printf("Parsing error on line %ld\n", lines);
            return false;
        }
        if (config.setting_type != TYPE_COMMENT_REGULAR) {
            map[config.name] = config;
        }
        ++lines;
    }
    return true;
}

// output: printf
void lookup_table(Config *config) {
    // search hash by config->name
    // compare config->setting
    // if (exists_configs->setting == config->setting) {
    //   printf("Y\n");
    // } else if (exists->setting != NULL) {
    //   printf("A config: %c, B config: %c\n", exists->setting, config->setting);
    // } else if (!(exists->is_not_set ^ config->is_not_set)) {
    //   printf("some is not set\n");
    // }
}

#ifndef GOOGLE_TEST

void print_config(const Config &config) {
    //    Config config;
    //bool ret = parse_line("CONFIG_ILLEGAL_POINTER_VALUE=0xdead000000000000", &config);
    //printf("parse ret: %s\n", ret ? "Success" : "FAIL");
    printf("name: %s\n", config.name);
    switch (config.setting_type) {
        case TYPE_STRING:
            printf("setting string: %s\n", config.setting_str);
            break;
        case TYPE_INTEGER:
            printf("setting integer: %ld (hex: 0x%lx)\n", config.setting_int, config.setting_int);
            break;
        case TYPE_YNM:
            printf("setting ynm: %c\n", config.setting_ynm);
            break;
        case TYPE_COMMENT_IS_NOT_SET:
            printf("# COMMENT, is not set: %d\n", config.is_not_set);
            break;
        default:
            printf("ERROR! setting type is error: %d\n", config.setting_type);
            break;
    }
    if (config.setting_type != TYPE_COMMENT_IS_NOT_SET && config.is_not_set == true) {
        printf("ERROR! config is enabled but is not set!\n");
    }
}

int main2() {
    create_table("kernel.config");
    string input;
    while (getline(std::cin, input)) {
        auto it = map.find(input);
        if (it != map.end()) {
            print_config(it->second);
        } else {
            cout << "Can't find " << input << endl;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s config1 config2\n", argv[0]);
        return 2;
    }

    create_table(argv[1]);

    string line;
    Config config;
    size_t lines = 1;
    fstream file;
    list<string> not_found;

    file.open(argv[2], ios::in);
    if (!file.is_open()) {
        perror(argv[2]);
        return false;
    }

    while (getline(file, line)) {
        if (!parse_line(line.c_str(), &config)) {
            printf("Parsing error on line %ld\n", lines);
            return false;
        }
        if (config.setting_type != TYPE_COMMENT_REGULAR && config.setting_type != TYPE_COMMENT_IS_NOT_SET) {
            auto it = map.find(config.name);
            if (it != map.end()) {
                if (config != it->second) {
                    printf("Different: %s\n", config.name);
                    print_config(config);
                    print_config(it->second);
                    printf("\n");
                }
            } else {
                not_found.push_back(line);
            }
        }
        ++lines;
    }

    cout << "Can't find these configs on " << argv[1] << endl;
    for (auto it = not_found.begin(); it != not_found.end(); ++it) {
        cout << *it << endl;
    }

    return 0;
}
#endif
