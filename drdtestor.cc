#include <stdio.h>
#include <algorithm>
#include <new>
#include <stdarg.h>
#include <errno.h>
#include <iostream>
#include <stdexcept>

#include "model.h"
#include "execution.h"
#include "action.h"
#include "schedule.h"
#include "common.h"
#include "cyclegraph.h"
#include "datarace.h"
#include "threads-model.h"
#include "bugmessage.h"
#include "fuzzer.h"
#include "drdtestor.h"
DRD_Testor::DRD_Testor()
{
}

/// @brief Initialize our data race detector from previous action trace
/// @param list Previous trace, we won't use the clock vector
DRD_Testor::DRD_Testor(actionlist *list) : clock_per_thread{}, ts{}, vs{}, ls{}
{
    sllnode<ModelAction *> *it;
    unsigned int hash = 0;
    int cur_seq = -1;
    for (it = list->begin(); it != NULL; it = it->getNext())
    {
        const ModelAction *act = it->getVal();
        this->action_trace.emplace_back(
            new ModelAction(act->get_type(), act->get_mo(), act->get_location(), act->get_value()));
        auto seq_num = static_cast<int>(act->get_seq_number());
        if (seq_num > cur_seq)
        {
            this->action_trace.back()->set_seq_number(act->get_seq_number());
            cur_seq = seq_num;
        }
        else
        {
            throw std::runtime_error("sequence number not getting larger");
        }
        hash = hash ^ (hash << 3) ^ (it->getVal()->hash());
    }
}

/// @brief Main entry of fast track algorithm
void DRD_Testor::test_data_race()
{
    int time = 0; // The global time
    for (int i = 0; i < MAIN_THREAD_ID; i++)
    {
        clock_per_thread.emplace_back(DRDClockVector());
    }
    int creator_id = MAIN_THREAD_ID - 1;
    for (const auto &act : action_trace)
    {
        time = act->get_seq_number();
        auto a_type = act->get_type();
        switch (a_type)
        {
        case THREAD_START:
            if (creator_id < 0)
                clock_per_thread.emplace_back(DRDClockVector());
            else
                clock_per_thread.emplace_back(DRDClockVector(&clock_per_thread[creator_id], act));
            break;
        case THREAD_CREATE:
            creator_id = act->get_tid();
            clock_per_thread[act->get_tid()].update(act->get_tid(), time);
            break;
        case ATOMIC_READ:
            break;
        default:
            break;
        }
    }
}

/// @brief Print data race if exists
void DRD_Testor::print_race()
{
    std::cout << "No race!" << std::endl;
}

/// @brief Create a new clock vector for current thread
/// @param parent Parent clock to create from
/// @param act Current thread action
DRDClockVector::DRDClockVector(DRDClockVector *parent, const ModelAction *act)
{
    num_threads = act != NULL ? act->get_tid() + 1 : 0;
    if (parent && parent->num_threads > num_threads)
        num_threads = parent->num_threads;
    if (parent)
        clock = std::vector<int>(parent->clock);
    else
        clock = std::vector<int>(num_threads, 0);
    if (clock.size() < num_threads)
        clock.push_back(0);
    if (act != NULL)
        clock[act->get_tid()] = act->get_seq_number();
}
/**
 * Merge a clock vector into this vector, using a pairwise comparison. The
 * resulting vector length will be the maximum length of the two being merged.
 * @param cv is the DRDClockVector being merged into this vector.
 */
bool DRDClockVector::merge(const DRDClockVector *cv)
{
    ASSERT(cv != NULL);
    bool changed = false;
    for (int i = num_threads; i < cv->num_threads; i++)
        clock.push_back(0);
    num_threads = clock.size();

    /* Element-wise maximum */
    for (int i = 0; i < cv->num_threads; i++)
        if (cv->clock[i] > clock[i])
        {
            clock[i] = cv->clock[i];
            changed = true;
        }

    return changed;
}

/// @brief Update vector clock time to global time
/// @param tid
/// @param time
void DRDClockVector::update(int tid, int time)
{
    clock[tid] = time;
}