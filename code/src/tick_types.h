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

struct buy_action {
    double up = 0.0;
    double down = 0.0;
    double keep = 0.0;
};

struct stream_sum {
    buy_action sale;
    buy_action buy;
    buy_action middle;
};

struct StreamRecord {
    std::vector<TickRecord> records;
    double gap = 0.0;
};

struct HeadTickData {
    TickRecord v_924;
    TickRecord pre_924;
    TickRecord v_925;
    double ratio_change_924 = 0.0;
    double ratio_change_pre_924 = 0.0;
};

struct DailyMetrics {
    long long valid_records_count = 0;
    double closing_price = 0.0;
    double am_closing_price = 0.0;
    long long am_vol = 0;
    long long pm_vol = 0;
    double total_turnover = 0.0;
    double am_turnover = 0.0; 
    double am_inflow = 0.0;
    double am_outflow = 0.0;
    double pm_inflow = 0.0;
    double pm_outflow = 0.0;

    HeadTickData head_data;
    bool head_calculated = false;
    TickRecord first_record;
    TickRecord last_record;
    stream_sum sum_info;
};

struct DayOutputMetrics {
    long long ticks_count = 0;  
    double pre_closing_price = 0.0;        
    double closing_price = 0.0;       
    double am_closing_price = 0.0;     
    double am_net_inflow_wan = 0.0;
    double pm_net_inflow_wan = 0.0;
    double net_inflow_wan = 0.0;
    double total_turnover_wan = 0.0;
    double am_turnover_wan = 0.0;     
    double am_turnover_ratio = 0.0;
    double total_vol_wan = 0.0;
    double am_vol_wan = 0.0;
    double avg_price = 0.0;
    double inflow_ratio = 0.0;
    double avg_vol_per_tick = 0.0;
    double net_per_change = 0.0;
    double pct_change = 0.0;
    double am_pct_change = 0.0;
    double start_change = 0.0;

    std::string date_str = "";
    double historical_total_inflow = 0.0;

    TickRecord first_record;
    TickRecord last_record;

    HeadTickData head_data;
    stream_sum sum_info;
};

#endif // TICK_TYPES_H
