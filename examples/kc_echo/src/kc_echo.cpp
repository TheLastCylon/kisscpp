#include "kc_echo.hpp"

//--------------------------------------------------------------------------------
kc_echo::kc_echo() : Server("localhost", "9100", 1)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__, "/tmp/kc_echo.log", true);
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

