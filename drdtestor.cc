#include "drdtestor.h"
#include <iostream>
DRD_Testor::DRD_Testor() : action_trace{}
{
}

/// @brief Initialize our data race detector from previous action trace
/// @param action_trace Previous trace, we won't use the clock vector
DRD_Testor::DRD_Testor(actionlist* action_trace) : action_trace{}
{
    
}

/// @brief Main entry of fast track algorithm
void DRD_Testor::test_data_race()
{

}

/// @brief Print data race if exists
void DRD_Testor::print_race()
{
    std::cout << "No race!" << std::endl;
}