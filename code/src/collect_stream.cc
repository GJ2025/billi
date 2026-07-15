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
void collect_price_action(by_price& rp, double trade, double gap) {
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

double sum_rp(const by_price& rp) { return rp.up + rp.down + rp.keep; }

void deal_classfy(DayOutputMetrics& out) {
    // 1. 已有的 fill 逻辑 (保持不变)
    auto fill = [&](classfy_bs_action& dest, const bs_action_group& src) {
        dest.buy = sum_rp(src.buy);
        dest.sale = sum_rp(src.sale);
        dest.middle = sum_rp(src.neutral);
    };

    fill(out.deal_super, out.stream_sum_info.super);
    fill(out.deal_big, out.stream_sum_info.big);
    fill(out.deal_middle, out.stream_sum_info.middle);
    fill(out.deal_small, out.stream_sum_info.small);

    // 计算总计
    out.deal_total.buy = out.deal_super.buy + out.deal_big.buy + out.deal_middle.buy + out.deal_small.buy;
    out.deal_total.sale = out.deal_super.sale + out.deal_big.sale + out.deal_middle.sale + out.deal_small.sale;
    out.deal_total.middle = out.deal_super.middle + out.deal_big.middle + out.deal_middle.middle + out.deal_small.middle;

    // 2. 新增：封装后的价格汇总逻辑
    auto sum_group = [](by_price& dest, const bs_action_group& src) {
        dest.up   = src.buy.up   + src.sale.up   + src.neutral.up;
        dest.down = src.buy.down + src.sale.down + src.neutral.down;
        dest.keep = src.buy.keep + src.sale.keep + src.neutral.keep;
    };

    sum_group(out.price_deal_super,  out.stream_sum_info.super);
    sum_group(out.price_deal_big,    out.stream_sum_info.big);
    sum_group(out.price_deal_middle, out.stream_sum_info.middle);
    sum_group(out.price_deal_small,  out.stream_sum_info.small);

    // 计算总计价格分布
    out.price_deal_total.up   = out.price_deal_super.up   + out.price_deal_big.up   + out.price_deal_middle.up   + out.price_deal_small.up;
    out.price_deal_total.down = out.price_deal_super.down + out.price_deal_big.down + out.price_deal_middle.down + out.price_deal_small.down;
    out.price_deal_total.keep = out.price_deal_super.keep + out.price_deal_big.keep + out.price_deal_middle.keep + out.price_deal_small.keep;
}

