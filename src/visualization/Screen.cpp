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
    zoom_level = 0.75;
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
    if ((zoom_level + zoom > 0.5)&&(zoom_level + zoom < 1)){
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

void Screen::drawLine(std::vector<char>& buffer, int x1, int y1, int x2, int y2, int width, float z1, float z2) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        float depthFactor = (z1 + z2) * 0.5f;  // 두 점의 평균 깊이
        char pixelChar = ' ';

        if (depthFactor < 5.0) pixelChar = '#';  // 가까운 선 (짙음)
        else if (depthFactor < 10.0) pixelChar = '@';
        else if (depthFactor < 15.0) pixelChar = '%';
        else if (depthFactor < 20.0) pixelChar = '*';
        else if (depthFactor < 30.0) pixelChar = ':';
        else pixelChar = '.';  // 먼 선 (연함)

        if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < buffer.size() / width) {
            buffer[y1 * width + x1] = pixelChar;
        }

        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}


void Screen::project() {
    float adjustedFOV = FOV / zoom_level;
    float fovRad = 1.0 / tan(adjustedFOV * 0.5 / 180.0 * PI);
    float focal_offset = 10.0f; // 소실점 오프셋

    screen_buffer_by_chain.clear();
    std::unordered_map<char, std::vector<char>> chain_screen;

    for (const auto& [chainID, chain_atoms] : data->get_on_screen_atoms()) {  
        if (!chain_atoms) continue; // nullptr 체크
        int num_atoms = data->get_num_chain_Atoms(chainID); 

        if (chain_screen.find(chainID) == chain_screen.end()) {
            chain_screen[chainID] = std::vector<char>(screen_width * screen_height, ' ');
        }

        // 선 그리기
        int prevScreenX = -1, prevScreenY = -1; // 이전 원자의 좌표 저장
        float prevZ = -1.0f;
        for (int i = 0; i < num_atoms; ++i) {  
            float* position = chain_atoms[i].get_position();
            float x = position[0];
            float y = position[1];
            float z = position[2] + focal_offset;
            if (z > 0) {  
                float projectedX = (x / z) * fovRad * aspect_ratio;
                float projectedY = (y / z) * fovRad;
                int screenX = (int)((projectedX + 1.0) * 0.5 * screen_width);
                int screenY = (int)((1.0 - projectedY) * 0.5 * screen_height);

                bool isInsideScreen = (screenX >= 0 && screenX < screen_width && screenY >= 0 && screenY < screen_height);

                // 선 연결 (이전 원자가 존재하는 경우)
                if (prevScreenX != -1 && prevScreenY != -1 && prevZ != -1.0f) {
                    if (isInsideScreen || (prevScreenX >= 0 && prevScreenX < screen_width && prevScreenY >= 0 && prevScreenY < screen_height)) {
                        // 깊이(z 값)를 추가하여 선을 그림
                        drawLine(chain_screen[chainID], prevScreenX, prevScreenY, screenX, screenY, screen_width, prevZ, z);
                    }
                }

                // 화면 밖이어도 좌표를 업데이트하여 연결 유지
                prevScreenX = screenX;
                prevScreenY = screenY;
                prevZ = z;
            }
        }
        
        // structure 그리기
        for (int i = 0; i < num_atoms; ++i) {  
            if (chain_atoms[i].get_structure() != '*'){
                float* position = chain_atoms[i].get_position();
                float x = position[0];
                float y = position[1];
                float z = position[2] + focal_offset; // z에 소실점 오프셋 적용

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
                        if (screen_show_structure) {
                            chain_screen[chainID][screenY * screen_width + screenX] = chain_atoms[i].get_structure();
                        }   
                        else {
                            chain_screen[chainID][screenY * screen_width + screenX] = '*';
                        }                 
                    }
                }
            }
        }
    }

    // ✅ 최종적으로 `mScreen`에 반영
    for (const auto& [chainID, buffer] : chain_screen) {
        for (int i = 0; i < screen_width * screen_height; ++i) {
            if (buffer[i] != ' ') {  
                mScreen[i] = buffer[i];
            }
        }
    }

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
            set_zoom_level(0.002);
            break;   
        // F, f (줌 아웃)
        case 102:
        case 70:
            set_zoom_level(-0.002);
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