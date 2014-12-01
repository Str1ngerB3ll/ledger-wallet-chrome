/*
 *
 * SQLite JS bridge
 * (C) 2014 Ledger
 *
 */

#include "sqlite_bridge.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "sqlite_nacl_vfs.h"

static bool HTML5FS_IS_MOUNTED = false;

SqliteBridgeInstance *INSTANCE;

SqliteBridgeInstance::SqliteBridgeInstance(PP_Instance instance, PPB_GetInterface interface) :
    pp::Instance(instance),
    _interface(interface),
    _commands_thread(this),
    _callback_factory(this)
{
    INSTANCE = this;
    _ppb_var = (PPB_Var *) interface(PPB_VAR_INTERFACE);
}

bool SqliteBridgeInstance::Init(uint32_t /*argc*/, const char * [] /*argn*/, const char * [] /*argv*/)
{
    LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Hello from native"));
    nacl_io_init_ppapi(pp_instance(), _interface);
    _commands_thread.Start();
    return true;
}

void SqliteBridgeInstance::HandleMessage(const pp::Var& var_message) {
    // Ignore the message if it is not a dictionary.
    if (!var_message.is_dictionary())
      return ;

    if (HTML5FS_IS_MOUNTED == false)
    {
        HTML5FS_IS_MOUNTED = true;
        _commands_thread.message_loop().PostWork(_callback_factory.NewCallback(&SqliteBridgeInstance::OpenFileSystem));
    }

    pp::VarDictionary request(var_message);

    // Ignore the message if it has no magic.
    if (!request.HasKey(pp::Var("magic")))
        return ;

    if (request.Get(pp::Var("magic")) == pp::Var("sqlite"))
        _commands_thread.message_loop().PostWork(_callback_factory.NewCallback(&SqliteBridgeInstance::HandleSqliteRequest, request));
}

void SqliteBridgeInstance::OpenFileSystem(int32_t)
{
    LogToConsole(PP_LOGLEVEL_LOG, pp::Var("Open File System"));
    sqlite3_vfs_register(sqlite3_nacl_vfs(), 0);
    // By default, nacl_io mounts / to pass through to the original NaCl
    // filesystem (which doesn't do much). Let's remount it to a memfs
    // filesystem.
    umount("/");
    //mount("", "/", "memfs", 0, "");
 LogToConsole(PP_LOGLEVEL_LOG, pp::Var(strerror(errno)));
    mount("",                                       /* source */
          "/",                            /* target */
          "html5fs",                                /* filesystemtype */
          0,                                        /* mountflags */
          "expected_size=1048576"); /* data */
     LogToConsole(PP_LOGLEVEL_LOG, pp::Var(strerror(errno)));
    {
    std::ofstream myfile;
    myfile.open ("/test.txt");

    if (!myfile.is_open())
    {
        LogToConsole(PP_LOGLEVEL_LOG, pp::Var(strerror(errno)));
        return;
    }
     myfile << "Writing this to a file.\n";
        myfile.close();
    /* print some text */
    }
    {
    std::string line;
      std::ifstream myfile ("/test.txt");
      if (myfile.is_open())
      {
        while ( getline (myfile,line) )
        {
         LogToConsole(PP_LOGLEVEL_LOG, pp::Var(line));
        }
        myfile.close();
      }
    }
    LogToConsole(PP_LOGLEVEL_LOG, pp::Var("File system mounted"));
}

void SqliteBridgeInstance::HandleSqliteRequest(int32_t, const pp::VarDictionary& request)
{
    HandleSqliteCommand(const_cast<SqliteBridgeInstance *>(this), request);
}

void SqliteBridgeInstance::PostResponse(const pp::VarDictionary& request, pp::VarDictionary& response)
{
    response.Set(pp::Var("magic"), request.Get(pp::Var("magic")));
    response.Set(pp::Var("request_id"), request.Get(pp::Var("request_id")));
    PostMessage(response);
}

class Module : public pp::Module {
 public:
  Module() : pp::Module() {}
  virtual ~Module() {}

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new SqliteBridgeInstance(instance, get_browser_interface());
  }
};

namespace pp {

Module* CreateModule() { return new ::Module(); }

}