#include "opts.h"


//  ./bin/parse_tick -t -N 90   -I 5 -d  $d/chuanheng -a > $s/a.txt
int parse_opt(int argc, char* argv[], ProgramOptions& opts){
    int opt;
    while ((opt = getopt(argc, argv, "AhparwsqBSmnTRbMtl:D:d:N:I:F:")) != -1) {
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
            case 'A': opts.show_sz = true; break;
            case 'M': opts.show_middle = true; break;
            case 'S': opts.show_small = true;  break;
            case 'T': opts.show_total = true; break;
            case 'l': opts.show_limit = std::stoi(optarg); break;
            case 'N': opts.tseq.cnt = std::stoi(optarg); break;
            case 'I': opts.tseq.intervel = std::stoi(optarg); break;
            case 'H': opts.tseq.start_hour = std::stoi(optarg); break;
            case 'F': opts.tseq.start_min = std::stoi(optarg); break;
            case 't': {
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

