// File  : configuration.hpp
// Author: Dirk J. Botha <bothadj@gmail.com>
//
// This file is part of kisscpp library.
//
// The kisscpp library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// The kisscpp library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the kisscpp library. If not, see <http://www.gnu.org/licenses/>.

#ifndef _CONFIGURATION_HPP_
#define _CONFIGURATION_HPP_

#include <string>
#include <sstream>
#include <cstdlib>
#include <boost/filesystem.hpp>
#include "boost_ptree.hpp"
#include "logstream.hpp"

namespace bfs = boost::filesystem;
//namespace bpt = boost::property_tree::ptree;

namespace kisscpp
{
  class Config
  {
    public:
      static Config* instance(const std::string app_id               = "kisscpp_application",
                              const std::string app_instance         = "0",
                              const std::string explicit_config_path = "");

      void           initiate(const std::string explicit_config_path = "");

      ~Config()
      {
        kisscpp::LogStream log(__PRETTY_FUNCTION__);
      };

      //--------------------------------------------------------------------------------
      template<typename T> T                  get         (const std::string &s)                  { return cfg_data.get         <T>(s);               }
      template<typename T> T                  get         (const std::string &s, T default_value) { return cfg_data.get         <T>(s,default_value); }
      template<typename T> boost::optional<T> get_optional(const std::string &s)                  { return cfg_data.get_optional<T>(s);               }

      std::string getApplicationId()       { return application_id; }
      std::string getApplicationInstance() { return application_instance; }

      //--------------------------------------------------------------------------------
      // TODO: Add method for reloading configuration.

    protected:
    private:
      Config           ()              { kisscpp::LogStream log(__PRETTY_FUNCTION__); };
      Config           (Config const&) { kisscpp::LogStream log(__PRETTY_FUNCTION__); };
      Config& operator=(Config const&) { kisscpp::LogStream log(__PRETTY_FUNCTION__); };

      Config(const std::string app_id,
             const std::string app_instance,
             const std::string explicit_config_path)
      {
        kisscpp::LogStream log(__PRETTY_FUNCTION__);

        application_id       = app_id;
        application_instance = app_instance;

        initiate(explicit_config_path);
      }

      void loadConfig();
      bool loadConfig(std::string &cfg_path, BoostPtree &pt);

      static Config   *singleton_instance;

      std::string      config_path_instance; // path to the configuration for this instance of the application
      std::string      config_path_common;   // path to the common configuration for applications with this application id

      BoostPtree       cfg_data;

      std::string      application_id;
      std::string      application_instance;
  };
}

#endif // _STATSKEEPER_HPP_
