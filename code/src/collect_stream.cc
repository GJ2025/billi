#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cctype>
#include <filesystem> 
#include <algorithm>  
#include <cmath>       
#include <cstdlib>   
#include <unistd.h> // 这是关键头文件
#include "tick_types.h"
#include "collect_stream.h"

bool record_change(TickRecord this_record, TickRecord pre_record){

    if (this_record.bs_type != pre_record.bs_type){
        return true;
    }

    if (this_record.bs_type == "S"){

        if (this_record.price <= pre_record.price){
            return false;
        }else{
            return true;
        }

    }else if (this_record.bs_type == "B"){

        if (this_record.price >= pre_record.price){
            return false;
        }else{
            return true;
        }

    }else{
        return true;
    }

    if (pre_record.price != this_record.price || pre_record.bs_type != this_record.bs_type){
        return true;
    }else{
        return false;
    }
}

bool last_record(TickRecord this_record){

    if (this_record.time == "15:00"){
        return true;
    }else{
        return false;
    }

}

bool first_record(TickRecord this_record){

    if (this_record.time == "09:25"){
        return true;
    }else{
        return false;
    }

}


void stream_new(StreamRecord& stream, TickRecord record, double pre_price){
    stream.records.clear();
    stream.records.push_back(record);
    stream.gap = record.price - pre_price;
}

void collect_price_action(range_price& sale, double trade, double gap){
    if (gap < 0.0){
        sale.down += trade;
    }else if (gap == 0.0){
        sale.keep += trade;
    }else{
        sale.up += trade;
    }

    return;
}

void collect_classfy_action(deal_range& deal, double trade, double gap){

    if (trade > 500000){
        collect_price_action(deal.super, trade, gap);
    }else if (trade > 200000){
        collect_price_action(deal.big, trade, gap);
    }else if (trade > 50000){
        collect_price_action(deal.middle, trade, gap);
    }else{
        collect_price_action(deal.small, trade, gap);
    }

    return;
}

void get_stream_sum(struct stream_sum& sum, StreamRecord& stream) {

    if (stream.records.empty()) return; 

    double total_stream_trade = 0.0;
    for (const auto& r : stream.records) {
        total_stream_trade += (r.volume * r.price * 100.0);
    }
    
    const std::string& bs_type = stream.records[0].bs_type;

    if (bs_type == "S") {
        collect_classfy_action(sum.classfy_s, total_stream_trade,stream.gap);
    } else if (bs_type == "B") {
        collect_classfy_action(sum.classfy_b, total_stream_trade,stream.gap);
    } else {
        collect_classfy_action(sum.classfy_m, total_stream_trade,stream.gap);
    }
}


double sum_action(range_price& deal){
    return deal.down+deal.keep+deal.up;
}

void deal_classfy(DayOutputMetrics& out) {

    out.deal_super.buy = sum_action(out.stream_sum_info.classfy_b.super);
    out.deal_super.sale = sum_action(out.stream_sum_info.classfy_s.super);
    out.deal_super.middle = sum_action(out.stream_sum_info.classfy_m.super);

    out.deal_big.buy = sum_action(out.stream_sum_info.classfy_b.big);
    out.deal_big.sale = sum_action(out.stream_sum_info.classfy_s.big);
    out.deal_big.middle = sum_action(out.stream_sum_info.classfy_m.big);

    out.deal_middle.buy = sum_action(out.stream_sum_info.classfy_b.middle);
    out.deal_middle.sale = sum_action(out.stream_sum_info.classfy_s.middle);
    out.deal_middle.middle = sum_action(out.stream_sum_info.classfy_m.middle);

    out.deal_small.buy = sum_action(out.stream_sum_info.classfy_b.small);
    out.deal_small.sale = sum_action(out.stream_sum_info.classfy_s.small);
    out.deal_small.middle = sum_action(out.stream_sum_info.classfy_m.small);

    out.deal_total.buy = out.deal_super.buy +  out.deal_big.buy + out.deal_middle.buy + out.deal_small.buy;
    out.deal_total.sale = out.deal_super.sale +  out.deal_big.sale + out.deal_middle.sale + out.deal_small.sale;
    out.deal_total.middle = out.deal_super.middle +  out.deal_big.middle + out.deal_middle.middle + out.deal_small.middle;

    return;
            
}

void print_classfy( DayOutputMetrics& out) {

    double jing_super = out.deal_super.buy - out.deal_super.sale;
    double jing_big = out.deal_big.buy - out.deal_big.sale;
    double jing_middle = out.deal_middle.buy - out.deal_middle.sale;
    double jing_small = out.deal_small.buy - out.deal_small.sale;
    double jing_total = out.deal_total.buy - out.deal_total.sale;

    std::cout << std::left << std::setw(11) << out.date_str << " | "
                << std::fixed << std::setprecision(2)
                << std::setw(12)  << out.deal_super.buy/10000 << " | "
                << std::setw(9)  << out.deal_super.sale/10000 << " | "

                << std::setw(12)  << out.deal_big.buy/10000 << " | "
                << std::setw(9)  << out.deal_big.sale/10000 << " | "

                << std::setw(9)  << out.deal_middle.buy/10000 << " | "
                << std::setw(9)  << out.deal_middle.sale/10000 << " | "

                << std::setw(9)  << out.deal_small.buy/10000 << " | "
                << std::setw(9)  << out.deal_small.sale/10000 << " | "

                << std::setw(9)  << out.deal_total.buy/10000 << " | "
                << std::setw(9)  << out.deal_total.sale/10000 << " | "
                 << std::setw(12)  << (out.deal_total.buy+out.deal_total.sale+out.deal_total.middle)/10000 << " | "  
                 << std::setw(12)  << out.total_vol_wan << " | " 

                << std::showpos
                << std::setw(9)  << jing_super/10000 << " | "
                << std::setw(9)  << jing_big/10000 << " | "
                << std::setw(9)  << jing_middle/10000 << " | "
                << std::setw(9)  << jing_small/10000 << " | "
                << std::setw(9)  << jing_total/10000 << " | "
                << std::noshowpos

                << std::setw(5)  << out.pre_closing_price << " | " 
                << std::setw(9)  << std::showpos << out.start_change << " | " << std::noshowpos
                << std::setw(9)  << std::showpos << out.pct_change << " | "  << std::noshowpos
                << std::setw(5)  << out.closing_price << " | " 
                << std::endl;

}

void update_stream_and_metrics(DailyMetrics& metrics, StreamRecord& stream, 
                               TickRecord& record, TickRecord& pre_record) {
    if (first_record(record)) {
        pre_record = record;
        metrics.first_record = record;
        stream_new(stream, record, record.price);
    }

    if (record_change(record, pre_record)) {
        get_stream_sum(metrics.stream_sum_info, stream);
        stream_new(stream, record, pre_record.price);
    } else {
        if (!first_record(record)) {
            stream.records.push_back(record);
        }
    }

    if (last_record(record)) {
        metrics.last_record = record;
        get_stream_sum(metrics.stream_sum_info, stream);
    }
}


