#include"VulkanGraphicsBase.h"
#include"dstring/dstring.h"
#include"fileManager.h"
#include"time/mytime.h"
#include<thread>

class BookManager: public VulkanGraphicsBase
{
public:
    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    MyFileManager::MyFileManagerInfo_std fmInfo_std;

    // MyFileManager::DeepSearcher deepSearcher;
    MyFileManager::DeepSearcher_std deepSearcher;

    // std::vector<std::thread> threads;

    static void openfile(const std::string& filename)
    {
        
        system((std::string("open ")+filename).c_str());
    }
    // template<class STRING_TYPE>
    void treeSearch_std()
    {
        // std::cout<<"enter "<<std::endl;
        for(auto& mf:fmInfo_std.filevec)
        {   
            std::cout<<"start proc "<<std::endl;
            auto key_wstring=Duality::toLower<std::wstring>(strconverter.from_bytes(std::string(searchString)));

            auto path=mf.path();
            auto path_wstring=Duality::toLower(path.wstring());
            // auto filename_wstring=path.wstring();
            if((mf.is_directory() || path_wstring.find(key_wstring)!=path_wstring.npos) && path.wstring().find(L".")!=0)
            // if((mf.is_directory() || Duality::Dstring(mf.path().filename().string()).toUpper().find(Duality::Dstring(searchString).toUpper())!=std::string::npos) && std::string(mf.path().filename().string()).find(".")!=0)
            {
                if(!mf.is_directory())
                {
                    if(ImGui::Button(strconverter.to_bytes(path.filename().wstring()).c_str()))
                    // if(ImGui::Button(mf.path().filename().string().c_str()))
                    {
                        // auto filedir=fmInfo_std.currentDir/mf.path().filename();
                        auto filedir=mf.path();
#ifdef __linux__
                        pid_t pid=fork();
                        if(pid==-1) perror("fork");
                        if(!pid)
                        {
                            int ret=execlp("xdg-open","xdg-open",filedir.c_str(),NULL);
                            if(ret==-1) perror("execvp");
                        }
#else
                        std::thread t(openfile,filedir.string());
                        t.detach();
#endif
                    }
                }
                else 
                // else if(ImGui::TreeNode(mf.path().filename().string().c_str()))
                {
                    if(ImGui::TreeNode(strconverter.to_bytes(path.filename().wstring()).c_str()))    
                    {
                        auto olddir=fmInfo_std.currentDir;
                        // fmInfo_std.setCurrentDir((olddir/mf.path().filename()).string());
                        fmInfo_std.setCurrentDir(path);

                        treeSearch_std();
                        // fmInfo_std.setCurrentDir(olddir.string());
                        fmInfo_std.setCurrentDir(olddir);
                        // std::cout<<"end gui frame "<<std::endl<<std::endl;
                        ImGui::TreePop();
                    }

                }
            }
        }
    }
    
    virtual void drawFrame()
    {
        guiFrame();
        ImDrawData* drawData=ImGui::GetDrawData();

        //!等待当前帧对应fence<-当前帧的上一帧提交的渲染结束
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        //!上面的wait成功后才能取图片，因为只有渲染结束后才能取。问题：为什么不需要在present处设置fence？
        VkResult result = vkAcquireNextImageKHR(device, swapchainStruct.swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) 
        {
            recreateSwapChain();
            return;
        } 
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        } 
        //!等待取回的图片对应fence,之所以有这一步，因为即使上一帧的渲染结束了，但取回的图片对应的帧的渲染有可能没结束，相当于最坏情况要等两个fence。待研究！
        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) 
        {
            vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }

        //!这里说明实际的handle只有两个，而某张图片对应的帧有可能会发生变化
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];
        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        //重置当前帧对应fence
        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        {
            VkCommandBufferBeginInfo beginInfo={};
            beginInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags|=VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            vkBeginCommandBuffer(commandBuffers[imageIndex],&beginInfo);

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapchainStruct.swapChainFramebuffers[imageIndex];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapchainStruct.swapChainExtent;
            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();
            vkCmdBeginRenderPass(commandBuffers[imageIndex],&renderPassInfo,VK_SUBPASS_CONTENTS_INLINE);
        }
        ImGui_ImplVulkan_RenderDrawData(drawData,commandBuffers[imageIndex]);
        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        vkEndCommandBuffer(commandBuffers[imageIndex]);

        //!当前帧对应的渲染结束前保持当前帧对应的fence锁住
        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapchainStruct.swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) 
        {
            framebufferResized = false;
            recreateSwapChain();
        } 
        else if (result != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void guiFrame()
    {
        // std::cout<<"begin gui frame "<<std::endl;

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto io=ImGui::GetIO();
        ImGui::PushFont(font1);
        ImFontAtlas* fontAtlas=io.Fonts;
        auto currentFont=fontAtlas->Fonts[0];
        ImGui::StyleColorsClassic();
        static bool searchState=false;

        bool t_open;
        
        // ImGui::StyleColorsLight();
        // ImGui::StyleColorsDark();
        
        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::TreeNode("Font Scaler"))
            {
                ImGui::Begin("Font scaler");
                ImGui::DragFloat("Font scale", &currentFont->Scale, 0.005f, 0.3f, 2.0f, "%.1f");
                ImGui::End();
                ImGui::TreePop();
            }
            ImGui::EndMainMenuBar();
        }
        static bool firstFrame=true;
        if(firstFrame)
        {
            ImGui::SetNextWindowSize(ImVec2{io.DisplaySize.x*0.5f,io.DisplaySize.y*0.95f});
            firstFrame=false;
        }
        ImGui::Begin("filebrowser",&t_open,ImGuiWindowFlags_MenuBar);
        // std::cout<<io.DisplaySize.x<<","<<io.DisplaySize.y<<std::endl;
        ImGui::PushItemWidth(io.DisplaySize.x*0.4);
        ImGui::InputText("KeyWord",searchString,20);
        // ImGui::SameLine();
        ImGui::InputText("RootDir",rootDir,100);
        ImGui::PopItemWidth();
        
        ImGui::Checkbox("search",&searchState);
        if(ImGui::TreeNode("filebrower"))
        {
            treeSearch_std();
            ImGui::TreePop();
        }
        ImGui::End();


        
        if(searchState)
        {
            static bool searchover=false;
            // static int targetNum=0;

            // ImGui::SetNextWindowSize(ImVec2{io.DisplaySize.x*0.5,io.DisplaySize.y*0.95});
            ImGui::Begin("filesearcher",&t_open,ImGuiWindowFlags_MenuBar);
            // searchAllFiles();
            static char keyword[50]="pdf";
#ifdef __linux__
            static char searchdir[100]="/home/number/Nutstore Files/Nutstore/";
#else
            static char searchdir[100]="D:\\approot\\";
#endif
            ImGui::PushItemWidth(io.DisplaySize.x*0.4);
            
            // ImGui::Text(keyword);
            // ImGui::Separator();
            
            if(ImGui::InputText("keyword",keyword,IM_ARRAYSIZE(keyword)))
                searchover=false;
            if(ImGui::InputText("fromDir",searchdir,IM_ARRAYSIZE(searchdir)))
            {
                // if (ImGui::IsItemHovered())

                searchover=false;
            }
            ImGui::Separator();
            ImGui::Text(("found "+std::to_string(deepSearcher.filevec.size())+" targets").c_str());
            
            //! todo
            if(ImGui::IsItemFocused())
            {
                
                // ImVec2 min=ImGui::GetItemRectMin();
                // ImVec2 max=ImGui::GetItemRectMax();
                // ImGui::BeginTooltip();

                // // std::cout<<"item rect"<<min.x<<','<<min.y<<std::endl;//<<" "<<max.x<<','<<max.y<<std::endl;
                // // ImGui::SetCursorPos({(min.x+max.x)/2,(min.y+max.y)/2});
                // ImGui::SetCursorPos(min);
                // ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                // ImGui::TextUnformatted(searchdir);
                // ImGui::PopTextWrapPos();
                // ImGui::EndTooltip();                
            }

                
            
            if(!searchover)
            {
                std::cout<<"deep search "<<searchdir<<" for "<<keyword<<std::endl;
                deepSearcher.deepSearch(searchdir,keyword);
                searchover=true;
            }
            ImGui::Separator();
            if(ImGui::TreeNode("targets"))
            {
                for(auto& x:deepSearcher.filevec)
                {
                    if(ImGui::Button(x.path().filename().string().c_str()))
                    {
#ifdef __linux__                 
                        // auto filedir=x.path+"/"+x.filename;
                        pid_t pid=fork();
                        if(pid==-1) perror("fork");
                        if(!pid)
                        {
                            int ret=execlp("xdg-open","xdg-open",x.path().c_str(),NULL);
                            if(ret==-1) perror("execvp");
                        }
#endif
                    }
                    ImGui::SameLine();
                    HelpMarker(x.path().string().c_str());
                    
                }
                ImGui::TreePop();
         
            }
   
            ImGui::PopItemWidth();
            

            ImGui::End();
            
        }
        ImGui::PopFont();
        // if(ImGui::TreeNode("demoWindow"))
        // {
            // ImGui::ShowDemoWindow();
        //     ImGui::TreePop();
        // }

        
        ImGui::Render();
        // std::cout<<"end gui frame "<<std::endl<<std::endl;

    }
    // std::string searchString;
    // std::string rootDir;
    char searchString[100]="pdf";
#ifdef __linux__
    char rootDir[100]="/home/number/Nutstore Files/Nutstore/";
#else
    char rootDir[100]="D:\\";
#endif
    void mainLoop()
    {
        // searchString.resize(100);
        // rootDir.resize(100);
        // rootDir="/home/number/";
        // searchString="pdf";
        
        while (!glfwWindowShouldClose(window)) 
        {
            // fmInfo.setCurrentDir(rootDir);
            fmInfo_std.setCurrentDir(rootDir);
            auto framestart=mytime::now();
            glfwPollEvents();
            drawFrame();
            auto frameend=mytime::now();
            auto dt=mytime::getDuration(framestart,frameend);
            if(dt<FrameInterval)
            {
                mytime::sleep(FrameInterval-dt);
            }
        }

        vkDeviceWaitIdle(device);
    }
    //xdg-open
    
    
};

int main()
{
    // Duality::Dstring ds("Asdf");
    // std::cout<<ds<<std::endl;
    // std::cout<<ds.toLower()<<std::endl;
    // std::cout<<ds.toUpper()<<std::endl;


    BookManager app;
    app.setWindowSize(1440,1096);
    app.run();
}