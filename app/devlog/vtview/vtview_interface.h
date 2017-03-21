#ifndef VTVIEW_INTERFACE_H
#define VTVIEW_INTERFACE_H

struct s_vtview_param
{
    void*  memptr;
    double capacity;
};

struct s_vtview_info
{
    double write_amp;
    double remaining_time;
    double remaining_tbw;
    double remaining_life;
    double remaining_spare;

    unsigned int data_read;
    unsigned int data_written;
	
	unsigned int temperature;
};

bool get_vtview_info (s_vtview_info& info);

#endif
