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

#include <mariadbwrap.hpp>

using namespace std;

int main()
{
  try
  {
    mariadb db("conf.json");
    string table_name = "people";

    // default query, returns records as vector of vector of string
    auto result = db.query("SELECT * FROM " + table_name + ";");
    for(auto record : result)
    {
      for(auto field : record) cout << field << " - ";
      cout << endl;
    }

    // custom query, accept a lambda
    db.query("SELECT * FROM " + table_name + ";", [&db](){
      MYSQL_FIELD *field;
      auto colnum = mysql_num_fields(db.res);

      while ((field = mysql_fetch_field(db.res)) != NULL)
        db.headers.emplace_back(field->name);

      while ((db.row = mysql_fetch_row(db.res)) != NULL)
      {
        std::vector<std::string> record;
        for(int i=0; i<colnum; ++i) record.emplace_back(db.row[i]);
        db.records.emplace_back(record);
      }

      for(auto h : db.headers) cout << h << " | ";
      cout << endl;
      for(auto rec : db.records)
      {
        for(auto field : rec)
          cout << field << " | ";
        cout << endl;
      }
    });
  }
  catch(std::exception &e)
  {
    std::cerr << "EXC: " << e.what() << std::endl;
  }
}