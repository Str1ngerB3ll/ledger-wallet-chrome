
test_insert = (db, count, callback) =>
  ledger.sqlite.sqlite3_exec db, "INSERT INTO Test VALUES(#{count}, 'hello world');", (result) =>
    e result.error if result.success is false
    if count < 0
      callback?()
    else
      test_insert(db, count - 1, callback)



CommandType =
  SQLITE_OPEN : 0
  SQLITE_CLOSE : 1
  SQLITE_KEY : 2
  SQLITE_EXEC : 3
  SQLITE_PREPARE : 4
  SQLITE_BIND : 5
  SQLITE_STEP : 6

SqlDataType =
  TEXT : 0
  INTEGER : 1
  NUMERIC : 2
  REAL : 3
  NULL : 4
  BLOB : 5

postCommand = (command, callback) ->
  command.magic = "sqlite"
  command.request_id = _.uniqueId()
  ledger.sqlite.__listenners[command.request_id] = callback
  $('embed[name="sqlite_bridge"]')[0].postMessage(command)

ledger.sqlite ?= {}

_.extend ledger.sqlite,

  sqlite3_open: (databaseName, callback) -> postCommand({command: CommandType.SQLITE_OPEN, databaseName: databaseName}, callback)

  sqlite3_key: (db, key , callback) -> postCommand({command: CommandType.SQLITE_KEY, key: key}, callback)

  sqlite3_close: (db, callback) -> postCommand {command: CommandType.SQLITE_CLOSE, db: db}, callback

  sqlite3_exec: (db, statement, callback) -> postCommand {command: CommandType.SQLITE_EXEC, db, statement}, callback

  __handleMessage: (event) ->
    data = event.data
    if data.magic is 'sqlite'
      listener = ledger.sqlite.__listenners[data.request_id]
      if listener?
        delete ledger.sqlite.__listenners[data.request_id]
        listener(data)

  __handleError: (event) ->
    e 'Sqlite ERROR'

  __handleCrash: (event) ->
    e 'SQlite CRASH'

  __listenners: {}

  sqlite3_integration_test: () ->
    before = _.now()
    ledger.sqlite.sqlite3_open '/test-2.sqlite', (result) =>
      l result
      ledger.sqlite.sqlite3_key result.db, ledger.crypto.SHA256.hashString("merguez"), =>
        ledger.sqlite.sqlite3_exec result.db, "CREATE TABLE Test(iteration INTEGER, test TEXT);", (result) =>
          l "create result", result
          ledger.sqlite.sqlite3_exec result.db, "BEGIN TRANSACTION", (result) =>
          test_insert result.db, 5000, =>
            ledger.sqlite.sqlite3_exec result.db, "END TRANSACTION", (result) =>
              l 'Insertion DONE ', _.now() - before
            ledger.sqlite.sqlite3_close 0

  sqlite3_integration_test_no_create: () ->
    before = _.now()
    ledger.sqlite.sqlite3_open '/test.sqlite', (result) =>
      ledger.sqlite.sqlite3_key result.db, ledger.crypto.SHA256.hashString("merguez"), =>
          l "create result", result
          test_insert result.db, 100, =>
            l 'Insertion DONE ', (_.now() - before)
            #ledger.sqlite.sqlite3_close result.db



$ () ->
  $('embed[name="sqlite_bridge"]')[0].addEventListener 'message', ledger.sqlite.__handleMessage, true
  $('embed[name="sqlite_bridge"]').on 'error', ledger.sqlite.__handleError
  $('embed[name="sqlite_bridge"]').on 'crash', ledger.sqlite.__handleCrash