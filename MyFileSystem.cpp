#include <iostream>
#include"mfs.h"
#define GLFW_INCLUDE_NONE
// #include"glfw-3.3.6\include\GLFW\glfw3.h"
#include"GLFW/glfw3.h"
#include"dstring/dstring.h"

#include <codecvt>
#include <locale>

// using convert_t = std::codecvt_utf8<wchar_t>;
// std::wstring_convert<convert_t, wchar_t> strconverter;

void test()
{
    std::string s1="asdfasdf";
    std::wstring s2=L"我的fhuasdfd";
    std::cout<<s2.c_str()<<std::endl;
    // std::cout<<strconverter.to_bytes(s2)<<std::endl;
    // std::wcout<<strconverter.from_bytes(s1)<<std::endl;
}



int main(int argc, char** argv)
{
    // glfwInit();
    // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // auto window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
    // while (!glfwWindowShouldClose(window)) 
    // {
    //     glfwPollEvents();
    // }
    // glfwDestroyWindow(window);
    // glfwTerminate();
    test();return 0;
    std::wstring str=L"asdfAA";
    // auto a=Duality::toUpper<std::string>(str);
    std::wcout<<Duality::toUpper<std::wstring>(str)<<std::endl;return 0;
    std::cout<<"asdf"<<std::endl;
    // directoryTest("/home/number/approot/");
    errortest("D:/approot");
}
