#pragma once
#include "common.h"
#include <vector>

const uint32_t childProcesses = 2000;

class SysCTest : public sc_module
{
public:
    SysCTest(sc_module_name name);
    ~SysCTest();
private:
    void KillAllRunningChildProcesses(sc_process_handle& h);
    void RunThread();
    void Counter(uint32_t start);
    void Init();
    sc_event* mProcStart;
    std::vector<sc_process_handle> mThreadHandles;
};

