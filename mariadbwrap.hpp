/*
mariadbwrap
Copyright (C) 2018 Alessandro Fabbri

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <vector>
#include <mysql/mysql.h>
#include <jsoncons/json.hpp>

struct mariadb
{
  MYSQL     *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;

  std::string HOSTNAME;
  std::string USERNAME;
  std::string PASSWORD;
  std::string DATABASE;
  char* SOCKET;
  int PORT_NO;
  int OPT;

  std::vector<std::vector<std::string>> records;
  std::vector<std::string> headers;

  mariadb(const std::string &conf)
  {
    jsoncons::json jconn;
    try
    {
      jconn = jsoncons::json::parse_file(conf);
    }
    catch(std::exception &e)
    {
      throw std::runtime_error(R"(Config json 'config.json' not found. Sample
{
  "HOSTNAME" : "localhost",
  "DATABASE" : "test",
  "USERNAME" : "root",
  "PASSWORD" : ""
})");
    }

    HOSTNAME = jconn.has_member("HOSTNAME") ? jconn["HOSTNAME"].as<std::string>() : std::string("localhost");
    DATABASE = jconn.has_member("DATABASE") ? jconn["DATABASE"].as<std::string>() : std::string("mysql");
    USERNAME = jconn.has_member("USERNAME") ? jconn["USERNAME"].as<std::string>() : std::string("root");
    PASSWORD = jconn.has_member("PASSWORD") ? jconn["PASSWORD"].as<std::string>() : std::string("");
    SOCKET   = NULL;
    PORT_NO  = jconn.has_member("PORT") ? jconn["PORT"].as<int>() : 3306;
    OPT      = 0;

    std::cout
      << "HOSTNAME " << HOSTNAME << std::endl
      << "DATABASE " << DATABASE << std::endl
      << "USERNAME " << USERNAME << std::endl
      << "PASSWORD " << PASSWORD << std::endl;

    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, HOSTNAME.c_str(), USERNAME.c_str(), PASSWORD.c_str(), DATABASE.c_str(), PORT_NO, SOCKET, OPT))
      throw std::runtime_error("MYSQL_ERROR " + std::string(mysql_error(conn)));
  }

  ~mariadb()
  {
    mysql_close(conn);
  }

  std::vector<std::vector<std::string>> query(const std::string &query = "SELECT * FROM people;")
  {
    std::vector<std::vector<std::string>> result;

    if(mysql_query(conn, query.c_str()))
      throw std::runtime_error("MYSQL_ERROR " + std::string(mysql_error(conn)));

    res = mysql_store_result(conn);
    auto colnum = mysql_num_fields(res);

    MYSQL_FIELD *field;
    result.resize(1);
    while ((field = mysql_fetch_field(res)) != NULL)
      result.back().emplace_back(field->name);

    while ((row = mysql_fetch_row(res)) != NULL)
    {
      std::vector<std::string> record;
      for(int i=0; i<colnum; ++i) record.emplace_back(row[i]);
      result.emplace_back(record);
    }

    // Release memories
    mysql_free_result(res);

    return result;
  }

  template<typename Func>
  void query(const std::string &query, Func cb)
  {
    records.clear();
    headers.clear();

    if(mysql_query(conn, query.c_str()))
      throw std::runtime_error("MYSQL_ERROR " + std::string(mysql_error(conn)));

    res = mysql_store_result(conn);

    cb();

    mysql_free_result(res);
  }
};
