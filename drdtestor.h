#ifndef __DRDTESTOR_H__
#define __DRDTESTOR_H__
#include "classlist.h"
// #include "action.h"
#include <vector>

class DRD_Testor
{
private:
    actionlist action_trace;
    
public:
    DRD_Testor();
    DRD_Testor(actionlist* action_trace);
    // ~DRD_Testor();

    void test_data_race();

    void print_race();

    
};
#endif