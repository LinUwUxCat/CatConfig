#include "../catconfig.h"


int main(int argc, char** argv){
    int number = 0;
    printf("%d\n", number);
    catloadint("bogus", &number);
    printf("%d\n", number);
    return 0;
}