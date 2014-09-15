#ifndef _HANDLER_ECHO_HPP_
#define _HANDLER_ECHO_HPP_

#include <iostream>
#include <string>
#include <unistd.h>

#include <kisscpp/logstream.hpp>
#include <kisscpp/request_handler.hpp>
#include <kisscpp/boost_ptree.hpp>
#include <kisscpp/request_status.hpp>

class EchoHandler : public kisscpp::RequestHandler  /// Step 5. Derive your handler from kisscpp::RequestHandler
{
  public:
    EchoHandler() :
      kisscpp::RequestHandler("echo", "Will echo back what you send in.") /// "echo" is the unique string that is used to identify your handler.
                                                                          /// i.e. requests to your application, that have "kcm-cmd" set to "echo"
                                                                          /// will cause this handler's run() method to be executed.
    {
      kisscpp::LogStream log(__PRETTY_FUNCTION__);
    }

    ~EchoHandler() {};

    void run(const BoostPtree &request, BoostPtree &response); /// Step 6. You must override the run method from kisscpp::RequestHandler
  protected:
  private:
};

#endif

