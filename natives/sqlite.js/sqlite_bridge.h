/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

class SqliteBridgeInstance : public pp::Instance
{
    public:
    SqliteBridgeInstance(PP_Instance instance);
    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
    virtual void HandleMessage(const pp::Var& var_message);

    void PostMessagef(const char* format, ...);
};
