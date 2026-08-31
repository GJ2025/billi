#ifndef TABLES_H
#define TABLES_H

struct Col {
    std::string name;
    int width;
    bool visible = true;
};

inline const std::vector<Col> will_price_table_cols = {
    {"Date", 11}, 
    {"Buy-Dn", 12, false}, 
    {"Buy-Kp", 12, false},  
    {"Buy-Up", 12},
    {"Sale-Dn", 12}, 
    {"Sale-Kp", 12, false}, 
    {"Sale-Up", 12, false}, 
    {"Neutral-Dn", 12, false},
    {"Neutral-Kp", 12, false},  
    {"Neutral-Up", 12, false},
    {"Buy", 12}, 
    {"Sale", 12},
    {"Neutral", 12},    
    {"Up", 12}, 
    {"Dn", 12},
    {"Kp", 12}, 
    {"WILL-Net", 10}, 
    {"PRICE-Net", 12},     
    {"Money", 12},  
    {"Volume", 5},
    {"Pre", 5},     
    {"StartCh", 9}, 
    {"Pct_925", 9},
    {"Pct_Pre", 9},   
    {"Close", 5}
};

inline const std::vector<Col> will_price_ratio_table_cols = {
    {"Date", 11}, 
    {"Buy-Dn", 12, false}, 
    {"Buy-Kp", 12, false},  
    {"Buy-Up", 12},
    {"Sale-Dn", 12}, 
    {"Sale-Kp", 12, false}, 
    {"Sale-Up", 12, false}, 
    {"Neutral-Dn", 12, false},
    {"Neutral-Kp", 12, false},  
    {"Neutral-Up", 12, false},
    {"Buy", 12}, 
    {"Sale", 12},
    {"Neutral", 12},    
    {"Up", 12}, 
    {"Dn", 12},
    {"Kp", 12}, 
    {"WILL-Net", 10}, 
    {"PRICE-Net", 12},     
    {"Money", 12},  
    {"Volume", 5},
    {"Pre", 5},     
    {"StartCh", 9}, 
    {"Pct_925", 9},
    {"Pct_Pre", 9},   
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
    {"WNetIn", 12, true}, 
    {"PNetIn", 12, true},
    {"WNET-P", 9},
    {"PNET-P", 9},
    {"Strip-WNetIn", 12, true}, 
    {"Strip-PNetIn", 12, true}, 
    {"shrink_grow", 12},
    {"pday", 4},
    {"Pct0", 5},
    {"Pct1", 5},
    {"REASON", 12}
};

static const std::vector<Col> will_table_cols = {
    {"Date", 11}, 
    {"Super-Buy", 12, false}, 
    {"Super-Sale", 10, false}, 
    
    {"Big-Buy", 12, false},  
    {"Big-Sale", 12, false},   
   
    {"Mid-Buy", 12, false},    
    {"Mid-Sale", 12, false},   
    


    {"Small-Buy", 9, false},  
    {"Small-Sale", 10, false}, 
    
    {"Super-NET", 12},
    {"Big-NET", 12},    
    {"Mid-NET", 12},
    {"Small-NET", 12},
    {"Tot-NET", 12},

    {"Tot-Buy", 12},    
    {"Tot-Sale", 12},
    {"Tot-Neutral", 12}, 

    {"Money", 12},     
    {"Volume", 12},
    {"Pre", 5},        
    {"StartCh", 9},    
    {"Pct_925", 9}, 
    {"Close", 5}
};

static const std::vector<Col> price_table_cols = {
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
    {"Pre", 5},      
    {"StartCh", 9},  
    {"Pct_925", 9}, 
    {"Close", 5}
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

// 专为 print_merge 准备的配置
static const std::vector<Col> merge_table_cols = {
    {"Date", 11},
    {"TotBuy(BSN)", 12}, 
    {"TotSale(BSN)", 12}, 
    {"TotNeu(BSN)", 12},
    {"TotUp(PRC)", 12},  
    {"TotDn(PRC)", 12},   
    {"TotKp(PRC)", 12},
    {"BSN-Net", 16},     
    {"PRC-Net", 16},
    {"Money", 12},       
    {"Volume", 12},
    {"Pre", 5},          
    {"StartCh", 9},       
    {"PctCh_925", 9}, 
    {"Close", 5}
};

static const std::vector<Col> data_all_table_cols = {
    {"Date", 11}, 
    {"Ticks", 5}, 
    {"AM-volume(W)", 12, false},
    {"AM-Money(W)", 11, false}, 
    {"AM-Money%", 11}, 
    {"Volume/Tick", 11}, 

    {"AM-NET", 11, false}, 
    {"PM-NET", 11, false},
    {"AM-P-NET", 11, false}, 
    {"PM-P-NET", 11, false}, 

    {"WNET", 8},
    {"PNET", 8},

    {"WillP", 8},
    {"PRICEP", 8},
    {"Distribute_M", 24, false},
    {"Distribute_V", 24},
    {"Money", 11},
    {"Volume", 9}, 
    
    {"NET/Money", 9, false},

    {"AvgPrice", 9, true},
    {"1st", 8}, 
    {"StartCh%", 8}, 
    {"AvgPct%", 8, false},
    {"AM-Close", 8, false}, 
    {"AM-Pct%", 8, false},
    {"BaseAvg%", 8, false},  
    {"Pct_925", 9},
    {"Pct_pre", 9}, 
    {"Close", 7},

    {"Divergence", 20}
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
