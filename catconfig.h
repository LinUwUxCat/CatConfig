#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/** 
 * @param S setting variable name
 * @param N name 
 * @param V var
 * @param T Type as char
 * Macro used internally to load a setting and set its type, because this becomes repetitive in functions.
 * Explanation of how this works :
 * - loads a setting using _catload
 * - if it's null, then it's invalid. return false (the functions we call this from all return bool)
 * - set the setting type
 * - If loaded is true, that means it has either already been loaded, or was just created by _catload.
 * - Return true if it's been created, false if it was already loaded and the provided variable isn't the same.
 */
#define CATLOADANDSETTYPE(S, N, V, T) \
catsetting* S = _catload(N, V); \
if (S == NULL) return false; \
S->type = T; \
if (S->loaded) return (S->value == (void*) V); \
S->loaded = true;

/**
 * Internal struct for a setting
 */
struct catsetting{
    char* name; // Name of the setting
    void* value;// Value of the setting. On init this is a pointer to a char*, when the setting is loaded it becomes a pointer to whatever it actually is
    char type;  // Type of setting. s = string, i = int, f = float, etc
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
    if (strchr(name, '=') || strchr(name, '\r') || strchr(name, '\r')) return NULL; // Refuse to create a setting with '=', '\r' or '\n' in the name
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
 * Reads the whole setting file and loads settings.
 * If the file does not exist or can't be read, empty settings will still be made.
 */
void catinit(char* filename){
    FILE* f = fopen(filename, "r");
    _settings = (catsetting*)malloc(1);
    if (f == NULL){ // If we cannot read the file, make empty settings still.
        _settingscount = 0;
        return;
    }
    char* line;
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

//Macro for below function
#define DOPRINT(S, V) fprintf(f, S, _settings[i].name, V);
/**
 * @param filename Name or Path to the config file
 * @param ignoreUnloaded Set this to true if only loaded settings should be saved, false if all settings should be saved. Default : false.
 * @return true if successful, false if saving failed (e.g. incorrect file permissions)
 * Save the config to the specified file.
 * Creates a file if it does not exist.
 */
bool catsave(char* filename, bool ignoreUnloaded = false){
    FILE* f = fopen(filename, "w");
    if (f == NULL) return false;
    for (int i = 0; i < _settingscount; i++){
        if (ignoreUnloaded && !_settings[i].loaded) continue;
        switch (_settings[i].type){
            case 's':
                DOPRINT("%s=%s\n", *(char**)(_settings[i].value)); // Write strings and unloaded settings as, well, string.
                break;
            case 'i':
                DOPRINT("%s=%d\n", *(int*)(_settings[i].value)); // Write int 
                break;
            case 'b':
                DOPRINT("%s=%s\n", (*(bool*)(_settings[i].value))?"true":"false"); // Write bool as "true" or "false"
                break;
            case 'f':
                DOPRINT("%s=%f\n", (*(float*)(_settings[i].value))); // Write float
                break;
            case 'd':
                DOPRINT("%s=%.12f\n", (*(double*)(_settings[i].value))); // Write double
                break;
            default:
                DOPRINT("%s=%s\n", _settings[i].value); // Note : this might be wrong, but invalid type shouldn't happen anyways.
                break;
        }
    }
    fclose(f);
    return true;
}

/**
 * @param freeAllStrings True if this function should call free() on all string type settings. Useful if you didn't reassign any string variable and didn't free them. Default : false
 * Frees memory allocated by catconfig.
 * Please call this before your code terminates!
 */
void catexit(bool freeAllStrings = false){
    for (int i = 0; i < _settingscount; i++){
        //free(_settings[i].name); // Name can only be freed if the setting was not created, but if it was loaded. Unsure how to work on this rn, as this means slight memory leak
        if (freeAllStrings && _settings[i].type == 's') free(_settings[i].value);
    }
    free(_settings);
}


/**
 * Load or create an integer setting.
 * @param name The name of the setting to load or create
 * @param var Pointer to the variable that will be used.
 * @return true if loading or creation succeeded, false if it didn't.
 * This can return false if the name is incorrect or if the setting was already loaded with another variable.
 */
bool catloadint(char* name, int* var){
    CATLOADANDSETTYPE(setting, name, var, 'i');
    *var = atoi((char*)(setting->value));
    free(setting->value);
    setting->value = var;
    return true;
}

template <typename t>
bool catloadint_2(char* name, t* var){
    CATLOADANDSETTYPE(setting, name, var, 'i');
    *var = (t)atoll((char*)(setting->value));
    free(setting->value);
    setting->value = var;
    return true;
}

/**
 * Load or create a double setting.
 * @param name The name of the setting to load or create
 * @param var Pointer to the variable that will be used.
 * @return true if loading or creation succeeded, false if it didn't.
 * This can return false if the name is incorrect or if the setting was already loaded with another variable.
 */
bool catloaddouble(char* name, double* var){
    CATLOADANDSETTYPE(setting, name, var, 'd');
    *var = atof((char*)(setting->value));
    free(setting->value);
    setting->value = var;
    return true;
}

/**
 * Load or create a float setting.
 * @param name The name of the setting to load or create
 * @param var Pointer to the variable that will be used.
 * @return true if loading or creation succeeded, false if it didn't.
 * This can return false if the name is incorrect or if the setting was already loaded with another variable.
 */
bool catloadfloat(char* name, float* var){
    CATLOADANDSETTYPE(setting, name, var, 'f');
    *var = (float)(atof((char*)(setting->value)));
    free(setting->value);
    setting->value = var;
    return true;
}

/**
 * Load or create a boolean setting.
 * @param name The name of the setting to load or create
 * @param var Pointer to the variable that will be used.
 * @return true if loading or creation succeeded, false if it didn't. 
 * This can return false if the name is incorrect or if the setting was already loaded.
 */
bool catloadbool(char* name, bool* var){
    CATLOADANDSETTYPE(setting, name, var, 'b');
    *var = (strcasecmp((char*)(setting->value), "false") != 0);
    free(setting->value);
    setting->value = var;
    return true;
}

/**
 * Load or create a string setting.
 * @param name The name of the setting to load or create
 * @param var Pointer to the variable that will be used.
 * @return true if loading or creation succeeded, false if it didn't.
 * This can return false if the name is incorrect or if the setting was already loaded.
 * @warning The returned string is allocated using malloc and must be freed by the user after saving.
 */
bool catloadstring(char* name, char** var){
    CATLOADANDSETTYPE(setting, name, var, 's');
    *var = (char*) malloc((strlen((char*)(setting->value)) + 1) * sizeof(char));
    strcpy(*var, (char*)(setting->value));
    free(setting->value);
    setting->value = var;
    return true;
}