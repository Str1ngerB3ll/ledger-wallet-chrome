/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#include "sqlite_commands.h"
#include "sqlite_bridge.h"


#define VarToString(var) _VarToString(bridgeInstance, var)

char *_VarToString(SqliteBridgeInstance * bridgeInstance, const pp::Var& var)
{
    if (!var.is_string())
        return NULL;
    uint32_t length;
    const char *utf8String = bridgeInstance->getPpbVar()->VarToUtf8(var.pp_var(), &length);
    char *string = (char *)sqlite3_malloc(sizeof(char) * (length + 1));
    memcpy(string, utf8String, length * sizeof(char));
    string[length] = '\0';
    return string;
}

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
    LOG("Open DB %s", request.Get(pp::Var("databaseName")).AsString().c_str());

    sqlite3 *db;
    pp::VarDictionary response;

    std::string *filename =  new std::string(request.Get(pp::Var("databaseName")).AsString());
    LOG("Open DB %s", filename->c_str());
    int rc = sqlite3_open_v2(request.Get(pp::Var("databaseName")).AsString().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, "nacl");
    if (rc)
    {
       LOG("FAILURE");
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

        response.Set(pp::Var("db"), pp::Var(reference));
    }

    bridgeInstance->PostResponse(request, response);
    //sqlite3_free((void *)filename);
}

static void HandleSqliteClose(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    LOG("Close DB");

    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;
    int rc = sqlite3_close(db);
    //int rc = SQLITE_OK;
    if (rc == SQLITE_OK)
    {
        OpenedDatabases[request.Get(pp::Var("db")).AsInt()] = NULL;
        response.Set(pp::Var("success"), pp::Var(true));
    }
    else
    {
        LOG("FAILURE");
        response.Set(pp::Var("success"), pp::Var(false));
        response.Set(pp::Var("error"), pp::Var(sqlite3_errmsg(db)));
    }
    bridgeInstance->PostResponse(request, response);
}

static void HandleSqliteKey(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    LOG("Key");

    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    unsigned int keyLength;
    const unsigned char *key = convert(request.Get(pp::Var("key")).AsString().c_str(), &keyLength);
    sqlite3_key(db, key, keyLength);
    pp::VarDictionary response;
    response.Set(pp::Var("success"), pp::Var(true));
    bridgeInstance->PostResponse(request, response);
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
      LOG("%20s", p_fields[i]);
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
    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;
    CurrentExecResponse = &response;
    char *statement = VarToString(request.Get(pp::Var("statement")));

    LOG(statement);

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
    sqlite3_free((void *)statement);
}

static std::vector<sqlite3_stmt *> Statements;

static void HandleSqlitePrepare(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    LOG("Close DB");

    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;

    bridgeInstance->PostResponse(request, response);
}

static void HandleSqliteBind(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{
    LOG("Close DB");

    sqlite3 *db = OpenedDatabases[request.Get(pp::Var("db")).AsInt()];
    pp::VarDictionary response;

    bridgeInstance->PostResponse(request, response);
}

static void HandleSqliteStep(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request)
{

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