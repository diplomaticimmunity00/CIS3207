#pragma once

#include <string>

void init_disk_junk();
int init_inode_table();
int init_file_table();

//fs functions

//utility
int get_file_index(const std::string&);
int get_file_index_by_fd(int);
//base fs
int make_fs(const std::string&);
int mount_fs(const std::string&);
int umount_fs(const std::string&);

//files
int fs_open(const std::string&);
int fs_close(int);
int fs_create(const std::string&);
int fs_delete(const std::string&);
int fs_mkdir(const std::string&);
int fs_write(int,char*,size_t);
int fs_read(int,char*,size_t);
int fs_get_filesize(int);
int fs_lseek(int, size_t);
int fs_truncate(int, size_t);

int read_inode_table_from_vdisk();
int write_inode_table_to_vdisk();

