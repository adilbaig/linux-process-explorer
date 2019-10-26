#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <unistd.h>
#include <limits.h>
#include <algorithm>
#include <dirent.h>

using namespace std;

struct ProcessBasicInfo
{
    string name;
    vector<string> args;
    string exe;
    string cwd;
    string root;

    vector<string> mountpoints;
    vector<string> fds;
    map<string, string> environment;
};

string do_readlink(string &path);

void fetch_name_and_args(ProcessBasicInfo &pbi, string &pid_str);

void fetch_visible_mountpoints(ProcessBasicInfo &pbi, string &pid_str);

void fetch_open_fds(ProcessBasicInfo &pbi, string &pid_str);

void fetch_environ(ProcessBasicInfo &pbi, string &pid_str);