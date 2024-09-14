#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
struct catsetting{
    char* name; // Name of the setting
    void* value;// Value of the setting. On init this is a pointer to a char*, when the setting is loaded it becomes a pointer to whatever it actually is
    char type;  // Type of setting. s = string, d = int, f = float, etc
    bool read;  // True if setting has been loaded by user, else false
};

catsetting* _settings;

//////// Internal stuff ////////

bool _catload(char* name, char* var){
    return name; // TODO : read the file and return variable
}

bool _catreadline(FILE* f, char** line){
    *line = (char*)malloc(1);
    int size = 0;
    char c = getc(f);
    while (c != EOF && c != '\n' && c != '\r'){
        size++;
        *line = (char*)realloc(*line, size * sizeof(char)+1);
        (*line)[size-1] = c;
        c = getc(f);
    }
    (*line)[size] = '\0';
    if (c == EOF) return false;
    return true;
}


/**
 * @param filename name or path to the config file
 * reads the whole setting file and loads settings.
 */
void catinit(char* filename){
    FILE* f = fopen(filename, "r");
    if (f == NULL) return;
    char* line;
    _settings = (catsetting*)malloc(1);
    bool reading = true;
    while (reading){
        reading = _catreadline(f, &line);
        char* paramname = (char*)malloc(1);
        int valsize = -1;
        int paramsize = 0;
        for (int i = 0; line[i]!='\0'; i++){
            if (valsize == -1) {
                if (line[i]=='='){
                    valsize = 0;
                    paramname[i] = '\0';
                    paramsize = i;
                } else {
                    paramname = (char*)realloc(paramname, i+2);
                    paramname[i] = line[i];
                }
            } else {
                valsize++;
            }
        }
        if (paramsize == 0) continue;
        if (valsize > -1){
            catsetting setting;
            setting.name = paramname;
            setting.value = malloc(valsize * sizeof(char) + 1);
            strcpy((char*)(setting.value), (line + paramsize + 1));
            setting.type = 's';
            setting.read = false;
            printf("SETTING %s - %s\n", setting.name, (char*)setting.value);
        }
        free(line);
    }
}

bool catsave(char* filename){
    FILE* f = fopen(filename, "w");
}

/**
 * Frees all allocated memory.
 * Please call this before your code terminates!
 */
void catexit(){

}


/**
 * TODO: Redo part of this
 */
bool catloadint(char* name, int* var){
    *var = 0;
    char* s;
    if (!_catload(name, s)) return false;
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
    return _catload(name, var);
}