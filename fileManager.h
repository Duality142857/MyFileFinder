#pragma once

#ifdef __linux__
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#endif
#include<stdio.h>
#include<string.h>
#include<string>
#include<vector>
#include<iostream>
#include<algorithm>
#include<queue>
#include"dstring/dstring.h"
#include<filesystem>

namespace fs=std::filesystem;
#ifdef __linux__
static int checkFileTypeTest(int argc, char** argv)
{
    struct stat sb;
    int ret;
    if (argc < 2) {
    fprintf (stderr,
    "usage: %s <file>\n", argv[0]);
    return 1;
    }
    ret = stat (argv[1], &sb);
    if (ret) {
    perror ("stat");
    return 1;
    }
    printf ("File type: ");
    switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:
    printf("block device node\n");
    break;
    case S_IFCHR:
    printf("character device node\n");
    break;
    case S_IFDIR:
    printf("directory\n");
    break;
    case S_IFIFO:
    printf("FIFO\n");
    break;
    case S_IFLNK:
    printf("symbolic link\n");
    break;
    case S_IFREG:
    printf("regular file\n");
    break;
    case S_IFSOCK:
    printf("socket\n");
    break;
    default:
    printf("unknown\n");
    }
    return 0;
}

static int find_file_in_dir(const char* path, const char* file)
{
    DIR* dir;
    dir=opendir(path);
    if(!dir)
    {
        printf("open dir failed!");return -1;
    }   
    struct dirent* entry;
    while((entry=readdir(dir))!=nullptr)
    {
        if(strcmp(entry->d_name,file)==0) return 1;
    }
    // if(errno && !entry) perror("readdir");
    return 0;
}
#endif

namespace MyFileManager{

#ifdef __linux__    
struct MyFile
{
    std::string filename;
    bool ifdir=false;
};
struct GlobalFile
{
    std::string filename;
    std::string path;
};

struct DeepSearcher
{
    std::vector<GlobalFile> filevec;
    struct 
    {
        bool operator()(GlobalFile f1,GlobalFile f2) const
        {
            return f1.filename<f2.filename;
        }
    } customLess;
    
    

    void deepSearch(const std::string& dirname,const std::string& keystring)
    {
        filevec.clear();
        std::queue<std::string> dirqueue;
        dirqueue.push(dirname);
        // std::cout<<"push "<<dirname<<std::endl;
        DIR* dir;
        struct dirent* entry;
        struct stat sb;
        while (!dirqueue.empty())
        {
            auto currentdirname=dirqueue.front();
            // std::cout<<"pop "<<currentdirname<<std::endl;
            dirqueue.pop();
            // std::cout<<"pop finish"<<std::endl;
            dir=opendir(currentdirname.c_str());
            // std::cout<<"open finish"<<std::endl;
            if(!dir){continue;}
            while((entry=readdir(dir))!=NULL)
            {
                auto stringname=std::string(entry->d_name);
                std::string fullpath=currentdirname+"/"+stringname;
                stat(fullpath.c_str(),&sb);
                if(stringname[0]=='.') continue;
                if(S_ISDIR(sb.st_mode))
                {
                    dirqueue.push(fullpath);//no '/' tail
                    // std::cout<<"push "<<fullpath<<std::endl;
                }
                else if(Duality::Dstring(stringname).toUpper().find(Duality::Dstring(keystring).toUpper())!=std::string::npos)// (stringname.find(keystring)!=stringname.npos)
                {
                    // std::cout<<"add file "<<stringname<<std::endl;
                    filevec.push_back({stringname,currentdirname});
                }
                
            }
            
            closedir(dir);
        }
        std::sort(filevec.begin(),filevec.end(),customLess);
    }

    
};

struct MyFileManagerInfo
{
    struct 
    {
        bool operator()(MyFile f1, MyFile f2) const 
        {
            return f1.ifdir>f2.ifdir 
            || (f1.ifdir==f2.ifdir && f1.filename<f2.filename);
        }
    } customLess;
    
    std::string currentDir;
    std::vector<MyFile> filevec;
    void setCurrentDir(const std::string& dirname)
    {
        currentDir=dirname;
        updateFileVec();
        std::sort(filevec.begin(),filevec.end(),customLess);
    }


    void updateFileVec()
    {
        // filevec.clear();//not clearing pointers
        filevec.resize(0);
        DIR* dir;
        // std::cout<<currentDir<<std::endl;
        dir=opendir(currentDir.c_str());
        if(!dir)
        {
            // std::cout<<"failed!"<<std::endl;
            //打开失败，例如无此文件
            return;
        }
        struct dirent* entry;
        struct stat sb;
        while((entry=readdir(dir))!=NULL)
        {
            filevec.push_back({entry->d_name,false});
            char fullpath[1024];
            strcpy(fullpath,currentDir.c_str());
            strcat(fullpath,entry->d_name);
            stat (fullpath, &sb);
            // std::cout<<"full path: "<<fullpath<<std::endl;
            if(S_ISDIR(sb.st_mode))
            {
                filevec[filevec.size()-1].ifdir=true;
            }
            // printf("%s\n",entry->d_name);
        }
        closedir(dir);
    }
};
#endif

struct DeepSearcher_std
{
    std::vector<fs::directory_entry> filevec;
    struct 
    {
        bool operator()(fs::directory_entry f1,fs::directory_entry f2)
        {
            return f1.path().filename()<f2.path().filename();
        }
    } customLess;

    void deepSearch(const std::string& dirname, const std::string& keystring)
    {
        filevec.clear();
        std::queue<fs::directory_entry> dirqueue;
        fs::directory_entry ent{dirname};
        if(!ent.is_directory()) return;
        dirqueue.push(fs::directory_entry{dirname});
        while(!dirqueue.empty())
        {
            auto currentdir=dirqueue.front();
            dirqueue.pop();
            fs::directory_iterator list(currentdir);
            std::cout<<"iter "<<currentdir.path().string()<<std::endl;
            for(auto& it:list)
            {
                if(it.path().filename().string()[0]=='.') continue;
                if(it.is_directory()) dirqueue.push(it);
                else if(Duality::Dstring(it.path().filename().string()).toUpper().find(Duality::Dstring(keystring).toUpper())!=std::string::npos)
                {
                    filevec.push_back(it);
                }   
            }
        }
        std::sort(filevec.begin(),filevec.end(),customLess);
    }
};


struct MyFileManagerInfo_std
{
    struct 
    {
        bool operator()(fs::directory_entry f1, fs::directory_entry f2) const 
        {
            return f1.is_directory()>f2.is_directory()
            || (f1.is_directory()>f2.is_directory() && f1.path().filename()<f2.path().filename());
        }
    } customLess;
    fs::path currentDir;
    std::vector<fs::directory_entry> filevec;

    void setCurrentDir(const std::string& dirname)
    {
        currentDir=dirname;
        updateFileVec();
        std::sort(filevec.begin(),filevec.end(),customLess);
    }

    void updateFileVec()
    {
        filevec.resize(0);
        fs::directory_entry ent(currentDir);
        if(!ent.is_directory()) return;
        fs::directory_iterator list(currentDir);
        for(auto& it:list)
        {
            // std::cout<<it.path().filename()<<std::endl;
            filevec.push_back(it);
            // fs::directory_entry ent(entry/it.path().filename());
        }


    }
};


#ifdef __linux__
static std::vector<MyFile> getDirFiles(const char* dirname)
{
    std::vector<MyFile> filevec;
    DIR* dir;
    dir=opendir(dirname);
    if(!dir) return filevec;
    struct dirent* entry;
    struct stat sb;
    while((entry=readdir(dir))!=NULL)
    {
        filevec.push_back({entry->d_name,false});
        char fullpath[128];
        strcpy(fullpath,dirname);
        strcat(fullpath,entry->d_name);
        stat (fullpath, &sb);
        if(S_ISDIR(sb.st_mode))
        {
            // printf("[]");
            filevec[filevec.size()-1].ifdir=true;
        }
        // printf("%s\n",entry->d_name);
    }
    closedir(dir);
    return filevec;
}
static void listDir(const char* dirname)
{
    DIR* dir;
    dir=opendir(dirname);
    if(!dir) return;
    struct dirent* entry;
    struct stat sb;
    while((entry=readdir(dir))!=NULL)
    {
        char fullpath[128];
        strcpy(fullpath,dirname);
        strcat(fullpath,entry->d_name);
        stat (fullpath, &sb);
        if(S_ISDIR(sb.st_mode))
        {
            printf("[]");
        }
        printf("%s\n",entry->d_name);
    }
    closedir(dir);
}
#endif
}