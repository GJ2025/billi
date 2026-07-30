#ifndef PROCESS_FILES_H
#define PROCESS_FILES_H

void run_preprocessing(const std::string& dir_path);
int initialize_and_get_files(std::string& dir_path, std::vector<std::string>& files_to_process, size_t show_limit);


#endif // PROCESS_FILES_H
