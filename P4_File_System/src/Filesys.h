#pragma once

#include <string>

void init_disk_junk();
int init_inode_table();

//fs functions

//base fs
int make_fs(const std::string&);
int mount_fs(const std::string&);
int umount_fs(const std::string&);
//files
int fs_open(const std::string&);

int read_inode_table_from_vdisk();
int write_inode_table_to_vdisk();

int create_file(const std::string&);
