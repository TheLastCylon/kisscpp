#include "kc_echo.hpp"

//--------------------------------------------------------------------------------
kc_echo::kc_echo() :
  Server(1, "kc_echo", "0", false)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  registerHandlers();
}

//--------------------------------------------------------------------------------
kc_echo::~kc_echo()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);
  stop();
}

//--------------------------------------------------------------------------------
void kc_echo::registerHandlers()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  echoHandler.reset(new EchoHandler()); /// Step 3. Instantiate your handler.
  register_handler(echoHandler);        /// Step 4. Register your handler.
}

