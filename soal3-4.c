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

char alamat[100],alamat2[100];
int buka=0;

//Mendapatkan file attribut
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    buka=1;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    int res;
    strcpy(alamat2,path);
    res = lstat(fpath, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

//Mengecek file access permissions. Akan dipanggil called for the access() system call.
static int xmp_access(const char *path, int mask)
{
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    int res;  
    res = access(fpath, mask);
    buka=0;
    if (res == -1)
        return -errno;
    return 0;
}

//Membaca target dari symbolic link
static int xmp_readlink(const char *path, char *buf, size_t size)
{
    buka=0;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = readlink(fpath, buf, size - 1);
    if (res == -1)
        return -errno;
    buf[res] = '\0';
    return 0;
}

//Membuka directory
static int xmp_opendir(const char *path, struct fuse_file_info *fi)
 {
    buka = 0;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
         DIR *dp = opendir(fpath);
         if (dp == NULL)
             return -errno;
         fi->fh = (unsigned long) dp;
         return 0;
}

//Membuka directory
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
    buka=0;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;
    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;
    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }
    closedir(dp);
    return 0;
}

//Me-release directory
static int xmp_releasedir(const char *path, struct fuse_file_info *fi)
 {
    buka = 0;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
         DIR *dp = (DIR *) fi->fh;
         (void) fpath;
         closedir(dp);
         return 0;
 }

//Membuat directory
static int xmp_mkdir(const char *path, mode_t mode)
{
    buka=0;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = mkdir(fpath, mode);
    if (res == -1)
        return -errno;
    return 0;
}

//Menghilangkan file
static int xmp_unlink(const char *path)
{
    buka=1;
    int res,res2;
    char fpath[1000],fpath2[1000];
    char temp2[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    strcpy(fpath2,fpath);
    res = unlink(fpath);
    strcpy(temp2,fpath2);
    while(1)
    {
        strcat(temp2,"-copy");      
        res2 = unlink(temp2);
        if (res2==-1) break;
    }
    if (res == -1)
        return -errno;
    /*else
        unlink(temp2);*/
    return 0;
}

//Menghilangkan directory
static int xmp_rmdir(const char *path)
{
    buka=0;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = rmdir(fpath);
    if (res == -1)
        return -errno;
    return 0;
}

//Create a symbolic link
static int xmp_symlink(const char *from, const char *to)
{
    buka=0;
    int res;
    char ffrom[1000];
    char ffrom2[1000];
    sprintf(ffrom,"%s%s",dirpath,from);
    sprintf(ffrom2,"%s%s",dirpath,to);
    res = symlink(ffrom, ffrom2);
    if (res == -1)
        return -errno;
    return 0;
}

//Membuat symbolic link
static int xmp_rename(const char *from, const char *to)
{
    buka=0;  
    int res;
    char ffrom[1000];
    char ffrom2[1000];
    sprintf(ffrom,"%s%s",dirpath,from);
    sprintf(ffrom2,"%s%s",dirpath,to);
    res = rename(ffrom, ffrom2);
    if(res == -1)
    return -errno;
    return 0;
}

//Membuat hard link
static int xmp_link(const char *from, const char *to)
{
    buka=0;
    int res;
    char ffrom[1000];
    char ffrom2[1000];
    sprintf(ffrom,"%s%s",dirpath,from);
    sprintf(ffrom2,"%s%s",dirpath,to);
    res = link(ffrom, ffrom2);
    if (res == -1)
        return -errno;
    return 0;
}

//Mengubah mod
static int xmp_chmod(const char *path, mode_t mode)
{
    buka=0;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = chmod(fpath, mode);
    if (res == -1)
        return -errno;
    return 0;
}

//Mengubah own
static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
    buka=0;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = lchown(fpath, uid, gid);
    if (res == -1)
        return -errno;
    return 0;
}

//Mengubah ukuran file
static int xmp_truncate(const char *path, off_t size)
{
    buka=0;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = truncate(fpath, size);
    if (res == -1)
        return -errno;
    return 0;
}

//Mengubah akses dan memodifikasi waktu dari sebuah file dengan nanosecond resolution
#ifdef HAVE_UTIMENSAT
static int xmp_utimens(const char *path, const struct timespec ts[2])
{
    buka=0;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
    if (res == -1)
        return -errno;
    return 0;
}
#endif

static int xmp_utime(const char *path,struct utimbuf *buf)
{
    buka=0;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    int res;
    res=utime(fpath,buf);
    if(res==-1)
        return -errno;
    return 0;
}

//Membuat node file
static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    buka=0;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res=mknod(fpath,mode,rdev);
    if (res==-1)
        return -errno;
    return 0;
}

//Operasi membuka file
static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    buka=0;
    int res;  
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    strcpy(alamat2,path);
    res = open(fpath, fi->flags);  
    if (res == -1)
        return -errno;
    close(res);
    return 0;
}

//Membaca data dari file yang telah dibuka
static int xmp_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
    buka=1;
    int fd;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    int res;
    strcpy(alamat,path);
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
static int xmp_write(const char *path, const char *buf, size_t size,off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    (void) fi;
    fd = open(fpath, O_WRONLY);
    //if (buka==1)
    //{
        char temp1[100],fpath2[1000];
        sprintf(fpath2,"%s%s",dirpath,alamat);  
        FILE *fd1, *fd2;
      
        strcpy(temp1,fpath2);
        strcat(temp1,".copy");
  
        char kata[100];
      
        fd1 = fopen(temp1, "w+");
        fd2 = fopen(fpath2, "a+");
        while(fgets(kata,100,fd2)!=NULL)
        {
            fprintf(fd1,"%s",kata);  
        }
        fclose(fd1);
        fclose(fd2);
    //}
    if (fd == -1)
        return -errno;
    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;
    close(fd);  
    return res;
}


//Mengalokasi ruang untuk membuka file
#ifdef HAVE_POSIX_FALLOCATE
static int xmp_fallocate(const char *path, int mode,off_t offset, off_t length, struct fuse_file_info *fi)
{
    buka=0;
    int fd;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    (void) fi;
    if (mode)
        return -EOPNOTSUPP;
    fd = open(fpath, O_WRONLY);
    if (fd == -1)
        return -errno;
    res = -posix_fallocate(fd, offset, length);
    close(fd);
    return res;
}
#endif

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .access = xmp_access,
    .readlink = xmp_readlink,
    .readdir = xmp_readdir,
    .opendir = xmp_opendir,
    .releasedir = xmp_releasedir,
    .mknod = xmp_mknod,
    .mkdir = xmp_mkdir,
    .symlink = xmp_symlink,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .link = xmp_link,
    .chmod = xmp_chmod,
    .chown = xmp_chown,
    .truncate = xmp_truncate,
    #ifdef HAVE_UTIMENSAT
    .utimens = xmp_utimens,
    #endif
    .open = xmp_open,
    .read = xmp_read,
    .write = xmp_write,
    .utime=xmp_utime,
    //.release = xmp_release,
    #ifdef HAVE_POSIX_FALLOCATE
    .fallocate = xmp_fallocate,
    #endif
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
