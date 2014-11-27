/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#ifndef _SQLITE_COMMANDS_H_
#define _SQLITE_COMMANDS_H_

#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/var.h>
#include <ppapi/cpp/var_dictionary.h>
#include <ppapi/cpp/var_array.h>
#include "sqlite_bridge.h"

class SqliteBridgeInstance;

bool HandleSqliteCommand(SqliteBridgeInstance * bridgeInstance, const pp::VarDictionary& request);

#endif
