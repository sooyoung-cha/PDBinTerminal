#include "Screen.hpp"
#include <ncurses.h>

#define NUM_PROTEIN 1

const float FOV = 90.0;
const float PI = 3.14159265359;

Screen::Screen(const int& width, const int& height, const bool& show_structure) {
    screen_width = width;
    screen_height = height;
    screen_show_structure = show_structure;
    aspect_ratio = (float)screen_width / screen_height;
    mScreen = new char[screen_height * screen_width];
    zoom_level = 1;
    clear_screen();

}

Screen::~Screen() {
    delete[] mScreen;
}

void Screen::set_protein(Protein* protein) {
    data = protein;
    initialize_colors();
}

void Screen::set_zoom_level(float zoom){
    if ((zoom_level + zoom > 0.5)&&(zoom_level + zoom < 2.0)){
        zoom_level += zoom;
    }
}


void Screen::initialize_colors() {
    initscr();
    if (!has_colors() || !can_change_color()) {
        endwin();
        std::cerr << "Terminal does not support colors!" << std::endl;
        return;
    }

    start_color();
    use_default_colors();

    int available_colors[] = {COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE};

    int color_index = 1;
    for (const auto& [chainID, _] : data->get_on_screen_atoms()) { 
        if (chain_colors.find(chainID) == chain_colors.end()) {  
            int assigned_color = available_colors[(color_index - 1) % 6];  
            init_pair(color_index, assigned_color, COLOR_BLACK);
            chain_colors[chainID] = color_index;
            color_index++;
        }
    }
}



void Screen::project() {
    float adjustedFOV = FOV / zoom_level;
    float fovRad = 1.0 / tan(adjustedFOV * 0.5 / 180.0 * PI);
    float focal_offset = 10.0f; // 소실점 오프셋

    screen_buffer_by_chain.clear();  // ✅ 이전 데이터를 초기화

    std::unordered_map<char, std::vector<char>> chain_screen; // ✅ 체인별 mScreen 저장소

    for (const auto& [chainID, chain_atoms] : data->get_on_screen_atoms()) {  
        if (!chain_atoms) continue; // ✅ nullptr 체크

        int num_atoms = data->get_num_chain_Atoms(chainID); // 해당 체인의 원자 개수

        // ✅ 체인별 화면 버퍼 초기화
        if (chain_screen.find(chainID) == chain_screen.end()) {
            chain_screen[chainID] = std::vector<char>(screen_width * screen_height, ' ');
        }

        for (int i = 0; i < num_atoms; ++i) {  
            float* position = chain_atoms[i].get_position();
            float x = position[0];
            float y = position[1];
            float z = position[2] + focal_offset; // ✅ z에 소실점 오프셋 적용

            char point = '*';
            if (screen_show_structure) {
                point = chain_atoms[i].get_structure();
            }

            if (z > 0) {  
                float projectedX = (x / z) * fovRad * aspect_ratio;
                float projectedY = (y / z) * fovRad;
                int screenX = (int)((projectedX + 1.0) * 0.5 * screen_width);
                int screenY = (int)((1.0 - projectedY) * 0.5 * screen_height);

                if (screenX >= 0 && screenX < screen_width && screenY >= 0 && screenY < screen_height) {
                    chain_screen[chainID][screenY * screen_width + screenX] = point;
                }
            }
        }
    }

    // ✅ 저장된 체인별 화면을 `mScreen`에 반영 (우선순위 적용 가능)
    for (const auto& [chainID, buffer] : chain_screen) {
        for (int i = 0; i < screen_width * screen_height; ++i) {
            if (buffer[i] != ' ') {  
                mScreen[i] = buffer[i];
            }
        }
    }

    // ✅ 체인별 색상 출력을 위해 저장
    screen_buffer_by_chain = chain_screen;

}


void Screen::clear_screen() {
    clear(); 
    std::fill(mScreen, mScreen + screen_height * screen_width, '_');
}

void Screen::print_screen() {
    // 더블 버퍼링을 위한 메모리 버퍼
    std::vector<std::vector<char>> buffer(screen_height, std::vector<char>(screen_width, ' '));

    // ✅ 메모리 버퍼에 먼저 그리기
    for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < screen_width; j++) {
            char point = mScreen[i * screen_width + j];

            // 현재 좌표에 있는 체인을 찾음
            char chainID = ' ';
            for (const auto& [cID, buffer] : screen_buffer_by_chain) {
                if (buffer[i * screen_width + j] == point) {
                    chainID = cID;
                    break;
                }
            }

            // 체인 색상 적용
            if (chain_colors.find(chainID) != chain_colors.end()) {
                int color_id = chain_colors[chainID];  
                buffer[i][j] = point;  // 메모리 버퍼에 저장
            } else {
                buffer[i][j] = point;
            }
        }
    }

    clear_screen();  // 기존 `mScreen` 초기화
    // ✅ 한번에 화면 출력
    for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < screen_width; j++) {
            char point = buffer[i][j];

            char chainID = ' ';
            for (const auto& [cID, buf] : screen_buffer_by_chain) {
                if (buf[i * screen_width + j] == point) {
                    chainID = cID;
                    break;
                }
            }

            if (chain_colors.find(chainID) != chain_colors.end()) {
                int color_id = chain_colors[chainID];
                attron(COLOR_PAIR(color_id));
                mvaddch(i, j, point);
                attroff(COLOR_PAIR(color_id));
            } else {
                mvaddch(i, j, point);
            }
        }
    }

    refresh();
}


void Screen::drawScreen() {
    project();
    print_screen();
}

bool Screen::handle_input(){
    bool keep_show = true;
    switch(getch()){
        // W, w (y 축 양의 이동)
        case 119:
        case 87:
            data->set_shift(0, 1, 0);
            break;
        // A, a (x 축 음의 이동)
        case 97:
        case 65:
            data->set_shift(-1, 0, 0);
            break;
        // S, s (y 축 음의 이동)
        case 115:
        case 83:
            data->set_shift(0, -1, 0);
            break;      
        // D, d (x 축 양의 이동)
        case 100:
        case 68:
            data->set_shift(1, 0, 0);
            break;

        // X, x (x 축 중심 회전)
        case 120:
        case 88:
            data->set_rotate(1, 0, 0);
            break;  
        // Y, y (y 축 중심 회전)
        case 121:
        case 89:
            data->set_rotate(0, 1, 0);
            break;  
        // Z, z (z 축 중심 회전)
        case 122:
        case 90:
            data->set_rotate(0, 0, 1);
            break;  

        // R, R (줌 인)
        case 114:
        case 82:
            set_zoom_level(-0.05);
            break;   
        // F, f (줌 아웃)
        case 102:
        case 70:
            set_zoom_level(0.05);
            break;   

        // Q, q
        case 113:
        case 81:
            keep_show = false;
            break;

        default:
            break;       
    }     

    return keep_show;
}