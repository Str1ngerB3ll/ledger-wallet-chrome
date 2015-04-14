
class ledger.api.UnspentOutputsRestClient extends ledger.api.RestClient

  getUnspentOutputsFromAddresses: (addresses, callback) ->
    addresses = (address for address in addresses when Bitcoin.Address.validate(address) is true)
    query = _(addresses).join(',')
    @_logger().info "getUnspentOutputsFromAddresses: Request API"
    @http().get
      url: "blockchain/addresses/#{query}/unspents"
      onSuccess: (response) =>
        callback?(response)
      onError: @networkErrorCallback(callback)

  getUnspentOutputsFromPaths: (addressesPaths, callback) ->
    deferred = ledger.defer(callback)
    deferred.promise.fail (ex) =>  @_logger().info "getUnspentOutputsFromPaths: Failed with", ex
    @_logger().info "getUnspentOutputsFromPaths: get addresses", addressesPaths
    ledger.wallet.pathsToAddresses addressesPaths, (addresses, notFound) =>
      @_logger().info "getUnspentOutputsFromPaths: got addresses", addresses
      if notFound.length == addressesPaths.length
        deferred.rejectWithError(ledger.errors.NotFound, "Missings addresses for path " + JSON.stringify(notFound))
      else
        @getUnspentOutputsFromAddresses _.values(addresses), (outputs, error) =>
          return deferred.reject(error) if error?
          paths = _.invert(addresses)
          for output in outputs
            output.paths = []
            output.paths.push paths[address] for address in output.addresses when paths[address]?
          deferred.resolve(outputs)
    deferred.promise

  _logger: -> ledger.utils.Logger.getLoggerByTag("UnspentOutputsRestClient")

ledger.api.UnspentOutputsRestClient.instance = new ledger.api.UnspentOutputsRestClient()