#include <string>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include "lpe/process.hpp"

using namespace std;

string vIntToStr(vector<int> vi)
{
    stringstream joinedValues;
    for (const auto &v : vi)
        joinedValues << v << ",";
    return joinedValues.str().substr(0, joinedValues.str().size() - 1);
}

void print_signalsv(vector<int> signals)
{
    const vector<string> sig_str = {
        "SIGHUP",
        "SIGINT",
        "SIGQUIT",
        "SIGILL",
        "SIGTRAP",
        "SIGABRT",
        "SIGBUS",
        "SIGFPE",
        "SIGKILL",
        "SIGUSR1",
        "SIGSEGV",
        "SIGUSR2",
        "SIGPIPE",
        "SIGALRM",
        "SIGTERM",
        "SIGSTKFLT",
        "SIGCHLD",
        "SIGCONT",
        "SIGSTOP",
        "SIGTSTP",
        "SIGTTIN",
        "SIGTTOU",
        "SIGURG",
        "SIGXCPU",
        "SIGXFSZ",
        "SIGVTALRM",
        "SIGPROF",
        "SIGWINCH",
        "SIGIO",
        "SIGPWR",
        "SIGSYS",
        "SIGRTMIN",
        "SIGRTMIN+1",
        "SIGRTMIN+2",
        "SIGRTMIN+3",
        "SIGRTMIN+4",
        "SIGRTMIN+5",
        "SIGRTMIN+6",
        "SIGRTMIN+7",
        "SIGRTMIN+8",
        "SIGRTMIN+9",
        "SIGRTMIN+10",
        "SIGRTMIN+11",
        "SIGRTMIN+12",
        "SIGRTMIN+13",
        "SIGRTMIN+14",
        "SIGRTMIN+15",
        "SIGRTMAX-14",
        "SIGRTMAX-13",
        "SIGRTMAX-12",
        "SIGRTMAX-11",
        "SIGRTMAX-10",
        "SIGRTMAX-9",
        "SIGRTMAX-8",
        "SIGRTMAX-7",
        "SIGRTMAX-6",
        "SIGRTMAX-5",
        "SIGRTMAX-4",
        "SIGRTMAX-3",
        "SIGRTMAX-2",
        "SIGRTMAX-1",
        "SIGRTMAX"};

    for (auto &&signum : signals)
    {
        cout << sig_str[signum] << ",";
    }

    cout << endl;
}

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
    auto myroot = "/proc/" + pid_str + "/root";
    pbi.exe = do_readlink(exe);
    pbi.cwd = do_readlink(cwd);
    pbi.root = do_readlink(myroot);

    fetch_visible_mountpoints(pbi, pid_str);
    fetch_open_fds(pbi, pid_str);
    fetch_environ(pbi, pid_str);
    fetch_status(pbi, pid_str);

    cout << "Program: " << pbi.name << endl;
    cout << "Args: ";
    for (auto &arg : pbi.args)
    {
        cout << arg;
    }
    cout << endl;

    cout << "CWD: " << pbi.cwd << endl;
    cout << "EXE: " << pbi.exe << endl;
    cout << "Root (Containerized): " << pbi.root << endl;

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

    cout << "Process IDs " << endl
         << "================" << endl;
    cout << "State: " << pbi.state << endl;
    cout << "PID: " << pbi.pid << endl;
    cout << "Parent PID: " << pbi.ppid << endl;
    cout << "Thread Group ID: " << pbi.tgid << endl;
    cout << "Numa Group ID: " << pbi.ngid << endl;
    cout << "Tracer PID: " << pbi.tracer_pid << endl;

    cout << "NameSpace Thread Group ID: " << vIntToStr(pbi.NStgid) << endl;
    cout << "NameSpace PID: " << vIntToStr(pbi.NSpid) << endl;
    cout << "NameSpace Parent Group ID: " << vIntToStr(pbi.NSpgid) << endl;
    cout << "NameSpace Session ID: " << vIntToStr(pbi.NSsid) << endl;

    printf("Real, effective, saved set, and filesystem UIDs: %d, %d, %d, %d\n", pbi.real_uid, pbi.effective_uid, pbi.saved_set_uid, pbi.filesystem_uid);
    printf("Real, effective, saved set, and filesystem GIDs: %d, %d, %d, %d\n", pbi.real_gid, pbi.effective_gid, pbi.saved_set_gid, pbi.filesystem_gid);

    cout << "FDSize: " << pbi.fdsize << endl;

    cout << "Groups: " << vIntToStr(pbi.group_ids) << endl;

    printf("VM Peak, Current, Locked, Pinned: %lu, %lu, %lu, %lu kb\n", pbi.VmPeak / 1024, pbi.VmSize / 1024, pbi.VmLck / 1024, pbi.VmPin / 1024);
    printf("RSS Peak, Current, Anon, File, Shmem: %lu, %lu, %lu, %lu, %lu kb\n", pbi.VmHWM / 1024, pbi.VmRSS / 1024, pbi.RssAnon / 1024, pbi.RssFile / 1024, pbi.RssShmem / 1024);
    printf("Memory Segments Data, Stack, Text, Shared Libs : %lu, %lu, %lu, %lu kb\n", pbi.VmData / 1024, pbi.VmStk / 1024, pbi.VmExe / 1024, pbi.VmLib / 1024);
    printf("Memory Page Table Entries Size : %lu kb\n", pbi.VmPTE / 1024);
    printf("Swapped Out VM Size : %lu kb\n", pbi.VmSwap / 1024);
    printf("HugeTLB Size : %lu kb\n", pbi.HugeTLBPages / 1024);

    cout << "Core Dumping? " << pbi.isCoreDumping << endl;
    cout << "Active Threads: " << pbi.threads << endl;

    cout << "Signals Queued: " << pbi.queued_signals << '/' << pbi.signals_limit << endl;
    cout << "Signals Pending Thread: ";
    print_signalsv(pbi.SigPnd);
    cout << "Signals Pending Process: ";
    print_signalsv(pbi.ShdPnd);
    cout << "Signals Blocked: ";
    print_signalsv(pbi.SigBlk);
    cout << "Signals Ignored: ";
    print_signalsv(pbi.SigIgn);
    cout << "Signals Caught: ";
    print_signalsv(pbi.SigCgt);

    cout << "List of CPUs Allowed (cpuset): " << pbi.Cpus_allowed << endl;
    cout << "List of Memory Banks Allowed (cpuset): " << pbi.Mems_allowed << endl;

    cout << "Context Switches (Voluntary,Involuntary): " << pbi.voluntary_ctxt_switches << "," << pbi.nonvoluntary_ctxt_switches << endl;

    return 0;
}
