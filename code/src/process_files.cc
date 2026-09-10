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
#include <unordered_map>
#include <string> 
#include <fstream>
#include <sstream>
#include <iconv.h>
#include "common.h"
#include "tick_types.h"
#include "process_files.h"

namespace fs = std::filesystem;

void run_preprocessing(const std::string& dir_path) {
    
    const char* c_env = std::getenv("c");
    std::string script_dir = c_env ? c_env : "."; 
    std::string script_path = (fs::path(script_dir) / "c.sh").string();

    if (!fs::exists(script_path)) {
        std::cerr << "Warning: Pre-processing script not found at: " << script_path  
                  << "\nSkipping pre-processing stage..." << std::endl;
    } else {
        std::string shell_cmd = "sh " + script_path + " " + dir_path;
        // std::cout << "Executing: " << shell_cmd << std::endl;
        
        int ret = std::system(shell_cmd.c_str());
        if (ret != 0) {
            std::cerr << "Error: Pre-processing script exited with code " << ret << std::endl;
        } 
    }
}

bool convert_gbk_to_utf8(const std::string& filepath) {
    // 1. 以二进制只读方式一次性将整个文件读入内存
    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) return false;
    
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::string content;
    content.resize(size);
    if (!ifs.read(&content[0], size)) {
        return false;
    }
    ifs.close();

    if (content.empty()) return true;

    // 2. 简易轻量判断：如果是纯 ASCII 或已经含有合法的 UTF-8 特征，可快速通过
    // （若文件绝大多数已是 UTF-8，可省去 iconv 开销）
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(content.data());
    size_t len = content.size();
    size_t ascii_count = 0;
    for (size_t i = 0; i < len; ++i) {
        if (bytes[i] <= 0x7F) ascii_count++;
    }
    // 如果全都是 ASCII 字符，直接当成 UTF-8/文本处理，只需清洗 \r 即可（或直接跳过）
    if (ascii_count == len) {
        // 检查是否存在 \r 需要替换
        if (content.find('\r') == std::string::npos) {
            return true; // 既是纯 ASCII 又无 \r，直接跳过，零开销！
        }
    }

    // 3. 使用 iconv 进行转码
    iconv_t cd = iconv_open("UTF-8", "GBK");
    if (cd == (iconv_t)-1) return false;

    char* inbuf = const_cast<char*>(content.data());
    size_t inbytesleft = content.size();
    
    std::vector<char> outbuf(inbytesleft * 3 + 256);
    char* outptr = outbuf.data();
    size_t outbytesleft = outbuf.size();
    size_t original_outsize = outbytesleft;

    if (iconv(cd, &inbuf, &inbytesleft, &outptr, &outbytesleft) == (size_t)-1) {
        iconv_close(cd);
        return false;
    }
    iconv_close(cd);

    size_t converted_size = original_outsize - outbytesleft;

    // 4. 高效处理 \r 转 \n（在内存中构建结果，而不是逐个字节写磁盘）
    std::string final_content;
    final_content.reserve(converted_size);
    
    for (size_t i = 0; i < converted_size; ++i) {
        char c = outbuf[i];
        if (c == '\r') {
            final_content.push_back('\n');
            if (i + 1 < converted_size && outbuf[i + 1] == '\n') {
                i++; // 跳过紧跟的 \n，避免双换行
            }
        } else {
            final_content.push_back(c);
        }
    }

    // 5. 一次性将处理后的数据写回文件（块写入，速度极快）
    std::ofstream ofs(filepath, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) return false;
    ofs.write(final_content.data(), final_content.size());
    
    return true;
}

int files_list(const std::string& dir_path, size_t show_limit, std::vector<std::string>& files_to_process) {

    // run_preprocessing(dir_path);

    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        std::cerr << "Error: Invalid directory path: " << dir_path << std::endl;
        return 1;
    }

    files_to_process.clear();
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            convert_gbk_to_utf8(entry.path().string());
            files_to_process.push_back(entry.path().string());
        }
    }
    std::sort(files_to_process.begin(), files_to_process.end());

    if (files_to_process.empty()) {
        std::cout << "Info: No valid .txt data files found. IN " << dir_path <<std::endl;
        return -1; 
    }

    if (show_limit && files_to_process.size() > show_limit){
        files_to_process.erase(
            files_to_process.begin(), 
            files_to_process.end() - show_limit
        );
    
        files_to_process.shrink_to_fit();
    }

    return 0;
}

std::string extract_stock_code(const std::string& filepath) {
    size_t underscore_pos = filepath.rfind('_');
    if (underscore_pos == std::string::npos) {
        return "";
    }
    
    size_t dot_pos = filepath.find('.', underscore_pos);
    if (dot_pos == std::string::npos || dot_pos <= underscore_pos + 1) {
        return "";
    }
    
    return filepath.substr(underscore_pos + 1, dot_pos - underscore_pos - 1);
}


void dir_map_code(const std::string& data_dir_path, std::unordered_map<std::string, std::string>& code_path_map) {

    if (!fs::exists(data_dir_path) || !fs::is_directory(data_dir_path)) {
        std::cerr << "Invalid directory path: " << data_dir_path << std::endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(data_dir_path)) {
        if (entry.is_directory()) {
            std::string dir_name = entry.path().filename().string();

            if (should_skip_directory(dir_name)) {
                continue;
            }

            std::vector<std::string> files_to_process;

            files_list(entry.path().string(), 1, files_to_process);

            if (files_to_process.size() != 0){
                // std::cout << entry.path().string() << "            :             " << extract_stock_code(files_to_process[0]) << std::endl;
                code_path_map[extract_stock_code(files_to_process[0])] = entry.path().string(); 
            }
        }
    }

    // std::cout << "\n--- Code Path Map Contents ---" << std::endl;
    // for (const auto& [code, path] : code_path_map) {
    //     std::cout << "Code: " << code << " -> Path: " << path << std::endl;
    // }

}


void process_files_IN_ALL(const std::vector<std::string>& files_to_process, const std::unordered_map<std::string, std::string>& code_2_path) {
    for (const auto& file_path : files_to_process) {
        std::string code = extract_stock_code(file_path);
        if (code.empty()) continue;

        auto it = code_2_path.find(code);
        if (it != code_2_path.end()) {
            std::cout << "Match found - Code: " << code << " -> File: " << file_path << "," << it->second << std::endl;

            // 构造目标文件路径（目标目录 + 原文件名）
            fs::path src_path(file_path);
            fs::path dest_path = fs::path(it->second) / src_path.filename();

            std::error_code ec;

            // 尝试直接移动（如果目标存在，某些系统下 rename 可能报错，先安全移除目标文件）
            if (fs::exists(dest_path, ec)) {
                fs::remove(dest_path, ec);
            }

            // 执行移动操作 (rename)
            fs::rename(src_path, dest_path, ec);
            if (ec) {
                // 如果跨分区/挂载点导致 rename 失败，则使用 复制覆盖 + 删除原文件 的方式
                fs::copy_file(src_path, dest_path, fs::copy_options::overwrite_existing, ec);
                if (!ec) {
                    fs::remove(src_path, ec);
                } else {
                    std::cerr << "Error moving file: " << ec.message() << std::endl;
                }
            }
        }
    }
}