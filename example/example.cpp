#include "../catconfig.h"


int main(int argc, char** argv){
    catinit("test.catcfg");
    int a = 0;
    catloadint("testing", &a);
    printf("a : %d\n", a);
    a = a+5;
    int b = 56;
    catloadint("newsetting", &b);
    printf("%d", b);
    catsave("test.catcfg"); // Check the file!
    catexit();
    return 0;
}