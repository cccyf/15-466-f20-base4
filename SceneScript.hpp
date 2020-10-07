#include <vector>

struct SceneScript{
    typedef struct{
        int index_a;
        int index_b;
        char txt[400];
    }Chapter;

    std::vector<Chapter> stories;

    Chapter cur_chapter;

    SceneScript();
    private:
    void LoadScript();
    
};