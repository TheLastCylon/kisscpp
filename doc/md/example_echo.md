# Example : kc_echo

Below, is the main.cpp for the kc\_echo application. It serves as a basic
demonstration of the power we have available to us, through KISSCPP.

It is a typical example of a main.cpp file, if you end up with a main.cpp
file, that has more in it, you either have some very specific requirements
or you aren't using the library as intended.

~~~~(main.cpp)
#include "kc_echo.hpp"

int main(int argc, char* argv[])
{
  try {
    kc_echo app;
    app.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
~~~~

Examine the code above for a while, I hope it's clear that kc\_echo is a class
name, and that it's being instantiated within the try block of our main function.
Immideately after, the run method of that object is then executed.

Now consider the kc\_echo class:

~~~~(kc_echo.hpp)
#ifndef _KC_ECHO_HPP_
#define _KC_ECHO_HPP_

#include <boost/thread.hpp>
#include <kisscpp/server.hpp>
#include <kisscpp/logstream.hpp>
#include "handler_echo.hpp"

class kc_echo : public kisscpp::Server      // Step 1: Derive your application's main class, from kisscpp::Server
{
  public:
    kc_echo();
    ~kc_echo();

  protected:
    void registerHandlers();                // This isn't stricly needed, I just like keep all my request-handler registreations in one place.

  private:
    kisscpp::RequestHandlerPtr echoHandler; // Step 2: you need atleast one request Handler.
};

#endif
~~~~

That's it, that is all you need in order to have a basic server set up.
Now consider the implimentation of that class:

~~~~(kc_echo.cpp)
#include "kc_echo.hpp"

kc_echo::kc_echo() : Server("localhost", "9100", 1)
{
  registerHandlers();
}

kc_echo::~kc_echo()
{
  stop();
}

void kc_echo::registerHandlers()
{
  echoHandler.reset(new EchoHandler()); // Step 3. Instantiate your handler.
  register_handler(echoHandler);        // Step 4. Register your handler.
}
~~~~

That's it, all we need to do now, is implement the "echo" request handler.
I follow a simple convention with handlers. I name the source files *handler_*
followed by the texctual id of the handler.
i.e. With this example we'll have handler_echo.cpp and handler_echo.hpp.
This serves to easily identify handlers from source file names.

Here is the EchoHandler class:
~~~~(handler_echo.hpp)
#ifndef _HANDLER_ECHO_HPP_
#define _HANDLER_ECHO_HPP_

#include <iostream>
#include <string>
#include <unistd.h>

#include <kisscpp/logstream.hpp>
#include <kisscpp/request_handler.hpp>
#include <kisscpp/boost_ptree.hpp>
#include <kisscpp/request_status.hpp>

class EchoHandler : public kisscpp::RequestHandler                           // Step 5. Derive your handler from kisscpp::RequestHandler
{
  public:
    EchoHandler() :
      kisscpp::RequestHandler("echo", "Will echo back what you send in.") {} // "echo" is the unique string that is used to identify your handler.
                                                                             // i.e. requests to your application, that have "kcm-cmd" set to "echo"
                                                                             // will cause this handler's run() method to be executed.

    ~EchoHandler() {};

    void run(const BoostPtree &request, BoostPtree &response);               // Step 6. You must override the run method from kisscpp::RequestHandler
  protected:
  private:
};

#endif
~~~~

And now the last file you need for a complete kisscpp application.

~~~~(handler_echo.cpp)
#include "handler_echo.hpp"

void EchoHandler::run(const BoostPtree &request, BoostPtree &response)
{
  try {
    // This is where you build your response
    response.put("kcm-sts" , kisscpp::RQST_SUCCESS);                // Make sure to set kcm-sts to RQST_SUCCESS in the event of success
    response.put("you-sent", request.get<std::string>("message"));  // populate any aditional response parameters.
  } catch (boost::property_tree::ptree_bad_path &e) {
    response.put("kcm-sts", kisscpp::RQST_MISSING_PARAMETER);       // Make sure to set "kcm-sts" to the appropriate error status.
    response.put("kcm-erm", e.what());
  } catch (std::exception& e) {
    response.put("kcm-sts", kisscpp::RQST_UNKNOWN);                    
    response.put("kcm-erm", e.what());
  }
}
~~~~

Remarkably, this is the part I consider the most difficult to understand. Yet it
also happens to be the portion that has the least going on.

