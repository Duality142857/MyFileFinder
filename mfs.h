#include<filesystem>
#include<iostream>
#include<string>
#ifdef _WIN32
const std::string SEP="\\";
#else
const std::string SEP="/";
#endif

// using namespace std::filesystem;
namespace fs=std::filesystem;

static void errortest(const std::string& path)
{
    fs::path p{path};
    
    fs::directory_entry ent(p);
    std::cout<<ent.path()<<std::endl;


    for(auto& it:fs::directory_iterator(ent))
    {
        std::cout<<"it "<<std::endl;//FANTASY VOCALOID - 泽野弘之 - 【RE：CREATORS】GRAVITY WALL【FV-VER】（FANTASY VOCALOID remix）
        // std::wcout<<it.path().c_str()<<std::endl;
        // auto cstr=it.path().u16string().c_str();
        // std::cout<<it.path().string()<<std::endl;
        wprintf(L"%s\n",it.path().wstring().c_str());
    }

}

static void directoryTest(std::string rootpath)
{
    fs::path rootDir{rootpath};
    if(!fs::exists(rootDir)) return;
    fs::directory_entry entry(rootDir);//文件入口
    if(entry.status().type()==fs::file_type::directory) std::cout<<"this is a directory!"<<std::endl;
    fs::directory_iterator list(rootDir);
    for(auto& it:list)
    {
        std::cout<<it.path().filename()<<std::endl;
        std::cout<<it.path().string()<<std::endl;
        std::cout<<it.path().relative_path()<<std::endl;
        std::cout<<it.path().parent_path()<<std::endl;
        std::cout<<it.path().root_directory()<<std::endl;
        std::cout<<it.path().root_name()<<std::endl;
        std::cout<<std::endl;
    }
    
}

static void fsTest()
{
    //平台无关路径分隔符
    std::cout<<"separator in this system "<<(char)std::filesystem::path::preferred_separator<<std::endl;

    std::filesystem::path rootPath{"D:/"};
    std::cout<<rootPath.parent_path()<<std::endl;
    std::cout<<rootPath.filename()<<std::endl;
    std::cout<<rootPath.extension()<<std::endl;
    //directory separator operator
    {
        std::string path1;
        path1.append("D:").append(SEP).append("approot");
        std::cout<<path1<<std::endl;
    }
    {
        std::filesystem::path root{"D:\\"};
        std::filesystem::path dir{"approot"};
        std::filesystem::path chaos{"chaos"};
        std::filesystem::path target=root/dir/chaos;
        std::cout<<target<<std::endl;
        
    }
    //create/remove directories
    {
        // std::filesystem::create_directories("testdir/a/b");
        // std::filesystem::remove_all("testdir");
        
    }


}