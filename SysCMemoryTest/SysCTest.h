#pragma once
#include "common.h"

const uint32_t childProcesses = 2000;

class SysCTest : public sc_module
{
public:
    SysCTest(sc_module_name name);
    ~SysCTest();
private:
    void RunThread();
    void Counter(uint32_t start);
    void Init();
    sc_event* mProcStart;
};

