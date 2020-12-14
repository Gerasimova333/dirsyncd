# dirsyncd
Solution for directory synchronization

# Build
g++ -std=c++17 dirsyncd.cpp -o dirsyncd

# Usage
dirsyncd <name of config file>

# Config file structure
Each line in config file is path to target directory.

Example:
```
/Users/user/raid/dump1
/Users/user/raid/dump2
/Users/user/raid/dump3
```
