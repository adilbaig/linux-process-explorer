#include "window/window.hpp"
#include <thread>
#include <chrono>
#include <string>

ProcessBasicInfo pbi = {};
// This declaration compiles, but app fails on startup with:
// _gtk_style_provider_private_get_settings: assertion 'GTK_IS_STYLE_PROVIDER_PRIVATE (provider)' failed
// MainWindow window;

// Setting a ptr works for a start. But when i 'interact' with the table, the next update segfaults.
// I printed the addresses, they hadn't changed. So maybe the internal reference to the window had moved.
// MainWindow* window_ptr;
const int thread_timer_ms = 2000;

void bg_fetch(string pid_str)
{
  while (true)
  {
    fetch_open_fds(pbi, pid_str);
    this_thread::sleep_for(chrono::milliseconds(thread_timer_ms));
  }
}

string vIntToStr(vector<int> vi)
{
    stringstream joinedValues;
    for (const auto &v : vi)
        joinedValues << v << ",";
    return joinedValues.str().substr(0, joinedValues.str().size() - 1);
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
    return -1;
  }

  auto exe = "/proc/" + pid_str + "/exe";
  auto cwd = "/proc/" + pid_str + "/cwd";
  auto myroot = "/proc/" + pid_str + "/root";
  pbi.exe = do_readlink(exe);
  pbi.cwd = do_readlink(cwd);
  pbi.root = do_readlink(myroot);

  fetch_name_and_args(pbi, pid_str);
  fetch_status(pbi, pid_str);
  fetch_visible_mountpoints(pbi, pid_str);
  fetch_environ(pbi, pid_str);
  fetch_limits(pbi, pid_str);
  fetch_timers(pbi, pid_str);

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