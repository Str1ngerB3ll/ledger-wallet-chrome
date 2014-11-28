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

#include <nacl_io/ioctl.h>
#include <nacl_io/nacl_io.h>
#include <sys/mount.h>

#include "sqlite_commands.h"

class SqliteBridgeInstance : public pp::Instance
{
    public:
    SqliteBridgeInstance(PP_Instance instance, PPB_GetInterface interface);
    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
    virtual void HandleMessage(const pp::Var& var_message);
    void PostResponse(const pp::VarDictionary& request, pp::VarDictionary& response);

    private:
    void HandleSqliteRequest(const pp::VarDictionary& request);

    private:
    PPB_GetInterface _interface;
};

#endif
