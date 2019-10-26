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
    
    int ctr, i, start, end;
    while (getline(myfile, line))
    {
        // replace( line.begin(), line.end(), ' ', '\t');
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
                cout << buff << endl;
            }
        }
        closedir(dir);
    }
}