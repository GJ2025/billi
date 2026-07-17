#include "tick_types.h"
#include "collect_stream.h"
#include <iostream>
#include <iomanip>
#include "common.h"

// --- 辅助逻辑 ---
bool record_change(TickRecord this_record, TickRecord pre_record) {
    if (this_record.bs_type != pre_record.bs_type) return true;
    if (this_record.bs_type == "S") return this_record.price > pre_record.price;
    if (this_record.bs_type == "B") return this_record.price < pre_record.price;
    return true;
}

bool last_record(TickRecord this_record) { return this_record.time == "15:00"; }
bool first_record(TickRecord this_record) { return this_record.time == "09:25"; }

void stream_new(StreamRecord& stream, TickRecord record, double pre_price) {
    stream.records.clear();
    stream.records.push_back(record);
    stream.gap = record.price - pre_price;
}

// --- 核心逻辑 ---
void collect_price_action(deal_price& rp, double trade, double gap) {
    if (gap < 0.0) rp.down += trade;
    else if (gap == 0.0) rp.keep += trade;
    else rp.up += trade;
}

void collect_bs_action(bs_action_group& group, const std::string& bs_type, double trade, double gap) {
    if (bs_type == "B") collect_price_action(group.buy, trade, gap);
    else if (bs_type == "S") collect_price_action(group.sale, trade, gap);
    else collect_price_action(group.neutral, trade, gap);
}

void summary_stream(struct stream_sum& sum, StreamRecord& stream) {
    if (stream.records.empty()) return;
    double total_trade = 0.0;
    for (const auto& r : stream.records) total_trade += (r.volume * r.price * 100.0);
    
    bs_action_group* group = (total_trade > 100 * WAN) ? &sum.super :
                             (total_trade > 30 * WAN) ? &sum.big :
                             (total_trade > 5 * WAN)  ? &sum.middle : &sum.small;
    
    collect_bs_action(*group, stream.records[0].bs_type, total_trade, stream.gap);
}

void update_stream_and_metrics(DailyMetrics& metrics, StreamRecord& stream, 
                               TickRecord& record, TickRecord& pre_record) {
    if (first_record(record)) {
        pre_record = record;
        metrics.first_record = record;
        stream_new(stream, record, record.price);
    }
    if (record_change(record, pre_record)) {
        summary_stream(metrics.stream_sum_info, stream);
        stream_new(stream, record, pre_record.price);
    } else if (!first_record(record)) {
        stream.records.push_back(record);
    }
    if (last_record(record)) {
        metrics.last_record = record;
        summary_stream(metrics.stream_sum_info, stream);
    }
    pre_record = record;
}

void deal_classfy(DayOutputMetrics& out) {

    auto fill_bsn = [&](deal_bsn& dest, const bs_action_group& src) {
        dest.buy = sum_price(src.buy);
        dest.sale = sum_price(src.sale);
        dest.neutral = sum_price(src.neutral);
    };

    fill_bsn(out.deal_super_bsn, out.stream_sum_info.super);
    fill_bsn(out.deal_big_bsn, out.stream_sum_info.big);
    fill_bsn(out.deal_middle_bsn, out.stream_sum_info.middle);
    fill_bsn(out.deal_small_bsn, out.stream_sum_info.small);


    out.deal_total_bsn.buy = sum_bsn_buy(out.deal_super_bsn, out.deal_big_bsn, out.deal_middle_bsn, out.deal_small_bsn);
    out.deal_total_bsn.sale = sum_bsn_sale(out.deal_super_bsn, out.deal_big_bsn, out.deal_middle_bsn, out.deal_small_bsn);
    out.deal_total_bsn.neutral = sum_bsn_neutral(out.deal_super_bsn, out.deal_big_bsn, out.deal_middle_bsn, out.deal_small_bsn);

  
    auto fill_price = [](deal_price& dest, const bs_action_group& src) {
        dest.up   = src.buy.up   + src.sale.up   + src.neutral.up;
        dest.down = src.buy.down + src.sale.down + src.neutral.down;
        dest.keep = src.buy.keep + src.sale.keep + src.neutral.keep;
    };

    fill_price(out.deal_super_price,  out.stream_sum_info.super);
    fill_price(out.deal_big_price,    out.stream_sum_info.big);
    fill_price(out.deal_middle_price, out.stream_sum_info.middle);
    fill_price(out.deal_small_price,  out.stream_sum_info.small);

 
    out.deal_total_price.up   = sum_price_up(out.deal_super_price, out.deal_big_price, out.deal_middle_price,out. deal_small_price);
    out.deal_total_price.down = sum_price_down(out.deal_super_price, out.deal_big_price, out.deal_middle_price,out. deal_small_price);
    out.deal_total_price.keep = sum_price_keep(out.deal_super_price, out.deal_big_price, out.deal_middle_price,out. deal_small_price);
}

