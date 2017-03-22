#ifndef VTVIEW_INTERFACE_H
#define VTVIEW_INTERFACE_H

struct s_vtview_param
{
    void*  memptr;
    string smartlog_backup;
    string smartlog_filename;
};

bool load_vtview_info();
bool save_vtview_info();

#endif
