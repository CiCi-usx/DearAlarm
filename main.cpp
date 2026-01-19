#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// 1. C 标准库
#include <cstdio>
#include <ctime>

// 2. C++ 标准库 (按字母排序)
#include <chrono>
#include <iomanip>
#include <sstream>

// 3. 第三方依赖库 (外部库)
#include <GLFW/glfw3.h>

// 4. 项目内组件/插件 (ImGui 后端)
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main(int, char**) {
    ma_engine engine;
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        // 失败处理
        printf("Failed to initialize miniaudio engine.\n");
        return 1;
    }

    // 1. 初始化 GLFW
    if (!glfwInit()){
        printf("Failed to initialize GLFW.\n");
        return 1;
    }
    
    // 高分屏适配支持1
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    // 设置 OpenGL 版本 (3.3 Core Profile)
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. 创建窗口
    GLFWwindow* window = glfwCreateWindow(600, 400, "DearAlarm - My First GUI", nullptr, nullptr);
    if (window == nullptr) return 1;

    int width, height, channels;
    unsigned char* pixels = stbi_load("app_icon.png", &width, &height, &channels, 4); 
    if (pixels) {
        GLFWimage images[1];
        images[0].width = width;
        images[0].height = height;
        images[0].pixels = pixels;
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(pixels);
    } else {
        printf("Failed to load icon! Check if app_icon.png exists.\n");
    }

    // 2. 创建窗口后修改：
    GLFWimage images[1];
    // 改用 .png 加载，这样 stbi_load 100% 没问题
    images[0].pixels = stbi_load("app_icon.png", &images[0].width, &images[0].height, 0, 4); 
    if (images[0].pixels) {
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 开启垂直同步

    // 3. 初始化 ImGui 上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // 高分屏适配支持2
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2.0f; // 2K 屏建议设置在 1.5f - 2.0f 之间
    (void)io;

    // 设置风格
    ImGui::StyleColorsDark();

    // 4. 设置后端绑定
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 闹钟状态变量
    int hour = 8;
    int minute = 0;
    bool alarm_active = false;
    ma_sound myAlarmSound;
    bool is_playing = false;

    // 5. 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // 启动 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- 闹钟界面代码开始 ---
        // --- 闹钟界面逻辑开始 ---
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("DearAlarm", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

        // 1. 获取并显示当前系统时间
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* local_tm = std::localtime(&now_time);

        ImGui::Text("Current Time: %02d:%02d:%02d", local_tm->tm_hour, local_tm->tm_min, local_tm->tm_sec);
        ImGui::Separator();

        // 2. 闹钟设定 UI
        ImGui::InputInt("Set Hour", &hour);
        ImGui::InputInt("Set Minute", &minute);

        if (ImGui::Button(alarm_active ? "ALARM ON (Click to Stop)" : "START ALARM")) {
            alarm_active = !alarm_active;
        }

        // 3. 核心监听逻辑：比对时间
        if (alarm_active) {
            static bool already_played = false;
            if (local_tm->tm_hour == hour && local_tm->tm_min == minute && !already_played) {
                const char* alarm_path = u8"alarm.wav";
                if (!already_played) {
                    // 时间到了！
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "!!! BEEP BEEP BEEP !!!");
                    ma_engine_play_sound(&engine, alarm_path, NULL);
                    // 这里以后就是触发音频播放的地方
                    is_playing = true;
                    already_played = true;
                }
            } else {
                ImGui::Text("Status: Waiting...");
                // 时间没到或者闹钟没开时，重置状态位以便下次响铃
                // 只有当分钟走过去后才重置，防止同一分钟内反复响
                if (local_tm->tm_min != minute) {
                    already_played = false;
                }
            }
        }

        // 停止按钮逻辑
        if (ImGui::Button("STOP SOUND")) {
            // 简单粗暴但有效的方法：直接把引擎重启，或者重置
            ma_engine_stop(&engine);
            ma_engine_start(&engine); // 重启引擎，音乐会消失，但引擎恢复待命
            is_playing = false;
        }

        ImGui::End();
        // --- 闹钟界面代码结束 ---

        // 渲染
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 6. 清理
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    // 销毁音频引擎
    ma_engine_uninit(&engine);

    return 0;
}
