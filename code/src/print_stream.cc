#include "tick_types.h"
#include "collect_stream.h"
#include <iostream>
#include <iomanip>

void print_will( DayOutputMetrics& out) {

    double jing_super = out.deal_super.buy - out.deal_super.sale;
    double jing_big = out.deal_big.buy - out.deal_big.sale;
    double jing_middle = out.deal_middle.buy - out.deal_middle.sale;
    double jing_small = out.deal_small.buy - out.deal_small.sale;
    double jing_total = out.deal_total.buy - out.deal_total.sale;

    std::cout << std::left << std::setw(11) << out.date_str << " | "
                << std::fixed << std::setprecision(2)
                << "buy "
                << std::setw(12)  << out.deal_super.buy/WAN << " | "
                << std::setw(9)  << out.deal_super.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_super/WAN  << std::noshowpos << " | "

                << std::setw(12)  << out.deal_big.buy/WAN << " | "
                << std::setw(9)  << out.deal_big.sale/WAN << " | "
                << std::showpos  << std::setw(9)  << jing_big/WAN << std::noshowpos << " | "

                << std::setw(9)  << out.deal_middle.buy/WAN << " | "
                << std::setw(9)  << out.deal_middle.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_middle/WAN << std::noshowpos << " | "

                << std::setw(9)  << out.deal_small.buy/WAN << " | "
                << std::setw(9)  << out.deal_small.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_small/WAN << std::noshowpos << " | "

                << std::setw(9)  << out.deal_total.buy/WAN << " | "
                << std::setw(9)  << out.deal_total.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_total/WAN << std::noshowpos << " | "

                << std::setw(12)  << (out.deal_total.buy+out.deal_total.sale+out.deal_total.middle)/WAN << " | "  
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
                << std::setw(9)  << out.price_deal_super.up/WAN << " | "
                << std::setw(9)  << out.price_deal_super.down/WAN << " | "
                // << std::setw(9)  << out.price_deal_super.keep/WAN << " | "
                << std::setw(9)  << std::showpos <<(out.price_deal_super.up - out.price_deal_super.down)/WAN << " | " << std::noshowpos

                << std::setw(9)  << out.price_deal_big.up/WAN << " | "
                << std::setw(9)  << out.price_deal_big.down/WAN << " | "
                // << std::setw(9)  << out.price_deal_big.keep/WAN << " | "
                << std::setw(9)  << std::showpos <<(out.price_deal_big.up - out.price_deal_big.down)/WAN << " | " << std::noshowpos

                << std::setw(9)  << out.price_deal_middle.up/WAN << " | "
                << std::setw(9)  << out.price_deal_middle.down/WAN << " | "
                // << std::setw(9)  << out.price_deal_middle.keep/WAN << " | "
                << std::setw(9)  << std::showpos <<(out.price_deal_middle.up - out.price_deal_middle.down)/WAN << " | " << std::noshowpos

                << std::setw(9)  << out.price_deal_small.up/WAN << " | "
                << std::setw(9)  << out.price_deal_small.down/WAN << " | "
                // << std::setw(9)  << out.price_deal_small.keep/WAN << " | "
                << std::setw(9)  << std::showpos <<(out.price_deal_small.up - out.price_deal_small.down)/WAN << " | " << std::noshowpos

                << std::setw(12)  << out.price_deal_total.up/WAN << " | "
                << std::setw(12)  << out.price_deal_total.down/WAN << " | "
                // << std::setw(12)  << out.price_deal_total.keep/WAN << " | "
                << std::setw(12)  << std::showpos <<(out.price_deal_total.up - out.price_deal_total.down)/WAN << " | " << std::noshowpos

  
                << std::setw(12)  << (out.price_deal_total.down+out.price_deal_total.up+out.price_deal_total.keep)/WAN << " | "  
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
                << std::setw(12)  << out.deal_total.buy/WAN << " | "
                << std::setw(12)  << out.deal_total.sale/WAN << " | "
                << std::setw(12)  << out.deal_total.middle/WAN << " | "
                << "up "
                << std::setw(12)  << out.price_deal_total.up/WAN << " | "
                << std::setw(12)  << out.price_deal_total.down/WAN << " | "
                << std::setw(12)  << out.price_deal_total.keep/WAN << " | "

                << std::setw(16)  << std::showpos <<(out.deal_total.buy - out.deal_total.sale)/WAN << " | " << std::noshowpos
                << std::setw(16)  << std::showpos <<(out.price_deal_total.up - out.price_deal_total.down)/WAN << " | " << std::noshowpos

  
                << std::setw(12)  << (out.price_deal_total.down+out.price_deal_total.up+out.price_deal_total.keep)/WAN << " | "  
                << std::setw(12)  << out.total_vol_wan << " | " 

                << std::setw(5)  << out.pre_closing_price << " | " 
                << std::setw(9)  << std::showpos << out.start_change << " | " << std::noshowpos
                << std::setw(9)  << std::showpos << out.pct_change << " | "  << std::noshowpos
                << std::setw(5)  << out.closing_price << " | " 
                << std::endl;

}

void print_will_price_header() {
    std::cout << std::left 
              << std::setw(11) << "Date"        << " | "
              // 下面这些宽度调整为与数据打印的 12/9/9 保持一致
              << std::setw(12) << "Buy-Dn"      << " | "
              << std::setw(9)  << "Buy-Kp"      << " | "
              << std::setw(9)  << "Buy-Up"      << " | "
              << std::setw(12) << "Sale-Dn"     << " | "
              << std::setw(9)  << "Sale-Kp"     << " | "
              << std::setw(9)  << "Sale-Up"     << " | "
              << std::setw(12) << "Mid-Dn"      << " | "
              << std::setw(9)  << "Mid-Kp"      << " | "
              << std::setw(9)  << "Mid-Up"      << " | "
              << std::setw(12) << "Tot-Up"      << " | "
              << std::setw(9)  << "Tot-Dn"      << " | "
              << std::setw(9)  << "Tot-Kp"      << " | "
              << std::setw(9)  << "Net"         << " | "
              << std::setw(12) << "Total"       << " | "
              << std::setw(12) << "Volume"      << " | "
              << std::setw(5)  << "Pre"         << " | "
              << std::setw(9)  << "StartCh"     << " | " 
              << std::setw(9)  << "PctCh"       << " | "
              << std::setw(5)  << "Close"       << " | "
              << std::endl;
    std::cout << std::string(250, '-') << std::endl; 
}

void print_slim_price(DayOutputMetrics& out, bs_action_group& super){

    double price_up = 0.0;
    double price_down = 0.0;
    double price_keep = 0.0;
    double total = 0.0;

    price_up = super.buy.up + super.sale.up + super.neutral.up;
    price_down = super.buy.down + super.sale.down + super.neutral.down;
    price_keep = super.buy.keep + super.sale.keep + super.neutral.keep;

    total = price_up - price_down;


    std::cout << std::left << std::setw(11) << out.date_str << " | "
                << std::fixed << std::setprecision(2)
                << std::setw(12)  << super.buy.down/WAN << " | "
                << std::setw(9)  << super.buy.keep/WAN << " | "
                << std::setw(9)  << super.buy.up/WAN << " | "

                << std::setw(12)  << super.sale.down/WAN << " | "
                << std::setw(9)  << super.sale.keep/WAN << " | "
                << std::setw(9)  << super.sale.up/WAN << " | "

                << std::setw(12)  << super.neutral.down/WAN << " | "
                << std::setw(9)  << super.neutral.keep/WAN << " | "
                << std::setw(9)  << super.neutral.up/WAN << " | "

                << std::setw(12)  << price_up/WAN << " | "
                << std::setw(9)  << price_down/WAN << " | "
                << std::setw(9)  << price_keep/WAN << " | "

                << std::setw(9)  << total/WAN << " | "

                 << std::setw(12)  << (out.deal_total.buy+out.deal_total.sale+out.deal_total.middle)/WAN << " | "  
                 << std::setw(12)  << out.total_vol_wan << " | " 


                << std::setw(5)  << out.pre_closing_price << " | " 
                << std::setw(9)  << std::showpos << out.start_change << " | " << std::noshowpos
                << std::setw(9)  << std::showpos << out.pct_change << " | "  << std::noshowpos
                << std::setw(5)  << out.closing_price << " | " 
                << std::endl;
}