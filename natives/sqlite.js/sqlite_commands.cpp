/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#include "sqlite_commands.h"

typedef void (*CommandFunction)(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request);

static void HandleSqliteOpen(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Open DB"));
}

CommandFunction COMMAND_FUNCTIONS[] = {

};

bool HandleSqliteCommand(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, request);
     bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, request.Get(pp::Var("command")));
    if (request.Get(pp::Var("command")).is_number())
    {
        int command = request.Get(pp::Var("command")).AsInt();
        if (command < sizeof(COMMAND_FUNCTIONS))
        {
            COMMAND_FUNCTIONS[command](bridgeInstance, request);
            return true;
        }
    }
    return false;
}