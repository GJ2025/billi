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
void process_last_record(DailyMetrics& metrics, StreamRecord& stream, TickRecord record, double pre_price);
void process_first_record(DailyMetrics& metrics, StreamRecord& stream, TickRecord record, double pre_closing_price);

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

    bsn_action_group dump;
    deal_summary deal_summary_total;
    get_slim_base(out.metrics, RecordScale::TOTAL, dump, deal_summary_total);

    double will_net_money = deal_summary_total.bsn.buy.money - deal_summary_total.bsn.sale.money;
    double price_net_money = deal_summary_total.price.up.money - deal_summary_total.price.down.money;

    // double avg_change = out.metrics.avg_price - prev_out.metrics.avg_price;
    #define PRICE_THRESHOLD 0.05

    std::vector<std::string> signals;

    if (out.pct_change_base_pre > 0 && will_net_money < 0) {
        signals.push_back("[UP_OUT]");
    }
    
    if (out.pct_change_base_pre > 0 && price_net_money < 0) {
        signals.push_back("[UP_POUT]");
    } 
    
    if (out.pct_change_base_pre < 0 && will_net_money > 0) {
        signals.push_back("[DN_IN]");
    }

    if (out.pct_change_base_pre < 0 && price_net_money > 0) {
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

void parse_tick_records(std::vector<TickRecord>& records, DailyMetrics& metrics, DailyMetrics& am_metrics, double prev_closing_price) {

    TickRecord pre_record;
    StreamRecord stream;


    for (const auto& record : records) {
        process_first_record(metrics, stream, record, prev_closing_price);

        if (pre_record.full() && record_change(record, pre_record)) {
            update_metrics_header(metrics.header, stream);
        }

        if (pre_record.full() && is_am_end(record.t, pre_record.t)) {
            update_metrics_header(metrics.header, stream);
            am_metrics = metrics;
        }

        if (pre_record.full()){
            update_stream(stream, record, pre_record);
        }

        update_metrics_by_record(metrics, record);

        process_last_record(metrics, stream, record, pre_record.price);

        pre_record = record;
    }
}

void parse_tick_file_by_tseq(std::ifstream& infile, std::vector<DailyMetrics>& all_metrics, std::vector<tickTime>& tick_times) {
    std::string line;
    TickRecord pre_record;
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

            process_first_record(current_metrics, stream, record, record.price);

            if (pre_record.full() && record_change(record, pre_record)) {
                update_metrics_header(current_metrics.header, stream);
            }

            if (tick_idx < tick_times.size() && check_time(record.t, tick_times[tick_idx]) >= 0) {
                all_metrics.push_back(current_metrics);
                tick_idx++; 
            }

            if (pre_record.full()){
                update_stream(stream, record, pre_record);
            }

            update_metrics_by_record(current_metrics, record);

            process_last_record(current_metrics, stream, record, pre_record.price);

        }else{
                std::cout << "failed=========== " << record.time << std::endl;
                exit(0);
        }

        pre_record = record;
    }
    infile.close();
}

void process_last_record(DailyMetrics& metrics, StreamRecord& stream, TickRecord record, double pre_price){
    if (last_record(record)) {

        update_metrics_header(metrics.header, stream);
        set_metrics_record(metrics, record, RecordType::LAST);
        metrics.this_1457_pirce = pre_price;


        get_record_stream_point(metrics.end_point,  record, pre_price);

    }
}

void process_first_record(DailyMetrics& metrics, StreamRecord& stream, TickRecord record, double pre_closing_price){
    if (first_record(record)) {
        set_metrics_record(metrics, record, RecordType::FIRST);

        if (pre_closing_price == 0){
            pre_closing_price = record.price;
        }

        stream_new(stream, record, pre_closing_price);
        metrics.pre_closing_price = pre_closing_price;

        get_record_stream_point(metrics.start_point,  record, pre_closing_price);
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

bool process_single_file(const std::string& filename, DayOutputMetrics& out, DayOutputMetrics& pre_out) {

    std::vector<TickRecord> records;

    DailyMetrics metrics;
    DailyMetrics am_metrics;

    read_tick_records(filename, records);

    parse_tick_records(records, metrics, am_metrics, pre_out.metrics.closing_price);

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

        bsn_action_group dump;
        deal_summary deal_summary_total;
        get_slim_base(out.metrics, RecordScale::TOTAL, dump, deal_summary_total);


        out.historical_total_inflow = prev_out.historical_total_inflow + 
                                        deal_summary_total.bsn.buy.money - deal_summary_total.bsn.sale.money;
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

    bsn_action_group dump;
    deal_summary deal_summary_total_am;
    get_slim_base(out.am_metrics, RecordScale::TOTAL, dump, deal_summary_total_am);

    deal_summary deal_summary_total;
    get_slim_base(out.am_metrics, RecordScale::TOTAL, dump, deal_summary_total);


    if (out.metrics.am_bsn.buy.money != deal_summary_total_am.bsn.buy.money){
        print__headers("TEST", test_table_cols);
        print_next(out.date_str, i, cols);
        print_next(out.metrics.ticks_count, i, cols);

        print_next(out.metrics.am_bsn.buy.money/WAN, i, cols);
        print_next(deal_summary_total_am.bsn.buy.money/WAN, i, cols);

        print_next(out.metrics.am_bsn.sale.money/WAN, i, cols);
        print_next(deal_summary_total_am.bsn.buy.money/WAN, i, cols);

        print_next(out.metrics.pm_bsn.buy.money/WAN, i, cols);
        print_next((deal_summary_total.bsn.buy.money - deal_summary_total_am.bsn.buy.money)/WAN, i, cols);

        print_next(out.metrics.pm_bsn.sale.money/WAN, i, cols);
        print_next((deal_summary_total.bsn.buy.money - deal_summary_total_am.bsn.buy.money)/WAN, i, cols);
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
    while ((opt = getopt(argc, argv, "hd:parwsqBSmnTRbl:MtD:")) != -1) {
        switch (opt) {
            case 'h': opts.show_head = true; break;
            case 'd': opts.lvmeng_dir_path = optarg; break;
            case 'D': opts.data_dir_path = optarg; break;
            case 'r': opts.show_income_ratio = true; break;
            case 'a': opts.show_all = true; break;
            case 'w': opts.show_will = true; break;
            case 'p': opts.show_price = true; break;
            case 'R': opts.show_super_ratio = true; break;
            case 'B': opts.show_big_ratio = true; break;
            case 'm': opts.show_middle_ratio = true; break;
            case 'n': opts.show_small_ratio = true; break;
            case 's': opts.show_super = true; break;
            case 'b': opts.show_big = true; break;
            case 'q': opts.show_total_ratio = true; break;
            case 'M': {
                opts.show_middle = true; 
                break;
            }
            case 'S': {
                opts.show_small = true; 
                break;
            }
            case 'T': {
                opts.show_total = true; 
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
            std::cout << file << ":" << target_company_id << std::endl;
            continue;
        }

        DayOutputMetrics out;

        if (!process_single_file(file, out, prev_out)) {
             std::cout << file << ":" << __LINE__ << std::endl;
            continue;
        }

        if (out.metrics.ticks_count <= 0) {
            std::cout << file << ":" << __LINE__ << std::endl;
            continue;
        }

        out.middle_metrics = out.metrics;

        sub_record_stream_point(out.middle_metrics.header, out.middle_metrics.start_point);
        sub_record_stream_point(out.middle_metrics.header, out.middle_metrics.end_point);

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

template <typename F>
int generic_adjacent_check(const std::vector<DayOutputMetrics>& out_vector, F should_break) {
    if (out_vector.size() < 2){
        return 0;
    } 

    int j = 0;
    for (size_t i = 0; i + 1 < out_vector.size(); ++i) {
        if (should_break(out_vector[i].metrics, out_vector[i + 1].metrics)){
            break;    
        } 
        ++j;
    }
    return j;
}

template <typename F>
int generic_fixed_base_check(const std::vector<DayOutputMetrics>& out_vector, F should_break) {
    if (out_vector.size() < 2){
        return 0;
    } 

    int j = 0;
    for (size_t i = 1; i < out_vector.size(); ++i) {
        if (should_break(out_vector[0].metrics, out_vector[i].metrics)){
            break;
        } 
        ++j;
    }
    return j;
}


int metrics_shrink_firm(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& a, const auto& b) { 
        return metrics_total_volume(a) > metrics_total_volume(b); 
    };

    return generic_adjacent_check(out_vector, pred);
}


int metrics_grow_firm(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& a, const auto& b) { 
        return metrics_total_volume(a) < metrics_total_volume(b); 
    };
    
    return generic_adjacent_check(out_vector, pred);
}


int metrics_shrink_loose(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return metrics_total_volume(base) > metrics_total_volume(curr); 
    };

    return generic_fixed_base_check(out_vector, pred);
}

int metrics_down_check_price_pre_max(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return base.closing_price >= curr.closing_price; 
    };
    
    return generic_fixed_base_check(std::vector<DayOutputMetrics>(out_vector.begin() + 1, out_vector.end()), pred);
}

int metrics_up_check_price_pre_max(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return base.closing_price < curr.closing_price; 
    };
    
    return generic_fixed_base_check(std::vector<DayOutputMetrics>(out_vector.begin() + 1, out_vector.end()), pred);
}

int metrics_down_check_price_adjacent(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return base.closing_price >= curr.closing_price; 
    };
    
    return generic_adjacent_check(out_vector, pred);
}

int metrics_up_check_price_adjacent(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return base.closing_price < curr.closing_price; 
    };
    
    return generic_adjacent_check(out_vector, pred);
}

int metrics_price_check_pre_max(const std::vector<DayOutputMetrics>& out_vector) {

    int down_day = metrics_down_check_price_pre_max(out_vector);
    int up_day = metrics_up_check_price_pre_max(out_vector);

    if (down_day != 0) {
        return 0 - down_day;
    }

    if (up_day != 0){
        return up_day;
    }

    return 0;
}

int metrics_price_check_adjacent(const std::vector<DayOutputMetrics>& out_vector) {

    int down_day = metrics_down_check_price_adjacent(out_vector);
    int up_day = metrics_up_check_price_adjacent(out_vector);

    if (down_day != 0) {
        return 0 - down_day;
    }

    if (up_day != 0){
        return up_day;
    }

    return 0;
}

int metrics_grow_loose(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return metrics_total_volume(base) < metrics_total_volume(curr); 
    };

    return generic_fixed_base_check(out_vector, pred);
}


void check_sub_conditions(const std::string& file, const VectorStats& v_stats, std::vector<SubCondition>& sub_conditions){
    for (const auto& sc : sub_conditions) {
        if (sc.satisfied) {
            print_signal(file, v_stats, sc);
        }
    }
}


void get_signal_from_metrics(size_t size, const std::vector<std::string>& files_to_process, const std::vector<DayOutputMetrics>& out_vector) {
    if (size < 2 || size > files_to_process.size() || size > out_vector.size()) {
        return;
    }

    VectorStats v_stats;

    const auto& file = files_to_process[0]; 

    metry_vector_summary(out_vector, v_stats);

    TradeCategoryStats& a0 = v_stats.a0;
    TradeCategoryStats& a1 = v_stats.a1;

    bool all_netin = (a0.all_will_netin > 0 || a0.all_price_netin > 0);
    bool middle_netin = (a0.strip_will_netin > 0 || a0.strip_price_netin > 0);


    std::vector<SubCondition> sub_conditions = {
        {
            a0.all_will_netin > 0 && a0.all_price_netin > 0 && a0.pct_change_base_pre < 0.3 &&  v_stats.price_down_day_adjacent > 3,
            "will_up"
        },
        {
            all_netin && a0.pct_change_base_pre < 0.1,
            "abnormal_all"
        },
        {
            middle_netin && a0.pct_change_base_925 < 0.1,
            "abnormal_middle"
        },
        {
            a0.all_will_netin > 0 && a0.all_price_netin > 0  
            && a0.all_will_netin_pct > 0 && a0.all_price_netin_pct > 0 
            && v_stats.price_day_adjacent[0] >= -1 && v_stats.price_day_adjacent[0] <= 3,
            "SPEEDUP(" + pct_base_string(a0.buyup_pct) + "vs" + pct_base_string(a0.buyup_pct - a1.buyup_pct) + ")" 
        },
        {
            a0.pct_change_base_925 > 0 && middle_netin == false,
            "up_out_m" 
        },
        {
            a0.pct_change_base_pre > 0 && all_netin == false,
            "up_out_all" 
        }        
        // {
        //     all_netin && ((a0.buydown_pct == 0 &&  a0.saleup_pct > a1.saleup_pct && a0.pct_change_base_pre > 1.0)),
        //     "will_down"
        // },
        // {
        //     base_condition && shrink_firm >= 3,
        //     "shrink_firm3" 
        // },
        // {
        //     shrink_loose >= 10,
        //     "shrink_loose6" 
        // },
        // {
        //     super_buy_down == 0 && super_sale_up == 0 && this_day_stats.total.volume * 4 < prev_day_stats.total.volume * 3,
        //     "controlled" 
        // },
    };


    check_sub_conditions(file, v_stats, sub_conditions);
}

void show_metrics_by_opts(const ProgramOptions& opts, const std::vector<DayOutputMetrics>& out_vector) {
    // 移除 const，确保成员指针允许被用于赋值操作
    bool ProgramOptions::* const flags[] = {
        &ProgramOptions::show_head,
        &ProgramOptions::show_all,
        &ProgramOptions::show_will,
        &ProgramOptions::show_price,
        &ProgramOptions::show_super,
        &ProgramOptions::show_super_ratio,
        &ProgramOptions::show_big,
        &ProgramOptions::show_big_ratio,
        &ProgramOptions::show_middle,
        &ProgramOptions::show_middle_ratio,
        &ProgramOptions::show_small,
        &ProgramOptions::show_small_ratio,
        &ProgramOptions::show_total,
        &ProgramOptions::show_total_ratio,
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
        std::cout << "impossible: " << data_dir_path << ":" << files_to_process.size() << "-" << out_vector.size()  << std::endl; 
        return;
    }

    size_t size = std::min(show_limit, out_vector.size());

    
    std::reverse(out_vector.begin(), out_vector.end());
    std::reverse(files_to_process.begin(), files_to_process.end());

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