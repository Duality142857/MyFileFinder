#include<filesystem>
#ifdef _WIN32
const std::string SEP="\\";
#else
const std::string SEP="/";
#endif

// using namespace std::filesystem;
namespace fs=std::filesystem;

static void directoryTest()
{
    fs::path rootDir{"D:\\"};
    if(!fs::exists(rootDir)) return;
    fs::directory_entry entry(rootDir);//文件入口
    if(entry.status().type()==fs::file_type::directory) std::cout<<"this is a directory!"<<std::endl;
    fs::directory_iterator list(rootDir);
    for(auto& it:list)
    {
        std::cout<<it.path().filename()<<std::endl;
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