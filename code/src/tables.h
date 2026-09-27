#ifndef TABLES_H
#define TABLES_H

#include "opts.h"

struct Col {
    std::string name;
    int width;
    int precision = 0;  
    bool show_pos = false;
    bool visible = true;
};


std::vector<tickTime> generate_sz_tick_times(size_t cnt , int interval_minutes);

inline void init_tick_columns(std::vector<Col>& cols) {
    cols.clear();
    cols.push_back({"Date", 11});

    std::vector<tickTime> tick_times = generate_sz_tick_times(opts.tseq.cnt, opts.tseq.intervel);

    for (size_t j = 0; j < tick_times.size(); ++j) {
        cols.push_back({format_tick_time(tick_times[j]), 8});
    }
}


inline const std::vector<Col> will_price_table_cols = {
    {"Date", 11}, 
    {"Buy-Dn", 12, 0, false, false}, 
    {"Buy-Kp", 12, 0, false, false},  
    {"Buy-Up", 12, 0, false, false},
    {"Sale-Dn", 12, 0, false, false}, 
    {"Sale-Kp", 12, 0, false, false}, 
    {"Sale-Up", 12, 0, false, false}, 
    {"Neutral-Dn", 12, 0, false, false},
    {"Neutral-Kp", 12, 0, false, false},  
    {"Neutral-Up", 12, 0, false, false},
    {"Buy", 12, 0}, 
    {"Sale", 12, 0},
    {"Neutral", 12, 0},    
    {"Up", 12, 0}, 
    {"Dn", 12, 0},
    {"Kp", 12, 0}, 
    {"WILL-Net", 10, 0, true}, 
    {"PRICE-Net", 12, 0, true},     
    {"Money", 12, 0},  
    {"Volume", 8, 0},
    {"Pre", 8, 0},     
    {"StartCh", 9, 2, true}, 
    {"Pct_925", 9, 2, true},
    {"Pct_Pre", 9, 2, true},   
    {"Close", 5}
};

inline const std::vector<Col> will_price_ratio_table_cols = {
    {"Date", 11}, 
    {"Buy-Dn", 12, 2, false, false}, 
    {"Buy-Kp", 12,  2, false, false}, 
    {"Buy-Up", 12,  2, false, false}, 
    {"Sale-Dn", 12,  2, false, false}, 
    {"Sale-Kp", 12,  2, false, false}, 
    {"Sale-Up", 12,  2, false, false},  
    {"Neutral-Dn", 12,  2, false, false}, 
    {"Neutral-Kp", 12,  2, false, false},   
    {"Neutral-Up", 12,  2, false, false}, 
    {"Buy", 12 , 2, false}, 
    {"Sale", 12, 2, false},
    {"Neutral", 12, 2, false},    
    {"Up", 12, 2, false}, 
    {"Dn", 12, 2, false},
    {"Kp", 12, 2, false}, 
    {"WILL-Net", 10, 2, true}, 
    {"PRICE-Net", 12, 2, true},     
    {"Money", 12, 0, false},  
    {"Volume", 8, 0, false},
    {"Pre", 8, 2, true},     
    {"StartCh", 9, 2, true}, 
    {"Pct_925", 9, 2, true},
    {"Pct_Pre", 9, 2, true},   
    {"Close", 5}
};

inline const std::vector<Col> quiet_buying_table_cols = {
    {"Date", 11}, 
    {"Buy-Dn", 12, true}, 
    {"Buy-Kp", 12, true},  
    {"Buy-Up", 12},
    {"Sale-Dn", 12,true},
    {"Sale-Dn-t", 12,false}, 
    {"Sale-Kp", 12, true}, 
    {"Sale-Up", 12,true}, 
    {"Neutral-Dn", 12, false},
    {"Neutral-Kp", 12, false},  
    {"Neutral-Up", 12, false},
    {"Keep", 7},
    {"Neutral", 7},
    {"NeuUp", 7},
    {"KeepBuy", 12},
    {"Pre", 5},     
    {"StartCh", 9}, 
    {"Pct_925", 9},
    {"Pct_Pre", 9},
    {"Total_m", 9, false},
    {"Total_v", 9}, 
    {"WILL-Net", 10}, 
    {"PRICE-Net", 12},    
    {"Close", 5}
};

inline const std::vector<Col> signal_table_cols = {
    {"File", 40,true},
    {"WNetIn", 9, true}, 
    {"PNetIn", 9, true},
    {"WNET-P", 6},
    {"PNET-P", 6},
    {"Strip-W", 9, true}, 
    {"Strip-P", 9, true}, 
    {"pvolume", 12},
    {"pday", 12},
    {"Pct0", 5},
    {"Pct1", 5},
    {"int", 3},
    {"REASON", 12}
};

static const std::vector<Col> will_table_cols = {
    {"Date", 11}, 
    {"Super-Buy", 12, 0, false, false}, 
    {"Super-Sale", 10, 0, false, false}, 
    
    {"Big-Buy", 12, 0, false, false},  
    {"Big-Sale", 12, 0, false, false},   
   
    {"Mid-Buy", 12, 0, false, false},    
    {"Mid-Sale", 12, 0, false, false},   
    
    {"Small-Buy", 9, 0, false, false},  
    {"Small-Sale", 10, 0, false, false}, 
    
    {"Super-NET", 9, 0, true},
    {"Big-NET", 9, 0, true},    
    {"Mid-NET", 9, 0, true},
    {"Small-NET", 9, 0, true},
    {"Tot-NET", 9, 0, true},

    {"Tot-Buy", 12, 0, true},    
    {"Tot-Sale", 12, 0, true},
    {"Tot-Neutral", 12, 0, true}, 

    {"Money", 12, 0, false},     
    {"Volume", 7, 0, false},
    {"Pre", 5, 2, false},        
    {"StartCh", 5, 2, true},    
    {"Pct_925", 5, 2, true}, 
    {"Close", 5, 2, true}
};

static const std::vector<Col> price_table_cols = {
    {"Date", 11},

    {"Super-Up", 9, 0, false, false}, 
    {"Super-Dn", 9, 0, false, false}, 
    

    {"Big-Up", 9, 0, false, false},  
    {"Big-Dn", 9, 0, false, false},   
    
    {"Mid-Up", 9, 0, false, false},  
    {"Mid-Dn", 9, 0, false, false}, 
    

    {"Small-Up", 9, 0, false, false}, 
    {"Small-Dn", 9, 0, false, false}, 
    

    {"Super-NET", 9, 0, true},
    {"Big-NET", 9, 0, true},
    {"Mid-NET", 9, 0, true},
    {"Small-NET", 9, 0, true},
    {"ToNET", 9, 0, true},

    {"Tot-Up", 12, 0, false},  
    {"Tot-Dn", 12, 0, false},
    {"Tot-KEEP", 12, 0, false},

    {"K/AL", 4, 2, false},

    {"Money", 12, 0, false},   
    {"Volume", 7, 0, false},
    {"Pre", 5, 2, false},      
    {"StartCh", 5, 2, true},  
    {"Pct_925", 5, 2, true}, 
    {"Close", 5, 2}
};


static const std::vector<Col> tseq_price_table_cols = {
    {"Date", 11},

    {"Super-Up", 9, false}, 
    {"Super-Dn", 9, false}, 
    

    {"Big-Up", 9, false},  
    {"Big-Dn", 9, false},   
    
    {"Mid-Up", 9, false},  
    {"Mid-Dn", 9, false}, 
    

    {"Small-Up", 9, false}, 
    {"Small-Dn", 9, false}, 
    

    {"Super-NET", 12},
    {"Big-NET", 9},
    {"Mid-NET", 9},
    {"Small-NET", 9},
    {"Tot-NET", 12},

    {"Tot-Up", 12},  
    {"Tot-Dn", 12},
    {"Tot-KEEP", 12},

    {"KEEP/ALL", 8},

    {"Money", 12},   
    {"Volume", 12},
    {"Close", 5}
};

static const std::vector<Col> data_all_table_cols = {
    // 成员顺序: { name, width, precision, show_pos, visible }
    {"Date", 11, 0, false, true},            
    {"Ticks", 5, 0, false, true},            
    {"AM-volume(W)", 12, 0, false, false},   
    {"AM-Money(W)", 11, 0, false, false},    
    
    {"AM-M-P", 6, 1, false, true},           
    {"Vol/Tick", 8, 0, false, true},         

    {"AM-NET", 11, 0, true, false},          
    {"PM-NET", 11, 0, true, false},          
    {"AM-P-NET", 11, 0, true, false},        
    {"PM-P-NET", 11, 0, true, false},        

    {"WNET", 8, 0, true, true},              
    {"PNET", 8, 0, true, true},              

    {"WillP", 8, 2, true, true},             
    {"PRICEP", 8, 2, true, true},            

    {"Strip-W", 8, 0, true, true},           
    {"Strip-P", 8, 0, true, true},           

    {"Distribute_M", 24, 0, false, false},   
    {"Distribute_V", 24, 0, false, true},    
    {"Money", 11, 0, false, true},           
    {"Volume", 9, 0, false, true},           

    {"NET/Money", 9, 1, true, false},        

    {"AvgP", 7, 2, false, true},             
    {"1st", 10, 2, true, true},              
    {"Star%", 5, 2, true, true},             
    {"Avg%", 5, 2, true, false},             
    {"AM-C", 5, 0, false, false},            
    {"AM-P%", 5, 2, true, false},            
    {"BaAvg%", 5, 2, true, false},           
    {"P925", 5, 2, true, true},              
    {"Ppre", 5, 2, true, true},              
    {"Close", 5, 2, false, true},            

    {"Divergence", 20, 0, false, true}       
};

static const std::vector<Col> tseq_data_all_table_cols = {
    {"Date", 11}, 
    {"Ticks", 5, false}, 
    {"AM-volume(W)", 12, false},
    {"AM-Money(W)", 11, false}, 
    {"AM-Money%", 11, false}, 
    {"V/Tick", 6}, 

    {"AM-NET", 11, false}, 
    {"PM-NET", 11, false},
    {"AM-P-NET", 11, false}, 
    {"PM-P-NET", 11, false}, 

    {"WNET", 8},
    {"PNET", 8},

    {"WillP", 8},
    {"PRICEP", 8},

    {"Strip-W", 8, false},
    {"Strip-P", 8, false},

    {"Distribute_M", 24, false},
    {"Distribute_V", 24},
    {"Money", 11},
    {"Volume", 9}, 
    
    {"NET/Money", 9, false},

    {"AvgPrice", 9, true},
    {"1st", 8}, 
    {"StartCh%", 8,  false}, 
    {"AvgPct%", 8, false},
    {"AM-Close", 8, false}, 
    {"AM-Pct%", 8, false},
    {"BaseAvg%", 8, false},  
    {"Pct_925", 9, false},
    {"Pct_pre", 9}, 
    {"Close", 7},

    {"Divergence", 20, false}
};


static const std::vector<Col> test_table_cols = {
    {"Date", 11}, 
    {"Ticks", 5, false}, 
    {"AM-inflow", 13},
    {"AM-Buy", 13},
    {"AM-outflow", 13}, 
    {"AM-Sale", 13}, 
    {"PM-inflow", 13},
    {"PM-Buy", 13},
    {"PM-outflow", 13}, 
    {"PM-Sale", 13},
};


#endif // TABLES_H
