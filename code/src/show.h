#ifndef SHOW_H
#define SHOW_H

#include "collect_stream.h"

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

inline void print_slim_price(const DayOutputMetrics& out,const DayOutputMetrics& prev_out, RecordScale t, const std::vector<Col>& cols) {

    int i = 0;
    bsn_action_group bsn_group ;
    deal_summary deal_summary;

    get_slim_base(out.metrics, t, bsn_group, deal_summary);

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

    print_next(deal_summary.bsn.buy.money / WAN, i, cols);
    print_next(deal_summary.bsn.sale.money / WAN, i, cols);
    print_next(deal_summary.bsn.neutral.money / WAN, i, cols);

    print_next(deal_summary.price.up.money / WAN, i, cols);
    print_next(deal_summary.price.down.money / WAN, i, cols);
    print_next(deal_summary.price.keep.money / WAN, i, cols);

    print_next_pos((deal_summary.bsn.buy.money - deal_summary.bsn.sale.money) / WAN, i, cols);

    print_next_pos((metrics_price_net(bsn_group)) / WAN, i, cols);

    print_next(deal_summary.type_total.money / WAN, i, cols);
    print_next(deal_summary.type_total.volume/ WAN, i, cols);
    
    print_next(prev_out.metrics.closing_price, i, cols);

    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);
    print_next_pos(out.pct_change_base_pre, i, cols);

    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}

inline void print_slim_price_ratio(const DayOutputMetrics& out,const DayOutputMetrics& prev_out, RecordScale t, const std::vector<Col>& cols) {

    int i = 0;
    bsn_action_group bsn_group ;
    deal_summary deal_summary;

    get_slim_base(out.metrics, t, bsn_group, deal_summary);
    double total_money = deal_summary.type_total.money;


    std::cout << std::left << std::fixed << std::setprecision(3);

    print_next(out.date_str, i, cols);
    print_next(bsn_group.buy.down.money / total_money, i, cols);
    print_next(bsn_group.buy.keep.money / total_money, i, cols);
    print_next(bsn_group.buy.up.money / total_money, i, cols);

    print_next(bsn_group.sale.down.money / total_money, i, cols);
    print_next(bsn_group.sale.keep.money / total_money, i, cols);
    print_next(bsn_group.sale.up.money / total_money, i, cols);

    print_next(bsn_group.neutral.down.money / total_money, i, cols);
    print_next(bsn_group.neutral.keep.money / total_money, i, cols);
    print_next(bsn_group.neutral.up.money / total_money, i, cols);

    print_next(deal_summary.bsn.buy.money / total_money, i, cols);
    print_next(deal_summary.bsn.sale.money / total_money, i, cols);
    print_next(deal_summary.bsn.neutral.money / total_money, i, cols);

    print_next(deal_summary.price.up.money / total_money, i, cols);
    print_next(deal_summary.price.down.money / total_money, i, cols);
    print_next(deal_summary.price.keep.money / total_money, i, cols);

    print_next_pos((deal_summary.bsn.buy.money - deal_summary.bsn.sale.money) /  deal_summary.total.money, i, cols);

    print_next_pos((metrics_price_net(bsn_group)) / deal_summary.total.money, i, cols);

    print_next(deal_summary.type_total.money / WAN, i, cols);
    print_next(deal_summary.type_total.volume/ WAN, i, cols);
    
    print_next(prev_out.metrics.closing_price, i, cols);

    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);
    print_next_pos(out.pct_change_base_pre, i, cols);

    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}



inline void print_quiet_buying_price(const DayOutputMetrics& out, const DayOutputMetrics& prev_out) {
    int i = 0;

    std::vector<Col> cols = quiet_buying_table_cols;

    double all_will_netin = metrics_bsn_net(out.metrics);
    double all_price_netin = metrics_price_net(out.metrics.header.total);

    bsn_action_group dump;
    deal_summary summary;
    get_slim_base(out.metrics, RecordScale::TOTAL, dump, summary);

    const bsn_action_group& bs = out.metrics.header.total;


    std::cout << std::left << std::fixed << std::setprecision(2);

    print_next(out.date_str, i, cols);

    print_next(pct_base(bs.buy.down.money, summary.total.money), i, cols);
    print_next(pct_base(bs.buy.keep.money, summary.total.money), i, cols);
    print_next(pct_base(bs.buy.up.money,   summary.total.money), i, cols);

    print_next(pct_base(bs.sale.down.money, summary.total.money), i, cols);
    print_next(pct_base((double)bs.sale.down.tick_count, summary.total.tick_count), i, cols);

    print_next(pct_base(bs.sale.keep.money, summary.total.money), i, cols);
    print_next(pct_base(bs.sale.up.money,   summary.total.money), i, cols);

    print_next(pct_base(bs.neutral.down.money, summary.total.money), i, cols);
    print_next(pct_base(bs.neutral.keep.money, summary.total.money), i, cols);
    print_next(pct_base(bs.neutral.up.money,   summary.total.money), i, cols);

    print_next(pct_base(summary.price.keep.money,   summary.total.money), i, cols);
    print_next(pct_base(summary.bsn.neutral.money,   summary.total.money), i, cols);
    print_next_pos(pct_base(bs.neutral.up.money - bs.neutral.down.money,   summary.total.money), i, cols);
    print_next_pos(pct_base(bs.buy.keep.money - bs.sale.keep.money ,   summary.total.money), i, cols);
    
    print_next(prev_out.metrics.closing_price, i, cols);

    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);
    print_next_pos(out.pct_change_base_pre, i, cols);
    print_next(summary.total.money/WAN, i, cols);
    print_next(summary.total.volume/WAN, i, cols);

    print_next_pos(all_will_netin/WAN, i, cols);
    print_next_pos(all_price_netin/WAN, i, cols);

    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}

inline void print_will(const DayOutputMetrics& out, const DayOutputMetrics& prev_out, const DailyMetrics& metrics, const std::vector<Col>& cols) {
    int i = 0;

    bsn_action_group dump;
    deal_summary deal_summary_super;
    deal_summary deal_summary_big;
    deal_summary deal_summary_middle;
    deal_summary deal_summary_small;
    deal_summary deal_summary_total;

    get_slim_base(metrics, RecordScale::SUPER, dump, deal_summary_super);
    get_slim_base(metrics, RecordScale::BIG, dump, deal_summary_big);
    get_slim_base(metrics, RecordScale::MIDDLE, dump, deal_summary_middle);
    get_slim_base(metrics, RecordScale::SMALL, dump, deal_summary_small);
    get_slim_base(metrics, RecordScale::TOTAL, dump, deal_summary_total);

    double jing_super = deal_summary_super.bsn.buy.money - deal_summary_super.bsn.sale.money;
    double jing_big = deal_summary_big.bsn.buy.money - deal_summary_big.bsn.sale.money;
    double jing_middle = deal_summary_middle.bsn.buy.money - deal_summary_middle.bsn.sale.money;
    double jing_small = deal_summary_small.bsn.buy.money - deal_summary_small.bsn.sale.money;
    double jing_total = deal_summary_total.bsn.buy.money - deal_summary_total.bsn.sale.money;

    std::cout << std::left << std::fixed << std::setprecision(2);
    print_next(out.date_str, i, cols);

    print_next(deal_summary_super.bsn.buy.money / WAN, i, cols);
    print_next(deal_summary_super.bsn.sale.money / WAN, i, cols);
    

    print_next(deal_summary_big.bsn.buy.money / WAN, i, cols);
    print_next(deal_summary_big.bsn.sale.money / WAN, i, cols);

    print_next(deal_summary_middle.bsn.buy.money / WAN, i, cols);
    print_next(deal_summary_middle.bsn.sale.money / WAN, i, cols);
    
    print_next(deal_summary_small.bsn.buy.money / WAN, i, cols);
    print_next(deal_summary_small.bsn.sale.money / WAN, i, cols);
   
    print_next_pos(jing_super / WAN, i, cols);
    print_next_pos(jing_big / WAN, i, cols);
    print_next_pos(jing_middle / WAN, i, cols);
    print_next_pos(jing_small / WAN, i, cols);
    print_next_pos(jing_total / WAN, i, cols);

    print_next(deal_summary_total.bsn.buy.money / WAN, i, cols);
    print_next(deal_summary_total.bsn.sale.money / WAN, i, cols);
    print_next(deal_summary_total.bsn.neutral.money / WAN, i, cols);

    print_next((deal_summary_total.total.money) / WAN, i, cols);
    print_next((deal_summary_total.total.volume) / WAN, i, cols);
    print_next(prev_out.metrics.closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);
    print_next(out.metrics.closing_price, i, cols);

    std::cout << std::endl;
}

inline void print_price(const DayOutputMetrics& out, const DayOutputMetrics& prev_out, const DailyMetrics& metrics, const std::vector<Col>& cols) {
    int i = 0;
    
    bsn_action_group dump;
    deal_summary deal_summary_super;
    deal_summary deal_summary_big;
    deal_summary deal_summary_middle;
    deal_summary deal_summary_small;
    deal_summary deal_summary_total;

    get_slim_base(metrics, RecordScale::SUPER, dump, deal_summary_super);
    get_slim_base(metrics, RecordScale::BIG, dump, deal_summary_big);
    get_slim_base(metrics, RecordScale::MIDDLE, dump, deal_summary_middle);
    get_slim_base(metrics, RecordScale::SMALL, dump, deal_summary_small);
    get_slim_base(metrics, RecordScale::TOTAL, dump, deal_summary_total);

    double all_money = deal_summary_total.total.money;

    std::cout << std::left << std::fixed << std::setprecision(2);
    print_next(out.date_str, i, cols);

    print_next(deal_summary_super.price.up.money / WAN, i, cols);
    print_next(deal_summary_super.price.down.money / WAN, i, cols);
    

    print_next(deal_summary_big.price.up.money / WAN, i, cols);
    print_next(deal_summary_big.price.down.money / WAN, i, cols);


    print_next(deal_summary_middle.price.up.money / WAN, i, cols);
    print_next(deal_summary_middle.price.down.money / WAN, i, cols);
    

    print_next(deal_summary_small.price.up.money / WAN, i, cols);
    print_next(deal_summary_small.price.down.money / WAN, i, cols);

    print_next_pos((metrics_price_net(out.metrics.header.super)) / WAN, i, cols);
    print_next_pos((metrics_price_net(out.metrics.header.big)) / WAN, i, cols);
    print_next_pos((metrics_price_net(out.metrics.header.middle)) / WAN, i, cols);
    print_next_pos((metrics_price_net(out.metrics.header.small)) / WAN, i, cols);
    print_next_pos((metrics_price_net(out.metrics.header.total)) / WAN, i, cols);

    print_next(deal_summary_total.price.up.money / WAN, i, cols);
    print_next(deal_summary_total.price.down.money / WAN, i, cols);
    print_next(deal_summary_total.price.keep.money / WAN, i, cols);
    print_next(deal_summary_total.price.keep.money / all_money, i, cols);


    print_next(all_money/WAN , i, cols);
    print_next((deal_summary_total.total.volume)/WAN, i, cols);
    print_next(prev_out.metrics.closing_price, i, cols);
    print_next_pos(out.start_change, i, cols);
    print_next_pos(out.pct_change_base_925, i, cols);
    print_next(out.metrics.closing_price, i, cols);
    std::cout << std::endl;

}

inline void print_tseq_price(DailyMetrics& metrics) {
    int i = 0;

    const std::vector<Col>& cols = tseq_price_table_cols;
    std::string tshow = format_tick_times(metrics.header.what);

    bsn_action_group dump;
    deal_summary deal_summary_super;
    deal_summary deal_summary_big;
    deal_summary deal_summary_middle;
    deal_summary deal_summary_small;
    deal_summary deal_summary_total;

    get_slim_base(metrics, RecordScale::SUPER, dump, deal_summary_super);
    get_slim_base(metrics, RecordScale::BIG, dump, deal_summary_big);
    get_slim_base(metrics, RecordScale::MIDDLE, dump, deal_summary_middle);
    get_slim_base(metrics, RecordScale::SMALL, dump, deal_summary_small);
    get_slim_base(metrics, RecordScale::TOTAL, dump, deal_summary_total);


    double all_money = deal_summary_total.total.money;


    print_next(tshow, i, cols);

    print_next(deal_summary_super.price.up.money / WAN, i, cols);
    print_next(deal_summary_super.price.down.money / WAN, i, cols);
    

    print_next(deal_summary_big.price.up.money / WAN, i, cols);
    print_next(deal_summary_big.price.down.money / WAN, i, cols);


    print_next(deal_summary_middle.price.up.money / WAN, i, cols);
    print_next(deal_summary_middle.price.down.money / WAN, i, cols);
    

    print_next(deal_summary_small.price.up.money / WAN, i, cols);
    print_next(deal_summary_small.price.down.money / WAN, i, cols);


    print_next_pos((metrics_price_net(metrics.header.super)) / WAN, i, cols);
    print_next_pos((metrics_price_net(metrics.header.big)) / WAN, i, cols);
    print_next_pos((metrics_price_net(metrics.header.middle)) / WAN, i, cols);
    print_next_pos((metrics_price_net(metrics.header.small)) / WAN, i, cols);
    print_next_pos((metrics_price_net(metrics.header.total)) / WAN, i, cols);

    print_next(deal_summary_total.price.up.money / WAN, i, cols);
    print_next(deal_summary_total.price.down.money / WAN, i, cols);
    print_next(deal_summary_total.price.keep.money / WAN, i, cols);
    print_next(deal_summary_total.price.keep.money / all_money, i, cols);


    print_next(all_money/WAN , i, cols);
    print_next((deal_summary_total.total.volume)/WAN, i, cols);

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


    std::string price_adjacent_up_days = format_with_sign(v_stats.price_day_adjacent[0]) + ":" +
                     format_with_sign(v_stats.price_day_adjacent[1]);

    print_next(price_adjacent_up_days, i, cols);


    print_next_pos(v_stats.a0.pct_change_base_925, i, cols);
    print_next_pos(v_stats.a0.pct_change_base_pre, i, cols);

    print_next(sc.description, i, cols);
    

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

void print_table_header() {
    std::cout << std::left << std::setw(12) << "Date" << " | "
              << std::right 
              << std::setw(12) << "Pre924" << " | "
              << std::setw(10) << "924"    << " | "
              << std::setw(10) << "925"    << " | "
              << std::setw(12) << "ChgPreDay" << " | "
              << std::setw(12) << "ChgPre924" << " | "
               << std::setw(6) << "b/s" << " | "
              << std::setw(12) << "Chg924"    << " | "
              << std::setw(12) << "PctChg" 
              << std::endl;
    std::cout << std::string(88, '-') << std::endl;
}

void print_header_info(const DayOutputMetrics& out, const DayOutputMetrics& pre_out) {
    double price_change_pct = 0.0;
    double ratio_change_pre_day = 0.0;
    if (pre_out.metrics.closing_price != 0.0) {
        price_change_pct = ((out.metrics.closing_price - pre_out.metrics.closing_price) / pre_out.metrics.closing_price) * 100.0;
        ratio_change_pre_day = ((out.metrics.head_data.v_925.price - pre_out.metrics.closing_price) / pre_out.metrics.closing_price) * 100.0;

    }

    std::cout << std::left  << std::setw(12) << out.date_str << " | "
              << std::right << std::fixed << std::setprecision(2)
              << std::setw(12) << out.metrics.head_data.pre_924.price << " | "
              << std::setw(10) << out.metrics.head_data.v_924.price   << " | "
              << std::setw(10) << out.metrics.head_data.v_925.price   << " | "
              << std::setw(11) << out.metrics.head_data.ratio_change_pre_924 << "% | "
              << std::setw(11) << ratio_change_pre_day << "% | "
              << std::setw(6) << out.metrics.head_data.v_925.bs_type << " | "
              << std::setw(11) << out.metrics.head_data.ratio_change_924 << "% | "
              << std::setw(11) << price_change_pct << "%"
              << std::endl;
}

void print_all() {
   
    std::cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    std::cout << "----------------" << std::endl; 
    std::cout << std::left  << std::setw(11) << "Date" << " | "
              << std::right << std::setw(5)  << "Ticks" << " | "
              << std::setw(9)  << "Vol" << " | "               
              << std::setw(9)  << "AM_Vol" << " | "
              << std::setw(11) << "AM_Turnover" << " | "  
              << std::setw(11) << "AM_Turn%" << " | " 
              << std::setw(10) << "Vol/Ticks" << " | "         
              << std::setw(11) << "Turnover" << " | "
              << std::setw(8)  << "AM_Close" << " | "  
              << std::setw(9)  << "Avg_Price" << " | " 
              << std::setw(7)  << "Close" << " | "  
              << std::setw(8)  << "AM_Chan%" << " | "               
              << std::setw(8)  << "Change%" << " | "     
              << std::setw(10) << "Net_In" << " | "     
              << std::setw(10) << "AM_Net_In" << " | "
              << std::setw(10) << "PM_Net_In" << " | " 
              << std::setw(9) << "Net_In%" << " | " 
              << std::setw(9) << "Net/change" << " | " 
              << std::setw(11) << "Hist_Cum" << " | "   
              << std::left  << std::setw(20) << "Signal" 
              << std::endl;
    std::cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    std::cout << "----------------" << std::endl;
}

void print_headers(const ProgramOptions& opts) {
    if (opts.show_head){
        print_table_header();
    } 

    if (opts.show_all){
        print__headers("ALL", data_all_table_cols);
    } 

    if (opts.show_will){
        print__headers("WILL", will_table_cols);
    }

    if (opts.show_price){
        print__headers("PRICE ", price_table_cols);
    }   

    if (opts.show_super){
        print__headers("SUPER", will_price_table_cols);
    }

    if (opts.show_super_ratio){
        print__headers("SUPER_RATIO ", will_price_ratio_table_cols) ;
    }   
    
    if (opts.show_big){
        print__headers("BIG", will_price_table_cols);
    }
    
    if (opts.show_big_ratio){
        print__headers("BIG_RATIO ", will_price_ratio_table_cols) ;
    }  

    if (opts.show_middle){
        print__headers("MIDDLE", will_price_table_cols);
    }
    
    if (opts.show_middle_ratio){
        print__headers("MIDDLE_RATIO ", will_price_ratio_table_cols) ;
    }

    if (opts.show_small){
        print__headers("SMALL", will_price_table_cols);
    }

    if (opts.show_small_ratio){
        print__headers("SMALL_RATIO ", will_price_ratio_table_cols) ;
    }

    if (opts.show_total){
        print__headers("TOTAL", will_price_table_cols);
    }

    if (opts.show_total_ratio){
        print__headers("TOTAL_RATIO", will_price_ratio_table_cols);
    }
}

void print_bodys(const ProgramOptions& opts, const DayOutputMetrics& out, const DayOutputMetrics& prev_out, std::string divergence)  {
        if (opts.show_head){
            print_header_info(out, prev_out);
        }  

        if (opts.show_all){
            print_all_data(out, prev_out, divergence);
        }

        if (opts.show_will){
            print_will(out, prev_out, out.metrics, will_table_cols);
        }  

        if (opts.show_price){
            print_price(out, prev_out, out.metrics, price_table_cols);
        } 

        if (opts.show_super){
            print_slim_price(out, prev_out, RecordScale::SUPER, will_price_table_cols);
        }

        if (opts.show_super_ratio){
            print_slim_price_ratio(out, prev_out, RecordScale::SUPER, will_price_ratio_table_cols);
        } 

        if (opts.show_big){
            print_slim_price(out, prev_out, RecordScale::BIG, will_price_table_cols);
        }
        
        if (opts.show_big_ratio){
            print_slim_price_ratio(out, prev_out, RecordScale::BIG, will_price_ratio_table_cols);
        } 

        if (opts.show_middle){
            print_slim_price(out, prev_out, RecordScale::MIDDLE, will_price_table_cols);
        }
        
        if (opts.show_middle_ratio){
            print_slim_price_ratio(out, prev_out, RecordScale::MIDDLE, will_price_ratio_table_cols);
        } 

        if (opts.show_small){
            print_slim_price(out, prev_out, RecordScale::SMALL, will_price_table_cols);
        }
        
        if (opts.show_small_ratio){
            print_slim_price_ratio(out, prev_out, RecordScale::SMALL, will_price_ratio_table_cols);
        } 

        if (opts.show_total){
            print_slim_price(out, prev_out, RecordScale::TOTAL, will_price_table_cols);
        } 

        if (opts.show_total_ratio){
            print_slim_price_ratio(out, prev_out, RecordScale::TOTAL, will_price_ratio_table_cols);
        } 
}

#endif // SHOW_H