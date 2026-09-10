#ifndef PROCESS_FILES_H
#define PROCESS_FILES_H

#define FILE_BUFF "/home/guoj/data/ALL"

void run_preprocessing(const std::string& dir_path);
int files_list(const std::string& dir_path, size_t show_limit, std::vector<std::string>& files_to_process);

inline bool should_skip_directory(const std::string& dir_name) {
    return dir_name.find("show") != std::string::npos  
           || dir_name.find("tseq_show") != std::string::npos
           || dir_name.find("ALL") != std::string::npos;
}

extern std::string extract_stock_code(const std::string& filepath);
extern void dir_map_code(const std::string& data_dir_path, std::unordered_map<std::string, std::string>& code_path_map);
extern void process_files_IN_ALL(const std::vector<std::string>& files_to_process, const std::unordered_map<std::string, std::string>& code_2_path);

extern std::unordered_map<std::string, std::string> code_path_map;


#endif // PROCESS_FILES_H
