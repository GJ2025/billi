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

    if (out.pct_change > 0 && will_net_money < 0) {
        signals.push_back("[UP_OUT]");
    }
    
    if (out.pct_change > 0 && price_net_money < 0) {
        signals.push_back("[UP_POUT]");
    } 
    
    if (out.pct_change < 0 && will_net_money > 0) {
        signals.push_back("[DN_IN]");
    }

    if (out.pct_change < 0 && price_net_money > 0) {
        signals.push_back("[DN_PIN]");
    }

    // if ((avg_change - PRICE_THRESHOLD) > 0 && will_net_money < 0) {
    //     signals.push_back("[AVUP_OUT]");
    // }
    
    // if ((avg_change - PRICE_THRESHOLD) > 0 && price_net_money < 0) {
    //     signals.push_back("[AVUP_POUT]");
    // } 
    
    // if ((avg_change + PRICE_THRESHOLD) < 0 && will_net_money > 0) {
    //     signals.push_back("[AVDN_IN]");
    // }

    // if ((avg_change + PRICE_THRESHOLD) < 0 && price_net_money > 0) {
    //     signals.push_back("[AVDN_PIN]");
    // }

    // if (will_net_money * price_net_money < 0.0){
    //     signals.push_back("[WILL_P_DIFF]");
    // }


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

std::string get_and_print_signals(DayOutputMetrics& out, const DayOutputMetrics& prev_out) {

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

void update_metrics_by_record(DailyMetrics& metrics, TickRecord& record){
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

void parse_tick_file(std::ifstream& infile, DailyMetrics& metrics, DailyMetrics& am_metrics) {
    std::string line;
    TickRecord pre_record;
    StreamRecord stream;

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

            process_first_record(metrics, record, pre_record);

            if (record_change(record, pre_record)) {
                update_metrics_header(metrics.header, stream);
            }

            if (is_am_end(record.t, pre_record.t)){
                update_metrics_header(metrics.header, stream);
                am_metrics = metrics;
            }

            update_stream(stream, record, pre_record);
            update_metrics_by_record(metrics, record);

            process_last_record(metrics, stream, record);

        }else{
                std::cout << "failed=========== " << record.time << std::endl;
                exit(0);
        }

        pre_record = record;
    }
    infile.close();
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

            process_first_record(current_metrics, record, pre_record);

            if (record_change(record, pre_record)) {
                update_metrics_header(current_metrics.header, stream);
            }

            if (tick_idx < tick_times.size() && 
                is_this_time_end(record.t, pre_record.t, tick_times[tick_idx])) {
                update_metrics_header(current_metrics.header, stream);
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
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    DailyMetrics metrics;
    DailyMetrics am_metrics;

    parse_tick_file(infile, metrics, am_metrics);

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
            out.pct_change =pct(out.metrics.closing_price, prev_out.metrics.closing_price);
            out.am_pct_change = pct(out.am_metrics.closing_price, prev_out.metrics.closing_price);
            out.start_change = pct(out.metrics.daily_first_record.price, prev_out.metrics.closing_price);
            out.avg_pct_change = pct(out.metrics.avg_price, prev_out.metrics.avg_price);
        }

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
            opts.dir_path = argv[++i];
        }
    }
    return 0;
}

int parse_opt(int argc, char* argv[], ProgramOptions& opts){
    int opt;
    while ((opt = getopt(argc, argv, "hd:parwsmbl:Mt")) != -1) {
        switch (opt) {
            case 'h': opts.show_head = true; break;
            case 'd': opts.dir_path = optarg; break;
            case 'r': opts.show_income_ratio = true; break;
            case 'a': opts.show_all = true; break;
            case 'w': opts.show_will = true; break;
            case 'p': opts.show_price = true; break;
            case 'm': opts.show_merge = true; break;
            case 's': opts.show_super = true; break;
            case 'b': opts.show_big = true; break;
            case 'M': opts.show_middle = true; break;
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

int main(int argc, char* argv[]) {
    ProgramOptions opts;
    std::vector<std::string> files_to_process;
    std::vector<tickTime> tick_times_seq;
    

    if (parse_opt(argc, argv, opts) != 0){
        return 1;
    }

    
    int init_status = initialize_and_get_files(opts.dir_path, files_to_process, opts.show_limit);
    if (init_status > 0) return init_status;
    if (init_status < 0) return 0;


    // ./bin/parse_tick -t -N 5 -I 3 -H 16 -M 20 -d $d/xiye/
    if (opts.tseq.cnt != 0){
        std::vector<DailyMetrics> all_metrics;
        tick_times_seq = min_vector(opts.tseq);
        show_time_vector(tick_times_seq);
         std::ifstream infile(files_to_process[0]);

        parse_tick_file_by_tseq(infile, all_metrics, tick_times_seq);
        return 0;
    }


    print_headers(opts);




    DayOutputMetrics prev_out;
    std::string divergengce;
    std::string target_company_id = extract_company_id(files_to_process[0]);

    files_to_process.shrink_to_fit();

    for (const auto& file : files_to_process) {

        if (!check_company_id_match(file, target_company_id)) {
            continue;
        }

        DayOutputMetrics out;

        if (!process_single_file(file, out)) {
            continue;
        }

        process_out(out, prev_out);

        deal_classfy(out.metrics);
        deal_classfy(out.am_metrics);

        // make_test(out);

        divergengce = get_and_print_signals(out, prev_out);

        print_bodys(opts, out, prev_out, divergengce);

        if (out.metrics.ticks_count > 0) {
            prev_out = out;
        }
    }

    print_headers(opts);

    return 0;
}