
CommandType =
  SQLITE_OPEN : 0
  SQLITE_KEY : 1
  SQLITE_CLOSE : 2
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
  ledger.sqlite.__listenners[command.request_id] = command
  $('embed[name="sqlite_bridge"]')[0].postMessage(command)

ledger.sqlite ?= {}

_.extend ledger.sqlite,

  sqlite3_open: (databaseName, callback) -> postCommand({command: CommandType.SQLITE_OPEN, databaseName: databaseName}, callback)

  sqlite3_key: (db, key, keyLength, callback) -> postCommand({command: CommandType.SQLITE_KEY, key, keyLength}, callback)

  __handleMessage: (event) ->
    if event.data.magic is 'sqlite'
      listener = ledger.sqlite.__listenners[event.request_id]
      if listener?
        delete ledger.sqlite.__listenners[event.request_id]
        listener(event.data)

  __handleError: (event) ->


  __handleCrash: (event) ->

  __listenners: {}


$ () ->
  $('embed[name="sqlite_bridge"]').on 'message', ledger.sqlite.__handleMessage
  $('embed[name="sqlite_bridge"]').on 'error', ledger.sqlite.__handleError
  $('embed[name="sqlite_bridge"]').on 'crash', ledger.sqlite.__handleCrash