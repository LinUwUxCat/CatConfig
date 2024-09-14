#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct catsetting{
    char* name; // Name of the setting
    void* value;// Value of the setting. On init this is a pointer to a char*, when the setting is loaded it becomes a pointer to whatever it actually is
    char type;  // Type of setting. s = string, d = int, f = float, etc
    bool loaded;  // True if setting has been loaded by user, else false
};

catsetting* _settings;
int _settingscount = 0;

//////// Internal stuff ////////

// Load or create a setting.
// var is used as the pointer to the new value and is expected to be of the correct type (set later in corresponding functions)
// we can differentiate if a setting is new or not based on the value of `loaded`
catsetting* _catload(char* name, void* var){
    // Load setting if it exists
    for (int i = 0; i < _settingscount; i++){
        if (strcmp(_settings[i].name, name) == 0){
            return &(_settings[i]);
        }
    }
    // If it doesn't, create it
    catsetting newsetting;
    newsetting.name = name;
    newsetting.loaded = true;
    newsetting.value = var;
    _settings = (catsetting*)realloc(_settings, sizeof(catsetting) * ++_settingscount);
    _settings[_settingscount-1] = newsetting;
    return &(_settings[_settingscount-1]);
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
            setting.loaded = false;
            _settings = (catsetting*)realloc(_settings, sizeof(catsetting) * ++_settingscount);
            _settings[_settingscount-1] = setting;
        }
        free(line);
    }
    fclose(f);
}

bool catsave(char* filename){
    FILE* f = fopen(filename, "w");
    if (f == NULL) return false;
    for (int i = 0; i < _settingscount; i++){
        switch (_settings[i].type){
            case 's':
                fprintf(f, "%s=%s\n", _settings[i].name, _settings[i].value); // Write strings and unloaded settings as, well, string.
                break;
            case 'd':
                fprintf(f, "%s=%d\n", _settings[i].name, *(int*)(_settings[i].value)); // Write int 
                break;
            default:
                fprintf(f, "%s=%s\n", _settings[i].name, _settings[i].value); // Note : this might be wrong, but invalid type shouldn't happen anyways.
                break;
        }
    }
    fclose(f);
    return true;
}

/**
 * Frees memory allocated by catconfig.
 * Please call this before your code terminates!
 */
void catexit(){

}


/**
 * Load or create an integer setting.
 * @param name The name of the setting to load or create
 * @param var Pointer to the variable that will be used.
 */
bool catloadint(char* name, int* var){
    char* s;
    catsetting* setting = _catload(name, var);
    if (setting == NULL) return false;
    setting->type = 'd';
    if (setting->loaded){
        return (setting->value == (void*) var);
    }
    *var = atoi((char*)(setting->value));
    free(setting->value);
    setting->value = var;
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
    //return _catload(name, var);
    return true;
}