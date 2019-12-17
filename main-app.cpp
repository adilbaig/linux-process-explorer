#include "window/window.hpp"

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

  auto exe = "/proc/" + pid_str + "/exe";
  auto cwd = "/proc/" + pid_str + "/cwd";
  auto myroot = "/proc/" + pid_str + "/root";
  pbi.exe = do_readlink(exe);
  pbi.cwd = do_readlink(cwd);
  pbi.root = do_readlink(myroot);

  fetch_name_and_args(pbi, pid_str);
  fetch_visible_mountpoints(pbi, pid_str);
  fetch_environ(pbi, pid_str);
  fetch_open_fds(pbi, pid_str);
  fetch_limits(pbi, pid_str);

  int c = 0;
  auto app = Gtk::Application::create(c, argv, "org.gtkmm.example");

  MainWindow window(pbi.name + " (" + pid_str + ")");
  window.env_table.set_env_variables(pbi.environment);
  window.mountpoint_table.set_mount_points(pbi.mountpoints);
  window.fd_table.set_vector(pbi.fds);
  window.lm_table.set_limits(pbi.limits);
  window.set_exe(pbi.exe);
  window.set_args(pbi.args);
  window.set_cwd(pbi.cwd);
  window.set_root(pbi.root);

  return app->run(window);
}