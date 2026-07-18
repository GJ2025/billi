#include <iostream>
#include <iomanip>
#include "common.h"
#include "tick_types.h"
#include "collect_stream.h"

void print_will( DayOutputMetrics& out) {

    double jing_super = out.deal_super_bsn.buy - out.deal_super_bsn.sale;
    double jing_big = out.deal_big_bsn.buy - out.deal_big_bsn.sale;
    double jing_middle = out.deal_middle_bsn.buy - out.deal_middle_bsn.sale;
    double jing_small = out.deal_small_bsn.buy - out.deal_small_bsn.sale;
    double jing_total = out.deal_total_bsn.buy - out.deal_total_bsn.sale;

    std::cout << std::left << std::setw(11) << out.date_str << " | "
                << std::fixed << std::setprecision(2)
                << "buy "
                << std::setw(12)  << out.deal_super_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_super_bsn.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_super/WAN  << std::noshowpos << " | "

                << std::setw(12)  << out.deal_big_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_big_bsn.sale/WAN << " | "
                << std::showpos  << std::setw(9)  << jing_big/WAN << std::noshowpos << " | "

                << std::setw(9)  << out.deal_middle_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_middle_bsn.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_middle/WAN << std::noshowpos << " | "

                << std::setw(9)  << out.deal_small_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_small_bsn.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_small/WAN << std::noshowpos << " | "

                << std::setw(9)  << out.deal_total_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_total_bsn.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_total/WAN << std::noshowpos << " | "

                << std::setw(12)  << (out.deal_total_bsn.buy+out.deal_total_bsn.sale+out.deal_total_bsn.neutral)/WAN << " | "  
                << std::setw(12)  << out.total_vol_wan << " | " 

                << std::setw(5)  << out.pre_closing_price << " | " 
                << std::setw(9)  << std::showpos << out.start_change << " | " << std::noshowpos
                << std::setw(9)  << std::showpos << out.pct_change << " | "  << std::noshowpos
                << std::setw(5)  << out.closing_price << " | " 
                << std::endl;

}

void print_price( DayOutputMetrics& out) {

    std::cout << std::left << std::setw(11) << out.date_str << " | "
                << std::fixed << std::setprecision(2)
                << "up "
                << std::setw(9)  << out.deal_super_price.up/WAN << " | "
                << std::setw(9)  << out.deal_super_price.down/WAN << " | "
                << std::setw(9)  << std::showpos <<(out.deal_super_price.up - out.deal_super_price.down)/WAN << " | " << std::noshowpos

                << std::setw(9)  << out.deal_big_price.up/WAN << " | "
                << std::setw(9)  << out.deal_big_price.down/WAN << " | "
                << std::setw(9)  << std::showpos <<(out.deal_big_price.up - out.deal_big_price.down)/WAN << " | " << std::noshowpos

                << std::setw(9)  << out.deal_middle_price.up/WAN << " | "
                << std::setw(9)  << out.deal_middle_price.down/WAN << " | "
                << std::setw(9)  << std::showpos <<(out.deal_middle_price.up - out.deal_middle_price.down)/WAN << " | " << std::noshowpos

                << std::setw(9)  << out.deal_small_price.up/WAN << " | "
                << std::setw(9)  << out.deal_small_price.down/WAN << " | "
                << std::setw(9)  << std::showpos <<(out.deal_small_price.up - out.deal_small_price.down)/WAN << " | " << std::noshowpos

                << std::setw(12)  << out.deal_total_price.up/WAN << " | "
                << std::setw(12)  << out.deal_total_price.down/WAN << " | "
                << std::setw(12)  << std::showpos <<(out.deal_total_price.up - out.deal_total_price.down)/WAN << " | " << std::noshowpos

                << std::setw(12)  << (out.deal_total_price.down+out.deal_total_price.up+out.deal_total_price.keep)/WAN << " | "  
                << std::setw(12)  << out.total_vol_wan << " | " 

                << std::setw(5)  << out.pre_closing_price << " | " 
                << std::setw(9)  << std::showpos << out.start_change << " | " << std::noshowpos
                << std::setw(9)  << std::showpos << out.pct_change << " | "  << std::noshowpos
                << std::setw(5)  << out.closing_price << " | " 
                << std::endl;

}

void print_merge( DayOutputMetrics& out) {


    std::cout << std::left << std::setw(11) << out.date_str << " | "
                << std::fixed << std::setprecision(2)

                << "buy "
                << std::setw(12)  << out.deal_total_bsn.buy/WAN << " | "
                << std::setw(12)  << out.deal_total_bsn.sale/WAN << " | "
                << std::setw(12)  << out.deal_total_bsn.neutral/WAN << " | "
                << "up "
                << std::setw(12)  << out.deal_total_price.up/WAN << " | "
                << std::setw(12)  << out.deal_total_price.down/WAN << " | "
                << std::setw(12)  << out.deal_total_price.keep/WAN << " | "

                << std::setw(16)  << std::showpos <<(out.deal_total_bsn.buy - out.deal_total_bsn.sale)/WAN << " | " << std::noshowpos
                << std::setw(16)  << std::showpos <<(out.deal_total_price.up - out.deal_total_price.down)/WAN << " | " << std::noshowpos

  
                << std::setw(12)  << (out.deal_total_price.down+out.deal_total_price.up+out.deal_total_price.keep)/WAN << " | "  
                << std::setw(12)  << out.total_vol_wan << " | " 

                << std::setw(5)  << out.pre_closing_price << " | " 
                << std::setw(9)  << std::showpos << out.start_change << " | " << std::noshowpos
                << std::setw(9)  << std::showpos << out.pct_change << " | "  << std::noshowpos
                << std::setw(5)  << out.closing_price << " | " 
                << std::endl;

}


// 专门负责打印带有标题的装饰线
void print_decorative_line(int total_width, const std::string& left_title, const std::string& right_title) {
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


void print_will_price_header(const std::string& title) {
    std::cout << std::left;
    int total_width = 0;

    // 1. 打印表头
    for (const auto& col : will_price_table_cols) {
        std::cout << std::setw(col.width) << col.name << " | ";
        total_width += (col.width + 3);
    }
    std::cout << std::endl;

    // 2. 调用封装好的装饰线逻辑
    print_decorative_line(total_width, title, title);
}

void print_slim_price(DayOutputMetrics& out, bs_action_group& super, deal_bsn& bsn, deal_price& price) {
    int i = 0;
    std::cout << std::left << std::fixed << std::setprecision(2);

    print_next(out.date_str, i);
    print_next(super.buy.down / WAN, i);
    print_next(super.buy.keep / WAN, i);
    print_next(super.buy.up / WAN, i);

    print_next(super.sale.down / WAN, i);
    print_next(super.sale.keep / WAN, i);
    print_next(super.sale.up / WAN, i);

    print_next(super.neutral.down / WAN, i);
    print_next(super.neutral.keep / WAN, i);
    print_next(super.neutral.up / WAN, i);

    print_next(bsn.buy / WAN, i);
    print_next(bsn.sale / WAN, i);
    print_next(bsn.neutral / WAN, i);

    print_next(price.up / WAN, i);
    print_next(price.down / WAN, i);
    print_next(price.keep / WAN, i);

    print_next_pos((bsn.buy - bsn.sale) / WAN, i);
    print_next_pos((price.up - price.down) / WAN, i);
    print_next((out.deal_total_bsn.buy + out.deal_total_bsn.sale + out.deal_total_bsn.neutral) / WAN, i);
    print_next(out.total_vol_wan, i);
    print_next(out.pre_closing_price, i);

    print_next_pos(out.start_change, i);
    print_next_pos(out.pct_change, i);

    print_next(out.closing_price, i);

    std::cout << std::endl;
}