#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/mutia/Documents";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
  char fpath[1000];
  sprintf(fpath,"%s%s",dirpath,path);
  res = lstat(fpath, stbuf);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
           off_t offset, struct fuse_file_info *fi)
{
 char fpath[1000];
  int res = 0;

  DIR *dp;
  struct dirent *de;

  (void) offset;
  (void) fi;

  dp = opendir(fpath);
  if (dp == NULL)
    return -errno;

  while ((de = readdir(dp)) != NULL) {
    char *newName;
    newName = strcat(de->d_name, "");
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
    res = (filler(buf, newName, &st, 0));
      if(res!=0) break;
  }

  closedir(dp);
  return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res, len, num=0, flag=0;
	char fpath[1000];
	struct stat st;
	st.st_atime = time(NULL) = lastaccess;
	sprintf(fpath, "%s%s", dirpath,path);
	//strcpy(lastaccess,fpath);
	strcy(st.st_atime, fpath);
	
	len = strlen(lastaccess);
	if((len >= 2) && strcmp(&(lastaccess[len - 4]), ".pdf") == 0){
		char command[100];
		sprintf(command,"zenity --error --text='Terjadi kesalahan! File berisi konten berbahaya.'");
		system(command);
	}
	else if((len >= 2) && strcmp(&(lastaccess[len - 4]), ".doc") == 0){
		char command[100];
		sprintf(command,"zenity --error --text='Terjadi kesalahan! File berisi konten berbahaya.'");
		system(command);
	}
	else if((len >= 2) && strcmp(&(lastaccess[len - 4]), ".txt") == 0){
		char command[100];
		sprintf(command,"zenity --error --text='Terjadi kesalahan! File berisi konten berbahaya.'");
		system(command);
	}

	res = open(fpath, fi->flags); //langsung dibuka file
	if(res == -1)
		return -errno;


	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
  int res = 0;
  int fd = 0 ;

  (void) fi;
  fd = open(fpath, O_RDONLY);
  if (fd == -1)
    return -errno;

  res = pread(fd, buf, size, offset);
  if (res == -1)
    res = -errno;

  close(fd);
  return res;
}

static struct fuse_operations xmp_oper = {
  .getattr  = xmp_getattr,
  .readdir  = xmp_readdir,
  //.open	    = xmp_open,
  .read   = xmp_read,
};

int main(int argc, char *argv[])
{
  umask(0);
  return fuse_main(argc, argv, &xmp_oper, NULL);
}

