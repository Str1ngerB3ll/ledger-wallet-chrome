/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#include "sqlite_commands.h"

typedef void (*CommandFunction)(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request);

static std::vector<sqlite3 *> OpenedDatabases;

static unsigned char gethex(const char *s, char **endptr)
{
  char hexDigit[] = {s[0], s[1], '\0'};
  *endptr += 2;
  return strtoul(hexDigit, NULL, 16);
}

static unsigned char *convert(const char *s, unsigned int *length) {
  unsigned char *answer = (unsigned char *)malloc((strlen(s) + 1) / 3);
  unsigned char *p;
  for (p = answer; *s; p++)
    *p = gethex(s, (char **)&s);
  *length = p - answer;
  return answer;
}

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
        int reference = -1;

        for (int index = 0; index < OpenedDatabases.size(); index++)
        {
            if (OpenedDatabases[index] == NULL)
            {
                reference = index;
                break;
            }
        }

        if (reference != -1)
        {
            OpenedDatabases[reference] = db;
        }
        else
        {
            reference = OpenedDatabases.size();
            OpenedDatabases.push_back(db);
        }

        bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var((int32_t)db));
        bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("SUCCESS"));
        response.Set(pp::Var("db"), pp::Var(reference));
    }

    bridgeInstance->PostResponse(request, response);
}

static void HandleSqliteClose(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Close DB"));

    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;

    int rc = sqlite3_close_v2(db);
    if (rc == SQLITE_OK)
    {
        OpenedDatabases[request.Get(pp::Var("db")).AsInt()] = NULL;
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

static void HandleSqliteKey(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Key"));

    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    unsigned int keyLength;
    const unsigned char *key = convert(request.Get(pp::Var("key")).AsString().c_str(), &keyLength);
    sqlite3_key(db, key, keyLength);
    pp::VarDictionary response;
    free((void *)key);
    response.Set(pp::Var("success"), pp::Var(true));
    bridgeInstance->PostResponse(request, response);
    sqlite3_exec(db, "CREATE TABLE Test(iteration INTEGER, test TEXT);", NULL, 0, NULL);
}

static pp::VarDictionary *CurrentExecResponse;

int SqliteExecCallback(void *p_data, int num_fields, char **p_fields, char **p_col_names)
{

  int i;

  int* nof_records = (int*) p_data;
  (*nof_records)++;

    for (i=0; i < num_fields; i++) {
      printf("%20s", p_col_names[i]);
    }

    printf("\n");
    for (i=0; i< num_fields*20; i++) {
      printf("=");
    }
    printf("\n");

  for(i=0; i < num_fields; i++) {
    if (p_fields[i]) {
      printf("%20s", p_fields[i]);
    }
    else {
      printf("%20s", " ");
    }
  }

  printf("\n");
  return 0;
}

static void HandleSqliteExec(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Exec"));
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, request.Get(pp::Var("statement")));


    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;
    CurrentExecResponse = &response;
    const char *statement = request.Get(pp::Var("statement")).AsString().data();

    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var(statement));

    int nrecs;
    char *errmsg;
    const int rc = sqlite3_exec(db, statement, SqliteExecCallback, &nrecs, &errmsg);

    if (rc != SQLITE_OK)
    {
        response.Set(pp::Var("success"), pp::Var(false));
        response.Set(pp::Var("error"), pp::Var(errmsg));
    }
    else
    {
        response.Set(pp::Var("success"), pp::Var(true));
        response.Set(pp::Var("count"), pp::Var(nrecs));
    }
    bridgeInstance->PostResponse(request, response);
}

static std::vector<sqlite3_stmt *> Statements;

static void HandleSqlitePrepare(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Close DB"));

    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;

    bridgeInstance->PostResponse(request, response);
}

static void HandleSqliteBind(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Close DB"));

    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;

    bridgeInstance->PostResponse(request, response);
}

static void HandleSqliteStep(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    bridgeInstance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Close DB"));

    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;

    bridgeInstance->PostResponse(request, response);
}

CommandFunction COMMAND_FUNCTIONS[] = {
    HandleSqliteOpen,
    HandleSqliteClose,
    HandleSqliteKey,
    HandleSqliteExec,
    HandleSqlitePrepare,
    HandleSqliteBind,
    HandleSqliteStep
};

bool HandleSqliteCommand(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    if (request.Get(pp::Var("command")).is_number())
    {
        int command = request.Get(pp::Var("command")).AsInt();
        if (command < sizeof(COMMAND_FUNCTIONS) / sizeof(CommandFunction))
        {
            COMMAND_FUNCTIONS[command](bridgeInstance, request);
            return true;
        }
    }
    return false;
}