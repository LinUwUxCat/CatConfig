#include "../catconfig.h"


int main(int argc, char** argv){
    catinit("test.catcfg");
    
    int a = 0;
    catloadint("testing", &a);
    printf("a : %d\n", a);
    a = a+5;

    bool b = true;
    catloadbool("boolsetting", &b);
    printf("b : %d\n", b);
    
    char* teststr = "test";
    catloadstring("strsetting", &teststr);
    printf("s : %s\n", teststr);

    catsave("test.catcfg");
    
    catexit();
    return 0;
}