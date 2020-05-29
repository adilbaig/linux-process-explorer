#ifndef GTKMM_EXAMPLEWINDOW_H
#define GTKMM_EXAMPLEWINDOW_H

#include <gtkmm.h>
#include <map>
#include <string>
#include <iostream>
#include "../lpe/process.hpp"

class EnvTable
{
public:
  Gtk::ScrolledWindow m_ScrolledWindow;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

protected:
  int col_ctr = 0;
  Gtk::TreeModelColumn<unsigned int> m_col_id;
  Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  Gtk::TreeModelColumn<Glib::ustring> m_col_value;

  Gtk::TreeView m_TreeView;
  Gtk::TreeModel::ColumnRecord m_Columns;

public:
  EnvTable();
  void set_env_variables(std::map<std::string, std::string>);
};

class FDTable
{
public:
  Gtk::ScrolledWindow m_ScrolledWindow;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

protected:
  int col_ctr = 0;
  Gtk::TreeModelColumn<unsigned int> m_col_fd;
  Gtk::TreeModelColumn<Glib::ustring> m_col_path;

  Gtk::TreeView m_TreeView;
  Gtk::TreeModel::ColumnRecord m_Columns;

public:
  FDTable();
  void set_fds(std::map<int, std::string>);
};

class MountPointTable
{
public:
  Gtk::ScrolledWindow m_ScrolledWindow;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

protected:
  int col_ctr = 0;
  Gtk::TreeModelColumn<unsigned int> m_col_id;
  Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  Gtk::TreeModelColumn<Glib::ustring> m_col_path;
  Gtk::TreeModelColumn<Glib::ustring> m_col_fs;
  Gtk::TreeModelColumn<Glib::ustring> m_col_options;

  Gtk::TreeView m_TreeView;
  Gtk::TreeModel::ColumnRecord m_Columns;

public:
  MountPointTable();
  void set_mount_points(std::vector<std::string>);
};

class LimitsTable
{
public:
  Gtk::ScrolledWindow m_ScrolledWindow;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

protected:
  int col_ctr = 0;
  Gtk::TreeModelColumn<Glib::ustring> m_col_limit;
  Gtk::TreeModelColumn<Glib::ustring> m_col_hard;
  Gtk::TreeModelColumn<Glib::ustring> m_col_soft;
  Gtk::TreeModelColumn<Glib::ustring> m_col_unit;

  Gtk::TreeView m_TreeView;
  Gtk::TreeModel::ColumnRecord m_Columns;

public:
  LimitsTable();
  void set_limits(std::vector<Limit>);
};

class TimerTable
{
public:
  Gtk::ScrolledWindow m_ScrolledWindow;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

protected:
  int col_ctr = 0;
  Gtk::TreeModelColumn<Glib::ustring> m_col_signal;
  Gtk::TreeModelColumn<Glib::ustring> m_col_notice;
  Gtk::TreeModelColumn<Glib::ustring> m_col_method;
  Gtk::TreeModelColumn<Glib::ustring> m_col_clock;

  Gtk::TreeView m_TreeView;
  Gtk::TreeModel::ColumnRecord m_Columns;

public:
  TimerTable();
  void set_timers(std::vector<TimerSignal>);
};

class MainWindow : public Gtk::Window
{
public:
  EnvTable env_table;
  FDTable fd_table;
  MountPointTable mountpoint_table;
  LimitsTable lm_table;
  TimerTable tm_table;
  ProcessBasicInfo* pbi_ptr;
  int update_timeout_ms;

  MainWindow(int);
  void initialize(std::string);
  virtual ~MainWindow();
  void set_exe(std::string);
  void set_args(std::vector<std::string>);
  void set_cwd(std::string);
  void set_root(std::string);

  bool on_timeout();

protected:
  Gtk::Box m_VBox;
  Gtk::Notebook m_Notebook;

  Gtk::Label l_exe, l_args, l_root, l_cwd;
  Gtk::Grid m_Grid;
};

#endif //GTKMM_EXAMPLEWINDOW_H