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

#include <ppapi/c/ppb_var.h>

#include <ppapi/utility/completion_callback_factory.h>
#include <ppapi/utility/threading/simple_thread.h>

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

    const PPB_Var *getPpbVar() const { return _ppb_var; };

    private:
    void HandleSqliteRequest(int32_t, const pp::VarDictionary& request);
    void OpenFileSystem(int32_t);

    private:
    PPB_GetInterface _interface;
    const PPB_Var *_ppb_var;
    pp::CompletionCallbackFactory<SqliteBridgeInstance> _callback_factory;
    pp::SimpleThread _commands_thread;
};

#endif
