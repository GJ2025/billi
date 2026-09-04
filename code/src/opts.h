#ifndef OPTS_H
#define OPTS_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include "time_seq.h"

struct ProgramOptions {
    bool show_head = false;
    bool show_price = false;
    bool show_all = false;
    bool show_will = false;
    bool show_income_ratio = false;
    bool show_super = false;
    bool show_big = false;
    bool show_middle = false;
    bool show_small = false;
    bool show_total = false;
    bool show_super_ratio = false;
    bool show_big_ratio = false;
    bool show_middle_ratio = false;
    bool show_small_ratio = false;
    bool show_total_ratio = false;
    bool show_t= false;
    SeqTime tseq;
    size_t show_limit = 10000;
    std::string lvmeng_dir_path;
    std::string data_dir_path;

    bool check_file() const {
        if (data_dir_path.empty()) {
            return true;
        }
        return false;
    }

    bool check_Dir() const {
        return !check_file();
    }
};

int parse_opt(int argc, char* argv[], ProgramOptions& opts);

#endif // OPTS_H
