
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>

#include <unistd.h>
#include <linux/limits.h>

#include <map>
#include <string>
#include <fstream>
#include <iostream>


#include <dirent.h>

#include "file_list.h"
#include "skimpfs.h"

#define SYSLOG(args...) syslog(LOG_DEBUG,## args)

std::string current_path = "/home/artem/work/skimpfs_new/tmp/";


static std::map<const std::string,FileStat> fileList = {};

void * skimpfs_init(struct fuse_conn_info *conn)
{
    std::string name = current_path.c_str();

    struct dirent *dp = nullptr;
    DIR*dirp = nullptr;

    dirp = opendir(name.c_str());

     while (dirp) {
         errno = 0;
         
         if ((dp = readdir(dirp)) != NULL) {
             name = std::string (current_path.c_str()) + dp->d_name;
             SYSLOG("%s %s",__func__, name.c_str());

             
             struct stat stbuf;

             int s = stat(name.c_str(), &stbuf);
             SYSLOG("%s %i %i",__func__, stbuf.st_mode, (stbuf.st_mode & S_IFMT));
                        
             if (stbuf.st_mode & S_IFMT) {
                 if (stbuf.st_mode & S_IFDIR) 
                 {
                     continue;
                 }
                            
               // FileStat fn(stbuf);

                fileList.emplace(std::string("/") + dp->d_name, std::move(stbuf));
                SYSLOG("%s %s %li",__func__, name.c_str(), stbuf.st_size);

            }
        } else {
            closedir(dirp);
            break;
         }


     }
     
    return nullptr;
}


int skimpfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    SYSLOG("%s %s",__func__, path);



    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for(auto fn:fileList)
      filler(buf, fn.first.c_str()+1 , NULL, 0);


    //filler(buf, skimpfs_path + 1, NULL, 0);
    //filler(buf, "new_file",       NULL, 0);
    return 0;
}

int skimpfs_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    SYSLOG("%s %s",__func__, path);

    if (strcmp(path, "/") == 0) {

        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;

        return res;
    } 
    

    auto fn = fileList.find(path); 

    if (fn == fileList.end()) {
        return -ENOENT;
    }
    else {
        

        std::string name = current_path.c_str();

        name += path + 1;

        int s = stat(name.c_str(), stbuf);
        SYSLOG("%s %s %i",__func__, name.c_str(), s);



        if (s != 0) {

          *stbuf=fn->second;
          //stbuf->st_size = fn->second.fileText.size();

          /*
         stbuf->st_mode = fn->second.fileStat.st_mode;
         stbuf->st_uid = fn->second.fileStat.st_uid;
         stbuf->st_gid = fn->second.fileStat.st_gid;
         stbuf->st_nlink = fn->second.fileStat.st_nlink;

       */
        }
        
    }



    return res;
}

int skimpfs_open(const char *path, struct fuse_file_info *fi)
{
    SYSLOG("%s %s flags:%X",__func__, path, fi->flags);

    if (strcmp(path, "/") == 0) {
        

        return 0;
    } 

    auto fn = fileList.find(path); 

    if (fn == fileList.end()) {
        return -ENOENT;
    }
    else {
        std::string name =  current_path.c_str();
   
        name += path + 1;

        SYSLOG("%s %s",__func__, name.c_str());

       // fn->second.content.open(name, /*std::ios_base::binary | */std::ios_base::app);
        
        fn->second.p_file = fopen(name.c_str(), "r+");
    
        if (!fn->second.p_file) {
            SYSLOG("%s fopen failed",__func__);
            return -ENOENT;
        }
    }

    return 0;
}

int skimpfs_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;

   //  std::ifstream istrm;


    SYSLOG("%s %s",__func__, path);

    auto fn = fileList.find(path); 

    if (fn == fileList.end()) {
        return -ENOENT;
    }

    //memcpy(buf, fn->second.fileText.c_str(), (fn->second.fileText.size() < size) ? fn->second.fileText.size() : size);

    //fn->second.fileText.erase(offset,fn->second.fileText.size() - offset);
    
     std::string name = current_path.c_str();

    

    
    name += path + 1;

    SYSLOG("%s %s offset:%li size:%li",__func__, name.c_str(), offset, size);
    //istrm.open(name, std::ifstream::binary);

//     if (istrm.is_open() ) {
//                 //ostrm.seekp(offset);
//             istrm.seekg(offset);
//             istrm.read(buf, size);
// 
//             SYSLOG("%s",__func__);
    
     
     if (fn->second.p_file && (feof(fn->second.p_file) != -1) && (ferror(fn->second.p_file)!= -1)) {
        if (fseek(fn->second.p_file, offset, SEEK_SET) == 0) {
             size_t c = fread(buf, 1, size, fn->second.p_file);
             SYSLOG("%s read: %ld",__func__, c);
             
        }
        else {
            SYSLOG("%s fseek failed",__func__);
            return -ENOENT;
        }
        
     } else {
         SYSLOG("%s error",__func__);
     }


    return size;
}


int skimpfs_chmod(const char *path, mode_t mode)
{
    SYSLOG("%s %s %o",__func__, path, mode);

    auto fn = fileList.find(path); 

    if (fn == fileList.end()) {
        return -ENOENT;
    }

    fn->second.st_mode = mode;

    return 0;

}

int skimpfs_chown(const char *path, uid_t uid, gid_t gid)
{
    SYSLOG("%s %s %d %d",__func__, path, uid, gid);

    auto fn = fileList.find(path); 

    if (fn == fileList.end()) {
        return -ENOENT;
    }

    fn->second.st_uid = uid;
    fn->second.st_gid = gid;

    return 0;

}

int skimpfs_create(const char *path, mode_t mode, struct fuse_file_info * p_info)
{
    SYSLOG("%s %s %i flags:%X",__func__, path, mode, p_info->flags); //todo:
    
    std::string name =  current_path.c_str();
   
    name += path + 1;

    SYSLOG("%s %s",__func__, name.c_str());
    
    fileList.insert(std::pair<std::string, FileStat>(path, FileStat(mode)));
    
    auto fn = fileList.find(path); 

    if (fn == fileList.end()) {
        return -ENOENT;
    }

    //fn->second.content.open(name,/* std::ios_base::binary | */std::ios_base::app);
    
    fn->second.p_file = fopen(name.c_str(), "w+");
    
    
    

   
    
    
    if (!fn->second.p_file) {
        SYSLOG("%s fopen failed",__func__);
        return -ENOENT;
    }
    
    SkimpHeader sk_header;
    
    fwrite(&sk_header, 1, sizeof(SkimpHeader),fn->second.p_file);

    


    return 0;

}

int skimpfs_truncate(const char *path, off_t offset)
{
    SYSLOG("%s %s %ld",__func__, path, offset);

    auto fn = fileList.find(path); 

     std::ofstream ostrm;

    if (fn == fileList.end()) {
        return -ENOENT;
    }

   // fn->second.fileText.erase(offset,fn->second.fileText.size() - offset);

    std::string name = current_path.c_str();

    
    name += "/tmp/skimpfs_";
    name += path + 1;
    ostrm.open(name, std::ios_base::out | std::ios_base::trunc);

    if (ostrm.is_open()) {
                //ostrm.seekp(offset);

                SYSLOG("%s opened",__func__);





    } else {
        SYSLOG("%s not opened",__func__);
    }


   return 0;
}



int skimpfs_write(const char *path, const char * buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;

    SYSLOG("%s %s",__func__, path);
    
    

    auto fn = fileList.find(path); 

    if (fn == fileList.end()) {
        return -ENOENT;
    }
    

    SYSLOG("%s %s %lu %lu",__func__, path, size, offset);
    
    if (offset > (fn->second.sh.max_size + sizeof(SkimpHeader))) {
        SYSLOG("%s %s",__func__, "max size error");
        return -ENOENT;
    }
    
    off_t addr = 0;


    
    offset += sizeof(SkimpHeader);
    
    if (fn->second.p_file && (feof(fn->second.p_file) != -1) && (ferror(fn->second.p_file)!= -1)) {
        
        if (fseek(fn->second.p_file, offset, SEEK_SET) == 0) {
            if ((offset + size) > fn->second.sh.max_size) {
                     addr = fn->second.sh.max_size - offset + sizeof(SkimpHeader);
                     SYSLOG("offset + size %s %ld %ld %ld",__func__, size, addr, offset);
                    
                 //   fseek(fn->second.p_file, sizeof(SkimpHeader), SEEK_SET);
                     
                    if (addr >= size) fwrite(buf, 1, size, fn->second.p_file);
                    else {
                         fwrite(buf, 1, addr, fn->second.p_file);

                         fseek(fn->second.p_file, sizeof(SkimpHeader) , SEEK_SET);
                         fwrite(buf + addr, 1, size - addr, fn->second.p_file);
                         SYSLOG("else %s %ld %ld %ld",__func__, size, addr, offset);
                     }
            }
            else {
                     SYSLOG("write %s %ld %ld %ld",__func__, size, addr, offset);
                     fwrite(buf, 1, size, fn->second.p_file);
                 }
            
                     
                     
                     
                     
                     
                     
            
            // size_t c = fwrite(buf, 1, size, fn->second.p_file);
             //size_t c = fread(buf, 1, size, fn->second.p_file);
           //  SYSLOG("%s write: %ld",__func__, c);
             
        }
        else {
            SYSLOG("%s fseek failed",__func__);
            return -ENOENT;
        }
        
     } else {
         SYSLOG("%s error",__func__);
     }


    return size;
}


int skimpfs_unlink (const char *path)
{
    SYSLOG("%s %s",__func__, path);

    auto fn = fileList.find(path); 

    if (fn == fileList.end()) {
        return -ENOENT;
    }


    fileList.erase(fn);


    return 0;
}

int skimpfs_release(const char *path, struct fuse_file_info * fi)
{
     SYSLOG("%s %s",__func__, path);
    
    auto fn = fileList.find(path); 

    if (fn == fileList.end()) {
        return -ENOENT;
    }

    if (fn->second.p_file) {
         fclose(fn->second.p_file);
         fn->second.p_file = nullptr;
    }

   
    //fn->second.content.close();
    
    return 0;
    
}



static struct fuse_operations skimpfs_oper = {
    .getattr  = skimpfs_getattr,
    .unlink   = skimpfs_unlink,
    .chmod    = skimpfs_chmod,
    .chown    = skimpfs_chown,
    .truncate = skimpfs_truncate,
    .open     = skimpfs_open,
    .read     = skimpfs_read,
    .write    = skimpfs_write,
    .release  = skimpfs_release,
    .readdir  = skimpfs_readdir,
    .init     = skimpfs_init,
    .create   = skimpfs_create
    
};

int main(int argc, char *argv[])
{
    // int c = 0;

    // while (-1 != (c = getopt(argc, argv, "p:")))
    // {
    //     switch (c)
    //     {
    //         case 'p':
    //         {
    //             current_path = optarg;
    //             break;
    //         }
    //         case '?':
    //         {
    //             std::cout << "Got unknown option." << std::endl; 
    //             break;
    //         }
    //         default:
    //         {
    //             std::cout << "Got unknown parse returns: " << c << std::endl; 
    //         }
    //     }
    // }



    SYSLOG("%s %s",__func__, current_path.c_str());
    return fuse_main(argc, argv, &skimpfs_oper, NULL);
}
