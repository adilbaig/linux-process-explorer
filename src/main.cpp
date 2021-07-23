#include <string>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include <thread>
#include <chrono>
// #include "lpe/process.hpp"
#include "window/window.hpp"
#include "ext/nlohmann/json-v3.9.1.hpp"
#include "ext/cxxopts.hpp"

using namespace std;

ProcessBasicInfo pbi = {};
// This declaration compiles, but app fails on startup with:
// _gtk_style_provider_private_get_settings: assertion 'GTK_IS_STYLE_PROVIDER_PRIVATE (provider)' failed
// MainWindow window;

// Setting a ptr works for a start. But when i 'interact' with the table, the next update segfaults.
// I printed the addresses, they hadn't changed. So maybe the internal reference to the window had moved.
// MainWindow* window_ptr;
const int thread_timer_ms = 2000;
const string START_WEBSERVER = "python3 -m http.server --bind=localhost --directory=web &";
const string LAUNCH_BROWSER = "xdg-open http://127.0.0.1:8000/";

string vIntToStr(vector<int> vi)
{
    stringstream joinedValues;
    for (const auto &v : vi)
        joinedValues << v << ",";
    return joinedValues.str().substr(0, joinedValues.str().size() - 1);
}

void print_signalsv(vector<int> signals)
{
    if (!signals.size())
    {
        cout << "-" << endl;
        return;
    }

    const vector<string> sig_str = {
        "__IGNORE_PLACEHOLDER__", // Because signals start at 1, not 0
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
        if (signum < sig_str.size())
            cout << sig_str[signum] << ",";
    }

    cout << endl;
}

void print_capabilitiesv(vector<int> caps)
{
    if (!caps.size())
    {
        cout << "-" << endl;
        return;
    }

    const vector<string> cap_str{
        "CAP_CHOWN",
        "CAP_DAC_OVERRIDE",
        "CAP_DAC_READ_SEARCH",
        "CAP_FOWNER",
        "CAP_FSETID",
        "CAP_KILL",
        "CAP_SETGID",
        "CAP_SETUID",
        "CAP_SETPCAP",
        "CAP_LINUX_IMMUTABLE",
        "CAP_NET_BIND_SERVICE",
        "CAP_NET_BROADCAST",
        "CAP_NET_ADMIN",
        "CAP_NET_RAW",
        "CAP_IPC_LOCK",
        "CAP_IPC_OWNER",
        "CAP_SYS_MODULE",
        "CAP_SYS_RAWIO",
        "CAP_SYS_CHROOT",
        "CAP_SYS_PTRACE",
        "CAP_SYS_PACCT",
        "CAP_SYS_ADMIN",
        "CAP_SYS_BOOT",
        "CAP_SYS_NICE",
        "CAP_SYS_RESOURCE",
        "CAP_SYS_TIME",
        "CAP_SYS_TTY_CONFIG",
        "CAP_MKNOD",
        "CAP_LEASE",
        "CAP_AUDIT_WRITE",
        "CAP_AUDIT_CONTROL",
        "CAP_SETFCAP",
        "CAP_MAC_OVERRIDE",
        "CAP_MAC_ADMIN",
        "CAP_SYSLOG",
        "CAP_WAKE_ALARM",
        "CAP_BLOCK_SUSPEND",
        "CAP_AUDIT_READ"};

    for (auto &&cap : caps)
    {
        if (cap < cap_str.size())
            cout << cap_str[cap] << ",";
    }

    cout << endl;
}

void bg_fetch(string pid_str)
{
    while (true)
    {
        fetch_open_fds(pbi, pid_str);
        this_thread::sleep_for(chrono::milliseconds(thread_timer_ms));
    }
}

void write_json(ProcessBasicInfo &pbi)
{
    // https://github.com/nlohmann/json#examples
    nlohmann::json j;

    j["exe"] = pbi.name;
    j["cwd"] = pbi.cwd;
    j["env"] = pbi.environment;

    // Save it to a file.
    std::ofstream o("web/response.json");
    o << j.dump(4) << std::endl;
}

void write_console(ProcessBasicInfo &pbi)
{
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
    for (auto const &[key, val] : pbi.fds)
    {
        cout << key << ") " << val << endl;
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

    cout << "Capabilities Inheritable: ";
    print_capabilitiesv(pbi.CapInh);
    cout << "Capabilities Permitted: ";
    print_capabilitiesv(pbi.CapInh);
    cout << "Capabilities Effective: ";
    print_capabilitiesv(pbi.CapInh);
    cout << "Capability Bounding Set: ";
    print_capabilitiesv(pbi.CapBnd);
    cout << "Capability Ambient Set: ";
    print_capabilitiesv(pbi.CapAmb);

    cout << "List of CPUs Allowed (cpuset): " << pbi.Cpus_allowed << endl;
    cout << "List of Memory Banks Allowed (cpuset): " << pbi.Mems_allowed << endl;

    cout << "Context Switches (Voluntary,Involuntary): " << pbi.voluntary_ctxt_switches << "," << pbi.nonvoluntary_ctxt_switches << endl;
}

int run_gtk(ProcessBasicInfo &pbi, std::string pid_str, char *argv[])
{
    int c = 0;
    auto app = Gtk::Application::create(c, argv, "org.gtkmm.example");
    auto title = pbi.name + " (" + pid_str + ")";

    MainWindow window(thread_timer_ms);
    window.pbi_ptr = &pbi;
    window.initialize(title);
    window.set_exe(pbi.exe);
    window.set_args(pbi.args);
    window.set_cwd(pbi.cwd);
    window.set_root(pbi.root);

    window.env_table.set_env_variables(pbi.environment);
    window.mountpoint_table.set_mount_points(pbi.mountpoints);
    window.lm_table.set_limits(pbi.limits);
    window.tm_table.set_timers(pbi.timers);

    // Put all Process Ids (Group, Thread, User etc.) in one map
    map<string, string> ids;
    ids.emplace("PID", to_string(pbi.pid));
    ids.emplace("Parent PID", to_string(pbi.ppid));
    ids.emplace("Thread Group ID", to_string(pbi.tgid));
    ids.emplace("Numa Group ID", to_string(pbi.ngid));
    ids.emplace("Tracer PID", to_string(pbi.tracer_pid));
    ids.emplace("NameSpace Thread Group ID", vIntToStr(pbi.NStgid));
    ids.emplace("NameSpace PID", vIntToStr(pbi.NSpid));
    ids.emplace("NameSpace Parent Group ID", vIntToStr(pbi.NSpgid));
    ids.emplace("NameSpace Session ID", vIntToStr(pbi.NSsid));
    ids.emplace("Real UID", to_string(pbi.real_uid));
    ids.emplace("Effective UID", to_string(pbi.effective_uid));
    ids.emplace("Saved Set UID", to_string(pbi.saved_set_uid));
    ids.emplace("FileSystem UID", to_string(pbi.filesystem_uid));
    ids.emplace("Real GID", to_string(pbi.real_gid));
    ids.emplace("Effective GID", to_string(pbi.effective_gid));
    ids.emplace("Saved Set GID", to_string(pbi.saved_set_gid));
    ids.emplace("FileSystem GID", to_string(pbi.filesystem_gid));

    window.id_table.set_ids(ids);

    // Start a background thread
    // This updates the `ProcessBasicInfo` struct with new information
    thread t1(bg_fetch, pid_str);
    t1.detach();

    return app->run(window);
}

int main(int argc, char *argv[])
{
    cxxopts::Options options("Linux Process Explorer", "A tool to review Linux process internal data as exposed by procfs");

    options.add_options()
    ("w,web", "Open in browser", cxxopts::value<bool>()->default_value("false"))
    ("g,gui", "Open in GTK", cxxopts::value<bool>()->default_value("false"))
    ("h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help") || argc < 2)
    {
        cout << "USAGE: " << argv[0] << " PID" << endl;
        cout << options.help() << endl;
        exit(0);
    }

    bool web = result["web"].as<bool>();
    bool gui = result["gui"].as<bool>();

    auto pid_str = string(argv[1]);
    int pid = stoi(pid_str);

    // https://stackoverflow.com/questions/9152979/check-if-process-exists-given-its-pid
    if (getpgid(pid) < 0)
    {
        cerr << "PID is not valid" << '\n';
        return 1;
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

    if (web)
    {
        write_json(pbi);

        system(START_WEBSERVER.c_str());
        system(LAUNCH_BROWSER.c_str());
    }
    else if (gui)
    {
        return run_gtk(pbi, pid_str, argv);
    }
    else
    {
        write_console(pbi);
    }

    return 0;
}