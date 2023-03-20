#pragma once
#include "execution.h"
#include "action.h"
#include <vector>

class DRD_Testor
{
private:
    std::vector<ModelAction> action_trace;
    
public:
    DRD_Testor();
    DRD_Testor(action_list_t* action_trace);
    // ~DRD_Testor();

    void test_data_race();

    void print_race();

    
};
