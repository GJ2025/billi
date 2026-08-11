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
#include <unistd.h> 
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include "common.h"
#include "tick_types.h"
#include "collect_stream.h"
#include "tick_print.h"
#include "process_files.h"
#include "time_seq.h"

namespace fs = std::filesystem;

bool record_should_process(TickRecord& record);
void process_last_record(DailyMetrics& metrics, StreamRecord& stream, TickRecord record);
void process_first_record(DailyMetrics& metrics, TickRecord record, TickRecord& pre_record);

bool is_loading_data(const std::string& str) {
    if (str.empty()) return false;
    return std::isdigit(static_cast<unsigned char>(str[0]));
}


std::string extract_company_id(const std::string& filename) {
    std::string pure_name = fs::path(filename).stem().string(); 
    
    size_t underscore_pos = pure_name.find('_');
    if (underscore_pos != std::string::npos) {
        std::string part1 = pure_name.substr(0, underscore_pos);
        std::string part2 = pure_name.substr(underscore_pos + 1);
        
        if (part1.find('-') != std::string::npos) return part2;
        if (part2.find('-') != std::string::npos) return part1;
        
        return (part1.length() < part2.length()) ? part1 : part2;
    }
    
    return pure_name;
}

bool check_company_id_match(const std::string& file_path, const std::string& target_company_id) {
    std::string current_company_id = extract_company_id(file_path);
    if (current_company_id != target_company_id) {
        std::cout << "[Skip File] Mismatched Company (" 
                  << current_company_id << " != " << target_company_id 
                  << "): " << fs::path(file_path).filename().string() << std::endl;
        return false;
    }
    return true;
}


std::string get_divergence_string(const DayOutputMetrics& out, const DayOutputMetrics& prev_out) {
    (void)prev_out;

    double will_net_money = out.metrics.deal_total_bsn.buy.money - out.metrics.deal_total_bsn.sale.money;
    double price_net_money = out.metrics.deal_total_price.up.money - out.metrics.deal_total_price.down.money;

    // double avg_change = out.metrics.avg_price - prev_out.metrics.avg_price;
    #define PRICE_THRESHOLD 0.05

    std::vector<std::string> signals;

    if (out.pct_change_base_925 > 0 && will_net_money < 0) {
        signals.push_back("[UP_OUT]");
    }
    
    if (out.pct_change_base_925 > 0 && price_net_money < 0) {
        signals.push_back("[UP_POUT]");
    } 
    
    if (out.pct_change_base_925 < 0 && will_net_money > 0) {
        signals.push_back("[DN_IN]");
    }

    if (out.pct_change_base_925 < 0 && price_net_money > 0) {
        signals.push_back("[DN_PIN]");
    }


    if (signals.empty()) {
        return "      -      ";
    }

    std::string result;
    for (size_t i = 0; i < signals.size(); ++i) {
        result += signals[i];
        if (i < signals.size() - 1) result += " ";
    }
    return result;
}

bool is_filled_tick(const DayOutputMetrics& out){
    bool had_one = (out.metrics.ticks_count > 0 && out.metrics.closing_price > 0.0);
    return had_one;
}

std::string get_and_print_signals(const DayOutputMetrics& out, const DayOutputMetrics& prev_out) {

    std::string divergence_str = get_divergence_string(out, prev_out); 
    return divergence_str;
}

void update_head_tick_data(HeadTickData& head_data, const TickRecord& record) {
    if (record.time.find("09:24") == 0) {
        head_data.pre_924 = head_data.v_924;
        head_data.v_924 = record;
    } else if (record.time.find("09:25") == 0) {
        head_data.v_925 = record;
    }
}

void calculate_head_tick_changes(HeadTickData& head_data) {
    auto calc_pct = [](double target, double base) -> double {
        if (base <= 0.0){
            return 0.0;
        }    
        
        return ((target - base) / base) * 100.0;
    };

    if (!head_data.v_924.time.empty()) {
        head_data.ratio_change_924 = calc_pct(head_data.v_925.price, head_data.v_924.price);
    }
    
    if (!head_data.pre_924.time.empty()) {
        head_data.ratio_change_pre_924 = calc_pct(head_data.v_925.price, head_data.pre_924.price);
    }
}

void process_head_data(DailyMetrics& metrics, const TickRecord& record) {
    if (metrics.head_data.v_925.time.empty()) {
        if (record.time.find("09:24") == 0 || record.time.find("09:25") == 0) {
            update_head_tick_data(metrics.head_data, record);
        }
    }
    
    if (!metrics.head_data.v_925.time.empty() && !metrics.head_calculated) {
        calculate_head_tick_changes(metrics.head_data);
        metrics.head_calculated = true; 
    }
}

void update_metrics_by_record(DailyMetrics& metrics, const TickRecord& record){
    metrics.ticks_count++;
    metrics.closing_price = record.price;

    trade am_trade;
    trade pm_trade;

    size_t volume = record.volume * 100; 
    double money = record.price * volume;

  if (is_am_time(record.t)) {

        if (record.bs_type == "B"){
            metrics.am_bsn.buy.money += money;
            metrics.am_bsn.buy.volume += volume;        
        }else if (record.bs_type == "S"){
            metrics.am_bsn.sale.money += money;
            metrics.am_bsn.sale.volume += volume;      
        }else{
            metrics.am_bsn.neutral.money += money;
            metrics.am_bsn.neutral.volume += volume;            
        } 
    }else {

        if (record.bs_type == "B"){
            metrics.pm_bsn.buy.money += money;
            metrics.pm_bsn.buy.volume += volume;         
        }else if (record.bs_type == "S"){
            metrics.pm_bsn.sale.money += money;
            metrics.pm_bsn.sale.volume += volume;     
        }else{
            metrics.pm_bsn.neutral.money += money;
            metrics.pm_bsn.neutral.volume += volume;  
        } 
    }

    am_trade.money = total_money(metrics.am_bsn);
    am_trade.volume = total_volume(metrics.am_bsn);

    pm_trade.money = total_money(metrics.pm_bsn);
    pm_trade.volume = total_volume(metrics.pm_bsn);

    metrics.all_money = am_trade.money + pm_trade.money;
    metrics.all_volume = am_trade.volume + pm_trade.volume;

    metrics.avg_price = metrics.all_money / metrics.all_volume;

    // if (last_record(record)){
    //     std::cout << "update_metrics_by_record   "<< metrics.avg_price << "=" << metrics.all_money << "/" << metrics.all_volume << std::endl;
    // }

    return;
}

void read_tick_records(const std::string& filename, std::vector<TickRecord>& records) {
    std::ifstream infile(filename);
    std::string line;
    records.clear(); 

    // 略过前两行表头
    std::getline(infile, line); 
    std::getline(infile, line);

    while (std::getline(infile, line)) {
        if (line.empty()) {
            continue;
        } 

        std::stringstream ss(line);
        TickRecord record;
        
        if (ss >> record) {
            if (!record_should_process(record)) {
                continue;
            }
            records.push_back(std::move(record));
        } else {
            std::cout << "failed=========== " << record.time << std::endl;
            exit(0);
        }
    }
    infile.close();
}

void parse_tick_records(std::vector<TickRecord>& records, DailyMetrics& metrics, DailyMetrics& am_metrics) {

    TickRecord pre_record;
    StreamRecord stream;
    bool has_pre = false;

    for (const auto& record : records) {
        process_first_record(metrics, record, pre_record);

        if (has_pre && record_change(record, pre_record)) {
            update_metrics_header(metrics.header, stream);
        }

        if (has_pre && is_am_end(record.t, pre_record.t)) {
            update_metrics_header(metrics.header, stream);
            am_metrics = metrics;
        }

        update_stream(stream, record, pre_record);
        update_metrics_by_record(metrics, record);

        process_last_record(metrics, stream, record);

        pre_record = record;
        has_pre = true;
    }
}

void parse_tick_file_by_tseq(std::ifstream& infile, std::vector<DailyMetrics>& all_metrics, std::vector<tickTime>& tick_times) {
    std::string line;
    TickRecord pre_record;
    tickTime pre_seq_time;
    StreamRecord stream;

    size_t tick_idx = 0;
    DailyMetrics current_metrics;

    std::getline(infile, line); 
    std::getline(infile, line);
    while (std::getline(infile, line)) {
        if (line.empty()){
            continue;
        } 

        std::stringstream ss(line);
        TickRecord record;
        
        if (ss >> record) {
            
            if (!record_should_process(record)){
                continue;
            }

            process_first_record(current_metrics, record, pre_record);

            if (record_change(record, pre_record)) {
                update_metrics_header(current_metrics.header, stream);
            }

            if (tick_idx < tick_times.size() 
                && (is_tick_time_end(record.t, pre_record.t, tick_times[tick_idx]) || is_tick_time_end(record.t, pre_seq_time, tick_times[tick_idx]))) {
                pre_seq_time = tick_times[tick_idx];
                all_metrics.push_back(current_metrics);
                tick_idx++; 
            }

            update_stream(stream, record, pre_record);
            update_metrics_by_record(current_metrics, record);

            process_last_record(current_metrics, stream, record);

        }else{
                std::cout << "failed=========== " << record.time << std::endl;
                exit(0);
        }

        pre_record = record;
    }
    infile.close();
}

void process_last_record(DailyMetrics& metrics, StreamRecord& stream, TickRecord record){
    if (last_record(record)) {

        update_metrics_header(metrics.header, stream);
        set_metrics_record(metrics, record, RecordType::LAST);

    }
}

void process_first_record(DailyMetrics& metrics, TickRecord record, TickRecord& pre_record){
    if (first_record(record)) {
        pre_record = record;
        set_metrics_record(metrics, record, RecordType::FIRST);
    }
}

bool record_should_process(TickRecord& record){

    if (!is_loading_data(record.time)){
        return false;        
    } 

    if (record.deal_count == 0){
        return false;   
    }  

    if (after_15(record.t)){
       return false;   
    } 

    return true;

}

bool process_single_file(const std::string& filename, DayOutputMetrics& out) {

    std::vector<TickRecord> records;

    DailyMetrics metrics;
    DailyMetrics am_metrics;

    read_tick_records(filename, records);

    parse_tick_records(records, metrics, am_metrics);

    if (metrics.ticks_count == 0) {
        return false;
    }
    
    std::string pure_name = fs::path(filename).filename().string();
    out.date_str = (pure_name.length() >= 10) ? pure_name.substr(0, 10) : pure_name;

    out.metrics = metrics;
    out.am_metrics = am_metrics;

    return true;
}

void process_out(DayOutputMetrics& out, DayOutputMetrics& prev_out){

        out.historical_total_inflow = prev_out.historical_total_inflow + 
                                        out.metrics.deal_total_bsn.buy.money - out.metrics.deal_total_bsn.sale.money;
        if (is_filled_tick(prev_out)) {
            out.pct_change_base_pre =pct(out.metrics.closing_price, prev_out.metrics.closing_price);
            out.am_pct_change = pct(out.am_metrics.closing_price, prev_out.metrics.closing_price);
            out.start_change = pct(out.metrics.daily_first_record.price, prev_out.metrics.closing_price);
            out.avg_pct_change = pct(out.metrics.avg_price, prev_out.metrics.avg_price);
        }

        out.pct_change_base_925 = pct(out.metrics.closing_price, out.metrics.daily_first_record.price);

        return;
}

void make_test(DayOutputMetrics& out){

    int i = 0;
    bool should_exist = false;
    const std::vector<Col>& cols = test_table_cols;

    if (out.metrics.am_bsn.buy.money != out.am_metrics.deal_total_bsn.buy.money){
        print__headers("TEST", test_table_cols);
        print_next(out.date_str, i, cols);
        print_next(out.metrics.ticks_count, i, cols);

        print_next(out.metrics.am_bsn.buy.money/WAN, i, cols);
        print_next(out.am_metrics.deal_total_bsn.buy.money/WAN, i, cols);

        print_next(out.metrics.am_bsn.sale.money/WAN, i, cols);
        print_next(out.am_metrics.deal_total_bsn.sale.money/WAN, i, cols);

        print_next(out.metrics.pm_bsn.buy.money/WAN, i, cols);
        print_next((out.metrics.deal_total_bsn.buy.money - out.am_metrics.deal_total_bsn.buy.money)/WAN, i, cols);

        print_next(out.metrics.pm_bsn.sale.money/WAN, i, cols);
        print_next((out.metrics.deal_total_bsn.sale.money - out.am_metrics.deal_total_bsn.sale.money)/WAN, i, cols);
        std::cout << std::endl;
        
        should_exist = true;
        
        print__headers("TEST", test_table_cols);
    }


    if (should_exist){
        exit(0);
    }


    return;
}

int parse_tseq_opt(int argc, char* argv[], ProgramOptions& opts) {
    // 直接遍历 -t 之后的参数
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-N" && i + 1 < argc) {
            opts.tseq.cnt = std::stoi(argv[++i]);
        } else if (arg == "-I" && i + 1 < argc) {
            opts.tseq.intervel = std::stoi(argv[++i]);
        } else if (arg == "-H" && i + 1 < argc) {
            opts.tseq.start_hour = std::stoi(argv[++i]);
        } else if (arg == "-M" && i + 1 < argc) {
            opts.tseq.start_min = std::stoi(argv[++i]);
        } else if (arg == "-d" && i + 1 < argc) {
            opts.lvmeng_dir_path = argv[++i];
        }
    }
    return 0;
}

int parse_opt(int argc, char* argv[], ProgramOptions& opts){
    int opt;
    while ((opt = getopt(argc, argv, "hd:parwsqSmbl:MtD:")) != -1) {
        switch (opt) {
            case 'h': opts.show_head = true; break;
            case 'd': opts.lvmeng_dir_path = optarg; break;
            case 'D': opts.data_dir_path = optarg; break;
            case 'r': opts.show_income_ratio = true; break;
            case 'a': opts.show_all = true; break;
            case 'w': opts.show_will = true; break;
            case 'p': opts.show_price = true; break;
            case 'm': opts.show_merge = true; break;
            case 's': opts.show_super = true; break;
            case 'b': opts.show_big = true; break;
            case 'q': opts.show_quiet = true; break;
            case 'M': {
                opts.show_middle = true; 
                break;
            }
            case 'S': {
                opts.show_small = true; 
                break;
            }
            case 'l': 
                {
                    opts.show_limit = std::stoi(optarg);
                    break;
                }
            case 't': {
                parse_tseq_opt(argc - optind, argv + optind, opts);
                optind = argc;
                opts.show_t = true;
                opts.show_limit = 1;
                break;
            }
            default:
                std::cerr << "Usage: " << argv[0] << " [-h] [-d path] [-p] [-a] [-r] [-w] [-s] [-m]" << std::endl;
                return 1;
        }
    }

    return 0;
}

void process_files_to_metrics(const std::vector<std::string>& files_to_process, std::vector<DayOutputMetrics>& out_vector) {
    out_vector.clear(); // 确保传入的 vector 是干净的

    DayOutputMetrics prev_out;  
    
    if (files_to_process.empty()) {
        return;
    }

    std::string target_company_id = extract_company_id(files_to_process[0]);

    for (const auto& file : files_to_process) {
        if (!check_company_id_match(file, target_company_id)) {
            continue;
        }

        DayOutputMetrics out;

        if (!process_single_file(file, out)) {
            continue;
        }

        if (out.metrics.ticks_count <= 0) {
            continue;
        }

        deal_classfy(out.metrics);
        deal_classfy(out.am_metrics);
        process_out(out, prev_out);

        out_vector.push_back(out);

        prev_out = out;

    }
}

void print_metrics(const ProgramOptions& opts,  const std::vector<DayOutputMetrics>& out_vector) {
    std::string divergengce;
    DayOutputMetrics prev_out;  

    print_headers(opts);

    for (const auto& out : out_vector) {

        divergengce = get_and_print_signals(out, prev_out);

        print_bodys(opts, out, prev_out, divergengce);

        if (out.metrics.ticks_count > 0) {
            prev_out = out;
        }
    }

    print_headers(opts);

    std::cout << "\r\n" << std::endl;
}


inline std::string get_display_file(const std::string& file) {
    namespace fs = std::filesystem;
    fs::path p(file);
    
    if (p.has_parent_path() && p.has_filename()) {
        return p.parent_path().filename().string() + "/" + p.filename().string();
    }
    return file;
}

int metrics_shrink_firm(const std::vector<DayOutputMetrics>& out_vector) {
    if (out_vector.size() < 2) {
        return 0;
    }

    int j = 0;
    // 从末尾向前遍历：对比 out_vector[i] 和 out_vector[i - 1]
    for (size_t i = out_vector.size() - 1; i >= 1; --i) {
        if (metrics_total_volume(out_vector[i].metrics) > metrics_total_volume(out_vector[i - 1].metrics)) {
            break;
        }
        ++j;
    }

    return j;
}

int metrics_grow_firm(const std::vector<DayOutputMetrics>& out_vector) {
    if (out_vector.size() < 2) {
        return 0;
    }

    int j = 0;
    // 从末尾向前遍历：对比 out_vector[i] 和 out_vector[i - 1]
    for (size_t i = out_vector.size() - 1; i >= 1; --i) {
        if (metrics_total_volume(out_vector[i].metrics) < metrics_total_volume(out_vector[i - 1].metrics)) {
            break;
        }
        ++j;
    }

    return j;
}


int metrics_shrink_loose(const std::vector<DayOutputMetrics>& out_vector) {
    if (out_vector.size() < 2) {
        return 0;
    }

    int j = 0;
    // 从末尾向前遍历：对比 out_vector[i] 和 out_vector[i - 1]
    for (size_t i = out_vector.size() - 1; i >= 1; --i) {
        if (metrics_total_volume(out_vector[out_vector.size() - 1].metrics) > metrics_total_volume(out_vector[i - 1].metrics)) {
            break;
        }
        ++j;
    }

    return j;
}

int metrics_grow_loose(const std::vector<DayOutputMetrics>& out_vector) {
    if (out_vector.size() < 2) {
        return 0;
    }

    int j = 0;
    // 从末尾向前遍历：对比 out_vector[i] 和 out_vector[i - 1]
    for (size_t i = out_vector.size() - 1; i >= 1; --i) {
        if (metrics_total_volume(out_vector[out_vector.size() - 1].metrics) < metrics_total_volume(out_vector[i - 1].metrics)) {
            break;
        }
        ++j;
    }

    return j;
}


void calculate_trade_stats(const record_stream& h, TradeCategoryStats& stats) {
   
    accumulate_group(h.super,  stats.buy_down, stats.buy_up, stats.buy_keep, 
                              stats.sale_down, stats.sale_up, stats.sale_keep, 
                              stats.neutral_down, stats.neutral_up, stats.neutral_keep);

    accumulate_group(h.big,    stats.buy_down, stats.buy_up, stats.buy_keep, 
                              stats.sale_down, stats.sale_up, stats.sale_keep, 
                              stats.neutral_down, stats.neutral_up, stats.neutral_keep);

    accumulate_group(h.middle, stats.buy_down, stats.buy_up, stats.buy_keep, 
                              stats.sale_down, stats.sale_up, stats.sale_keep, 
                              stats.neutral_down, stats.neutral_up, stats.neutral_keep);

    accumulate_group(h.small,  stats.buy_down, stats.buy_up, stats.buy_keep, 
                              stats.sale_down, stats.sale_up, stats.sale_keep, 
                              stats.neutral_down, stats.neutral_up, stats.neutral_keep);


    calculate_total(stats.total, stats.buy_down, stats.buy_up, stats.buy_keep, 
                                 stats.sale_down, stats.sale_up, stats.sale_keep, 
                                 stats.neutral_down, stats.neutral_up, stats.neutral_keep);
}

void get_signal_from_metrics(size_t size, const std::vector<std::string>& files_to_process, const std::vector<DayOutputMetrics>& out_vector) {
    if (size < 2 || size > files_to_process.size() || size > out_vector.size()) {
        return;
    }

    TradeCategoryStats current;
    TradeCategoryStats prev;

    const auto& file = files_to_process[size - 1]; 
    const auto& out = out_vector[size - 1];
    const auto& pre_out = out_vector[size - 2];

    const auto& h = out.metrics.header;
    const auto& prev_h = pre_out.metrics.header;

    calculate_trade_stats(h, current);
    calculate_trade_stats(prev_h, prev);

    double all_will_netin = metrics_bsn_net(out.metrics);
    double all_price_netin = metrics_price_net(out.metrics);

    double prev_all_will_netin = metrics_bsn_net(pre_out.metrics);
    double prev_all_price_netin = metrics_price_net(pre_out.metrics);

    double will_netin_change = (all_will_netin - prev_all_will_netin) / std::abs(prev_all_will_netin);
    double price_netin_change = (all_price_netin - prev_all_price_netin) / std::abs(prev_all_price_netin);

    double out_buyup = pct_base(current.buy_up.money,   current.total.money);
    double prev_out_buyup = pct_base(prev.buy_up.money,   prev.total.money);

    double out_buy_down = pct_base(current.buy_down.money,   current.total.money);
    double out_sale_up = pct_base(current.sale_up.money,   current.total.money);
    double prev_out_sale_up = pct_base(prev.sale_up.money,   current.total.money);

    int shrink_firm = metrics_shrink_firm(out_vector);
    int grow_firm = metrics_grow_firm(out_vector);

    int shrink_loose = metrics_shrink_loose(out_vector);
    int grow_loose = metrics_grow_loose(out_vector);

    bool base_condition = (all_will_netin > 0 || all_price_netin > 0);

    std::vector<SubCondition> sub_conditions = {
        {
            base_condition && (out.pct_change_base_925 < 0.1 || out.pct_change_base_pre < 0.1),
            "decs"
        },
        {
            base_condition && (will_netin_change > 0 && price_netin_change > 0 
                && out.pct_change_base_925 > 0 && out.pct_change_base_pre > 0 
                && pre_out.pct_change_base_925 > -0.9 && pre_out.pct_change_base_pre > -0.9
                && out_buyup > prev_out_buyup ),
                // && metrics_total_volume(out.metrics) > metrics_total_volume(pre_out.metrics),
            "SPEEDUP(" + std::to_string(out_buyup) + "vs" + std::to_string(out_buyup - prev_out_buyup) + ")" 
        },
        {
            base_condition && ((out.pct_change_base_925 < 0 || out.pct_change_base_pre < 0) && shrink_firm >= 3),
            "desc_shr"
        },
        {
            base_condition && ((out_buy_down == 0 &&  out_sale_up > prev_out_sale_up && out.pct_change_base_pre > 1.0)),
            "will_down"
        },
        {
            base_condition && shrink_firm >= 3,
            "shr" 
        },
        {
            shrink_loose >= 6,
            "shr_loose" 
        }
    };

    // 3. 检查是否有任意子条件满足（如果需要，还可以顺便记录是哪条规则触发的）
    bool any_sub_condition = false;
    std::string triggered_desc;

    for (const auto& sc : sub_conditions) {
        if (sc.satisfied) {
            any_sub_condition = true;
            if (triggered_desc.empty()) {
                triggered_desc = sc.description;
            } else {
                triggered_desc += "---" + sc.description; // 如果需要用其他符号连接，修改这里
            }
        }
    }
    // 4. 最终触发判断
    if (any_sub_condition) {
        signal_info a;
        a.all_price_netin = all_price_netin;
        a.all_will_netin = all_will_netin;
        a.will_netin_change = will_netin_change;
        a.price_netin_change = price_netin_change;
        a.display_file = get_display_file(file);
        a.out = out;

        a.trigger_reason = triggered_desc;
        a.shrink_firm = shrink_firm;
        a.grow_firm = grow_firm;

        a.shrink_loose = shrink_loose;
        a.grow_loose = grow_loose;

        print_signal(out, a);
    }
}

void show_metrics_by_opts(const ProgramOptions& opts, const std::vector<DayOutputMetrics>& out_vector) {
    // 移除 const，确保成员指针允许被用于赋值操作
    bool ProgramOptions::* const flags[] = {
        &ProgramOptions::show_head,
        &ProgramOptions::show_all,
        &ProgramOptions::show_merge,
        &ProgramOptions::show_will,
        &ProgramOptions::show_price,
        &ProgramOptions::show_super,
        &ProgramOptions::show_big,
        &ProgramOptions::show_middle,
        &ProgramOptions::show_small,
        &ProgramOptions::show_quiet,
    };

    for (auto flag_ptr : flags) {
        if (opts.*flag_ptr) {
            ProgramOptions this_opts{};
            this_opts.*flag_ptr = true; 
            print_metrics(this_opts, out_vector);
        }
    }
}

void handle_tseq_mode(const ProgramOptions& opts, const std::vector<std::string>& files_to_process) {
    if (files_to_process.empty()) {
        return;
    }

    std::vector<tickTime> tick_times_seq;
    std::vector<DailyMetrics> all_metrics;
    
    tick_times_seq = min_vector(opts.tseq);
    std::reverse(tick_times_seq.begin(), tick_times_seq.end());
    show_time_vector(tick_times_seq);
    
    std::ifstream infile(files_to_process[0]);
    parse_tick_file_by_tseq(infile, all_metrics, tick_times_seq);

    print__headers("PRICE ", tseq_price_table_cols);

    for (size_t i = 0; i < all_metrics.size() && i < tick_times_seq.size(); ++i) {
        auto& m = all_metrics[i];
        const auto& t = tick_times_seq[i]; 
        
        deal_classfy(m);
        print_tseq_price(t, m);
    }

    print__headers("PRICE ", tseq_price_table_cols);
}

int init_and_get_files_wrapper(const ProgramOptions& opts, std::vector<std::string>& files_to_process) {
    return initialize_and_get_files(opts.lvmeng_dir_path, opts.show_limit, files_to_process);
}

void select_stock(const std::string& data_dir_path, size_t show_limit) {
    std::vector<std::string> files_to_process;
    std::vector<DayOutputMetrics> out_vector;

    initialize_and_get_files(data_dir_path, show_limit, files_to_process);
    process_files_to_metrics(files_to_process, out_vector); 

    if (files_to_process.size() !=  out_vector.size() ){
        std::cout << "impossible: " << data_dir_path << " " << std::endl; 
        return;
    }

    size_t size = std::min(show_limit, out_vector.size());

    
    get_signal_from_metrics(size, files_to_process, out_vector);


}

void process_subdirectories(const std::string& data_dir_path, size_t show_limit) {
    namespace fs = std::filesystem;

    // 检查目录是否存在且是一个目录
    if (!fs::exists(data_dir_path) || !fs::is_directory(data_dir_path)) {
        std::cerr << "Invalid directory path: " << data_dir_path << std::endl;
        return;
    }

    print__headers("QUIET", signal_table_cols);

    // 只遍历1层目录，使用 fs::directory_iterator
    for (const auto& entry : fs::directory_iterator(data_dir_path)) {
        if (entry.is_directory()) {
            std::string dir_name = entry.path().filename().string();

            // 过滤掉包含 show 或 tseq_show 的目录
            if (dir_name.find("show") != std::string::npos || 
                dir_name.find("tseq_show") != std::string::npos) {
                continue;
            }

            // std::cout << entry.path().string() << "\r\n" << std::endl;
            select_stock(entry.path().string(), show_limit);
        }
    }

    print__headers("QUIET", signal_table_cols);
}


int main(int argc, char* argv[]) {
    ProgramOptions opts;
    std::vector<std::string> files_to_process;
    std::vector<DayOutputMetrics> out_vector;

    if (parse_opt(argc, argv, opts) != 0){
        return 1;
    }

    if (opts.tseq.cnt != 0){
        initialize_and_get_files(opts.lvmeng_dir_path, opts.show_limit, files_to_process);    
        handle_tseq_mode(opts, files_to_process);
    }if (!opts.data_dir_path.empty()){

        process_subdirectories(opts.data_dir_path, opts.show_limit);

    }else{
        initialize_and_get_files(opts.lvmeng_dir_path, opts.show_limit, files_to_process);
        process_files_to_metrics(files_to_process, out_vector);
        show_metrics_by_opts(opts, out_vector);
    }

    
    return 0;
}