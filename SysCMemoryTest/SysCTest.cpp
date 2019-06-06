#include "SysCTest.h"
#include <iostream>
#include <vector>

typedef SysCTest SC_CURRENT_USER_MODULE;

SysCTest::SysCTest(sc_module_name name)
    : sc_module(name),
    mProcStart(NULL)
{
    Init();
    SC_THREAD(RunThread);
}

void SysCTest::RunThread()
{
    for (uint32_t c = 0; c < childProcesses; ++c)
    {
        mProcStart[c].notify(SC_ZERO_TIME);
        sc_core::wait(sc_time(1, SC_PS));
    }
}

/* http://forums.accellera.org/topic/5850-how-do-i-terminate-all-threads-at-end-of-simulation/ */

void SysCTest::KillAllRunningChildProcesses(sc_process_handle& h)
{
    const ::std::vector<sc_object*>& children = h.get_child_objects();
    for (unsigned i(0); i < children.size(); ++i) {
        // convert objects to process handles, if valid() and !terminated() call kill(SC_INCLUDE_DESCENDANTS) 
        sc_process_handle h = sc_process_handle(children.at(i));
        if (h.valid()) {
            if (!h.terminated()) {
                cout << "Killing thread: " << h.name() << endl;
                h.kill(SC_INCLUDE_DESCENDANTS);
            }
        }
    }
}

SysCTest::~SysCTest()
{
    delete[] mProcStart;

    for (uint32_t t = 0; t < childProcesses; ++t)
    {
        KillAllRunningChildProcesses(mThreadHandles[t]);
    }
}

void SysCTest::Init()
{
    mProcStart = new sc_event[childProcesses];
    mThreadHandles.resize(childProcesses);

    for (uint32_t t = 0; t < childProcesses; ++t)
    {
        sc_spawn_options opt;
        opt.dont_initialize();
        opt.set_sensitivity(&mProcStart[t]);

        ostringstream oss;
        oss << "Counter" << t;
        mThreadHandles[t] = sc_spawn(sc_bind(&SysCTest::Counter, this, t), oss.str().c_str(), &opt);
        sc_set_stack_size(mThreadHandles[t], 16);
    }
}
void SysCTest::Counter(uint32_t start)
{
    uint32_t stack_dummy[256];
    std::vector<uint32_t> buffer(1 << 16);
    
    while (true) {
        std::cout << sc_time_stamp() << " Counter(" << start << ") running" << std::endl;

        for (uint32_t c = 0; c < start; ++c)
        {
            buffer[c]++;
            stack_dummy[c % 256]++;
            sc_core::wait(sc_time(1, SC_PS));
        }

        std::cout << sc_time_stamp() << " Counter(" << start << ") stopped" << std::endl;
        sc_core::wait(mProcStart[start]);
    }
}