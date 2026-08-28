#ifndef COLLECT_STREAM_H
#define COLLECT_STREAM_H

#include <string>
#include <vector>
#include <iomanip>
#include "common.h"
#include "tick_types.h"

enum class RecordType {
    FIRST,       
    LAST
};

enum class RecordScale {
    SUPER,       
    BIG,
    MIDDLE,
    SMALL,
    TOTAL
};

struct trade {
    double money = 0.0;
    size_t volume = 0;
    size_t tick_count = 0;

    trade& operator+=(const trade& rhs) {
        money += rhs.money;
        volume += rhs.volume;
        tick_count += rhs.tick_count;
        return *this;
    }

    trade& operator-=(const trade& rhs) {
        money -= rhs.money;
        volume -= rhs.volume;
        tick_count -= rhs.tick_count;
        return *this;
    }
};


inline trade operator+(trade lhs, const trade& rhs) {
    lhs += rhs; // 利用已有的 += 运算符
    return lhs; // 返回相加后的副本
}

inline trade operator-(trade lhs, const trade& rhs) {
    lhs -= rhs;
    return lhs;
}

struct deal_price {
    trade up;
    trade down;
    trade keep;
};

struct bsn_action_group {
    deal_price buy;
    deal_price sale;
    deal_price neutral; 
};

struct record_stream {
    bsn_action_group super;
    bsn_action_group big;
    bsn_action_group middle;
    bsn_action_group small;
    bsn_action_group total;
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
    size_t ticks_count = 0;
    double closing_price = 0.0;
    double pre_closing_price = 0.0;
    double this_1457_pirce = 0.0;
    double avg_price = 0.0;
    double all_money = 0.0;
    size_t all_volume = 0.0;

    deal_bsn am_bsn;
    deal_bsn pm_bsn;

    HeadTickData head_data;
    bool head_calculated = false;
    TickRecord daily_first_record;
    TickRecord daily_last_record;
    record_stream header;
    record_stream start_point;
    record_stream end_point;

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

// 定义子条件结构体：包含条件结果与描述说明
struct SubCondition {
    bool satisfied;         // 子条件是否满足
    std::string description; // 子条件说明
};


// 定义一个结构体来存放所有分类的 trade 统计
struct TradeCategoryStats {
    trade buy_down{};
    trade buy_up{};
    trade buy_keep{};

    trade sale_down{};
    trade sale_up{};
    trade sale_keep{};

    trade neutral_down{};
    trade neutral_up{};
    trade neutral_keep{};

    trade total{};

    double all_will_netin = 0.0;
    double all_price_netin = 0.0;
    double strip_will_netin = 0.0; 
    double strip_price_netin = 0.0;

    double buyup_pct = 0.0;
    double buydown_pct = 0.0;

    double saleup_pct = 0.0;

    double pct_change_base_pre = 0.0;
    double pct_change_base_925 = 0.0;

    double all_will_netin_pct = 0.0;
    double all_price_netin_pct = 0.0;

    double strip_will_netin_pct = 0.0;
    double strip_price_netin_pct = 0.0; 

};

struct VectorStats {
    int price_down_day = 0;
    int price_up_day = 0;

    int price_day = 0;

    int volume_shrink_firm = 0;
    int volume_grow_firm = 0;

    int volume_shrink_loose = 0;
    int volume_grow_loose = 0;

    TradeCategoryStats a0;
    TradeCategoryStats a1;

    // double all_will_netin_pct = 0.0;
    // double all_netin_pct = 0.0;

};


struct DayOutputMetrics {
    
    std::string date_str = "";

    double am_pct_change = 0.0;
    double pct_change_base_pre = 0.0;
    double pct_change_base_925 = 0.0;
    double start_change = 0.0;
    double avg_pct_change = 0.0;
    double historical_total_inflow = 0.0;
    
    DailyMetrics middle_metrics;
    DailyMetrics metrics;
    DailyMetrics am_metrics;

};

struct signal_info {
    double all_will_netin = 0.0;
    double all_price_netin = 0.0;

    double will_netin_change = 0.0;
    double price_netin_change = 0.0;
    size_t shrink_firm;
    size_t grow_firm;
    size_t shrink_loose;
    size_t grow_loose;
    int price_day;
    std::string display_file;
    DayOutputMetrics out;
    std::string trigger_reason;
};

struct Col {
    std::string name;
    int width;
    bool visible = true;
};

inline const std::vector<Col> will_price_table_cols = {
    {"Date", 11}, 
    {"Buy-Dn", 12}, 
    {"Buy-Kp", 12},  
    {"Buy-Up", 12},
    {"Sale-Dn", 12}, 
    {"Sale-Kp", 12}, 
    {"Sale-Up", 12}, 
    {"Neutral-Dn", 12},
    {"Neutral-Kp", 12},  
    {"Neutral-Up", 12},
    {"Tot-Buy", 12}, 
    {"Tot-Sale", 12},
    {"Tot-Neutral", 12},    
    {"Tot-Up", 12}, 
    {"Tot-Dn", 12},
    {"Tot-Kp", 12}, 
    {"WILL-Net", 10}, 
    {"PRICE-Net", 12},     
    {"Money", 12},  
    {"Volume", 12},
    {"Pre", 5},     
    {"StartCh", 9}, 
    {"Pct_925", 9},   
    {"Close", 5}
};

inline const std::vector<Col> quiet_buying_table_cols = {
    {"Date", 11}, 
    {"Buy-Dn", 12, true}, 
    {"Buy-Kp", 12, true},  
    {"Buy-Up", 12},
    {"Sale-Dn", 12,true},
    {"Sale-Dn-t", 12,false}, 
    {"Sale-Kp", 12, true}, 
    {"Sale-Up", 12,true}, 
    {"Neutral-Dn", 12, false},
    {"Neutral-Kp", 12, false},  
    {"Neutral-Up", 12, false},
    {"Keep", 7},
    {"Neutral", 7},
    {"NeuSub", 7},
    {"Up-Dn", 12},
    {"Pre", 5},     
    {"StartCh", 9}, 
    {"Pct_925", 9},
    {"Pct_Pre", 9},
    {"Total_m", 9, false},
    {"Total_v", 9}, 
    {"WILL-Net", 10}, 
    {"PRICE-Net", 12},    
    {"Close", 5}
};

inline const std::vector<Col> signal_table_cols = {
    {"File", 40,true},
    {"WNetIn", 12, true}, 
    {"PNetIn", 12, true},
    {"WNET-P", 9},
    {"PNET-P", 9},
    {"Strip-WNetIn", 12, true}, 
    {"Strip-PNetIn", 12, true}, 
    {"shrink_grow", 12},
    {"pday", 4},
    {"Pct0", 5},
    {"Pct1", 5},
    {"REASON", 12}
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
    {"Pct_925", 9}, 
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
    {"Pct_925", 9}, 
    {"Close", 5}
};


static const std::vector<Col> tseq_price_table_cols = {
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
    // {"Pre", 5},      
    // {"StartCh", 9},  
    // {"PctCh", 9}, 
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
    {"PctCh_925", 9}, 
    {"Close", 5}
};

static const std::vector<Col> data_all_table_cols = {
    {"Date", 11}, 
    {"Ticks", 5}, 
    {"AM-volume(W)", 12, false},
    {"AM-Money(W)", 11, false}, 
    {"AM-Money%", 11}, 
    {"Volume/Tick", 11}, 

    {"AM-NET", 11, false}, 
    {"PM-NET", 11, false},
    {"AM-P-NET", 11, false}, 
    {"PM-P-NET", 11, false}, 

    {"WNET", 8},
    {"PNET", 8},

    {"WillP", 8},
    {"PRICEP", 8},
    {"Distribute_M", 24, false},
    {"Distribute_V", 24},
    {"Money", 11},
    {"Volume", 9}, 
    
    {"NET/Money", 9, false},
    // {"NetPer%", 9,false}, 
    // {"HistNetIn(W)", 11, false}, 

    {"AvgPrice", 9, true},
    {"1st", 8}, 
    {"StartCh%", 8}, 
    {"AvgPct%", 8, false},
    {"AM-Close", 8, false}, 
    {"AM-Pct%", 8, false},
    {"BaseAvg%", 8, false},  
    {"Pct_925", 9},
    {"Pct_pre", 9}, 
    {"Close", 7},

    {"Divergence", 20}
};


static const std::vector<Col> test_table_cols = {
    {"Date", 11}, 
    {"Ticks", 5, false}, 
    {"AM-inflow", 13},
    {"AM-Buy", 13},
    {"AM-outflow", 13}, 
    {"AM-Sale", 13}, 
    {"PM-inflow", 13},
    {"PM-Buy", 13},
    {"PM-outflow", 13}, 
    {"PM-Sale", 13},
};

inline double total_money(const deal_bsn& deal) {
    return deal.buy.money + deal.sale.money + deal.neutral.money;
}

inline size_t total_volume(const deal_bsn& deal) {
    return deal.buy.volume + deal.sale.volume + deal.neutral.volume;
}

struct deal_probability_distribution {
    double super = 0.0;
    double big = 0.0;
    double middle = 0.0;
    double small = 0.0;
    std::string description;
};

struct DailyDistributions {
    deal_probability_distribution vol_dist;
    deal_probability_distribution money_dist;
};

template <typename RetType>
using MetricFunc = RetType(*)(const deal_bsn&);

template <typename RetType>
inline deal_probability_distribution deal_pro_distri(const DailyMetrics& metrics, MetricFunc<RetType> get_value) {
    double super_v  = static_cast<double>(get_value(metrics.deal_super_bsn));
    double big_v    = static_cast<double>(get_value(metrics.deal_big_bsn));
    double middle_v = static_cast<double>(get_value(metrics.deal_middle_bsn));
    double small_v  = static_cast<double>(get_value(metrics.deal_small_bsn));
    double total_v  = static_cast<double>(get_value(metrics.deal_total_bsn));

    deal_probability_distribution abc;
    if (total_v > 0) {
        abc.super   = super_v / total_v;
        abc.big     = big_v / total_v;
        abc.middle  = middle_v / total_v;
        abc.small   = small_v / total_v;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);
    // oss << "Super: " << (abc.super * 100) << "%, "
    //     << "Big: " << (abc.big * 100) << "%, "
    //     << "Middle: " << (abc.middle * 100) << "%, "
    //     << "Small: " << (abc.small * 100) << "%";
    oss <<  (abc.super * 100) <<
        "  " << (abc.big * 100) <<
        "  " << (abc.middle * 100) << 
        "  " << (abc.small * 100) ;
    abc.description = oss.str();

    return abc;
}

inline double get_first_record_net(const DailyMetrics& metrics){

    double total_money =  metrics.daily_first_record.volume * 100 * metrics.daily_first_record.price/WAN;

    if (metrics.daily_first_record.bs_type == "B"){

        return total_money;

    }else if (metrics.daily_first_record.bs_type == "S"){

        return 0 - total_money;

    }else{

        return 0;
    }

}

inline void get_daily_distributions(const DailyMetrics& metrics, DailyDistributions& result) {

    result.vol_dist   = deal_pro_distri(metrics, total_volume);
    result.money_dist = deal_pro_distri(metrics, total_money);
    return ;
}

inline double metrics_total_money(const DailyMetrics& metrics){

    const deal_bsn& deal_total_bsn = metrics.deal_total_bsn;

    double total_money = deal_total_bsn.buy.money + deal_total_bsn.neutral.money + deal_total_bsn.sale.money;

    return total_money;
}

inline double metrics_total_volume(const DailyMetrics& metrics){

    const deal_bsn& deal_total_bsn = metrics.deal_total_bsn;

    return deal_total_bsn.buy.volume + deal_total_bsn.neutral.volume + deal_total_bsn.sale.volume;
}

inline double metrics_bsn_net(const DailyMetrics& metrics){
    // double all_will_netin = metrics.deal_total_bsn.buy.money - metrics.deal_total_bsn.sale.money;

    trade buy = metrics.header.total.buy.down + metrics.header.total.buy.keep + metrics.header.total.buy.up;
    trade sale = metrics.header.total.sale.down + metrics.header.total.sale.keep + metrics.header.total.sale.up;

    trade net_in = buy - sale;

    // double all_will_netin = metrics.header.total.buy.down.money

     return net_in.money;
}

inline double metrics_price_net(const bsn_action_group& total){
    // double all_price_netin = metrics.deal_total_price.up.money - metrics.deal_total_price.down.money;
    // return all_price_netin;

    // trade up = metrics.header.total.sale.up + metrics.header.total.buy.up + metrics.header.total.neutral.up;
    // trade down = metrics.header.total.sale.down + metrics.header.total.buy.down +  metrics.header.total.neutral.down;
    
    trade up =  total.buy.up + total.sale.up + total.neutral.up;
    trade down = total.sale.down +total.buy.down +  total.neutral.down;
    
    trade net_in = up - down;


    return net_in.money;
} 


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


inline void set_metrics_record(DailyMetrics& metrics, TickRecord record, RecordType t){

    if (t == RecordType::FIRST){
        metrics.daily_first_record = record;
    }else if (t == RecordType::LAST){
         metrics.daily_last_record = record;
    }

}

inline bool last_record(TickRecord this_record) { return this_record.time == "15:00"; }
inline bool first_record(TickRecord this_record) { return this_record.time == "09:25"; }

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

inline size_t sum_tick_count(const deal_price& price) { 
    return price.up.tick_count + price.down.tick_count + price.keep.tick_count; 
}

inline void add_trade(trade& target, const trade& s, const trade& b, const trade& m, const trade& sm) {
    target.money += (s.money + b.money + m.money + sm.money);
    target.volume += (s.volume + b.volume + m.volume + sm.volume);
    target.tick_count += (s.tick_count + b.tick_count + m.tick_count + sm.tick_count);
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


inline void get_slim_base(const DayOutputMetrics& out, RecordScale t,  bsn_action_group& super, deal_bsn& bsn, deal_price& price){
    
    if (t == RecordScale::SUPER){

        super = out.metrics.header.super;
        bsn = out.metrics.deal_super_bsn;
        price =  out.metrics.deal_super_price;

    }else if(t == RecordScale::BIG){

        super = out.metrics.header.big;
        bsn = out.metrics.deal_big_bsn;
        price =  out.metrics.deal_big_price;

    }else if(t == RecordScale::MIDDLE){

        super = out.metrics.header.middle;
        bsn = out.metrics.deal_middle_bsn;
        price =  out.metrics.deal_middle_price;

    }else if(t == RecordScale::SMALL){
        super = out.metrics.header.small;
        bsn = out.metrics.deal_small_bsn;
        price =  out.metrics.deal_small_price;
    }else{
        super = out.metrics.header.total;
        bsn = out.metrics.deal_total_bsn;
        price =  out.metrics.deal_total_price;
    }

}

inline void print_slim_price(const DayOutputMetrics& out,const DayOutputMetrics& prev_out, RecordScale t, const std::vector<Col>& cols) {

    int i = 0;
    bsn_action_group bsn_group ;
    deal_bsn bsn; 
    deal_price price ;

    get_slim_base(out, t, bsn_group, bsn, price);

    std::cout << std::left << std::fixed << std::setprecision(2);

    print_next(out.date_str, i, cols);
    print_next(bsn_group.buy.down.money / WAN, i, cols);
    print_next(bsn_group.buy.keep.money / WAN, i, cols);
    print_next(bsn_group.buy.up.money / WAN, i, cols);

    print_next(bsn_group.sale.down.money / WAN, i, cols);
    print_next(bsn_group.sale.keep.money / WAN, i, cols);
    print_next(bsn_group.sale.up.money / WAN, i, cols);

    print_next(bsn_group.neutral.down.money / WAN, i, cols);
    print_next(bsn_group.neutral.keep.money / WAN, i, cols);
    print_next(bsn_group.neutral.up.money / WAN, i, cols);

    print_next(bsn.buy.money / WAN, i, cols);
    print_next(bsn.sale.money / WAN, i, cols);
    print_next(bsn.neutral.money / WAN, i, cols);

    print_next(price.up.money / WAN, i, cols);
    print_next(price.down.money / WAN, i, cols);
    print_next(price.keep.money / WAN, i, cols);

    print_next_pos((bsn.buy.money - bsn.sale.money) / WAN, i, cols);
    // print_next_pos((price.up.money - price.down.money) / WAN, i, cols);

    print_next_pos((metrics_price_net(bsn_group)) / WAN, i, cols);

    print_next((bsn.buy.money + bsn.sale.money + bsn.neutral.money) / WAN, i, cols);
    print_next((bsn.buy.volume + bsn.sale.volume + bsn.neutral.volume)/ WAN, i, cols);
    
    print_next(prev_out.metrics.closing_price, i, cols);

    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);

    // print_next(out.pm_closing_price, i, cols);
    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}

inline void accumulate_group(const bsn_action_group& group,
                             trade& b_down, trade& b_up, trade& b_keep,
                             trade& s_down, trade& s_up, trade& s_keep,
                             trade& n_down, trade& n_up, trade& n_keep) 
{
    // 1. buy
    b_down += group.buy.down;
    b_up   += group.buy.up;
    b_keep += group.buy.keep;

    // 2. sale
    s_down += group.sale.down;
    s_up   += group.sale.up;
    s_keep += group.sale.keep;

    // 3. neutral
    n_down += group.neutral.down;
    n_up   += group.neutral.up;
    n_keep += group.neutral.keep;
}

// 辅助函数：将所有的买、卖、平盘以及各种价格变动（down, up, keep）统一汇总到 total 中
inline void calculate_total(trade& total,
                     const trade& b_down, const trade& b_up, const trade& b_keep,
                     const trade& s_down, const trade& s_up, const trade& s_keep,
                     const trade& n_down, const trade& n_up, const trade& n_keep) 
{
    // 1. 先清空 total（也可以直接用 total = {}; 初始化）
    total = {}; 

    // 2. 利用重载的 += 依次累加 9 个 trade 对象
    total += b_down;
    total += b_up;
    total += b_keep;

    total += s_down;
    total += s_up;
    total += s_keep;

    total += n_down;
    total += n_up;
    total += n_keep;
}

inline void print_quiet_buying_price(const DayOutputMetrics& out, const DayOutputMetrics& prev_out) {
    int i = 0;

    std::vector<Col> cols = quiet_buying_table_cols;

    // 定义 9 个方向的 trade 变量
    trade buy_down{};
    trade buy_up{};
    trade buy_keep{};

    trade sale_down{};
    trade sale_up{};
    trade sale_keep{};

    trade neutral_down{};
    trade neutral_up{};
    trade neutral_keep{};

    trade total{};
    trade neutral_all{};
    trade keep_all{};

    double all_will_netin = metrics_bsn_net(out.metrics);
    double all_price_netin = metrics_price_net(out.metrics.header.total);
    
    const auto& h = out.metrics.header;

    // 1. 调用累加辅助函数，把 4 个档位的数据加到 9 个变量中
    accumulate_group(h.super,  buy_down, buy_up, buy_keep, sale_down, sale_up, sale_keep, neutral_down, neutral_up, neutral_keep);
    accumulate_group(h.big,    buy_down, buy_up, buy_keep, sale_down, sale_up, sale_keep, neutral_down, neutral_up, neutral_keep);
    accumulate_group(h.middle, buy_down, buy_up, buy_keep, sale_down, sale_up, sale_keep, neutral_down, neutral_up, neutral_keep);
    accumulate_group(h.small,  buy_down, buy_up, buy_keep, sale_down, sale_up, sale_keep, neutral_down, neutral_up, neutral_keep);

    // 2. 计算出 total 中的 money 和 volume
    calculate_total(total, buy_down, buy_up, buy_keep, sale_down, sale_up, sale_keep, neutral_down, neutral_up, neutral_keep);


    neutral_all = neutral_down + neutral_up + neutral_keep;
    keep_all = buy_keep + sale_keep + neutral_keep;

    std::cout << std::left << std::fixed << std::setprecision(2);

    // 打印日期
    print_next(out.date_str, i, cols);

    // 3. 修正后的 Buy 系列打印（分别对应 down, keep, up）
    print_next(pct_base(buy_down.money, total.money), i, cols);
    print_next(pct_base(buy_keep.money, total.money), i, cols);
    print_next(pct_base(buy_up.money,   total.money), i, cols);

    // std::cout << "[DEBUG] "
    //       << "buy_up.money: " << buy_up.money << ", "
    //       << "buy_keep.money: " << buy_keep.money << ", "
    //       << "buy_down.money: " << buy_down.money << ", "
    //       << "total.money: " << total.money << std::endl;

    // 4. 修正后的 Sale 系列打印（分别对应 down, keep, up）
    print_next(pct_base(sale_down.money, total.money), i, cols);
    print_next(pct_base((double)sale_down.tick_count, total.tick_count), i, cols);

    // std::cout << sale_down.money << ":dd" << total.tick_count << std::endl;

    print_next(pct_base(sale_keep.money, total.money), i, cols);
    print_next(pct_base(sale_up.money,   total.money), i, cols);

    // 5. 修正后的 Neutral 系列打印（分别对应 down, keep, up）
    print_next(pct_base(neutral_down.money, total.money), i, cols);
    print_next(pct_base(neutral_keep.money, total.money), i, cols);
    print_next(pct_base(neutral_up.money,   total.money), i, cols);

    print_next(pct_base(keep_all.money,   total.money), i, cols);
    print_next(pct_base(neutral_all.money,   total.money), i, cols);
    print_next_pos(pct_base(neutral_up.money - neutral_down.money,   total.money), i, cols);
    print_next_pos(pct_base(sale_keep.money+sale_up.money-buy_down.money-buy_keep.money,   total.money), i, cols);
    
    // 其他基础指标打印
    print_next(prev_out.metrics.closing_price, i, cols);

    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);
    print_next_pos(out.pct_change_base_pre, i, cols);
    print_next(total.money/WAN, i, cols);
    print_next(total.volume/WAN, i, cols);

    print_next_pos(all_will_netin/WAN, i, cols);
    print_next_pos(all_price_netin/WAN, i, cols);

    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}

inline void print_will(const DayOutputMetrics& out, const DayOutputMetrics& prev_out, const DailyMetrics& metrics, const std::vector<Col>& cols) {
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
    print_next(prev_out.metrics.closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);
    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}

inline void print_price(const DayOutputMetrics& out, const DayOutputMetrics& prev_out, const DailyMetrics& metrics, const std::vector<Col>& cols) {
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


    // print_next_pos((metrics.deal_super_price.up.money - metrics.deal_super_price.down.money) / WAN, i, cols);
    // print_next_pos((metrics.deal_big_price.up.money - metrics.deal_big_price.down.money) / WAN, i, cols);
    // print_next_pos((metrics.deal_middle_price.up.money - metrics.deal_middle_price.down.money) / WAN, i, cols);
    // print_next_pos((metrics.deal_small_price.up.money - metrics.deal_small_price.down.money) / WAN, i, cols);
    // print_next_pos((metrics.deal_total_price.up.money - metrics.deal_total_price.down.money) / WAN, i, cols);


    print_next_pos((metrics_price_net(out.metrics.header.super)) / WAN, i, cols);
    print_next_pos((metrics_price_net(out.metrics.header.big)) / WAN, i, cols);
    print_next_pos((metrics_price_net(out.metrics.header.middle)) / WAN, i, cols);
    print_next_pos((metrics_price_net(out.metrics.header.small)) / WAN, i, cols);
    print_next_pos((metrics_price_net(out.metrics.header.total)) / WAN, i, cols);

    print_next(metrics.deal_total_price.up.money / WAN, i, cols);
    print_next(metrics.deal_total_price.down.money / WAN, i, cols);
    print_next(metrics.deal_total_price.keep.money / WAN, i, cols);
    print_next(metrics.deal_total_price.keep.money / all_money, i, cols);


    print_next(all_money/WAN , i, cols);
    print_next((out.metrics.deal_total_price.down.volume + out.metrics.deal_total_price.up.volume + out.metrics.deal_total_price.keep.volume)/WAN, i, cols);
    print_next(prev_out.metrics.closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);
    print_next(out.metrics.closing_price, i, cols);
    std::cout << std::endl;

}

inline void print_tseq_price(const tickTime& t, DailyMetrics& metrics) {
    int i = 0;

    const std::vector<Col>& cols = tseq_price_table_cols;
    std::string tshow = format_tick_times(t);

    double all_money = metrics.deal_total_price.down.money + metrics.deal_total_price.up.money + metrics.deal_total_price.keep.money;


    print_next(tshow, i, cols);

    print_next(metrics.deal_super_price.up.money / WAN, i, cols);
    print_next(metrics.deal_super_price.down.money / WAN, i, cols);
    

    print_next(metrics.deal_big_price.up.money / WAN, i, cols);
    print_next(metrics.deal_big_price.down.money / WAN, i, cols);


    print_next(metrics.deal_middle_price.up.money / WAN, i, cols);
    print_next(metrics.deal_middle_price.down.money / WAN, i, cols);
    

    print_next(metrics.deal_small_price.up.money / WAN, i, cols);
    print_next(metrics.deal_small_price.down.money / WAN, i, cols);


    print_next_pos((metrics_price_net(metrics.header.super)) / WAN, i, cols);
    print_next_pos((metrics_price_net(metrics.header.big)) / WAN, i, cols);
    print_next_pos((metrics_price_net(metrics.header.middle)) / WAN, i, cols);
    print_next_pos((metrics_price_net(metrics.header.small)) / WAN, i, cols);
    print_next_pos((metrics_price_net(metrics.header.total)) / WAN, i, cols);

    print_next(metrics.deal_total_price.up.money / WAN, i, cols);
    print_next(metrics.deal_total_price.down.money / WAN, i, cols);
    print_next(metrics.deal_total_price.keep.money / WAN, i, cols);
    print_next(metrics.deal_total_price.keep.money / all_money, i, cols);


    print_next(all_money/WAN , i, cols);
    print_next((metrics.deal_total_price.down.volume + metrics.deal_total_price.up.volume + metrics.deal_total_price.keep.volume)/WAN, i, cols);
    // print_next(prev_out.metrics.closing_price, i, cols);
    // print_next_pos(out.start_change, i, cols);
    // print_next_pos(out.pct_change, i, cols);
    print_next(metrics.closing_price, i, cols);
    std::cout << std::endl;

}

inline void print_signal(const std::string& file, const VectorStats& v_stats, SubCondition sc) {
    int i = 0;

    const std::vector<Col>& cols = signal_table_cols;

    std::cout << std::left << std::fixed << std::setprecision(2);

    print_next(get_display_file(file), i, cols);
    print_next_pos(v_stats.a0.all_will_netin/WAN, i, cols);
    print_next_pos(v_stats.a0.all_price_netin/WAN, i, cols);


    print_next_pos(v_stats.a0.all_will_netin_pct, i, cols);
    print_next_pos(v_stats.a0.all_price_netin_pct, i, cols);

    print_next_pos(v_stats.a0.strip_will_netin/WAN, i, cols);
    print_next_pos(v_stats.a0.strip_price_netin/WAN, i, cols);


    std::string volume_shrink_or_grow = std::to_string(v_stats.volume_shrink_firm) + "-" +
                     std::to_string(v_stats.volume_shrink_loose) + "-" +
                     std::to_string(v_stats.volume_grow_firm) + "-" +
                     std::to_string(v_stats.volume_grow_loose);


    print_next(volume_shrink_or_grow, i, cols);
    // print_next(v_stats.volume_shrink_loose, i, cols);
    // print_next(v_stats.volume_grow_firm, i, cols);
    // print_next(v_stats.volume_grow_loose, i, cols);


    print_next_pos(v_stats.price_day, i, cols);


    print_next_pos(v_stats.a0.pct_change_base_925, i, cols);
    print_next_pos(v_stats.a0.pct_change_base_pre, i, cols);

    print_next(sc.description, i, cols);
    

    std::cout << std::endl;

}


inline void print_merge(const DayOutputMetrics& out, const DayOutputMetrics& prev_out, const DailyMetrics& metrics, const std::vector<Col>& cols) {
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
    print_next_pos((metrics_price_net(metrics.header.total)) / WAN, i, cols);

    // 5. 成交总量相关
    print_next((metrics.deal_total_price.down.money + metrics.deal_total_price.up.money + metrics.deal_total_price.keep.money) / WAN, i, cols);
    print_next((metrics.deal_total_price.down.volume + metrics.deal_total_price.up.volume + metrics.deal_total_price.keep.volume) / WAN, i, cols);

    // 6. 价格与涨跌幅
    print_next(prev_out.metrics.closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);
    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}

inline void print_all_data(const DayOutputMetrics& out, const DayOutputMetrics& prev_out, const std::string& divergence_str) {
    int i = 0;
    size_t total_volume = 0;
    double avg_price = 0.0;

    const std::vector<Col>& cols = data_all_table_cols;

    double am_money_ratio = 0.0;
    double am_total_money = metrics_total_money(out.am_metrics);
    double am_will_netin = metrics_bsn_net(out.am_metrics);
    double am_price_netin = metrics_price_net(out.am_metrics.header.total);

    double total_money = metrics_total_money(out.metrics);

    double all_will_netin = metrics_bsn_net(out.metrics);
    double all_price_netin = metrics_price_net(out.metrics.header.total);

    double prev_all_will_netin = metrics_bsn_net(prev_out.metrics);
    double prev_all_price_netin = metrics_price_net(prev_out.metrics.header.total);

    

    am_money_ratio = am_total_money/total_money; 
    total_volume = metrics_total_volume(out.metrics);

    avg_price = total_money/total_volume;

    // std::cout <<"avg data in print all DATA: " <<avg_price << " = " << total_money << " / " << total_volume << std::endl;

    std::cout << std::left << std::fixed << std::setprecision(1);

    print_next(out.date_str, i, cols);
    print_next(out.metrics.ticks_count, i, cols);
    print_next(metrics_total_volume(out.am_metrics), i, cols);
    print_next(am_total_money, i, cols);
    
    print_next(am_money_ratio * 100, i, cols);
    print_next(total_volume/out.metrics.ticks_count, i, cols);

    print_next_pos(am_will_netin/WAN, i, cols);
    print_next_pos((all_will_netin - am_will_netin)/WAN, i, cols);


    print_next_pos(am_price_netin/WAN, i, cols);
    print_next_pos((all_price_netin - am_price_netin)/WAN, i, cols);

    print_next_pos(all_will_netin/WAN, i, cols);
    print_next_pos(all_price_netin/WAN, i, cols);


    print_next_pos((all_will_netin - prev_all_will_netin)/std::abs(prev_all_will_netin), i, cols);
    print_next_pos((all_price_netin - prev_all_price_netin)/std::abs(prev_all_price_netin), i, cols);

    DailyDistributions result;
    get_daily_distributions(out.metrics,  result);
    print_next(result.money_dist.description, i, cols);
    print_next(result.vol_dist.description, i, cols);


    print_next(total_money/WAN, i, cols);
    print_next(total_volume/WAN, i, cols);


    print_next_pos(all_will_netin/total_money, i, cols);

    print_next(avg_price, i, cols);

    print_next_pos(get_first_record_net(out.metrics), i, cols);

    std::cout << std::left << std::fixed << std::setprecision(2);

    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.avg_pct_change, i, cols);

    print_next(out.am_metrics.closing_price, i, cols);

    print_next_pos(out.am_pct_change, i, cols);
    
    print_next_pos((out.metrics.closing_price - avg_price)/avg_price, i, cols);

    print_next_pos(out.pct_change_base_925, i, cols);
    print_next_pos(out.pct_change_base_pre, i, cols);
    print_next(out.metrics.closing_price, i, cols);

    print_next(divergence_str, i, cols);

    std::cout << std::endl;

    return;
}

void collect_bs_action(bsn_action_group& group, const std::string& bs_type, double money, size_t volume, double gap, size_t tick_count);
void update_stream(StreamRecord& stream, const TickRecord& record, const TickRecord& pre_record);
void deal_classfy(DailyMetrics& out);
void print__headers(const std::string& title, const std::vector<Col>& cols);

extern void update_metrics_header(record_stream& header, StreamRecord& stream);
extern bool record_change(TickRecord this_record, const TickRecord pre_record);
extern void stream_new(StreamRecord& stream, TickRecord record, double pre_price);
extern void get_record_stream_point(record_stream& this_point, TickRecord r, double pre_price);
extern void sub_record_stream_point(record_stream& this_point, record_stream& that_point);
void calculate_trade_stats(const record_stream& h, TradeCategoryStats& stats);
void metry_summary(const DayOutputMetrics& out, TradeCategoryStats& stats); 

int metrics_up_check(const std::vector<DayOutputMetrics>& out_vector);
int metrics_price_check(const std::vector<DayOutputMetrics>& out_vector);
int metrics_grow_loose(const std::vector<DayOutputMetrics>& out_vector);
int metrics_down_check(const std::vector<DayOutputMetrics>& out_vector);
int metrics_shrink_loose(const std::vector<DayOutputMetrics>& out_vector);
int metrics_grow_firm(const std::vector<DayOutputMetrics>& out_vector);
int metrics_shrink_firm(const std::vector<DayOutputMetrics>& out_vector);
void metry_vector_summary(const std::vector<DayOutputMetrics>& out_vector, VectorStats& stats);


#endif // COLLECT_STREAM_H