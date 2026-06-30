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

namespace fs = std::filesystem;

struct TerminalColor {
    std::string start;
    std::string end;
};

// 原始定义的整个文件统计/TickData结构体（保留作扩展或外部兼容）
struct TickData {
    std::string time;       
    double price;           
    int volume;             
    int type_count;         
    std::string bs_type;    
};

// ================== 单行 Tick 记录结构体 ==================
struct TickRecord {
    std::string time = "";
    double price = 0.0;
    int volume = 0;
    int type_count = 0;
    std::string bs_type = "-";

    // 重载流输入运算符，一键解析整行（包括时间戳）
    friend std::istream& operator>>(std::istream& is, TickRecord& record) {
        if (is >> record.time >> record.price >> record.volume >> record.type_count) {
            // 如果行尾没有 B/S 属性，则默认为 "-"
            if (!(is >> record.bs_type)) {
                record.bs_type = "-";
            }
        }
        return is;
    }
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

    // ====== 新增整合字段 ======
    std::string date_str = "";
    double historical_total_inflow = 0.0;
};
// ====================================================================

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
        std::cout << "\033[33m[Skip File]\033[0m Mismatched Company (" 
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

int initialize_and_get_files(int argc, char* argv[], std::string& dir_path, std::vector<std::string>& files_to_process) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <Directory_Path>" << std::endl;
        return 1;
    }

    dir_path = argv[1];
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

void print_header() {
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

void print_data_row(const DayOutputMetrics& out,  
                    const std::string& divergence_str, const std::string& row_color_start, const std::string& row_color_end) {
      
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
              
    std::cout << row_color_start 
              << std::setw(7)  << format_inflow(out.closing_price)  << " | "
              << std::setw(8)  << format_percent_value(out.pct_change)    << " | "
              << std::setw(10) << format_inflow(out.net_inflow_wan) << " | "
              << std::setw(10) << format_inflow(out.am_net_inflow_wan) << " | "
              << std::setw(10) << format_inflow(out.pm_net_inflow_wan) << " | " 
              << std::setw(9) << format_percent_value(out.inflow_ratio)  << " | "
              << std::setw(9) << format_percent_value(out.net_per_change)  << " | "
              << std::setw(11) << format_inflow(out.historical_total_inflow)   << row_color_end << " | ";
    
    std::cout << std::left << std::setw(20) << divergence_str << std::endl;
}

void get_price_color(const DayOutputMetrics& out, const DayOutputMetrics& prev_out,TerminalColor& color) {
    if (out.closing_price > prev_out.closing_price) {
        color.start = "\033[31m"; // 红色
        color.end = "\033[0m";
    } else if (out.closing_price < prev_out.closing_price) {
        color.start = "\033[32m"; // 绿色
        color.end = "\033[0m";
    }
    
    return ;
}

std::string get_divergence_string(const DayOutputMetrics& out, const DayOutputMetrics& prev_out) {
    std::vector<std::string> signals;

    // 1. 价格与流入背离
    if (out.pct_change > 0 && out.net_inflow_wan < 0) {
        signals.push_back("\033[33m[UP/NET_OUT]\033[0m");
    } else if (out.pct_change < 0 && out.net_inflow_wan > 0) {
        signals.push_back("\033[35m[DN/NET_IN ]\033[0m");
    }

    // 2. 均价与流入背离
    if (prev_out.avg_price > 0.0) {
        if (out.avg_price > prev_out.avg_price && out.net_inflow_wan < 0) {
            signals.push_back("\033[1;31m[AV_UP/OUT]\033[0m");
        } else if (out.avg_price < prev_out.avg_price && out.net_inflow_wan > 0) {
            signals.push_back("\033[1;32m[AV_DN/IN ]\033[0m");
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

void get_and_print_signals(DayOutputMetrics& out, const DayOutputMetrics& prev_out) {
    bool has_prev = (prev_out.ticks_count > 0 && prev_out.closing_price > 0.0);
    
    std::string row_color_start = "";
    std::string row_color_end = "";
    TerminalColor colour = {"", ""}; 

    if (has_prev) {
        out.pct_change = ((out.closing_price - prev_out.closing_price) / prev_out.closing_price) * 100.0;
        get_price_color(out, prev_out, colour);
    }

    std::string divergence_str = get_divergence_string(out, prev_out); 

    if (std::abs(out.pct_change) <= 1.0) {
        out.net_per_change = 0.0;
    }else{
        out.net_per_change = out.inflow_ratio/out.pct_change;
    }

    print_data_row(out, divergence_str, colour.start, colour.end);
}

void parse_tick_file(std::ifstream& infile, DailyMetrics& metrics) {
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        TickRecord record;
        
        if (ss >> record) {
            if (!is_loading_data(record.time)) continue;
            if (record.type_count == 0) continue; 

            metrics.valid_records_count++;
            metrics.closing_price = record.price; 

            long long current_vol = static_cast<long long>(record.volume) * 100; 
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
        }
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

    return true;
}

int main(int argc, char* argv[]) {
    std::string dir_path;
    std::vector<std::string> files_to_process;

    int init_status = initialize_and_get_files(argc, argv, dir_path, files_to_process);
    if (init_status > 0)  return init_status; 
    if (init_status < 0)  return 0;           

    print_header();

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
        
        get_and_print_signals(out, prev_out);
        
        if (out.ticks_count > 0) {
            prev_out = out;
        }
    }

    print_header();
    return 0;
}
