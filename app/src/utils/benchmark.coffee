
class BenchmarkReport

  constructor: (name, closure) ->
    if _.isFunction(name)
      @_closure = name
      @_name = 'benchmark'
    else
      @_closure = closure
      @_name = name

  start: () ->
    @_beginTime = _.now()
    closure(@done.bind(@))

  done: () ->
    @_endTime = _.now()

  getElapsedTime: () -> @_endTime - @_beginTime

  logReport: () -> l "Report for #{@_name}: Elapsed time #{@getElapsedTime() / 1000}s"

ledger.benchmark = (name, closure) ->
  report = new BenchmarkReport name, closure
  report.start()
  report