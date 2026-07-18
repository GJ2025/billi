#ifndef COLLECT_STREAM_H
#define COLLECT_STREAM_H

#include <string>
#include <vector>
#include <iomanip>
#include "common.h"
#include "tick_types.h"

struct deal_price {
    double up = 0.0;
    double down = 0.0;
    double keep = 0.0;
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
    double buy = 0.0;
    double sale = 0.0;
    double neutral = 0.0;
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
    stream_sum stream_sum_info;
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

struct Col {
    std::string name;
    int width;
    bool visible = true;
};

inline const std::vector<Col> will_price_table_cols = {
    {"Date", 11}, 
    {"Buy-Dn", 12, false}, 
    {"Buy-Kp", 9, false},  
    {"Buy-Up", 9},
    {"Sale-Dn", 12,true}, 
    {"Sale-Kp", 9, false}, 
    {"Sale-Up", 9,false}, 
    {"Neutral-Dn", 10, false},
    {"Neutral-Kp", 10, false},  
    {"Neutral-Up", 10, false},
    {"Tot-Buy", 12}, 
    {"Tot-Sale", 9},
    {"Tot-Neutral", 12, true},    
    {"Tot-Up", 9}, 
    {"Tot-Dn", 9},
    {"Tot-Kp", 9}, 
    {"WILL-Net", 10}, 
    {"Price-Net", 9},     
    {"Total", 12},  
    {"Volume", 12},
    {"Pre", 5},     
    {"StartCh", 9}, 
    {"PctCh", 9},   
    {"Close", 5}
};

static const std::vector<Col> price_table_cols = {
    {"Date", 11}, 
    {"Super-Up", 9}, 
    {"Super-Dn", 9}, 
    {"Super-Net", 9},
    {"Big-Up", 9},   
    {"Big-Dn", 9},   
    {"Big-Net", 9},
    {"Mid-Up", 9},   
    {"Mid-Dn", 9},   
    {"Mid-Net", 9},
    {"Small-Up", 9}, 
    {"Small-Dn", 9}, 
    {"Small-Net", 9},
    {"Tot-Up", 12},  
    {"Tot-Dn", 12},  
    {"Tot-Net", 12},
    {"Total", 12},   
    {"Volume", 12},
    {"Pre", 5},      
    {"StartCh", 9},  
    {"PctCh", 9}, 
    {"Close", 5}
};

static const std::vector<Col> will_table_cols = {
    {"Date", 11}, 
    {"Super-Buy", 12}, 
    {"Super-Sale", 10}, 
    {"Super-Net", 9},
    {"Big-Buy", 12},  
    {"Big-Sale", 9},   
    {"Big-Net", 9},
    {"Mid-Buy", 9},    
    {"Mid-Sale", 9},   
    {"Mid-Net", 9},
    {"Small-Buy", 9},  
    {"Small-Sale", 10}, 
    {"Small-Net", 9},
    {"Tot-Buy", 9},    
    {"Tot-Sale", 9},   
    {"Tot-Net", 9},
    {"Total", 12},     
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
    {"Total", 12},       
    {"Volume", 12},
    {"Pre", 5},          
    {"StartCh", 9},       
    {"PctCh", 9}, 
    {"Close", 5}
};

template<typename T>
inline void print_next(const T& val, int& index, const std::vector<Col>& cols) {
    // 仅当当前列可见时执行打印
    if (index < (int)cols.size() && cols[index].visible) {
        std::cout << std::setw(cols[index].width) << val << " | ";
    }
    // 索引始终递增，以匹配调用序列
    index++;
}

template<typename T>
inline void print_next_pos(const T& val, int& index, const std::vector<Col>& cols) {
    if (index < (int)cols.size() && cols[index].visible) {
        std::cout << std::showpos << std::setw(cols[index].width) << val << " | " << std::noshowpos;
    }
    index++;
}

inline double sum_price(const deal_price& price) { return price.up + price.down + price.keep; }
inline double sum_bsn_buy(deal_bsn& super, deal_bsn& big, deal_bsn& middle, deal_bsn& small){
    return super.buy + big.buy + middle.buy + small.buy ;
}
inline double sum_bsn_sale(deal_bsn& super, deal_bsn& big, deal_bsn& middle, deal_bsn& small){
    return super.sale + big.sale + middle.sale + small.sale ;
}
inline double sum_bsn_neutral(deal_bsn& super, deal_bsn& big, deal_bsn& middle, deal_bsn& small){
    return super.neutral + big.neutral + middle.neutral + small.neutral ;
}
inline double sum_price_up(deal_price& super, deal_price& big, deal_price& middle, deal_price& small){
    return super.up + big.up + middle.up + small.up ;
}
inline double sum_price_down(deal_price& super, deal_price& big, deal_price& middle, deal_price& small){
    return super.down + big.down + middle.down + small.down ;
}
inline double sum_price_keep(deal_price& super, deal_price& big, deal_price& middle, deal_price& small){
    return super.keep + big.keep + middle.keep + small.keep ;
}


// 专门负责打印带有标题的装饰线
inline void print_decorative_line(int total_width, const std::string& left_title, const std::string& right_title) {
    // 扣除掉分隔符引起的额外长度，并计算可用的线条空间
    int line_len = total_width - 3;
    
    // 计算标题占用的长度
    int left_len = static_cast<int>(left_title.length());
    int right_len = static_cast<int>(right_title.length());
    
    // 计算中间剩余的横线长度
    int mid_space = line_len - left_len - right_len - 4; // 4 为左右两侧括号和空格的预留
    if (mid_space < 2) mid_space = 2;

    std::cout << "[ " << left_title << " ]" 
              << std::string(mid_space, '-') 
              << "[ " << right_title << " ]" 
              << std::endl;
}


inline void print_will_price_header(const std::string& title, const std::vector<Col>& cols) {
    std::cout << std::left;
    int total_width = 0;

    // 1. 打印表头：仅遍历可见列
    for (const auto& col : cols) {
        if (col.visible) {
            std::cout << std::setw(col.width) << col.name << " | ";
            total_width += (col.width + 3);
        }
    }
    std::cout << std::endl;

    // 2. 打印对齐的装饰线
    print_decorative_line(total_width, title, title);
}

inline void print_price(DayOutputMetrics& out, const std::vector<Col>& cols) {
    int i = 0;
    std::cout << std::left << std::fixed << std::setprecision(2);
    print_next(out.date_str, i, cols);
    print_next(out.deal_super_price.up / WAN, i, cols);
    print_next(out.deal_super_price.down / WAN, i, cols);
    print_next_pos((out.deal_super_price.up - out.deal_super_price.down) / WAN, i, cols);
    print_next(out.deal_big_price.up / WAN, i, cols);
    print_next(out.deal_big_price.down / WAN, i, cols);
    print_next_pos((out.deal_big_price.up - out.deal_big_price.down) / WAN, i, cols);
    print_next(out.deal_middle_price.up / WAN, i, cols);
    print_next(out.deal_middle_price.down / WAN, i, cols);
    print_next_pos((out.deal_middle_price.up - out.deal_middle_price.down) / WAN, i, cols);
    print_next(out.deal_small_price.up / WAN, i, cols);
    print_next(out.deal_small_price.down / WAN, i, cols);
    print_next_pos((out.deal_small_price.up - out.deal_small_price.down) / WAN, i, cols);
    print_next(out.deal_total_price.up / WAN, i, cols);
    print_next(out.deal_total_price.down / WAN, i, cols);
    print_next_pos((out.deal_total_price.up - out.deal_total_price.down) / WAN, i, cols);
    print_next((out.deal_total_price.down + out.deal_total_price.up + out.deal_total_price.keep) / WAN, i, cols);
    print_next(out.total_vol_wan, i, cols);
    print_next(out.pre_closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change, i, cols);
    print_next(out.closing_price, i, cols);
    std::cout << std::endl;
}

inline void print_slim_price(DayOutputMetrics& out, bs_action_group& super, deal_bsn& bsn, deal_price& price, const std::vector<Col>& cols) {
    int i = 0;
    std::cout << std::left << std::fixed << std::setprecision(2);

    print_next(out.date_str, i, cols);
    print_next(super.buy.down / WAN, i, cols);
    print_next(super.buy.keep / WAN, i, cols);
    print_next(super.buy.up / WAN, i, cols);

    print_next(super.sale.down / WAN, i, cols);
    print_next(super.sale.keep / WAN, i, cols);
    print_next(super.sale.up / WAN, i, cols);

    print_next(super.neutral.down / WAN, i, cols);
    print_next(super.neutral.keep / WAN, i, cols);
    print_next(super.neutral.up / WAN, i, cols);

    print_next(bsn.buy / WAN, i, cols);
    print_next(bsn.sale / WAN, i, cols);
    print_next(bsn.neutral / WAN, i, cols);

    print_next(price.up / WAN, i, cols);
    print_next(price.down / WAN, i, cols);
    print_next(price.keep / WAN, i, cols);

    print_next_pos((bsn.buy - bsn.sale) / WAN, i, cols);
    print_next_pos((price.up - price.down) / WAN, i, cols);
    print_next((out.deal_total_bsn.buy + out.deal_total_bsn.sale + out.deal_total_bsn.neutral) / WAN, i, cols);
    print_next(out.total_vol_wan, i, cols);
    print_next(out.pre_closing_price, i, cols);

    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change, i, cols);

    print_next(out.closing_price, i, cols);

    std::cout << std::endl;
}


inline void print_will(DayOutputMetrics& out, const std::vector<Col>& cols) {
    int i = 0;
    std::cout << std::left << std::fixed << std::setprecision(2);

    // 计算净额
    double jing_super = out.deal_super_bsn.buy - out.deal_super_bsn.sale;
    double jing_big = out.deal_big_bsn.buy - out.deal_big_bsn.sale;
    double jing_middle = out.deal_middle_bsn.buy - out.deal_middle_bsn.sale;
    double jing_small = out.deal_small_bsn.buy - out.deal_small_bsn.sale;
    double jing_total = out.deal_total_bsn.buy - out.deal_total_bsn.sale;

    // 顺序打印 (需严格对应 cols 定义的顺序)
    print_next(out.date_str, i, cols);

    // Super
    print_next(out.deal_super_bsn.buy / WAN, i, cols);
    print_next(out.deal_super_bsn.sale / WAN, i, cols);
    print_next_pos(jing_super / WAN, i, cols);

    // Big
    print_next(out.deal_big_bsn.buy / WAN, i, cols);
    print_next(out.deal_big_bsn.sale / WAN, i, cols);
    print_next_pos(jing_big / WAN, i, cols);

    // Middle
    print_next(out.deal_middle_bsn.buy / WAN, i, cols);
    print_next(out.deal_middle_bsn.sale / WAN, i, cols);
    print_next_pos(jing_middle / WAN, i, cols);

    // Small
    print_next(out.deal_small_bsn.buy / WAN, i, cols);
    print_next(out.deal_small_bsn.sale / WAN, i, cols);
    print_next_pos(jing_small / WAN, i, cols);

    // Total
    print_next(out.deal_total_bsn.buy / WAN, i, cols);
    print_next(out.deal_total_bsn.sale / WAN, i, cols);
    print_next_pos(jing_total / WAN, i, cols);

    // 其余统计项
    print_next((out.deal_total_bsn.buy + out.deal_total_bsn.sale + out.deal_total_bsn.neutral) / WAN, i, cols);
    print_next(out.total_vol_wan, i, cols);
    print_next(out.pre_closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change, i, cols);
    print_next(out.closing_price, i, cols);

    std::cout << std::endl;
}

inline void print_merge(DayOutputMetrics& out, const std::vector<Col>& cols) {
    int i = 0;
    std::cout << std::left << std::fixed << std::setprecision(2);

    // 1. 日期
    print_next(out.date_str, i, cols);

    // 2. BSN 汇总 (对应原代码 "buy" 后的三项)
    print_next(out.deal_total_bsn.buy / WAN, i, cols);
    print_next(out.deal_total_bsn.sale / WAN, i, cols);
    print_next(out.deal_total_bsn.neutral / WAN, i, cols);

    // 3. Price 汇总 (对应原代码 "up" 后的三项)
    print_next(out.deal_total_price.up / WAN, i, cols);
    print_next(out.deal_total_price.down / WAN, i, cols);
    print_next(out.deal_total_price.keep / WAN, i, cols);

    // 4. 净额 (带有正负号)
    print_next_pos((out.deal_total_bsn.buy - out.deal_total_bsn.sale) / WAN, i, cols);
    print_next_pos((out.deal_total_price.up - out.deal_total_price.down) / WAN, i, cols);

    // 5. 成交总量相关
    print_next((out.deal_total_price.down + out.deal_total_price.up + out.deal_total_price.keep) / WAN, i, cols);
    print_next(out.total_vol_wan, i, cols);

    // 6. 价格与涨跌幅
    print_next(out.pre_closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change, i, cols);
    print_next(out.closing_price, i, cols);

    std::cout << std::endl;
}

void collect_bs_action(bs_action_group& group, const std::string& bs_type, double trade, double gap);
void update_stream_and_metrics(DailyMetrics& metrics, StreamRecord& stream, TickRecord& record, TickRecord& pre_record);
void deal_classfy(DayOutputMetrics& out);
inline void print_will(DayOutputMetrics& out, const std::vector<Col>& cols);
inline void print_slim_price(DayOutputMetrics& out, bs_action_group& super, deal_bsn& bsn, deal_price& price, const std::vector<Col>& cols); 
void print_will_price_header(const std::string& title, const std::vector<Col>& cols) ;
inline void print_price(DayOutputMetrics& out, const std::vector<Col>& cols);
inline void print_merge(DayOutputMetrics& out, const std::vector<Col>& cols);

#endif // COLLECT_STREAM_H