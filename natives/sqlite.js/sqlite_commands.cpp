/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#include "sqlite_commands.h"

typedef void (*CommandFunction)(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request);

static unsigned int LastReference = 0;

static std::map<unsigned int, sqlite3 *> OpenedDatabase;

static void HandleSqliteOpen(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Open DB"));
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var(request.Get(pp::Var("databaseName")).AsString().c_str()));

    sqlite3 *db;
    pp::VarDictionary response;

    const char *filename = request.Get(pp::Var("databaseName")).AsString().c_str();
    int rc = sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, "unix-none");
    if (rc)
    {
       bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("FAILURE"));
       response.Set(pp::Var("error"), pp::Var(sqlite3_errmsg(db)));
       response.Set(pp::Var("errno"), pp::Var(strerror(errno)));
    }
    else
    {
        int reference = LastReference++;
        OpenedDatabase[reference] = db;
        bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var((int32_t)db));
        bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("SUCCESS"));
        response.Set(pp::Var("db"), pp::Var(reference));
    }

    bridgeInstance->PostResponse(request, response);
}

static void HandleSqliteClose(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Close DB"));

    sqlite3 *db = OpenedDatabase[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;

    int rc = sqlite3_close_v2(db);
    if (rc == SQLITE_OK)
    {
        bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("SUCCESS"));
        response.Set(pp::Var("success"), pp::Var(true));
    }
    else
    {
        bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var((int32_t)db));
        bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("FAILURE"));
        response.Set(pp::Var("success"), pp::Var(false));
        response.Set(pp::Var("error"), pp::Var(sqlite3_errmsg(db)));
    }

    bridgeInstance->PostResponse(request, response);
}

CommandFunction COMMAND_FUNCTIONS[] = {
    HandleSqliteOpen,
    HandleSqliteClose
};

bool HandleSqliteCommand(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
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