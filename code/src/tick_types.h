#ifndef TICK_TYPES_H
#define TICK_TYPES_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

struct tickTime {
    int hour = 0;
    int minute = 0;
};

struct TickRecord {
    std::string time = "";
    double price = 0.0;
    long long volume = 0;
    int deal_count = 0;
    std::string bs_type = "-";
    tickTime t;

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
    bool show_merge = false;
    size_t show_limit = 1000;
    std::string dir_path;
};


#endif // TICK_TYPES_H
