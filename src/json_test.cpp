#include <iostream>
#include <iomanip>
#include <fstream>


#include "json.hpp"

using nlohmann::json;
using namespace std;

int main()
{
    // create a JSON object
    json j ={
        {"name", "test_name"},
        {"Tiles", {
            {
            "Black_box", {
                {200, 200, 32, 32, false}, {140, 149, 32, 32, true}
                    }
                }
            }
        }
    };
    

    fstream test;
    string testfile = "../bin/testjson.json";

    test.open(testfile);

    test << j.at("/Tiles/Black_box"_json_pointer);

    test.close();


}