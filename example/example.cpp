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

    double d = 3.430592304239423429;
    float f = 5.3204234f;
    catloaddouble("dvar", &d);
    catloadfloat("fvar", &f);
    printf("d %.12f | f %f\n", d, f);

    catsave("test.catcfg");
    
    catexit();
    return 0;
}