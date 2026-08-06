#ifndef PROCESS_FILES_H
#define PROCESS_FILES_H

void run_preprocessing(const std::string& dir_path);
int initialize_and_get_files(const std::string& dir_path, size_t show_limit, std::vector<std::string>& files_to_process);


#endif // PROCESS_FILES_H
