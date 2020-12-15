#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <algorithm>

using namespace std;

namespace fs = filesystem;
using LinesT = vector<string>;
using PathsT = vector<fs::path>;

class Config 
{
public:

    Config(const string& cfgpath)
        : configPath(cfgpath) { }

    LinesT data()
    {
        ifstream fConfig(configPath);
        LinesT rv;
        string line;
        if (fConfig.is_open()) {
            while (getline(fConfig, line))
                rv.push_back(line);
        }
        return rv;
    }

private:
    string configPath;
};

class DirSynchronizer
{
public:
    DirSynchronizer(const PathsT& dirs)
        : dirs(dirs)
    {
        for (size_t i = 1; i < dirs.size(); ++i) {
            recursiveCopyDir(dirs[0], dirs[i]);
        }
        lastChangeTime = lastChangeTimeFromDir(dirs.back());
    }

    void sync()
    {
        for (auto& target: dirs) {
            if (lastChangeTimeFromDir(target) > lastChangeTime) {
                for (auto& dir : dirs) {
                    if (dir != target)
                    {
                        recursiveCopyDir(target, dir);
                        lastChangeTime = lastChangeTimeFromDir(dir);
                    }
                }
                break;
            }
        }
    }

private:
    void recursiveCopyDir(const fs::path& src, const fs::path& dst) {
        fs::remove_all(dst);
        fs::create_directory(dst);
        fs::copy(src, dst, fs::copy_options::copy_symlinks);
    }

    fs::file_time_type lastChangeTimeFromDir(const fs::path& dir) {
        fs::file_time_type rv = fs::last_write_time(dir);
        for (auto& path: fs::recursive_directory_iterator(dir))
            rv = max(rv, fs::last_write_time(path));
        return rv;
    }

    PathsT dirs;
    fs::file_time_type lastChangeTime;
};

static void printUsage() {
    cout << "Usage:" << endl
    << "    dirsyncd <config-file path>" << endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage();
        return 0;
    }
    LinesT dirs = Config(argv[1]).data();
    DirSynchronizer synchronizer(PathsT(dirs.begin(), dirs.end()));
    while (true) {
        synchronizer.sync();       
    }
    return 0;
}
