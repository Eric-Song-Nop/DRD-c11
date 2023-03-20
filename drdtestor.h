#ifndef __DRDTESTOR_H__
#define __DRDTESTOR_H__
#include <cstddef>
#include <inttypes.h>

#include "mymemory.h"
#include "hashtable.h"
#include "config.h"
#include "modeltypes.h"
#include "stl-model.h"
#include "params.h"
#include "mypthread.h"
#include "mutex.h"
#include <condition_variable>
#include "classlist.h"
#include "action.h"

#include <unordered_map>

#define INITIAL_THREAD_ID	0
#define MAIN_THREAD_ID		1
#include <vector>

class DRDAction{
public:
    unsigned int seq_number = 0;
    unsigned int t_id = 0;
    action_type a_type;
    memory_order order;
    uint64_t loc;

    DRDAction(const ModelAction* action);
};

class DRDClockVector
{
public:
    DRDClockVector();
    DRDClockVector(const DRDAction &act);
    DRDClockVector(const DRDClockVector &parent, const DRDAction &act);
    bool merge(const DRDClockVector *cv);
    void update(int tid, int time);
    
private:
    std::vector<int> clock; // number of threads vectors
    int num_threads; // How many threads are stored
};

struct ThreadState
{
    int tid;
    DRDClockVector clock;
    int epoch;
};

struct VarState
{
    int w_epoch;
    int r_epoch;
    DRDClockVector r_clock;
};

struct LockState
{
    DRDClockVector clock;
};
class DRD_Testor
{
private:
    std::vector<DRDAction> action_trace;
    std::vector<ThreadState> clock_per_thread; // each one is for each thread
    std::unordered_map<int, ThreadState> ts;
    std::unordered_map<uint64_t, VarState> vs;
    std::unordered_map<cdsc::mutex *, LockState> ls;

public:
    DRD_Testor();
    DRD_Testor(actionlist* list);
    // ~DRD_Testor();

    void test_data_race();

    void print_race();
};
#endif