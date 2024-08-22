
/**
 * @file main.cpp
 * @author Erkhme Byambadorj (b.erkhme@gmail.com)
 * @brief ImGui based nodeChart
 * @version 0.1
 * @date 2024-08-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */


// #include <allegro5/allegro.h>
// #include <allegro5/allegro_primitives.h>
// #include <imgui/imgui.h>
// #include <imgui/backends/imgui_impl_allegro5.h>



#include <vector>
#include <list>

#include <string>
#include <sstream>
#include <math.h>


#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>

#include <HC_useful/useful.hpp>

#include "guiNC_constants.hpp"
#include "gui_nodeChart.hpp"


bool running_main = true;

bool opt__enable_grid = true;


static bool IsLegacyNativeDupe(ImGuiKey key) {
    return key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1;
}
std::vector<int>* update_keys(std::vector<int>* ptr_pressed_key=nullptr, size_t* ptr_num_keys_pressed=nullptr);
std::vector<int>* update_mouse();
bool isKeyPressed(int keyID, std::vector<int>* pressed_keys);

int main(int argc, char** argv) {

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_set_new_display_flags(!ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY* display = al_create_display(dim__main.x, dim__main.y);
    al_set_window_title(display, "al window title");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); //(void)io;
    ImGuiStyle& style = ImGui::GetStyle();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


    ImGui::StyleColorsDark();
    ImGui_ImplAllegro5_Init(display);

    ImVec4 clear_color = ImVec4(0.20f, 0.20f, 0.2f, 0.88f);

    static ImGuiWindowFlags window0_flags   = 0;
    static ImGuiWindowFlags window1_flags   = 0;
    static ImGuiChildFlags  child0_flags    = 0;

    window0_flags |= ImGuiWindowFlags_NoMove;
    window0_flags |= ImGuiWindowFlags_NoResize;
    window0_flags |= ImGuiWindowFlags_NoCollapse;
    window0_flags |= ImGuiWindowFlags_MenuBar;
    window0_flags |= ImGuiWindowFlags_NoTitleBar;
    window0_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    

    window1_flags |= ImGuiWindowFlags_NoResize;


    gNC::guiNodeChart proj0;


    static int cnt = 0;

    while(running_main) {
        static std::vector<int>* pressed_keys = update_keys();
        update_keys();

        ALLEGRO_EVENT al_event;
        while (al_get_next_event(queue, &al_event)) {
            ImGui_ImplAllegro5_ProcessEvent(&al_event);
            if(al_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running_main = false;
            if(al_event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
                ImGui_ImplAllegro5_InvalidateDeviceObjects();
                al_acknowledge_resize(display);
                ImGui_ImplAllegro5_CreateDeviceObjects();
            }
        }
        ImGui_ImplAllegro5_NewFrame();
        ImGui::NewFrame();
        //--------------------
        ImGui::SetNextWindowSizeConstraints(dim__main, dim__main);
        style.WindowRounding = 0;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 0));
        ImGui::Begin(" ", NULL, window0_flags);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(dim__main);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // proj0.screen_pos[0]=cnt;
        // proj0.screen_pos[1]=cnt;

        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open")) { }
                if(ImGui::MenuItem("Save")) { }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Program")) {
                if(ImGui::MenuItem("Close")){ running_main = false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if(ImGui::BeginTabBar("Tabs")) {
            if(ImGui::BeginTabItem("project 0")) {

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 0));
                assert(ImGui::BeginChild("tab0: contents", ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_NoMove));
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();
                
                if(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
                    if(isKeyPressed(655, pressed_keys)) {
                        proj0.setScreen_pos(io.MouseDelta.x, io.MouseDelta.y, 1);
                    }
                }

                ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
                ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
                if(canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
                if(canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
                ImVec2 canvas_p1 = ImVec2(canvas_p0.x+canvas_sz.x, canvas_p0.y+canvas_sz.y);


                static const int GRID_STEP = 64;
                if(opt__enable_grid) {
                    draw_list->PushClipRect(ImVec2(0, 20), ImVec2(proj0.screen_dim[0], proj0.screen_dim[1]), true);
                    for(float x=0; x<proj0.screen_dim[0]; x+=GRID_STEP)
                        draw_list->AddLine(ImVec2(x+(proj0.screen_pos[0] % GRID_STEP), 0+canvas_p0.y), ImVec2(x+(proj0.screen_pos[0]%GRID_STEP), canvas_p1.y), IM_COL32(200, 200, 200, 40));

                    for(float y=0; y<proj0.screen_dim[1]; y+=GRID_STEP)
                        draw_list->AddLine(ImVec2(canvas_p0.x, y+(proj0.screen_pos[1] % GRID_STEP)), ImVec2(canvas_p1.x, y+(proj0.screen_pos[1]%GRID_STEP)), IM_COL32(200, 200, 200, 40));
                    draw_list->PopClipRect();
                }

                if(cnt==0) {
                    proj0.NODE_create(100, 100, "node0", "desc0", "body0");
                    proj0.NODE_create( 30, 150, "node1", "desc1", "body1");
                    proj0.NODE_create(999, 500, "node2", "desc2", "body2");
                }
                style.WindowRounding = 15.0f;
                proj0.draw();

                
                ImGui::EndChild();

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        cnt++;

        ImGui::SetCursorPos(ImVec2(10, dim__main[1]-25));
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f/io.Framerate, io.Framerate);
        ImGui::End();
        //--------------------
        ImGui::Render();
        al_clear_to_color(al_map_rgba_f(clear_color.x*clear_color.w, clear_color.y*clear_color.w, clear_color.z*clear_color.w, clear_color.w));
        ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
        al_flip_display();
        std::cout.flush();
    }

    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}



std::vector<int>* update_keys(
    std::vector<int>* ptr_pressed_key,
    size_t* ptr_num_keys_pressed
) {
    static std::vector<int> pressed_keys;
    static size_t num_keys_pressed = 0;
    ImGuiKey start_key = (ImGuiKey)0;

    pressed_keys.clear();
    for(ImGuiKey key=start_key; key<ImGuiKey_NamedKey_END; key=(ImGuiKey)(key+1)) {
        if(IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;
        pressed_keys.push_back(key);
    }
    num_keys_pressed = pressed_keys.size();

    return &pressed_keys;
}
std::vector<int>* update_mouse() {
    static std::vector<int> pressed_mouse;
    static size_t num_mouse_pressed = 0;
    ImGuiMouseButton start_mouse = (ImGuiMouseButton)0;

    pressed_mouse.clear();
    
    return &pressed_mouse;
}
bool isKeyPressed(int keyID, std::vector<int>* pressed_keys) {
    if(pressed_keys->size() > 0 && searchVec<int>(*pressed_keys, keyID) != -1)
        return true;
    return false;
}
