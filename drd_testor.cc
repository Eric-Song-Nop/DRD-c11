#include "drd_testor.h"
DRD_Testor::DRD_Testor()
{
}

/// @brief Initialize our data race detector from previous action trace
/// @param action_trace Previous trace, we won't use the clock vector
DRD_Testor::DRD_Testor(actionlist* action_trace) : action_trace{}
{
    
}

/// @brief Main entry of fast track algorithm
void test_data_race()
{

}

/// @brief Print data race if exists
void DRD_Testor::print_race()
{
}