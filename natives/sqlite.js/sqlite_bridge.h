/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#ifndef _SQLITE_BRIDGE_H_
#define _SQLITE_BRIDGE_H_

#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/var.h>
#include <ppapi/cpp/var_dictionary.h>
#include <ppapi/cpp/var_array.h>
#include "sqlite_commands.h"

class SqliteBridgeInstance : public pp::Instance
{
    public:
    SqliteBridgeInstance(PP_Instance instance);
    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
    virtual void HandleMessage(const pp::Var& var_message);
    void PostResponse(const pp::VarDictionary& request, pp::VarDictionary& response);

    private:
    void HandleSqliteRequest(const pp::VarDictionary& request);
};

#endif
