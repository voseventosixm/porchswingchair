#ifndef VTVIEW_INTERFACE_H
#define VTVIEW_INTERFACE_H

#include <string>
#include <vector>

using namespace std;

struct s_vtview_param
{
    void*  memptr;
    string config_path;
    string backup_path;
    string binary_path;
    string device_path;

public:
    string config_file;
    string device_name;

    string currlog_file;
    string fulllog_file;
};

struct s_vtview_info
{
    vector<string> backup_list;
    vector<string> binary_list;
};

bool load_vtview_info();
bool save_vtview_info();
bool save_vtview_currlog();
bool save_vtview_fulllog();

bool get_vtview_info(s_vtview_info& info);

#endif
