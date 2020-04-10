#include "process.hpp"

using namespace std;

string do_readlink(string &path)
{
    char buff[PATH_MAX];
    ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff) - 1);
    if (len != -1)
    {
        buff[len] = '\0';
        return string(buff);
    }

    return "";
}

void fetch_name_and_args(ProcessBasicInfo &pbi, string &pid_str)
{
    string line;
    string filename = "/proc/" + pid_str + "/cmdline";
    ifstream myfile(filename);

    if (!myfile.is_open())
    {
        return;
    }

    getline(myfile, line);
    myfile.close();

    int start = 0, end = 0;
    for (auto c : line)
    {
        if (c == '\0')
        {
            if (start == 0)
                pbi.name = line.substr(start, end);
            else
                pbi.args.push_back(line.substr(start, end));

            start = end;
        }
        else
            end++;
    }
}

void fetch_visible_mountpoints(ProcessBasicInfo &pbi, string &pid_str)
{
    string line;
    string filename = "/proc/" + pid_str + "/mounts";
    ifstream myfile(filename);

    if (!myfile.is_open())
    {
        return;
    }

    while (getline(myfile, line))
    {
        pbi.mountpoints.push_back(line + '\n');
    }

    myfile.close();
}

/**
 * @see http://man7.org/linux/man-pages/man5/proc.5.html
 */
void fetch_open_fds(ProcessBasicInfo &pbi, string &pid_str)
{
    string path = "/proc/" + pid_str + "/fd/";

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            string p = path + string(ent->d_name);

            char buff[PATH_MAX];
            ssize_t len = ::readlink(p.c_str(), buff, sizeof(buff) - 1);
            if (len != -1)
            {
                buff[len] = '\0';
                string s = buff;
                pbi.fds.push_back(s);
            }
        }
        closedir(dir);
    }
}

/**
 * Fetch Environment variables
 */
void fetch_environ(ProcessBasicInfo &pbi, string &pid_str)
{
    string filename = "/proc/" + pid_str + "/environ";
    ifstream myfile(filename);

    if (!myfile.is_open())
    {
        return;
    }

    string line;
    int end;
    while (getline(myfile, line, '\0'))
    {
        end = line.find('=', 0);
        if (end < 0)
        {
            continue;
        }

        pbi.environment.insert(pair<string, string>(line.substr(0, end), line.substr(end + 1)));
    }

    myfile.close();
}

void fetch_status(ProcessBasicInfo &pbi, string &pid_str)
{
    string filename = "/proc/" + pid_str + "/status";
    ifstream myfile(filename);

    if (!myfile.is_open())
    {
        return;
    }

    string tmp;
    vector<string> args;
    while (getline(myfile, tmp))
        args.push_back(tmp.substr(tmp.find('\t') + 1));

    // Process IDs
    pbi.state = args[2][0];
    pbi.tgid = stoi(args[3]);
    pbi.ngid = stoi(args[4]);
    pbi.pid = stoi(args[5]);
    pbi.ppid = stoi(args[6]);
    pbi.tracer_pid = stoi(args[7]);

    auto pushToVec = [&tmp](string str, vector<int> &container) {
        stringstream X(str);
        while (getline(X, tmp, '\t'))
        {
            container.push_back(stoi(tmp));
        }
    };

    pushToVec(args[12], pbi.NStgid);
    pushToVec(args[13], pbi.NSpid);
    pushToVec(args[14], pbi.NSpgid);
    pushToVec(args[15], pbi.NSsid);

    // User IDs, Group IDs
    vector<int> uids;
    pushToVec(args[8], uids);
    pbi.real_uid = uids[0];
    pbi.effective_uid = uids[1];
    pbi.saved_set_uid = uids[2];
    pbi.filesystem_uid = uids[3];

    vector<int> gids;
    pushToVec(args[9], gids);
    pbi.real_gid = gids[0];
    pbi.effective_gid = gids[1];
    pbi.saved_set_gid = gids[2];
    pbi.filesystem_gid = gids[3];

    // FD Size
    pbi.fdsize = stoi(args[10]);

    // Groups
    stringstream X(args[11]);
    while (getline(X, tmp, ' '))
    {
        pbi.group_ids.push_back(stoi(tmp));
    }

    auto toBytes = [&](string size) {
        auto pos = size.find_last_of(' ');
        auto unit = size.substr(pos+1);

        auto rez = stoul(size);
        size_t mul = 1;
        if(unit == "kB")
            mul = 1024;
        else if(unit == "mB")
            mul = 1024 * 1024;
        else if(unit == "gB")
            mul = 1024 * 1024 * 1024;
        
        return rez * mul;
    };

    // Memory
    pbi.VmPeak = toBytes(args[16]);
    pbi.VmSize = toBytes(args[17]);
    pbi.VmLck = toBytes(args[18]);
    pbi.VmPin = toBytes(args[19]);
    pbi.VmHWM = toBytes(args[20]);
    pbi.VmRSS = toBytes(args[21]);
    pbi.RssAnon = toBytes(args[22]);
    pbi.RssFile = toBytes(args[23]);
    pbi.RssShmem = toBytes(args[24]);
    pbi.VmData = toBytes(args[25]);
    pbi.VmStk = toBytes(args[26]);
    pbi.VmExe = toBytes(args[27]);
    pbi.VmLib = toBytes(args[28]);
    pbi.VmPTE = toBytes(args[29]);
    pbi.VmSwap = toBytes(args[30]);
    pbi.HugeTLBPages = toBytes(args[31]);

    // Misc.
    pbi.isCoreDumping = bool(stoi(args[32]));
    pbi.threads = stoul(args[33]);

    //Signals
    auto p = args[34].find('/');
    pbi.queued_signals = stoul(args[34].substr(0, p));
    pbi.signals_limit = stoul(args[34].substr(p+1));

    pbi.SigPnd = stoi(args[35], 0, 16);
    pbi.ShdPnd = stoi(args[36], 0, 16);
    pbi.SigBlk = stoi(args[37], 0, 16);
}

void fetch_limits(ProcessBasicInfo &pbi, string &pid_str)
{
    pid_t pid = stoi(pid_str);

    for (uint i = 0; i < RLIM_NLIMITS; i++)
    {
        struct rlimit curr_limit;
        auto r = prlimit(pid, (__rlimit_resource)i, nullptr, &curr_limit);

        struct Limit limit;

        switch (i)
        {
        case RLIMIT_CPU:
            limit.limit = "Max CPU time";
            limit.unit = "secs";
            break;
        case RLIMIT_FSIZE:
            limit.limit = "Largest file that can be created";
            limit.unit = "bytes";
            break;
        case RLIMIT_DATA:
            limit = {"Max size of data segment", 0, 0, "bytes"};
            break;
        case RLIMIT_STACK:
            limit = {"Max size of stack segment", 0, 0, "bytes"};
            break;
        case RLIMIT_CORE:
            limit = {"Max core file size", 0, 0, "bytes"};
            break;
        case RLIMIT_RSS:
            limit = {"Max resident set size", 0, 0, "bytes"};
            break;
        case RLIMIT_NOFILE:
            limit = {"Max number of open files", 0, 0, "files"};
            break;
        case RLIMIT_AS:
            limit = {"Max address space", 0, 0, "bytes"};
            break;
        case RLIMIT_NPROC:
            limit = {"Max number of processes", 0, 0, "processes"};
            break;
        case RLIMIT_MEMLOCK:
            limit = {"Locked-in-memory address space", 0, 0, ""};
            break;
        case RLIMIT_LOCKS:
            limit = {"Max file locks", 0, 0, "locks"};
            break;
        case RLIMIT_SIGPENDING:
            limit = {"Max number of pending signals", 0, 0, "signals"};
            break;
        case RLIMIT_MSGQUEUE:
            limit = {"Max bytes in POSIX message queues", 0, 0, "bytes"};
            break;
        case RLIMIT_NICE:
            limit = {"Max nice priority allowed", 0, 0, ""};
            break;
        case RLIMIT_RTPRIO:
            limit = {"Max realtime priority allowed", 0, 0, ""};
            break;
        case RLIMIT_RTTIME:
            limit = {"Max realtime timeout", 0, 0, "us"};
            break;
        default:
            break;
        }

        limit.rlim = curr_limit;
        pbi.limits.push_back(limit);
    }
}

void fetch_timers(ProcessBasicInfo &pbi, string &pid_str)
{
    // @see:  http://man7.org/linux/man-pages/man5/proc.5.html. Find '/proc/[pid]/timers'

    ifstream myfile("/proc/" + pid_str + "/timers");
    if (!myfile.is_open())
    {
        return;
    }

    string line;
    while (getline(myfile, line))
    {
        // Ignore reading the first line. It is the timer Id
        string signal, notice, method, clockid;
        size_t start, slash, dot;

        // Get the signal number
        getline(myfile, line);
        start = line.find(' ') + 1;
        slash = line.find('/');
        signal = line.substr(start, slash - start);

        // Get the nofication method
        getline(myfile, line);
        start = line.find(' ') + 1;
        slash = line.find('/');
        dot = line.find('.');
        notice = line.substr(start, slash - start);
        method = line.substr(slash + 1, slash - dot);

        // Get the clock id
        getline(myfile, line);
        start = line.find(' ') + 1;
        clockid = line.substr(start, line.length());

        TimerSignal t;
        t.signal = "(" + signal + ") " + strsignal(atoi(signal.c_str()));
        t.notification = notice;
        t.method = method;

        switch (atoi(clockid.c_str()))
        {
        case CLOCK_BOOTTIME:
            t.clock = "CLOCK_BOOTTIME";
            break;
        case CLOCK_BOOTTIME_ALARM:
            t.clock = "CLOCK_BOOTTIME_ALARM";
            break;
        case CLOCK_MONOTONIC:
            t.clock = "CLOCK_MONOTONIC";
            break;
        case CLOCK_MONOTONIC_COARSE:
            t.clock = "CLOCK_MONOTONIC_COARSE";
            break;
        case CLOCK_MONOTONIC_RAW:
            t.clock = "CLOCK_MONOTONIC_RAW";
            break;
        case CLOCK_PROCESS_CPUTIME_ID:
            t.clock = "CLOCK_PROCESS_CPUTIME_ID";
            break;
        case CLOCK_REALTIME:
            t.clock = "CLOCK_REALTIME";
            break;
        case CLOCK_REALTIME_ALARM:
            t.clock = "CLOCK_REALTIME_ALARM";
            break;
        case CLOCK_REALTIME_COARSE:
            t.clock = "CLOCK_REALTIME_COARSE";
            break;
        case CLOCK_TAI:
            t.clock = "CLOCK_TAI";
            break;
        case CLOCK_THREAD_CPUTIME_ID:
            t.clock = "CLOCK_THREAD_CPUTIME_ID";
            break;
        default:
            break;
        }

        pbi.timers.push_back(t);
    }

    myfile.close();
}