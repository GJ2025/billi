#ifndef TICK_TYPES_H
#define TICK_TYPES_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "time_seq.h"

struct TickRecord {
    std::string time = "";
    double price = 0.0;
    long long volume = 0;
    int deal_count = 0;
    std::string bs_type = "-";
    tickTime t;

    bool full() const {
        // 如果 time 为空，通常代表这是一个未被正确赋值或初始化为空的记录
        return deal_count != 0;
    }

    // 为了保持内联，operator>> 依然放在这里
    friend std::istream& operator>>(std::istream& is, TickRecord& record) {
        if (is >> record.time >> record.price >> record.volume >> record.deal_count) {
            if (!(is >> record.bs_type)) {
                record.bs_type = "-";
            }
        }
        std::stringstream ss(record.time);
        char delimiter;
        if (ss >> record.t.hour >> delimiter >> record.t.minute) {
            // 解析成功
        }
        is.clear();
        return is;
    }
};


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


#endif // TICK_TYPES_H
