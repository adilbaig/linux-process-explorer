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
#include <signal.h>
#include <linux/capability.h>
#include <sys/resource.h>

using namespace std;

// Represents a ulimit
struct Limit
{
    string limit;
    rlimit rlim;
    string unit;
};

// Represents an active timer
// @see http://man7.org/linux/man-pages/man2/timer_create.2.html
struct TimerSignal
{
    string signal;
    string notification;
    string method;
    string clock; // One of CLOCK_* in <time.h>
};

struct ProcessBasicInfo
{
    string name;
    vector<string> args;
    string exe;
    string cwd;
    string root;

    vector<string> mountpoints;
    map<int, string> fds;
    map<string, string> environment;

    vector<Limit> limits;
    vector<TimerSignal> timers;

    ssize_t umask;
    char state;
    int tgid, ngid, pid, ppid, tracer_pid;

    int real_uid, effective_uid, saved_set_uid, filesystem_uid;
    int real_gid, effective_gid, saved_set_gid, filesystem_gid;
    int fdsize;

    vector<string> groups;
    vector<int> group_ids;
    vector<int> NStgid, NSpid, NSpgid, NSsid;

    // Memory
    size_t VmPeak, VmSize, VmLck, VmPin, VmHWM, VmRSS;
    size_t RssAnon, RssFile, RssShmem;
    size_t VmData, VmStk, VmExe, VmLib, VmPTE, VmPMD;
    size_t VmSwap;
    size_t HugeTLBPages;
    bool isCoreDumping;
    size_t threads;

    // Signals
    uint queued_signals, signals_limit;
    vector<int> SigPnd, ShdPnd, SigBlk, SigIgn, SigCgt;

    // Capabilities
    vector<int> CapInh, CapPrm, CapEff;
    vector<int> CapBnd, CapAmb;
    bool NoNewPrivs;
    int Seccomp;
    bool Speculation_Store_Bypass;

    // CPU & Mem alowed (cpuset)
    string Cpus_allowed;
    string Mems_allowed;

    // Other Stats
    size_t voluntary_ctxt_switches, nonvoluntary_ctxt_switches;
};

string do_readlink(string &path);

void fetch_name_and_args(ProcessBasicInfo &pbi, string &pid_str);

void fetch_visible_mountpoints(ProcessBasicInfo &pbi, string &pid_str);

void fetch_open_fds(ProcessBasicInfo &pbi, string &pid_str);

void fetch_environ(ProcessBasicInfo &pbi, string &pid_str);

void fetch_stats(ProcessBasicInfo &pbi, string &pid_str);

void fetch_limits(ProcessBasicInfo &pbi, string &pid_str);

void fetch_timers(ProcessBasicInfo &pbi, string &pid_str);

void fetch_status(ProcessBasicInfo &pbi, string &pid_str);