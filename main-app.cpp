#include "window/window.hpp"
#include <thread>
#include <chrono>

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
  fetch_visible_mountpoints(pbi, pid_str);
  fetch_environ(pbi, pid_str);
  // fetch_open_fds(pbi, pid_str);
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
  // window.fd_table.set_fds(pbi.fds);
  window.lm_table.set_limits(pbi.limits);
  window.tm_table.set_timers(pbi.timers);

  // Start a background thread
  // This updates the `ProcessBasicInfo` struct with new information
  thread t1(bg_fetch, pid_str);
  // t1 = thread;
  t1.detach();

  return app->run(window);
}