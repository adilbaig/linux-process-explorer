#include <iostream>
#include "window.hpp"

MainWindow::MainWindow(std::string title)
    : m_VBox(Gtk::ORIENTATION_VERTICAL),
      fd_table("File Descriptor")
{
  set_title("Linux Process Explorer: " + title);
  set_border_width(5);
  set_default_size(-1, -1);

  add(m_VBox);

  // Add the ScrolledWindow to the Notebook, with the button underneath:
  m_Notebook.set_border_width(10);
  m_Notebook.append_page(env_table.m_ScrolledWindow, "Environment");
  m_Notebook.append_page(mountpoint_table.m_ScrolledWindow, "Visible MountPoints");
  m_Notebook.append_page(fd_table.m_ScrolledWindow, "File Descriptors");

  //Set some defaults so the alignment can kick in.
  l_exe.set_text("Exe:");
  l_args.set_text("Args:");
  l_cwd.set_text("CWD:");
  l_root.set_text("Root:");
  l_exe.set_halign(Gtk::ALIGN_START);
  l_args.set_halign(Gtk::ALIGN_START);
  l_cwd.set_halign(Gtk::ALIGN_START);
  l_root.set_halign(Gtk::ALIGN_START);

  m_Grid.add(l_exe);
  m_Grid.attach_next_to(l_args, l_exe, Gtk::POS_BOTTOM, 1, 1);
  m_Grid.attach_next_to(l_cwd, l_args, Gtk::POS_BOTTOM, 1, 1);
  m_Grid.attach_next_to(l_root, l_cwd, Gtk::POS_BOTTOM, 1, 1);
  m_Grid.set_border_width(5);

  m_VBox.pack_start(m_Grid, Gtk::PACK_SHRINK);
  m_VBox.pack_start(m_Notebook);

  show_all_children();
  maximize();
}

MainWindow::~MainWindow()
{
}

void MainWindow::set_exe(std::string var)
{
  l_exe.set_markup("<b>Exe: </b> " + var);
}

void MainWindow::set_root(std::string var)
{
  l_root.set_markup("<b>Root: </b> " + var);
}

void MainWindow::set_cwd(std::string var)
{
  l_cwd.set_markup("<b>CWD: </b> " + var);
}

void MainWindow::set_args(std::vector<std::string> var)
{
  std::string s;
  if(var.size()){
    for(auto const& a: var) {
      s += a + ' ';
    }
  } else {
    s = "-";
  }

  l_args.set_markup("<b>Args: </b> " + s);
}

EnvTable::EnvTable()
{
  m_ScrolledWindow.add(m_TreeView);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  // Add columsn to column group
  m_Columns.add(m_col_id);
  m_Columns.add(m_col_name);
  m_Columns.add(m_col_value);

  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);

  //Add the TreeView's view columns:
  m_TreeView.append_column("#", m_col_id);
  m_TreeView.append_column("Variable", m_col_name);
  m_TreeView.append_column("Value", m_col_value);

  //Make the columns sortable
  int s = 0;
  m_TreeView.get_column(s++)->set_sort_column(m_col_id);
  m_TreeView.get_column(s++)->set_sort_column(m_col_name);
  m_TreeView.get_column(s++)->set_sort_column(m_col_value);
}

void EnvTable::set_env_variables(std::map<std::string, std::string> vars)
{
  Gtk::TreeModel::Row row;
  for (auto const &[key, val] : vars)
  {
    row = *(m_refTreeModel->append());
    row[m_col_id] = ++col_ctr;
    row[m_col_name] = key;
    row[m_col_value] = val;
  }
}

StringVectorTable::StringVectorTable(std::string column_name)
{
  m_ScrolledWindow.add(m_TreeView);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  // Add columns to column group
  m_Columns.add(m_col_id);
  m_Columns.add(m_col_str);

  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);

  //Add the TreeView's view columns:
  m_TreeView.append_column("#", m_col_id);
  m_TreeView.append_column(column_name, m_col_str);

  //Make the columns sortable
  int s = 0;
  m_TreeView.get_column(s++)->set_sort_column(m_col_id);
  m_TreeView.get_column(s++)->set_sort_column(m_col_str);
}

void StringVectorTable::set_vector(std::vector<std::string> vars)
{
  Gtk::TreeModel::Row row;
  for (auto const &val : vars)
  {
    row = *(m_refTreeModel->append());
    row[m_col_id] = ++col_ctr;
    row[m_col_str] = val;
  }
}

MountPointTable::MountPointTable()
{
  m_ScrolledWindow.add(m_TreeView);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  // Add columsn to column group
  m_Columns.add(m_col_id);
  m_Columns.add(m_col_name);
  m_Columns.add(m_col_path);
  m_Columns.add(m_col_fs);
  m_Columns.add(m_col_options);

  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);

  //Add the TreeView's view columns:
  m_TreeView.append_column("#", m_col_id);
  m_TreeView.append_column("Mount", m_col_name);
  m_TreeView.append_column("Path", m_col_path);
  m_TreeView.append_column("File System", m_col_fs);
  m_TreeView.append_column("Options", m_col_options);

  //Make the columns sortable
  int s = 0;
  m_TreeView.get_column(s++)->set_sort_column(m_col_id);
  m_TreeView.get_column(s++)->set_sort_column(m_col_name);
  m_TreeView.get_column(s++)->set_sort_column(m_col_path);
  m_TreeView.get_column(s++)->set_sort_column(m_col_fs);
  m_TreeView.get_column(s++)->set_sort_column(m_col_options);
}

void MountPointTable::set_mount_points(std::vector<std::string> vars)
{
  Gtk::TreeModel::Row row;
  for (auto const &val : vars)
  {
    std::istringstream iss(val);
    std::vector<std::string> toks;
    std::string item;

    while (std::getline(iss, item, ' '))
    {
      toks.push_back(item);
    }

    row = *(m_refTreeModel->append());
    row[m_col_id] = ++col_ctr;
    row[m_col_name] = toks[0];
    row[m_col_path] = toks[1];
    row[m_col_fs] = toks[2];
    row[m_col_options] = toks[3];
  }
}