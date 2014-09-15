#ifndef _KC_ECHO_HPP_
#define _KC_ECHO_HPP_

#include <boost/thread.hpp>
#include <kisscpp/server.hpp>
#include <kisscpp/logstream.hpp>
#include "handler_echo.hpp"

// ----------------------- TODO: -----------------------------
class kc_echo : public kisscpp::Server /// Step 1: Derive your application's main class, from kisscpp::Server
{
  public:
    kc_echo();
    ~kc_echo();

  protected:
    void registerHandlers(); /// This isn't stricly needed, I just like keep all my request-handler registreations in one place.

  private:
    kisscpp::RequestHandlerPtr echoHandler; /// Step 2: you need atleast one request Handler.
};

#endif

