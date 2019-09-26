#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <limits.h>
using namespace std;

struct ProcessBasicInfo {
    string name;
    vector<string> args;
    string exe;
    string cwd;
};

std::string do_readlink(string& path) {
    char buff[PATH_MAX];
    ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }

    return "";
}

void fetchNameAndArgs(ProcessBasicInfo& pbi, int pid) {
    string line;
    string filename = "/proc/" + to_string(pid) + "/cmdline";
    ifstream myfile (filename);

    if(!myfile.is_open()) {
        return;    
    }

    getline(myfile, line);
    myfile.close();

    int start = 0, end = 0;
    for(auto c : line) {
        if (c == '\0') {
            if (start == 0)
                pbi.name = line.substr(start, end);
            else
                pbi.args.push_back(line.substr(start, end));

            start = end;
        } else
            end++;
    }
}

int main() {

    int pid = 0;
    std::cout << "Enter PID : ";
    std::cin >> pid;
    if(pid <= 0) {
        return -1;
    }

    // https://stackoverflow.com/questions/9152979/check-if-process-exists-given-its-pid 
    if(getpgid(pid) < 0) {
        cerr << "PID is not valid" << '\n';
    }

    ProcessBasicInfo pbi = {};
    fetchNameAndArgs(pbi, pid);
    auto exe = "/proc/" + to_string(pid) + "/exe";
    auto cwd = "/proc/" + to_string(pid) + "/cwd";
    pbi.exe = do_readlink(exe);
    pbi.cwd = do_readlink(cwd);

    cout << "Program: " << pbi.name << endl;
    cout << "Args: " ;
    for (auto &arg : pbi.args) {
        cout << arg ;
    }
    cout << endl;

    cout << "CWD: " << pbi.cwd << endl;
    cout << "EXE: " << pbi.exe << endl;

    return 0;
}