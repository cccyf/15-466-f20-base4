
#include "SceneScript.hpp"
#include "data_path.hpp"
#include <iostream>
#include <fstream>
#include <string>

void SceneScript::LoadScript(){
    std::string fn = data_path("script");
    std::ifstream myfile(fn);
    std::string line;
    std::string index0;
    std::string index1;
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            getline(myfile, index0);
            getline(myfile, index1);
            Chapter nc = {std::stoi(index0), std::stoi(index1), ""};
            strcpy(nc.txt, line.c_str());
            nc.txt[strlen(line.c_str())] = 0;
            stories.push_back(nc);
        }
        myfile.close();
    };
}

SceneScript::SceneScript(){
    LoadScript();
    cur_chapter = stories[0];
}