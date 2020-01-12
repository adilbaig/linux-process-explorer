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

void fetch_stats(ProcessBasicInfo &pbi, string &pid_str)
{
    string filename = "/proc/" + pid_str + "/stats";
    ifstream myfile(filename);

    if (!myfile.is_open())
    {
        return;
    }

    string line;
    while (getline(myfile, line))
    {
        // Process the line into multiple structs
    }

    myfile.close();
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
    // ifstream myfile("/home/adil/workspace/linux-process-explorer/timers");
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