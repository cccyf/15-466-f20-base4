
#include "SceneScript.hpp"

SceneScript::SceneScript(){
    Chapter ch1 = {0, 1, "text 0"};
    Chapter ch2 = {0, 1, "text 1"};
    stories.push_back(ch1);
    stories.push_back(ch2);

    cur_chapter = ch1;
}