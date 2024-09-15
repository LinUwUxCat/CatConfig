# CatConfig
meow!

A config library for C++.
## Features
Designed to avoid a repetition issue with libconfig, CatConfig loads the entire config file at first and saves to it with a single command, without needing to specify which variables you want to save.

Additionally, CatConfig will save all valid entries that were read, even if you did not load all of them. As an example, if your config file looks like this:
```
settingOne=123
settingTwo=456
```
but you're only loading `settingOne` in code, `settingTwo` will get saved anyways so you can reuse it later (you can choose to not save unloaded settings, but this isn't the default).

Example of CatConfig code to load a config file, register an integer and a boolean, and save them to the file:
```c
catinit("test.catcfg"); // Initialization and reading config - if the file does not exist, make an empty config
catloadint("myInt", &myInt); // Loads an integer, creates the setting if it doesn't exist
catloadbool("myBool", &myBool); // Loads a boolean, creates the setting if it doesn't exist
/*
    Your program here ....
                            */
catsave("test.catcfg"); // Save config 
catexit(); // Exit
```
This code skips initializing variables and setting the default value, which only has to be done once before `catloadthing`.
Example of corresponding libconfig code:
```c
config_init(&config); // Initialisation
config_read_file(&config, "file.conf"); // Reading config file

config_lookup_int(&config, "myInt", &myInt); // Load an integer, if it exists
config_lookup_bool(&config, "myBool", (int*)&myBool); // Load a boolean, if it exists

root = config_root_setting(&config); // A "root" object, created by the user, is needed 
/* 
    Your program here .... 
                            */
setting = config_setting_get_member(root,"myInt"); // Get the integer setting 
if(setting==NULL) setting = config_setting_add(root, "myInt", CONFIG_TYPE_INT); // Create the integer as a setting, if it doesn't exist
config_setting_set_int(setting, myInt); // Save the integer to the config object
setting = config_setting_get_member(root,"myBool"); // Get the boolean setting
if(setting==NULL) setting = config_setting_add(root, "myBool", CONFIG_TYPE_BOOL); // Create the boolean as a setting, if it doesn't exist
config_setting_set_bool(setting, myBool); // Save the boolean to the config object

config_write_file(&config, "file.conf"); // Writing to config file
config_destroy(&config); // Destroy and exit
```
And I skipped initializing variables, setting default values if `config_read_file` or `config_lookup_thing` fails, etc.

In the end, this makes CatConfig way more efficient on the developer's side of things.

## Limitations
- Settings cannot have the characters `=`, `\n` or `\r` in them.
- Values cannot have the characters `\n` or `\r` in them