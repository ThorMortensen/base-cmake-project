# Rovsing Base CMake Project

This is the base skeleton project for working with CMake. 

Please fork and rename this project when a new project is needed. 
Go through the skeleton and change files marked with `*_CHANGE_ME`

This project should work with all editors and standalone.
(It's been tested with CLion IDE and VSCode and I suggest you use one of them)

Please remember to update this readme to fit you new project.

# About 
This project-stile aspires to simplify and standardize the CMake projects.
To simplify the project we have added as much automation as possible.

***Simplify***
All files are automatically added to the project so no need to keep track of files
and renaming when not using an IDE.

Project name are taken from the folder name and git tags are added automatically.


***Standardize***
Use git submodules for internal library's. We have made it as painless as possible to add git submodules to
these projects so don't hesitate to spin out code to submodules.

All git submodules are added, pulled and updated to master --> head when running the `-DGIT_UP=ON` option.


# Structure

The project structure is as follows specific:
```
CMakeList.txt <ROOT-PROJECT> (this folder)
    |
    + CMakeList.txt <PROJECT> (project specific code)
    |
    + CMakeList.txt <Submodule 1> 
    + CMakeList.txt <Submodule 2> 
    + CMakeList.txt <Submodule ...> 
```
**_Responsibilities_** 

- ROOT-PROJECT
  - Global project settings like c++ version, compile flags, cmake options.
  - Glue project and library's together.
  - Run tests and sanitizers etc.
- PROJECT
  - Project specific code
  - Installer
- Submodules 
  - Library specific code.
  - Code generation etc.

All these components should be Plug&Play with minimal need for editing any CMakeList.txt

### Usage 

To add a submodule library add the submodule in git and add the folder
name in the root CMakeList.txt file at the bottom of the file under "Project Composition".
```
########################
#  Project Composition
########################
# library's
add_subdirectory(cmake_rovcommon) # <--- Add libs here
add_subdirectory(rov_protobuf)
```

The executable is currently set to the project name (auto folder name) but can be et manually

```
# Executables
add_subdirectory(${PROJECT_NAME})  # <--- Add exe here
```

# Executables
add_subdirectory(PROJECT_NAME_CHANGE_ME)

## Development environment

### Clion
Install Clion: https://www.jetbrains.com/clion/
Setup remote: https://blog.jetbrains.com/clion/2018/09/initial-remote-dev-support-clion/

Connect to Pi and enjoy :-)

###VS-Code
Install VS code: https://code.visualstudio.com/.
Install the following extensions

austin.code-gnu-global
cschlosser.doxdocgen
DavidAnson.vscode-markdownlint
ms-vscode-remote.remote-ssh
ms-vscode-remote.remote-ssh-edit
ms-vscode-remote.remote-wsl
ms-vscode.cmake-tools
ms-vscode.cpptools
twxs.cmake
xaver.clang-format

Setup remote: https://code.visualstudio.com/docs/remote/remote-overview

Connect to Pi and remember to also install the extensions on the Pi through the remote client.

Enjoy :-)

--------------------
### Submodules
This project use git-submodules. 
Some basic submodules you're probably going to need has been added to this base project.

To download the submodules go to the project root folder and run:

If embedded-toolbox is installed use the update script:
```
rov_up 
```
Else run this command:
```
git pull && git submodule update --init --recursive && git submodule foreach --recursive git checkout master && git submodule foreach --recursive git pull
```

### Dependencies
Install dependencies 
```
sudo apt install <Add OS dependencies here>
```

### Pi network setup

Create this file:
```
sudo nano /etc/network/interfaces.d/eth0_0
```

Write this in the file:
```
auto eth0:0
allow-hotplug eth0:0
iface eth0:0 inet static
    address 192.168.xxx.xxx
    netmask 255.255.0.0
    gateway 192.168.255.1
``


