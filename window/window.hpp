#ifndef GTKMM_EXAMPLEWINDOW_H
#define GTKMM_EXAMPLEWINDOW_H

#include <gtkmm.h>
#include <map>
#include <string>

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

class StringVectorTable
{
public:
  Gtk::ScrolledWindow m_ScrolledWindow;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

protected:
  int col_ctr = 0;
  Gtk::TreeModelColumn<unsigned int> m_col_id;
  Gtk::TreeModelColumn<Glib::ustring> m_col_str;

  Gtk::TreeView m_TreeView;
  Gtk::TreeModel::ColumnRecord m_Columns;

public:
  StringVectorTable(std::string);
  void set_vector(std::vector<std::string>);
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

class MainWindow : public Gtk::Window
{
public:
  EnvTable env_table;
  StringVectorTable fd_table;
  MountPointTable mountpoint_table;

  MainWindow(std::string);
  virtual ~MainWindow();
  void set_exe(std::string);
  void set_args(std::vector<std::string>);
  void set_cwd(std::string);
  void set_root(std::string);
  
protected:
  Gtk::Box m_VBox;
  Gtk::Notebook m_Notebook;

  Gtk::Label l_exe, l_args, l_root, l_cwd;
  Gtk::Grid m_Grid;
};

#endif //GTKMM_EXAMPLEWINDOW_H