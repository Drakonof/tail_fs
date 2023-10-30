#ifndef SKIMPFS_H
#define SKIMPFS_H

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <syslog.h>
#include "file_list.h"



// #define SYSLOG(args...) syslog(LOG_INFO,## args)

int skimpfs_getattr(const char *path, struct stat *stbuf);

int skimpfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi);

int skimpfs_open(const char *path, struct fuse_file_info *fi);
int skimpfs_write(const char *path, const char * buf, size_t size, off_t offset, struct fuse_file_info *fi);

int skimpfs_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi);

int skimpfs_chmod(const char *path, mode_t mode);
int skimpfs_chown(const char *path, uid_t uid, gid_t gid);


int skimpfs_create(const char *path, mode_t mode, struct fuse_file_info * p_info);

int skimpfs_truncate(const char *path, off_t offset);

int skimpfs_unlink (const char *path);

void * skimpfs_init(struct fuse_conn_info *conn);

#endif //SKIMPFS_H
