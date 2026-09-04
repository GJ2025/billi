#include "opts.h"

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
                opts.show_limit = 2;
                break;
            }
            default:
                std::cerr << "Usage: " << argv[0] << " [-h] [-d path] [-p] [-a] [-r] [-w] [-s] [-m]" << std::endl;
                return 1;
        }
    }

    return 0;
}

