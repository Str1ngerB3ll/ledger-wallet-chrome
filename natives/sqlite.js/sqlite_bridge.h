/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

typedef enum CommandType {
    SQLITE_OPEN = 0,
    SQLITE_KEY = 1,
    SQLITE_CLOSE = 2,
    SQLITE_EXEC = 3,
    SQLITE_PREPARE = 4,
    SQLITE_BIND = 5,
    SQLITE_STEP = 6
} CommandType;

typedef enum SqlDataType {
    TEXT = 0,
    INTEGER = 1
    NUMERIC = 2,
    REAL = 3,
    NULL = 4,
    BLOB = 5
} SqlDataType;

class SqliteBridgeInstance : public pp::Instance
{
    public:
    SqliteBridgeInstance(PP_Instance instance);
    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
    virtual void HandleMessage(const pp::Var& var_message);
};
