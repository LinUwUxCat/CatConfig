#pragma once
#include <cstdio>

char* _catconfigfilename;

struct catsetting{
    char* name;
    void* value;
    char type;
};

char* _catload(char* name){
    return name; // TODO : read the file and return variable
}

bool catloadint(char* name, int* var){
    *var = 0;
    char* s = _catload(name); // String
    if (s[0] == '\0') return false;
    // Start of own atoi
    int i = 0; // Current character index
    char c = s[i]; // Current character
    int n = 0; // Current character converted to a number
    int sign = 1;
    
    while (c != '\0'){
        //Handle sign
        if (i == 0){
            if (c == '-') {
                sign = -1;
                continue;
            }
            if (c == '+') continue;
        }
        
        n = c - 0x30;
        if (n < 0 || n > 9) break;
        //printf("%d - %d - %d\n", *var);
        *var = (*var * 10) + n;
        
        i++;
        c = s[i];
    }
    *var *= sign;
    return true;
}

/**
 * @param name the name of the setting to load
 * @param var the variable to load it to
 * @return true if the setting is 
 * Loads a setting to a string. 
 * Be aware that 
 */
bool catloadstring(char* name, char* var){
    var = _catload(name);
    return true;
}