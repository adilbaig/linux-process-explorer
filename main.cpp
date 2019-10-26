#include <string>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include "lpe/process.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "USAGE: " << argv[0] << " PID" << endl;
        return 1;
    }

    auto pid_str = string(argv[1]);
    int pid = stoi(pid_str);
    if (pid <= 0)
    {
        return -1;
    }

    // https://stackoverflow.com/questions/9152979/check-if-process-exists-given-its-pid
    if (getpgid(pid) < 0)
    {
        cerr << "PID is not valid" << '\n';
    }

    ProcessBasicInfo pbi = {};

    fetch_name_and_args(pbi, pid_str);

    auto exe = "/proc/" + pid_str + "/exe";
    auto cwd = "/proc/" + pid_str + "/cwd";
    pbi.exe = do_readlink(exe);
    pbi.cwd = do_readlink(cwd);

    fetch_visible_mountpoints(pbi, pid_str);
    fetch_open_fds(pbi, pid_str);
    fetch_environ(pbi, pid_str);

    cout << "Program: " << pbi.name << endl;
    cout << "Args: ";
    for (auto &arg : pbi.args)
    {
        cout << arg;
    }
    cout << endl;

    cout << "CWD: " << pbi.cwd << endl;
    cout << "EXE: " << pbi.exe << endl;

    cout << "Visibile Mountpoints: " << endl;
    for (auto &arg : pbi.mountpoints)
    {
        cout << arg;
    }
    cout << endl;

    cout << "Open FDs: " << endl;
    for (auto &arg : pbi.fds)
    {
        cout << arg << endl;
    }

    cout << "Environment Variables: " << endl;
    // https://stackoverflow.com/questions/26281979/c-loop-through-map
    for (auto const &[key, val] : pbi.environment)
    {
        cout << key << " = " << val << endl;
    }

    return 0;
}