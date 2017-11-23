#define FUSE_USE_VERSION 30

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux

#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#include <string.h>
#endif

static const char *dirpath = "/home/dayday/Downloads";

// char alamat[100],alamat2[100];
// int buka=0;


//Mendapatkan file attribut
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
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

//Membuat symbolic link
// static int xmp_rename(const char *from, const char *to)
// {
//     buka=0;  
//     int res;
//     char newdir[1000];
//     char nfrom[1000];
//     char nto[1000];
//     char arg[1000];
  
//     sprintf(newdir, "/home/dayday/Downloads/simpanan");

//     sprintf(arg,"mkdir -p %s",newdir);
//     system (arg);

//     sprintf(nfrom,"%s%s",dirpath,from);
//     sprintf(nto,"%s%s",newdir,to);
//     res = rename(nfrom, nto);
//     if(res == -1)
//     return -errno;
//     return 0;
// }

//Mengubah mod
// static int xmp_chmod(const char *path, mode_t mode)
// {
//     buka=0;
//     int res;
//     char fpath[1000];
//     sprintf(fpath,"%s%s",dirpath,path);
//     res = chmod(fpath, mode);
//     if (res == -1)
//         return -errno;
//     return 0;
// }

//Membuat node file
// static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
// {
//     buka=0;
//     int res;
//     char fpath[1000];
//     sprintf(fpath,"%s%s",dirpath,path);
//     res=mknod(fpath,mode,rdev);
//     if (res==-1)
//         return -errno;
//     return 0;
// }
const char *ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}
//Membaca data dari file yang telah dibuka
static int xmp_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
    buka=1;
    int fd;
    char fpath[1000];
    
    if(strcmp(path,"/") == 0){
    	path=dirpath;
    	sprintf(fpath,"%s/simpanan",path);
    	}
    else sprintf(fpath, "%s/simpanan/%s",dirpath,path);
    int res = 0;
    
    char read[1000], dirmake[1000];
    sprintf(read, "%s.copy", fpath);
    rename(fpath, read);
    
    if(strcmp(ext(read),"copy")==0){
        system("zenity --error --text='File yang anda buka adalah file hasil salinan. File tidak bisa diubah maupun disalin kembali!\'");   
    	system("chmod 000 /home/dayday/Downloads/simpanan/*.copy");
	return -errno;
    }
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

//Menulis data di data yg dibuka
// static int xmp_write(const char *path, const char *buf, size_t size,off_t offset, struct fuse_file_info *fi)
// {
//     int fd;
//     int res;
//     char fpath[1000];
//     sprintf(fpath,"%s%s",dirpath,path);
//     (void) fi;
//     fd = open(fpath, O_WRONLY);
//     //if (buka==1)
//     //{
//        /* char temp1[100],fpath2[1000];
//         sprintf(fpath2,"%s%s",dirpath,alamat);  
//         FILE *fd1, *fd2;
      
//         strcpy(temp1,fpath2);
//         strcat(temp1,".copy");
  
//         char kata[100];
      
//         fd1 = fopen(temp1, "w+");
//         fd2 = fopen(fpath2, "a+");
//         while(fgets(kata,100,fd2)!=NULL)
//         {
//             fprintf(fd1,"%s",kata);  
//         }
//         fclose(fd1);
//         fclose(fd2);
//     //}*/
//     if (fd == -1)
//         return -errno;
//     res = pwrite(fd, buf, size, offset);
//     if (res == -1)
//         res = -errno;
//     close(fd);  
//     return res;
// }

// //Membuat directory
// static int xmp_mkdir(const char *path, mode_t mode)
// {
//     buka=0;
//     int res;
//     char fpath[1000];
//     sprintf(fpath,"%s%s",dirpath,path);
//     res = mkdir(fpath, mode);
//     if (res == -1)
//         return -errno;
//     return 0;
// }

// //mengganti alokasi memori tiap file
//     static int xmp_truncate(const char *path, off_t size){
//     int res;
//     char fpath[1000];
//     sprintf(fpath,"%s%s", dirpath, path);
//     res = truncate(fpath, size);
//     if(res == -1)
//     	return -errno;

//     return 0;
// }

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    //.access = xmp_access,
    //.readlink = xmp_readlink,
    .readdir = xmp_readdir,
    //.opendir = xmp_opendir,
    //.releasedir = xmp_releasedir,
//     .mknod = xmp_mknod,
//     .mkdir = xmp_mkdir,
    //.symlink = xmp_symlink,
    //.unlink = xmp_unlink,
    //.rmdir = xmp_rmdir,
    //.rename = xmp_rename,
    //.link = xmp_link,
    //.chmod = xmp_chmod,
    //.chown = xmp_chown,
    //.truncate = xmp_truncate,
    //#ifdef HAVE_UTIMENSAT
    //.utimens = xmp_utimens,
    //#endif
    //.open = xmp_open,
    .read = xmp_read,
    //.write = xmp_write,
    //.utime=xmp_utime,
    //.release = xmp_release,
    //#ifdef HAVE_POSIX_FALLOCATE
    //.fallocate = xmp_fallocate,
    //#endif
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
