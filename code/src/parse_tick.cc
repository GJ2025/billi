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

namespace fs = std::filesystem;

struct TerminalColor {
    std::string start;
    std::string end;
};


// ================== 单行 Tick 记录结构体 ==================
struct TickRecord {
    std::string time = "";
    double price = 0.0;
    long long volume = 0;
    int deal_count = 0;
    std::string bs_type = "-";

    // 重载流输入运算符，一键解析整行（包括时间戳）
    friend std::istream& operator>>(std::istream& is, TickRecord& record) {
        if (is >> record.time >> record.price >> record.volume >> record.deal_count) {
            // 如果行尾没有 B/S 属性，则默认为 "-"
            if (!(is >> record.bs_type)) {
                record.bs_type = "-";
            }
        }

        is.clear();
        return is;
    }
};

// ================== 单行 Tick 记录结构体 ==================

struct buy_action {
   double up = 0.0;
   double down = 0.0;
   double keep = 0.0;
};


struct stream_sum {
   buy_action sale;
   buy_action buy;
   buy_action middle;
};

// ================== 单行 Tick 记录结构体 ==================
struct StreamRecord {
    struct TickRecord record;
    double gap = 0.0;
};

struct HeadTickData {
    TickRecord v_924;
    TickRecord pre_924;
    TickRecord v_925;

    double ratio_change_924 = 0.0;
    double ratio_change_pre_924 = 0.0;
};


// ================== 单日解析基础统计指标的结构体（局部内部使用） ==================
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

    stream_sum sum_info;
};


// ================== 单日衍生/计算输出指标的结构体 ==================
struct DayOutputMetrics {
    long long ticks_count = 0;         
    double closing_price = 0.0;        
    double am_closing_price = 0.0;     
    double am_net_inflow_wan = 0.0;
    double pm_net_inflow_wan = 0.0;
    double net_inflow_wan = 0.0;
    double total_turnover_wan = 0.0;
    double am_turnover_wan = 0.0;     
    double am_turnover_ratio = 0.0;   // 新增：上午成交额占比 (%)
    double total_vol_wan = 0.0;
    double am_vol_wan = 0.0;
    double avg_price = 0.0;
    double inflow_ratio = 0.0;
    double avg_vol_per_tick = 0.0;
    double net_per_change = 0.0;
    double pct_change = 0.0;
    double am_pct_change = 0.0;

    // ====== 新增整合字段 ======
    std::string date_str = "";
    double historical_total_inflow = 0.0;

    HeadTickData head_data;
    stream_sum sum_info;
};
// ====================================================================

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
    // 打印分割线，让视觉效果更专业
    std::cout << std::string(88, '-') << std::endl;
}

void print_header_info(const DayOutputMetrics& out, const DayOutputMetrics& pre_out) {
    double price_change_pct = 0.0;
    double ratio_change_pre_day = 0.0;
    if (pre_out.closing_price != 0.0) {
        price_change_pct = ((out.closing_price - pre_out.closing_price) / pre_out.closing_price) * 100.0;
        ratio_change_pre_day = ((out.head_data.v_925.price - pre_out.closing_price) / pre_out.closing_price) * 100.0;

    }

   

    std::cout << std::left  << std::setw(12) << out.date_str << " | "
              << std::right << std::fixed << std::setprecision(2)
              << std::setw(12) << out.head_data.pre_924.price << " | "
              << std::setw(10) << out.head_data.v_924.price   << " | "
              << std::setw(10) << out.head_data.v_925.price   << " | "
              << std::setw(11) << out.head_data.ratio_change_pre_924 << "% | "
              << std::setw(11) << ratio_change_pre_day << "% | "
              << std::setw(6) << out.head_data.v_925.bs_type << " | "
              << std::setw(11) << out.head_data.ratio_change_924 << "% | "
              << std::setw(11) << price_change_pct << "%"
              << std::endl;
}

bool is_loading_data(const std::string& str) {
    if (str.empty()) return false;
    return std::isdigit(static_cast<unsigned char>(str[0]));
}

bool is_am_time(const std::string& tick_time) {
    // 兼容可能带有秒或不带秒的格式，标准解析小时和分钟
    int hour = 0, minute = 0;
    char delimiter;
    std::stringstream ss(tick_time);
    
    if (ss >> hour >> delimiter >> minute) {
        // 如果采用的是 12 小时制且数据中未标明 PM，但已知下午 1 点到 3 点交易
        // 或者是标准的 24 小时制 09:15 - 11:30
        if (hour >= 1 && hour <= 3) { 
            // 假设 1, 2, 3 代表下午的 13, 14, 15 点
            return false; 
        }
        
        int total_minutes = hour * 60 + minute;
        // 上午收盘时间 11:30 = 11 * 60 + 30 = 690 分钟
        return total_minutes <= 690;
    }
    return false;
}

void collect_buy_action(buy_action& sale, double trade, double gap){
    if (gap < 0.0){
        sale.down += trade;
    }else if (gap == 0.0){
        sale.keep += trade;
    }else{
        sale.up += trade;
    }

    return;
}


void get_stream_sum(struct stream_sum& sum, StreamRecord stream){

    double trade = stream.record.volume * stream.record.price * 100;

    if (stream.record.bs_type == "S"){
        collect_buy_action(sum.sale, trade, stream.gap);
    }else if(stream.record.bs_type == "B"){
        collect_buy_action(sum.buy, trade, stream.gap);
    }else{
        collect_buy_action(sum.middle, trade, stream.gap);
    }
    return;
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

void run_preprocessing(const std::string& dir_path) {
    std::cout << "======================= Pre-processing =======================" << std::endl;
    
    const char* c_env = std::getenv("c");
    std::string script_dir = c_env ? c_env : "."; 
    std::string script_path = (fs::path(script_dir) / "c.sh").string();

    if (!fs::exists(script_path)) {
        std::cerr << "Warning: Pre-processing script not found at: " << script_path  
                  << "\nSkipping pre-processing stage..." << std::endl;
    } else {
        std::string shell_cmd = "sh " + script_path + " " + dir_path;
        std::cout << "Executing: " << shell_cmd << std::endl;
        
        int ret = std::system(shell_cmd.c_str());
        if (ret != 0) {
            std::cerr << "Error: Pre-processing script exited with code " << ret << std::endl;
        } else {
            std::cout << "Pre-processing completed successfully.\n" << std::endl;
        }
    }
}

int initialize_and_get_files(std::string& dir_path, std::vector<std::string>& files_to_process) {

    run_preprocessing(dir_path);

    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        std::cerr << "Error: Invalid directory path: " << dir_path << std::endl;
        return 1;
    }

    files_to_process.clear();
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            files_to_process.push_back(entry.path().string());
        }
    }
    std::sort(files_to_process.begin(), files_to_process.end());

    if (files_to_process.empty()) {
        std::cout << "Info: No valid .txt data files found." << std::endl;
        return -1; 
    }

    return 0;
}

void print_all() {
    // 总长度再次扩展 14 字符以兼容新的占比列
    std::cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    std::cout << "----------------" << std::endl; 
    std::cout << std::left  << std::setw(11) << "Date" << " | "
              << std::right << std::setw(5)  << "Ticks" << " | "
              << std::setw(9)  << "Vol" << " | "               
              << std::setw(9)  << "AM_Vol" << " | "
              << std::setw(11) << "AM_Turnover" << " | "  
              << std::setw(11) << "AM_Turn%" << " | " // 新增表头：上午成交额占比
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

std::string format_inflow(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) 
       << value;
    return ss.str();
}

std::string format_percent_value(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2)
        << value
        << "%";
    return ss.str();
}

void print_data_row(const DayOutputMetrics& out,  const std::string& divergence_str) {
      
    std::cout << std::left << std::setw(11) << out.date_str << " | "
              << std::right << std::setw(5) << out.ticks_count << " | "
              << std::fixed << std::setprecision(2)
              << std::setw(9)  << out.total_vol_wan << " | " 
              << std::setw(9)  << out.am_vol_wan << " | "
              << std::setw(11) << out.am_turnover_wan << " | " 
              << std::setw(11) << format_percent_value(out.am_turnover_ratio) << " | " // 打印新字段：上午成交额占比
              << std::setw(10) << std::fixed << std::setprecision(1) << out.avg_vol_per_tick << " | " 
              << std::fixed << std::setprecision(2)
              << std::setw(11) << out.total_turnover_wan << " | "
              << std::setw(8)  << format_inflow(out.am_closing_price) << " | "
              << std::setw(9)  << format_inflow(out.avg_price) << " | "; 
              
    std::cout << std::setw(7)  << format_inflow(out.closing_price)  << " | "
              << std::setw(8)  << format_percent_value(out.am_pct_change)    << " | "
              << std::setw(8)  << format_percent_value(out.pct_change)    << " | "
              << std::setw(10) << format_inflow(out.net_inflow_wan) << " | "
              << std::setw(10) << format_inflow(out.am_net_inflow_wan) << " | "
              << std::setw(10) << format_inflow(out.pm_net_inflow_wan) << " | " 
              << std::setw(9) << format_percent_value(out.inflow_ratio)  << " | "
              << std::setw(9) << format_percent_value(out.net_per_change)  << " | "
              << std::setw(11) << format_inflow(out.historical_total_inflow)   <<  " | ";
    
    std::cout << std::left << std::setw(20) << divergence_str << std::endl;
}

void print_income_header() {
    std::cout << std::left 
              << std::setw(11) << "Date"        << " | "
              << std::setw(9)  << "Buy-Dn"      << " | "
              << std::setw(9)  << "Buy-Kp"      << " | "
              << std::setw(9)  << "Buy-Up"      << " | "
              << std::setw(9)  << "Sale-Dn"     << " | "
              << std::setw(9)  << "Sale-Kp"     << " | "
              << std::setw(9)  << "Sale-Up"     << " | "
              << std::setw(9)  << "Mid-Dn"      << " | "
              << std::setw(9)  << "Mid-Kp"      << " | "
              << std::setw(9)  << "Mid-Up"      << " | "
              << std::setw(9)  << "Total"       << " | " 
               << std::setw(9)  << "Bs   "       << " | "
              << std::setw(9)  << "Keep "       << " | "
              << std::setw(9)  << "Chang"       << " | "
              << std::endl;
    std::cout << std::string(135, '-') << std::endl; // 分割线
}


void print_income(const DayOutputMetrics& out, bool ratio) {

    double all = out.sum_info.sale.down 
                    + out.sum_info.sale.keep
                    + out.sum_info.sale.up

                    + out.sum_info.buy.down
                    + out.sum_info.buy.keep
                    + out.sum_info.buy.up

                    + out.sum_info.middle.down
                    + out.sum_info.middle.keep
                    + out.sum_info.middle.up;

    double keep =   out.sum_info.buy.keep - out.sum_info.sale.keep;
    double bs =  out.sum_info.buy.up - out.sum_info.sale.down;
    
    if (ratio){
            std::cout << std::left << std::setw(11) << out.date_str << " | "
              << std::fixed << std::setprecision(3)
              << std::setw(9)  << out.sum_info.buy.down/all << " | "
              << std::setw(9)  << out.sum_info.buy.keep/all << " | " 
              << std::setw(9)  << out.sum_info.buy.up/all << " | " 
              << std::setw(9)  << out.sum_info.sale.down/all << " | "
              << std::setw(9)  << out.sum_info.sale.keep/all << " | " 
              << std::setw(9)  << out.sum_info.sale.up/all << " | " 
              << std::setw(9)  << out.sum_info.middle.down/all << " | "
              << std::setw(9)  << out.sum_info.middle.keep/all << " | " 
              << std::setw(9)  << out.sum_info.middle.up/all << " | " 
              << std::setw(9)  << all/all << " | " 
              << std::setw(9)  << bs/all << " | "
              << std::setw(9)  << keep/all << " | "
               << std::setw(9)  << out.pct_change << " | "  
              << std::endl;
    }else{

            std::cout << std::left << std::setw(11) << out.date_str << " | "
              << std::fixed << std::setprecision(2)
              << std::setw(9)  << out.sum_info.buy.down/10000 << " | "
              << std::setw(9)  << out.sum_info.buy.keep/10000 << " | " 
              << std::setw(9)  << out.sum_info.buy.up/10000 << " | " 
              << std::setw(9)  << out.sum_info.sale.down/10000 << " | "
              << std::setw(9)  << out.sum_info.sale.keep/10000 << " | " 
              << std::setw(9)  << out.sum_info.sale.up/10000 << " | " 
              << std::setw(9)  << out.sum_info.middle.down/10000 << " | "
              << std::setw(9)  << out.sum_info.middle.keep/10000 << " | " 
              << std::setw(9)  << out.sum_info.middle.up/10000 << " | " 
              << std::setw(9)  << all/10000 << " | " 
              << std::setw(9)  << bs/10000 << " | "
              << std::setw(9)  << keep/10000 << " | "
               << std::setw(9)  << out.pct_change << " | "  
              << std::endl;
    }
}


std::string get_divergence_string(const DayOutputMetrics& out, const DayOutputMetrics& prev_out) {
    std::vector<std::string> signals;

    // 1. 价格与流入背离
    if (out.pct_change > 0 && out.net_inflow_wan < 0) {
        signals.push_back("[UP_OUT]");
    } else if (out.pct_change < 0 && out.net_inflow_wan > 0) {
        signals.push_back("[DN_IN]");
    }

    // 2. 均价与流入背离
    if (prev_out.avg_price > 0.0) {
        if (out.avg_price > prev_out.avg_price && out.net_inflow_wan < 0) {
            signals.push_back("[AVUP_OUT]");
        } else if (out.avg_price < prev_out.avg_price && out.net_inflow_wan > 0) {
            signals.push_back("[AVDN_IN]");
        }
    }

    // 3. 结果合并
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

bool out_is_full(const DayOutputMetrics& out){
    bool had_one = (out.ticks_count > 0 && out.closing_price > 0.0);
    return had_one;
}

void get_and_print_signals(DayOutputMetrics& out, const DayOutputMetrics& prev_out) {

    std::string divergence_str = get_divergence_string(out, prev_out); 

    print_data_row(out, divergence_str);
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
        if (base <= 0.0) return 0.0; // 防止除以零及负价格异常
        return ((target - base) / base) * 100.0;
    };

    if (!head_data.v_924.time.empty()) {
        head_data.ratio_change_924 = calc_pct(head_data.v_925.price, head_data.v_924.price);
    }
    
    if (!head_data.pre_924.time.empty()) {
        // 【已修正】这里赋值给了 last_2_924_change
        head_data.ratio_change_pre_924 = calc_pct(head_data.v_925.price, head_data.pre_924.price);
    }
}

void process_head_data(DailyMetrics& metrics, const TickRecord& record) {
    // 1. 捕获关键节点数据
    if (metrics.head_data.v_925.time.empty()) {
        if (record.time.find("09:24") == 0 || record.time.find("09:25") == 0) {
            update_head_tick_data(metrics.head_data, record);
        }
    }
    
    // 2. 触发计算（当 v_925 已获取且尚未计算过时）
    if (!metrics.head_data.v_925.time.empty() && !metrics.head_calculated) {
        calculate_head_tick_changes(metrics.head_data);
        metrics.head_calculated = true; // 锁定，防止后续循环重复计算
    }
}

bool record_change(TickRecord this_record,TickRecord pre_record){

    if (pre_record.price == this_record.price 
        && (pre_record.bs_type == this_record.bs_type || this_record.bs_type == "-")){
        return false;
    }else{
        return true;
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


bool record_empty(TickRecord this_record){

    if (this_record.deal_count == 0){
        return true;
    }else{
        return false;
    }
}

bool stream_empty(StreamRecord stream){
    return record_empty(stream.record);
}

void stream_new(StreamRecord& stream, TickRecord record, double pre_price){
    stream.record = record;
    stream.gap = record.price - pre_price;
}

void parse_tick_file(std::ifstream& infile, DailyMetrics& metrics) {
    std::string line;
    TickRecord pre_record;
    StreamRecord stream;

    std::getline(infile, line); 
    std::getline(infile, line);
    while (std::getline(infile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        TickRecord record;
        
        if (ss >> record) {
            if (!is_loading_data(record.time)) continue;
            process_head_data(metrics, record);
            if (record.deal_count == 0) continue; 


            if (first_record(record)){
                pre_record = record;
                stream_new(stream, record, record.price);
            }

            if (record_change(record, pre_record)){
                get_stream_sum(metrics.sum_info, stream);
                stream_new(stream, record, pre_record.price);
            }else{
                if (!first_record(record)){
                    stream.record.volume += record.volume;    
                }
            }
            
            if (last_record(record)){
                get_stream_sum(metrics.sum_info, stream);
            }
            
            metrics.valid_records_count++;
            metrics.closing_price = record.price; 

            long long current_vol = record.volume * 100; 
            double current_amount = record.price * current_vol;
            metrics.total_turnover += current_amount; 

            if (is_am_time(record.time)) {
                metrics.am_closing_price = record.price; 
                metrics.am_vol += current_vol;
                metrics.am_turnover += current_amount; 
                if (record.bs_type == "B") metrics.am_inflow += current_amount;
                else if (record.bs_type == "S") metrics.am_outflow += current_amount;
            } else {
                metrics.pm_vol += current_vol;
                if (record.bs_type == "B") metrics.pm_inflow += current_amount;
                else if (record.bs_type == "S") metrics.pm_outflow += current_amount;
            }
        }else{
                std::cout << "failed=========== " << record.time << std::endl;
                exit(0);

        }

        pre_record = record;
    }
    infile.close();
}

bool process_single_file(const std::string& filename, DayOutputMetrics& out) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    DailyMetrics metrics; 
    parse_tick_file(infile, metrics);

    if (metrics.valid_records_count == 0) {
        return false;
    }

    out.ticks_count = metrics.valid_records_count;
    out.closing_price = metrics.closing_price;
    out.am_closing_price = metrics.am_closing_price;

    out.am_net_inflow_wan = (metrics.am_inflow - metrics.am_outflow) / 10000.0;
    out.pm_net_inflow_wan = (metrics.pm_inflow - metrics.pm_outflow) / 10000.0; 
    out.net_inflow_wan = out.am_net_inflow_wan + out.pm_net_inflow_wan;  
    
    out.total_turnover_wan = metrics.total_turnover / 10000.0;
    out.am_turnover_wan = metrics.am_turnover / 10000.0; 
    
    // 计算上午成交额在全天成交额中的占比
    if (metrics.total_turnover > 0.0) {
        out.am_turnover_ratio = (metrics.am_turnover / metrics.total_turnover) * 100.0;
    } else {
        out.am_turnover_ratio = 0.0;
    }

    long long total_vol = metrics.am_vol + metrics.pm_vol;
    out.total_vol_wan = total_vol / 10000.0;  
    out.am_vol_wan = metrics.am_vol / 10000.0;

    if (out.total_vol_wan > 0.0) {
        out.avg_price = out.total_turnover_wan / out.total_vol_wan;
    }

    if (out.total_turnover_wan > 0.0) {
        out.inflow_ratio = (out.net_inflow_wan / out.total_turnover_wan) * 100.0;
    }

    out.avg_vol_per_tick = (out.total_vol_wan * 10000.0) / metrics.valid_records_count; 

    
    std::string pure_name = fs::path(filename).filename().string();
    out.date_str = (pure_name.length() >= 10) ? pure_name.substr(0, 10) : pure_name;

    out.head_data = metrics.head_data;
    out.sum_info = metrics.sum_info;

    return true;
}


int main(int argc, char* argv[]) {

    int opt;
    bool show_head = false;
    bool show_income = false;
    bool show_all = false;
    bool show_income_ratio = false;
    std::string dir_path;
    std::vector<std::string> files_to_process;

    // "h" 表示支持 -h 选项
    // "p:" 表示 -p 后必须带一个值 (比如 -p /data)
    while ((opt = getopt(argc, argv, "hp:iar")) != -1) {
        switch (opt) {
            case 'h':
                show_head = true;
                break;
            case 'p':
                dir_path = optarg; // optarg 会自动指向 -p 后面的参数值
                break;
            case 'i':
                show_income = true;
                break;
            case 'r':
                show_income_ratio = true;
            break;
            case 'a':
                show_all = true;
                break;           
            default:
                std::cerr << "Usage: " << argv[0] << " [-h] [-p path]" << std::endl;
                return 1;
        }
    }

    int init_status = initialize_and_get_files(dir_path, files_to_process);
    if (init_status > 0)  return init_status; 
    if (init_status < 0)  return 0;           

    if (show_head){
        print_table_header();
    }

    if (show_income){
        print_income_header();
    }
    
    if (show_all){
        print_all();
    }

    DayOutputMetrics prev_out; 
    std::string target_company_id = extract_company_id(files_to_process[0]);

    for (const auto& file : files_to_process) {
        if (!check_company_id_match(file, target_company_id)) {
            continue;
        }

        DayOutputMetrics out;
        if (!process_single_file(file,  out)) {
            continue; 
        }

        out.historical_total_inflow = prev_out.historical_total_inflow + out.net_inflow_wan;
        if (out_is_full(prev_out)) {
            out.pct_change = ((out.closing_price - prev_out.closing_price) / prev_out.closing_price) * 100.0;
            out.am_pct_change = ((out.am_closing_price - prev_out.closing_price) / prev_out.closing_price) * 100.0;
        }

        if (std::abs(out.pct_change) <= 1.0) {
            out.net_per_change = 0.0;
        }else{
            out.net_per_change = out.inflow_ratio/out.pct_change;
        }

        if (show_head){
            print_header_info(out, prev_out);
        }
        
        if (show_all){
            get_and_print_signals(out, prev_out);
        }
        
        if (show_income){
            print_income(out,show_income_ratio);
        }


        if (out.ticks_count > 0) {
            prev_out = out;
        }
    }

    if (show_head){
        print_table_header();
    }

    if (show_income){
        print_income_header();
    }
    
    if (show_all){
        print_all();
    }
    return 0;
}
