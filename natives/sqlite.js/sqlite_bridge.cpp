/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#include "sqlite_bridge.h"

SqliteBridgeInstance::SqliteBridgeInstance(PP_Instance instance) : pp::Instance(instance)
{

}

bool SqliteBridgeInstance::Init(uint32_t /*argc*/, const char * [] /*argn*/, const char * [] /*argv*/)
{
  LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Hello from native"));
  return true;
}

void SqliteBridgeInstance::HandleMessage(const pp::Var& var_message) {
//  const char kTokenMessage[] = "token:";
//  const size_t kTokenMessageLen = strlen(kTokenMessage);
//  const char kGetFileMessage[] = "getFile";
//
//  if (!var_message.is_string()) {
//    return;
//  }
//
//  std::string message = var_message.AsString();
//  printf("Got message: \"%s\"\n", message.c_str());
//  if (message.compare(0, kTokenMessageLen, kTokenMessage) == 0) {
//    // Auth token
//    std::string auth_token = message.substr(kTokenMessageLen);
//    worker_thread_.message_loop().PostWork(callback_factory_.NewCallback(
//        &Instance::ThreadSetAuthToken, auth_token));
//  } else if (message == kGetFileMessage) {
//    // Request
//    if (!is_processing_request_) {
//      is_processing_request_ = true;
//      worker_thread_.message_loop().PostWork(
//          callback_factory_.NewCallback(&Instance::ThreadRequestThunk));
//    }
//  }
}

void SqliteBridgeInstance::PostMessagef(const char* format, ...) {
//  const size_t kBufferSize = 1024;
//  char buffer[kBufferSize];
//  va_list args;
//  va_start(args, format);
//  vsnprintf(&buffer[0], kBufferSize, format, args);
//
//  PostMessage(buffer);
}

class Module : public pp::Module {
 public:
  Module() : pp::Module() {}
  virtual ~Module() {}

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new SqliteBridgeInstance(instance);
  }
};

namespace pp {

Module* CreateModule() { return new ::Module(); }

}