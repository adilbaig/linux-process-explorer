#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
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

    vector<string> mountpoints;
};

string do_readlink(string &path);

void fetch_name_and_args(ProcessBasicInfo &pbi, string &pid_str);

void fetch_visible_mountpoints(ProcessBasicInfo &pbi, string &pid_str);

void fetch_open_fds(ProcessBasicInfo &pbi, string &pid_str);