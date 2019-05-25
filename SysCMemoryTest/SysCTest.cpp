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

SysCTest::~SysCTest()
{
    delete[] mProcStart;
}

void SysCTest::Init()
{
    mProcStart = new sc_event[childProcesses];
    
    for (uint32_t t = 0; t < childProcesses; ++t)
    {
        sc_spawn_options opt;
        opt.dont_initialize();
        opt.set_sensitivity(&mProcStart[t]);

        ostringstream oss;
        oss << "Counter" << t;
        sc_spawn(sc_bind(&SysCTest::Counter, this, t), oss.str().c_str(), &opt);
    }
}
void SysCTest::Counter(uint32_t start)
{
    std::vector<uint32_t> buffer(1 << 16);

    while (true) {
        std::cout << sc_time_stamp() << " Counter(" << start << ") running" << std::endl;

        for (uint32_t c = 0; c < start; ++c)
        {
            buffer[c]++;
            sc_core::wait(sc_time(1, SC_PS));
        }

        std::cout << sc_time_stamp() << " Counter(" << start << ") stopped" << std::endl;
        sc_core::wait(mProcStart[start]);
    }
}