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

void Screen::set_protein(Protein* protein) {
  data = protein;
}

void Screen::set_zoom_level(float zoom){
    if ((zoom_level + zoom > 0.5)&&(zoom_level + zoom < 1.5)){
        zoom_level += zoom;
    }
}

void Screen::project() {
    float adjustedFOV = FOV / zoom_level;
    float fovRad = 1.0 / tan(adjustedFOV * 0.5 / 180.0 * PI);
    std::unordered_map<char, std::unordered_map<int, Atom>> atoms = data->get_on_screen_atoms();  

    float focal_offset = 10.0f; // 소실점을 뒤로 이동하기 위한 오프셋 추가

    clear_screen();
    std::cout << "On Screen Atoms: " << atoms.size() << std::endl;

    for (const auto& [chainID, chain_atoms] : atoms) {  // 체인별 순회
        for (const auto& [idx, atom] : chain_atoms) {  // 잔기별 순회
            float* position = atom.get_position();  // 원자 좌표 가져오기
            float x = position[0];
            float y = position[1];
            float z = position[2] + focal_offset; // z에 소실점 오프셋 적용

            char point = '*';
            if (screen_show_structure) {
                point = atom.get_structure();
            }

            if (z > 0) { // z 값이 양수일 때만 투영 계산
                float projectedX = (x / z) * fovRad * aspect_ratio;
                float projectedY = (y / z) * fovRad;
                int screenX = (int)((projectedX + 1.0) * 0.5 * screen_width);
                int screenY = (int)((1.0 - projectedY) * 0.5 * screen_height);

                if (screenX >= 0 && screenX < screen_width && screenY >= 0 && screenY < screen_height) {
                    mScreen[screenY * screen_width + screenX] = point;
                }
            }
        }
    }
    std::cout << "Projecting done" << std::endl;
}

void Screen::clear_screen() {
    clear(); 
    std::fill(mScreen, mScreen + screen_height * screen_width, '_');
}

void Screen::print_screen() {
    for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < screen_width; j++) {
            mvaddch(i, j, mScreen[i * screen_width + j]); 
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