@ledger ?= {}
@ledger.utils ?= {}

Levels = {}
for level, i in ["NONE", "RAW", "FATAL", "ERROR", "WARN", "BAD", "GOOD", "INFO", "VERB", "DEBUG", "TRACE", "ALL"]
  Levels[level] = i++

###
  Utility class for dealing with logs
###
class @ledger.utils.Logger
  @Levels: Levels

  @_loggers: {}

  # Return storage instance if initialized or undefined.
  # @return [ledger.storage.ChromeStore, undefined]
  @store: -> @_store ?= new ledger.storage?.ChromeStore("logs")

#################################
# Class methods
#################################
  
  # @return [Q.Promise]
  @logs: (cb) ->
    d = ledger.defer(cb)
    @store()?.keys (keys) =>
      @store().get keys, (items) => d.resolve(_.values(items))
    d.promise

  # @return [legder.utils.Logger]
  @getLoggerByTag: (tag) ->
    @_loggers[tag] ?= new @(tag)

#################################
# Instance methods
#################################

  # Logger's constructor
  # @param [String, Number, Boolean] level of the Logger
  constructor: (tag, @level=ledger.config?.defaultLoggingLevel) ->
    @_tag = tag
    @level = Levels[@level] if typeof @level == "string"
    @level = Levels.ALL if @level is true
    @level = Levels.None if @level is false
    @constructor._loggers[tag] = this

  #################################
  # Accessors
  #################################

  # Return Logger class storage if initialized or undefined.
  # @return [ledger.storage.ChromeStore, undefined]
  store: -> @constructor.store()

  # Sets the log level
  # @param [Boolean] active or not the logger.
  setLevel: (level) -> @level = level

  #
  levelName: (level=@level) -> _.invert(Levels)[level]

  # Sets the active state
  # @param [Boolean] active or not the logger.
  setActive: (active) -> @level = if active then Levels.INFO else Levels.NONE

  # Gets the current active state.
  # @return [Boolean] The current enabled flag.
  isActive: -> @level > Levels.NONE

  isFatal: -> @level >= Levels.FATAL
  isError: -> @level >= Levels.ERROR
  isErr: @prototype.isError
  isWarn: -> @level >= Levels.WARN
  isWarning: @prototype.isWarn
  isBad: -> @level >= Levels.BAD
  isGood: -> @level >= Levels.GOOD
  isGood: @prototype.isSuccess
  isInfo: -> @level >= Levels.INFO
  isVerb: -> @level >= Levels.VERB
  isVerbose: @prototype.isVerb
  isDebug: -> @level >= Levels.DEBUG
  isTrace: -> @level >= Levels.TRACE

  #################################
  # Logging methods
  #################################

  fatal: (args...) -> @_log(Levels.FATAL, args...)
  error: (args...) -> @_log(Levels.ERROR, args...)
  err: @prototype.error
  warn: (args...) -> @_log(Levels.WARN, args...)
  warning: @prototype.warn
  bad: (args...) -> @_log(Levels.BAD, args...)
  good: (args...) -> @_log(Levels.GOOD, args...)
  success: @prototype.good
  info: (args...) -> @_log(Levels.INFO, args...)
  verb: (args...) -> @_log(Levels.VERB, args...)
  verbose: @prototype.verb
  debug: (args...) -> @_log(Levels.DEBUG, args...)
  raw: (args...) -> @_log(Levels.RAW, args...)
  trace: (args...) -> @_log(Levels.TRACE, args...)

  #################################
  # Stored Logs
  #################################

  # Clear logs from entries older than 24h
  clear: ->
    @store()?.keys (keys) =>
      now = new Date().getTime()
      @store().remove(key) for key in keys when (now - key > 86400000) # 86400000ms => 24h

  # Retreive saved logs
  # @param [Function] cb A callback invoked once we get the logs as an array
  # @return [Q.Promise]
  logs: (cb) ->
    @constructor.logs().then (logs) =>
      logs = logs.filter (l) => l.tag is @tag
      cb?(logs)
      logs

  # Save a log in chrome local storage
  # @private
  # @param [String] msg Message to log.
  # @param [String] msgType Log level.
  _storeLog: (msg, msgType) ->
      now = new Date()
      log = {}
      log[now.getTime()] = date: now.toUTCString(), type: msgType, msg: msg, tag: @_tag
      @clear()
      @store()?.set(log)

  #################################
  # Protected. Formatting methods
  #################################

  ###
  Generic log function. Add header with usefull informations + log to console + store in DB.

  @exemple Simple call 
    @_log(Levels.VERB, "Entering in function with args", arg1, arg2)

  @param [Number] level defined in Levels.
  @return undefined
  ###
  _log: (level, args...) ->
    return unless level <= @level
    @_storeLog(@_stringify(args...), @levelName(level))
    if ledger.isDev
      args = (if level != Levels.RAW then [@_header(level)] else []).concat(args)
      @_consolify(level, args...)

  ###
  Add usefull informations like level and timestamp.
  @param [Number] level
  @return String
  ###
  _header: (level, date) ->
    _.str.sprintf('[%s][%s]', @_timestamp(date), @levelName(level))

  ###
  @param [Date] date
  @return String
  ###
  _timestamp: (date=new Date()) ->
    _.str.sprintf("%s.%03d", date.toLocaleTimeString(), date.getMilliseconds())

  ###
  Convert correctly arguments into string.
  @return String
  ###
  _stringify: (args...) ->
    formatter = if typeof args[0] is 'string' then ""+args.shift().replace(/%/g,'%%') else ""
    params = for arg in args
      formatter += " %s"
      if (! arg?) || typeof arg == 'string' || typeof arg == 'number' || typeof arg == 'boolean'
        arg
      else if typeof arg == 'object' && (arg instanceof RegExp || arg instanceof Date)
        arg
      else if typeof arg == 'object' && arg instanceof HTMLElement
        "HTMLElement." + arg.tagName
      else # Arrays and Hashs
        try
          JSON.stringify(arg)
        catch err
          "<< stringify error: #{err} >>"
    _.str.sprintf(formatter, params...)

  ###
  Add color depending of level.
  @return undefined
  ###
  _consolify: (level, args...) ->
    args = [].concat(args)

    # Add color
    if typeof args[0] is 'string'
      args[0] = "%c" + args[0].replace(/%/g,'%%')
    else
      args.splice 0, 0, "%c"
    args.splice 1, 0, switch level
      when Levels.FATAL, Levels.ERROR, Levels.BAD then 'color: #f00'
      when Levels.WARN then 'color: #f60'
      when Levels.INFO then 'color: #00f'
      when Levels.GOOD then 'color: #090'
      when Levels.DEBUG then 'color: #444'
      when Levels.TRACE then 'color: #888'
      else 'color: #000'

    # Add arguments catchers to colorify strings
    for arg in args[2..-1]
      args[0] += if typeof arg is 'string' then " %s"
      else if typeof arg is 'number' || typeof arg is 'boolean' then " %o"
      else if typeof arg is 'object' && arg instanceof RegExp then " %o"
      else if typeof arg is 'object' && arg instanceof Date then " %s"
      else if typeof arg is 'object' && arg instanceof window.HTMLElement then " %o"
      else " %O"

    method = switch level
      when Levels.FATAL, Levels.ERROR then "error"
      when Levels.WARN then "warn"
      when Levels.INFO, Levels.GOOD, Levels.BAD then "info"
      when Levels.DEBUG then "debug"
      else "log"

    console[method](args...)

ledger.utils.logger = new ledger.utils.Logger("DeprecatedLogger")

# Shortcuts
if @ledger.isDev
  @l = console.log.bind(console)
  @e = console.error.bind(console)
