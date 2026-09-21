#include <string.h>
#include "opts.h"

bool ProgramOptions::* const flags[] = {
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


//  ./bin/parse_tick -t -N 90 -I 5 -d  $d/chuanheng -a > $s/a.txt
int parse_opt(int argc, char* argv[], ProgramOptions& opts){
    int opt;
    while ((opt = getopt(argc, argv, "aAd:D:wpl:tN:I:H:M:r:R:P:")) != -1) {
        switch (opt) {
            case 'a': opts.show_all = true; break;
            case 'A': opts.show_sz = true; break;

            case 'd': opts.lvmeng_dir_path = optarg; break;
            case 'D': opts.data_dir_path = optarg; break;
            case 'P': opts.pre_day = std::stoi(optarg); break;

            case 'w': opts.show_will = true; break;
            case 'p': opts.show_price = true; break;

            case 'l': opts.show_limit = std::stoi(optarg); break;

            case 'N': opts.tseq.cnt = std::stoi(optarg); break;
            case 'I': opts.tseq.intervel = std::stoi(optarg); break;
            case 'H': opts.tseq.start_hour = std::stoi(optarg); break;
            case 'M': opts.tseq.start_min = std::stoi(optarg); break;
            case 't': {
                opts.show_t = true;
                opts.show_limit = 2;
                break;
            }
            case 'r': {
                if (strchr(optarg, 's') != NULL) opts.show_super = true;
                if (strchr(optarg, 'b') != NULL) opts.show_big = true;
                if (strchr(optarg, 'm') != NULL) opts.show_middle = true;
                if (strchr(optarg, 'S') != NULL) opts.show_small = true;
                if (strchr(optarg, 't') != NULL) opts.show_total = true;

                break;
            }
            case 'R': {
                if (strchr(optarg, 's') != NULL) opts.show_super_ratio = true;
                if (strchr(optarg, 'b') != NULL) opts.show_big_ratio = true;
                if (strchr(optarg, 'm') != NULL) opts.show_middle_ratio = true;
                if (strchr(optarg, 'S') != NULL) opts.show_small_ratio = true;
                if (strchr(optarg, 't') != NULL) opts.show_total_ratio = true;

                break;
            }
 
            default:
                std::cerr << "Usage: " << argv[0] << " find in opts.cc" << std::endl;
                return 1;
        }
    }

    return 0;
}

