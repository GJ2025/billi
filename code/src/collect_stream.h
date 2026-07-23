#ifndef COLLECT_STREAM_H
#define COLLECT_STREAM_H

#include <string>
#include <vector>
#include <iomanip>
#include "common.h"
#include "tick_types.h"

struct trade {
    double money = 0.0;
    size_t volume = 0;
};


struct deal_price {
    trade up;
    trade down;
    trade keep;
};

struct bs_action_group {
    deal_price buy;
    deal_price sale;
    deal_price neutral; 
};

struct stream_sum {
    bs_action_group super;
    bs_action_group big;
    bs_action_group middle;
    bs_action_group small;
};

struct deal_bsn {
    trade buy;
    trade sale;
    trade neutral;
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
    long long ticks_count = 0;
    double closing_price = 0.0;
    // long long am_vol = 0;
    // long long pm_vol = 0;
    // double pm_turnover = 0.0;
    // double am_turnover = 0.0; 
    // double am_inflow = 0.0;
    // double am_outflow = 0.0;
    // double pm_inflow = 0.0;
    // double pm_outflow = 0.0;

    HeadTickData head_data;
    bool head_calculated = false;
    TickRecord first_record;
    TickRecord last_record;
    stream_sum stream_sum_info;

    deal_bsn deal_super_bsn;
    deal_bsn deal_big_bsn;
    deal_bsn deal_middle_bsn;
    deal_bsn deal_small_bsn;
    deal_bsn deal_total_bsn;

    deal_price deal_super_price;
    deal_price deal_big_price;
    deal_price deal_middle_price;
    deal_price deal_small_price;
    deal_price deal_total_price; 
};


struct DayOutputMetrics {
    
    double pre_closing_price = 0.0;        
    double am_net_inflow_wan = 0.0;
    double pm_net_inflow_wan = 0.0;
    double am_turnover_wan = 0.0;      
    double am_turnover_ratio = 0.0;
    double am_vol_wan = 0.0;
    double am_pct_change = 0.0;

    double net_inflow_wan = 0.0;
    double total_turnover_wan = 0.0;

    double total_vol_wan = 0.0;
    double avg_price = 0.0;
    double inflow_ratio = 0.0;
    double avg_vol_per_tick = 0.0;
    double net_per_change = 0.0;
    double pct_change = 0.0;
    double start_change = 0.0;

    std::string date_str = "";
    double historical_total_inflow = 0.0;

    DailyMetrics metrics;
    DailyMetrics am_metrics;


   
};

struct Col {
    std::string name;
    int width;
    bool visible = true;
};

inline const std::vector<Col> will_price_table_cols = {
    {"Date", 11}, 
    {"Buy-Dn", 12, false}, 
    {"Buy-Kp", 12, false},  
    {"Buy-Up", 12},
    {"Sale-Dn", 12,true}, 
    {"Sale-Kp", 12, false}, 
    {"Sale-Up", 12,false}, 
    {"Neutral-Dn", 12, false},
    {"Neutral-Kp", 12, false},  
    {"Neutral-Up", 12, false},
    {"Tot-Buy", 12}, 
    {"Tot-Sale", 12},
    {"Tot-Neutral", 12, true},    
    {"Tot-Up", 12}, 
    {"Tot-Dn", 12},
    {"Tot-Kp", 12}, 
    {"WILL-Net", 10}, 
    {"PRICE-Net", 12},     
    {"Money", 12},  
    {"Volume", 12},
    {"Pre", 5},     
    {"StartCh", 9}, 
    {"PctCh", 9},   
    {"Close", 5}
};

static const std::vector<Col> will_table_cols = {
    {"Date", 11}, 
    {"Super-Buy", 12, false}, 
    {"Super-Sale", 10, false}, 
    
    {"Big-Buy", 12, false},  
    {"Big-Sale", 12, false},   
   
    {"Mid-Buy", 12, false},    
    {"Mid-Sale", 12, false},   
    


    {"Small-Buy", 9, false},  
    {"Small-Sale", 10, false}, 
    
    {"Super-NET", 12},
    {"Big-NET", 12},    
    {"Mid-NET", 12},
    {"Small-NET", 12},
    {"Tot-NET", 12},

    {"Tot-Buy", 12},    
    {"Tot-Sale", 12},
    {"Tot-Neutral", 12}, 

    {"Money", 12},     
    {"Volume", 12},
    {"Pre", 5},        
    {"StartCh", 9},    
    {"PctCh", 9}, 
    {"Close", 5}
};

static const std::vector<Col> price_table_cols = {
    {"Date", 11},

    {"Super-Up", 9, false}, 
    {"Super-Dn", 9, false}, 
    

    {"Big-Up", 9, false},  
    {"Big-Dn", 9, false},   
    
    {"Mid-Up", 9, false},  
    {"Mid-Dn", 9, false}, 
    

    {"Small-Up", 9, false}, 
    {"Small-Dn", 9, false}, 
    

    {"Super-NET", 12},
    {"Big-NET", 9},
    {"Mid-NET", 9},
    {"Small-NET", 9},
    {"Tot-NET", 12},

    {"Tot-Up", 12},  
    {"Tot-Dn", 12},
    {"Tot-KEEP", 12},

    {"KEEP/ALL", 8},

    {"Money", 12},   
    {"Volume", 12},
    {"Pre", 5},      
    {"StartCh", 9},  
    {"PctCh", 9}, 
    {"Close", 5}
};

// 专为 print_merge 准备的配置
static const std::vector<Col> merge_table_cols = {
    {"Date", 11},
    {"TotBuy(BSN)", 12}, 
    {"TotSale(BSN)", 12}, 
    {"TotNeu(BSN)", 12},
    {"TotUp(PRC)", 12},  
    {"TotDn(PRC)", 12},   
    {"TotKp(PRC)", 12},
    {"BSN-Net", 16},     
    {"PRC-Net", 16},
    {"Money", 12},       
    {"Volume", 12},
    {"Pre", 5},          
    {"StartCh", 9},       
    {"PctCh", 9}, 
    {"Close", 5}
};

static const std::vector<Col> data_all_table_cols = {
    {"Date", 11}, 
    {"Ticks", 5}, 
    {"AM-Vol(W)", 9, false},
    {"AM-Turn(W)", 11, false}, 
    {"AM-Turn%", 11}, 
    {"AvgVol/Tick", 11},
    {"TotTurn(W)", 11},
    {"Vol(W)", 9},  

    {"NetIn(W)", 10},
    {"AM-NetIn(W)", 11}, 
    {"PM-NetIn(W)", 11}, 
    {"Inflow%", 9, false},
    {"NetPer%", 9,false}, 
    {"HistNetIn(W)", 11, false}, 

    {"AvgPrice", 9},
    {"AM-Close", 8, true}, 
    {"AM-Pct%", 8, true}, 
    {"Close", 7}, 
    {"Pct%", 8}, 

    {"Divergence", 20}
};

template<typename T>
inline void print_next(const T& val, int& index, const std::vector<Col>& cols) {
    if (index < (int)cols.size() && cols[index].visible) {
        std::cout << std::setw(cols[index].width) << val << " | ";
    }

    index++;
}

template<typename T>
inline void print_next_pos(const T& val, int& index, const std::vector<Col>& cols) {
    if (index < (int)cols.size() && cols[index].visible) {
        std::cout << std::showpos << std::setw(cols[index].width) << val << " | " << std::noshowpos;
    }
    index++;
}

inline std::string format_inflow(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) 
       << value;
    return ss.str();
}

inline std::string format_percent_value(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2)
        << value
        << "%";
    return ss.str();
}

inline double sum_money(const deal_price& price) { 
    return price.up.money + price.down.money + price.keep.money; 
}


inline size_t sum_volume(const deal_price& price) { 
    return price.up.volume + price.down.volume + price.keep.volume; 
}

inline void add_trade(trade& target, const trade& s, const trade& b, const trade& m, const trade& sm) {
    target.money += (s.money + b.money + m.money + sm.money);
    target.volume += (s.volume + b.volume + m.volume + sm.volume);
}

inline void sum_bsn_buy(deal_bsn& s, deal_bsn& b, deal_bsn& m, deal_bsn& sm, deal_bsn& total) {
    add_trade(total.buy, s.buy, b.buy, m.buy, sm.buy);
}

inline void sum_bsn_sale(deal_bsn& s, deal_bsn& b, deal_bsn& m, deal_bsn& sm, deal_bsn& total) {
    add_trade(total.sale, s.sale, b.sale, m.sale, sm.sale);
}

inline void sum_bsn_neutral(deal_bsn& s, deal_bsn& b, deal_bsn& m, deal_bsn& sm, deal_bsn& total) {
    add_trade(total.neutral, s.neutral, b.neutral, m.neutral, sm.neutral);
}


inline void sum_price_up(deal_price& s, deal_price& b, deal_price& m, deal_price& sm, deal_price& total) {
    add_trade(total.up, s.up, b.up, m.up, sm.up);
}

inline void sum_price_down(deal_price& s, deal_price& b, deal_price& m, deal_price& sm, deal_price& total) {
    add_trade(total.down, s.down, b.down, m.down, sm.down);
}

inline void sum_price_keep(deal_price& s, deal_price& b, deal_price& m, deal_price& sm, deal_price& total) {
    add_trade(total.keep, s.keep, b.keep, m.keep, sm.keep);
}


inline void print_decorative_line(int total_width, const std::string& left_title, const std::string& right_title) {
    int line_len = total_width - 3;
    
    int left_len = static_cast<int>(left_title.length());
    int right_len = static_cast<int>(right_title.length());
    
    int mid_space = line_len - left_len - right_len - 4; 
    if (mid_space < 2) mid_space = 2;

    std::cout << "[ " << left_title << " ]" 
              << std::string(mid_space, '-') 
              << "[ " << right_title << " ]" 
              << std::endl;
}


inline void print__headers(const std::string& title, const std::vector<Col>& cols) {
    std::cout << std::left;
    int total_width = 0;

    for (const auto& col : cols) {
        if (col.visible) {
            std::cout << std::setw(col.width) << col.name << " | ";
            total_width += (col.width + 3);
        }
    }
    std::cout << std::endl;

    print_decorative_line(total_width, title, title);
}

inline void print_slim_price(DayOutputMetrics& out, bs_action_group& super, deal_bsn& bsn, deal_price& price, const std::vector<Col>& cols) {
    int i = 0;
    std::cout << std::left << std::fixed << std::setprecision(2);

    print_next(out.date_str, i, cols);
    print_next(super.buy.down.money / WAN, i, cols);
    print_next(super.buy.keep.money / WAN, i, cols);
    print_next(super.buy.up.money / WAN, i, cols);

    print_next(super.sale.down.money / WAN, i, cols);
    print_next(super.sale.keep.money / WAN, i, cols);
    print_next(super.sale.up.money / WAN, i, cols);

    print_next(super.neutral.down.money / WAN, i, cols);
    print_next(super.neutral.keep.money / WAN, i, cols);
    print_next(super.neutral.up.money / WAN, i, cols);

    print_next(bsn.buy.money / WAN, i, cols);
    print_next(bsn.sale.money / WAN, i, cols);
    print_next(bsn.neutral.money / WAN, i, cols);

    print_next(price.up.money / WAN, i, cols);
    print_next(price.down.money / WAN, i, cols);
    print_next(price.keep.money / WAN, i, cols);

    print_next_pos((bsn.buy.money - bsn.sale.money) / WAN, i, cols);
    print_next_pos((price.up.money - price.down.money) / WAN, i, cols);

    print_next((bsn.buy.money + bsn.sale.money + bsn.neutral.money) / WAN, i, cols);
    print_next((bsn.buy.volume + bsn.sale.volume + bsn.neutral.volume)/ WAN, i, cols);
    
    print_next(out.pre_closing_price, i, cols);

    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change, i, cols);

    // print_next(out.pm_closing_price, i, cols);
    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}


inline void print_will(DayOutputMetrics& out, DailyMetrics& metrics, const std::vector<Col>& cols) {
    int i = 0;
    std::cout << std::left << std::fixed << std::setprecision(2);

    double jing_super = metrics.deal_super_bsn.buy.money - metrics.deal_super_bsn.sale.money;
    double jing_big = metrics.deal_big_bsn.buy.money - metrics.deal_big_bsn.sale.money;
    double jing_middle = metrics.deal_middle_bsn.buy.money - metrics.deal_middle_bsn.sale.money;
    double jing_small = metrics.deal_small_bsn.buy.money - metrics.deal_small_bsn.sale.money;
    double jing_total = metrics.deal_total_bsn.buy.money - metrics.deal_total_bsn.sale.money;

    print_next(out.date_str, i, cols);

    print_next(metrics.deal_super_bsn.buy.money / WAN, i, cols);
    print_next(metrics.deal_super_bsn.sale.money / WAN, i, cols);
    

    print_next(metrics.deal_big_bsn.buy.money / WAN, i, cols);
    print_next(metrics.deal_big_bsn.sale.money / WAN, i, cols);

    print_next(metrics.deal_middle_bsn.buy.money / WAN, i, cols);
    print_next(metrics.deal_middle_bsn.sale.money / WAN, i, cols);
    

    print_next(metrics.deal_small_bsn.buy.money / WAN, i, cols);
    print_next(metrics.deal_small_bsn.sale.money / WAN, i, cols);
   

    print_next_pos(jing_super / WAN, i, cols);
    print_next_pos(jing_big / WAN, i, cols);
    print_next_pos(jing_middle / WAN, i, cols);
    print_next_pos(jing_small / WAN, i, cols);
    print_next_pos(jing_total / WAN, i, cols);

    print_next(metrics.deal_total_bsn.buy.money / WAN, i, cols);
    print_next(metrics.deal_total_bsn.sale.money / WAN, i, cols);
    print_next(metrics.deal_total_bsn.neutral.money / WAN, i, cols);


    // 其余统计项
    print_next((metrics.deal_total_bsn.buy.money + metrics.deal_total_bsn.sale.money + metrics.deal_total_bsn.neutral.money) / WAN, i, cols);
    print_next((metrics.deal_total_bsn.buy.volume+ metrics.deal_total_bsn.sale.volume + metrics.deal_total_bsn.neutral.volume) / WAN, i, cols);
    print_next(out.pre_closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change, i, cols);
    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}

inline void print_price(DayOutputMetrics& out, DailyMetrics& metrics, const std::vector<Col>& cols) {
    int i = 0;
    double all_money = metrics.deal_total_price.down.money + metrics.deal_total_price.up.money + metrics.deal_total_price.keep.money;


    std::cout << std::left << std::fixed << std::setprecision(2);
    print_next(out.date_str, i, cols);

    print_next(metrics.deal_super_price.up.money / WAN, i, cols);
    print_next(metrics.deal_super_price.down.money / WAN, i, cols);
    

    print_next(metrics.deal_big_price.up.money / WAN, i, cols);
    print_next(metrics.deal_big_price.down.money / WAN, i, cols);


    print_next(metrics.deal_middle_price.up.money / WAN, i, cols);
    print_next(metrics.deal_middle_price.down.money / WAN, i, cols);
    

    print_next(metrics.deal_small_price.up.money / WAN, i, cols);
    print_next(metrics.deal_small_price.down.money / WAN, i, cols);


    print_next_pos((metrics.deal_super_price.up.money - metrics.deal_super_price.down.money) / WAN, i, cols);
    print_next_pos((metrics.deal_big_price.up.money - metrics.deal_big_price.down.money) / WAN, i, cols);
    print_next_pos((metrics.deal_middle_price.up.money - metrics.deal_middle_price.down.money) / WAN, i, cols);
    print_next_pos((metrics.deal_small_price.up.money - metrics.deal_small_price.down.money) / WAN, i, cols);
    print_next_pos((metrics.deal_total_price.up.money - metrics.deal_total_price.down.money) / WAN, i, cols);

    print_next(metrics.deal_total_price.up.money / WAN, i, cols);
    print_next(metrics.deal_total_price.down.money / WAN, i, cols);
    print_next(metrics.deal_total_price.keep.money / WAN, i, cols);
    print_next(metrics.deal_total_price.keep.money / all_money, i, cols);


    print_next(all_money/WAN , i, cols);
    print_next(out.total_vol_wan, i, cols);
    print_next(out.pre_closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change, i, cols);
    print_next(out.metrics.closing_price, i, cols);
    std::cout << std::endl;

}


inline void print_merge(DayOutputMetrics& out, DailyMetrics& metrics, const std::vector<Col>& cols) {
    int i = 0;
    std::cout << std::left << std::fixed << std::setprecision(2);

    // 1. 日期
    print_next(out.date_str, i, cols);

    // 2. BSN 汇总 (对应原代码 "buy" 后的三项)
    print_next(metrics.deal_total_bsn.buy.money / WAN, i, cols);
    print_next(metrics.deal_total_bsn.sale.money / WAN, i, cols);
    print_next(metrics.deal_total_bsn.neutral.money / WAN, i, cols);

    // 3. Price 汇总 (对应原代码 "up" 后的三项)
    print_next(metrics.deal_total_price.up.money / WAN, i, cols);
    print_next(metrics.deal_total_price.down.money / WAN, i, cols);
    print_next(metrics.deal_total_price.keep.money / WAN, i, cols);

    // 4. 净额 (带有正负号)
    print_next_pos((metrics.deal_total_bsn.buy.money - metrics.deal_total_bsn.sale.money) / WAN, i, cols);
    print_next_pos((metrics.deal_total_price.up.money - metrics.deal_total_price.down.money) / WAN, i, cols);

    // 5. 成交总量相关
    print_next((metrics.deal_total_price.down.money + metrics.deal_total_price.up.money + metrics.deal_total_price.keep.money) / WAN, i, cols);
    print_next((metrics.deal_total_price.down.volume + metrics.deal_total_price.up.volume + metrics.deal_total_price.keep.volume) / WAN, i, cols);

    // 6. 价格与涨跌幅
    print_next(out.pre_closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change, i, cols);
    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}

inline void print_all_data(const DayOutputMetrics& out, const std::string& divergence_str, const std::vector<Col>& cols) {
    int i = 0;
    const deal_bsn& deal_total_bsn = out.am_metrics.deal_total_bsn;


    std::cout << std::left << std::fixed << std::setprecision(2);

    print_next(out.date_str, i, cols);
    print_next(out.metrics.ticks_count, i, cols);
    // print_next(out.am_vol_wan, i, cols);
    print_next(deal_total_bsn.buy.volume + deal_total_bsn.neutral.volume + deal_total_bsn.sale.volume, i, cols);
    print_next(deal_total_bsn.buy.money + deal_total_bsn.neutral.money + deal_total_bsn.sale.money, i, cols);
    
    print_next(out.am_turnover_ratio, i, cols);
    print_next(out.avg_vol_per_tick, i, cols);
    
    print_next(out.total_turnover_wan, i, cols);
    print_next(out.total_vol_wan, i, cols);

    
    print_next_pos(out.net_inflow_wan, i, cols);
    print_next_pos(out.am_net_inflow_wan, i, cols);
    print_next_pos(out.pm_net_inflow_wan, i, cols);
    print_next_pos(out.inflow_ratio, i, cols);
    print_next_pos(out.net_per_change, i, cols);
    print_next(out.historical_total_inflow, i, cols);

    print_next(out.avg_price, i, cols);

    // print_next(out.am_closing_price, i, cols);
    print_next(out.am_metrics.closing_price, i, cols);
    print_next_pos(out.am_pct_change, i, cols);

    print_next(out.metrics.closing_price, i, cols);
    print_next_pos(out.pct_change, i, cols);
    
    print_next(divergence_str, i, cols);

    std::cout << std::endl;
}

void collect_bs_action(bs_action_group& group, const std::string& bs_type, double trade, size_t volume, double gap);
void update_stream_and_metrics(DailyMetrics& metrics, StreamRecord& stream, TickRecord& record, TickRecord& pre_record);
void deal_classfy(DailyMetrics& out);
inline void print_will(DayOutputMetrics& out, DailyMetrics& metrics, const std::vector<Col>& cols);
inline void print_slim_price(DayOutputMetrics& out, bs_action_group& super, deal_bsn& bsn, deal_price& price, const std::vector<Col>& cols); 
void print__headers(const std::string& title, const std::vector<Col>& cols) ;
inline void print_price(DayOutputMetrics& out, DailyMetrics& metrics, const std::vector<Col>& cols);
inline void print_merge(DayOutputMetrics& out, DailyMetrics& metrics, const std::vector<Col>& cols);
inline void print_all_data(const DayOutputMetrics& out, const std::string& divergence_str, const std::vector<Col>& cols);

#endif // COLLECT_STREAM_H