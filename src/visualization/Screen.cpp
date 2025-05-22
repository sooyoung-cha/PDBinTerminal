#include "Screen.hpp"

#define NUM_PROTEIN 1

const float FOV = 90.0;
const float PI = 3.14159265359f;

Screen::Screen(const int& width, const int& height, const bool& show_structure, const std::string& mode) {
    screen_width = width;
    screen_height = height;
    screen_show_structure = show_structure;
    screen_mode = mode;
    aspect_ratio = (float)screen_width / screen_height;
    zoom_level = 3;
}

Screen::~Screen() {
}

void Screen::set_protein1(Protein* protein) {
    data1 = protein;
}

void Screen::set_protein2(Protein* protein) {
    data2 = protein;
}

char Screen::getPixelCharFromDepth(float z) {
    if (z < 9.30) return '#';
    else if (z < 9.50) return '@';
    else if (z < 9.70) return '%';
    else if (z < 9.90) return '*';
    else if (z < 10.10) return '^';
    else if (z < 10.30) return '.';
    else return '_';
}

void Screen::drawLine(std::vector<RenderPoint>& points,
                      int x1, int y1, int x2, int y2,
                      float z1, float z2, char chainID, char structure) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = std::max(abs(dx), abs(dy));
    if (steps == 0) steps = 1;

    float xIncrement = (float)dx / steps;
    float yIncrement = (float)dy / steps;
    float zIncrement = (z2 - z1) / steps;

    float x = x1;
    float y = y1;
    float z = z1;

    for (int i = 0; i <= steps; ++i) {
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);

        if (ix >= 0 && ix < screen_width && iy >= 0 && iy < screen_height) {
            points.push_back({ix, iy, z, getPixelCharFromDepth(z), chainID, structure});
        }

        x += xIncrement;
        y += yIncrement;
        z += zIncrement;
    }
}

void Screen::assign_colors_to_points(std::vector<RenderPoint>& points) {
    initscr();
    if (!has_colors() || !can_change_color()) {
        endwin();
        std::cerr << "Terminal does not support colors!" << std::endl;
        return;
    }

    start_color();
    use_default_colors();

    // rainbow 팔레트 (순서 중요)
    int rainbow[] = {
        COLOR_RED, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN, COLOR_BLUE, COLOR_MAGENTA
    };
    int rainbow_size = sizeof(rainbow) / sizeof(int);

    if (screen_mode == "default") {
        init_pair(1, COLOR_GREEN, -1);
        for (auto& pt : points) {
            pt.color_id = 1;
        }
    }

    else if (screen_mode == "chain") {
        std::unordered_map<char, int> local_chain_colors;
        int color_index = 1;
        int available_colors[] = {
            COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
        };
        int num_colors = sizeof(available_colors) / sizeof(int);

        for (auto& pt : points) {
            char cID = pt.chainID;
            if (local_chain_colors.find(cID) == local_chain_colors.end()) {
                int col = available_colors[(color_index - 1) % num_colors];
                init_pair(color_index, col, -1);
                local_chain_colors[cID] = color_index++;
            }
            pt.color_id = local_chain_colors[cID];
        }

        chain_colors = std::move(local_chain_colors);  // 저장
    }

    else if (screen_mode == "rainbow") {
        int total = points.size();

        // PDB 스타일 rainbow 색상 ID 목록 (N → C 방향)
        int num_colors = rainbow_ids.size();

        // COLOR_PAIR 등록
        for (int i = 0; i < num_colors; ++i) {
            init_pair(i + 1, rainbow_ids[i], -1);
        }

        // 포인트에 색상 지정 (등간격으로 할당)
        for (int i = 0; i < total; ++i) {
            int color_index = (i * num_colors) / total;
            color_index = std::min(color_index, num_colors - 1);
            points[i].color_id = color_index + 1;
        }
    }

    else {
        std::cerr << "Unknown mode: " << screen_mode << std::endl;
    }
}

void Screen::project() {
    float adjustedFOV = FOV / zoom_level;
    float fovRad = 1.0 / tan(adjustedFOV * 0.5 / 180.0 * PI);
    float focal_offset = 10.0f;

    std::vector<RenderPoint> finalPoints1;
    std::vector<RenderPoint> finalPoints2;

    // project dots and connect them into line
    for (const auto& [chainID, chain_atoms] : data1->get_atoms()) {
        if (chain_atoms.size() == 0) continue;

        int num_atoms = data1->get_chain_length(chainID);
        std::vector<RenderPoint> chainPoints1;

        int prevScreenX = -1, prevScreenY = -1;
        float prevZ = -1.0f;

        for (int i = 0; i < num_atoms; ++i) {
            float* position = chain_atoms[i].get_position();
            float x = position[0];
            float y = position[1];
            float z = position[2] + focal_offset;
            char structure = chain_atoms[i].get_structure();

            float projectedX = (x / z) * fovRad;
            float projectedY = (y / z) * fovRad;
            int screenX = (int)((projectedX + 1.0) * 0.5 * screen_width);
            int screenY = (int)((1.0 - projectedY) * 0.5 * screen_height);

            if (prevScreenX != -1 && prevScreenY != -1) {
                drawLine(chainPoints1, prevScreenX, prevScreenY, screenX, screenY, prevZ, z, chainID, structure);
            }
            
            if (screenX >= 0 && screenX < screen_width && screenY >= 0 && screenY < screen_height) {
                chainPoints1.push_back({screenX, screenY, z, getPixelCharFromDepth(z), chainID, structure});
            }

            prevScreenX = screenX;
            prevScreenY = screenY;
            prevZ = z;
        }

        finalPoints1.insert(finalPoints1.end(), chainPoints1.begin(), chainPoints1.end());
    }
    for (const auto& [chainID, chain_atoms] : data2->get_atoms()) {
        if (chain_atoms.size() == 0) continue;

        int num_atoms = data2->get_chain_length(chainID);
        std::vector<RenderPoint> chainPoints2;

        int prevScreenX = -1, prevScreenY = -1;
        float prevZ = -1.0f;

        for (int i = 0; i < num_atoms; ++i) {
            float* position = chain_atoms[i].get_position();
            float x = position[0];
            float y = position[1];
            float z = position[2] + focal_offset;
            char structure = chain_atoms[i].get_structure();

            float projectedX = (x / z) * fovRad;
            float projectedY = (y / z) * fovRad;
            int screenX = (int)((projectedX + 1.0) * 0.5 * screen_width);
            int screenY = (int)((1.0 - projectedY) * 0.5 * screen_height);

            if (prevScreenX != -1 && prevScreenY != -1) {
                drawLine(chainPoints2, prevScreenX, prevScreenY, screenX, screenY, prevZ, z, chainID, structure);
            }
            
            if (screenX >= 0 && screenX < screen_width && screenY >= 0 && screenY < screen_height) {
                chainPoints2.push_back({screenX, screenY, z, getPixelCharFromDepth(z), chainID, structure});
            }


            prevScreenX = screenX;
            prevScreenY = screenY;
            prevZ = z;
        }

        finalPoints2.insert(finalPoints2.end(), chainPoints2.begin(), chainPoints2.end());
    }

    assign_colors_to_points(finalPoints1); 
    assign_colors_to_points(finalPoints2); 

    for (const auto& pt : finalPoints1) {
        int idx = pt.y * screen_width + pt.x;
        if (pt.z < screenPixels[idx].depth) {
            screenPixels[idx].depth = pt.z;
            screenPixels[idx].pixel = pt.pixel;
            screenPixels[idx].color_id = pt.color_id;
        }
    }
    for (const auto& pt : finalPoints2) {
        int idx = pt.y * screen_width + pt.x;
        if (pt.z < screenPixels[idx].depth) {
            screenPixels[idx].depth = pt.z;
            screenPixels[idx].pixel = pt.pixel;
            screenPixels[idx].color_id = pt.color_id;
        }
    }
}


void Screen::print_screen() {
    clear();  // ncurses 화면 초기화

    for (int i = 0; i < screen_height; ++i) {
        for (int j = 0; j < screen_width; ++j) {
            int idx = i * screen_width + j;
            const ScreenPixel& px = screenPixels[idx];

            if (px.color_id > 0) {
                attron(COLOR_PAIR(px.color_id));
                mvaddch(i, j, px.pixel);
                attroff(COLOR_PAIR(px.color_id));
            } else {
                mvaddch(i, j, px.pixel);
            }
        }
    }

    refresh();  // 출력 적용
}

void Screen::drawScreen() {
    clear_screen();
    project();
    print_screen();
}

void Screen::clear_screen() {
    clear(); 
    screenPixels.assign(screen_width * screen_height, ScreenPixel());
}

void Screen::set_zoom_level(float zoom){
    if ((zoom_level + zoom > 0.5)&&(zoom_level + zoom < 10)){
        zoom_level += zoom;
    }
}

bool Screen::handle_input(){
    bool keep_show = true;
    switch(getch()){
        // struct 1
        case 49:
            structNum = 1;
            break;
        // strcut 2
        case 50:
            structNum = 2;
            break;
        // both (default)
        case 48:
            structNum = 0;
            break;
        // W, w (y 축 양의 이동)
        case 119:
        case 87:
            if (structNum == 1) {
                data1->set_shift(0, 0.1, 0);
            } else if (structNum == 2) {
                data2->set_shift(0, 0.1, 0);
            } else {
                data1->set_shift(0, 0.1, 0);
                data2->set_shift(0, 0.1, 0);
            }
            break;
        // A, a (x 축 음의 이동)
        case 97:
        case 65:
            if (structNum == 1) {
                data1->set_shift(-0.1, 0, 0);
            } else if (structNum == 2) {
                data2->set_shift(-0.1, 0, 0);
            } else {
                data1->set_shift(-0.1, 0, 0);
                data2->set_shift(-0.1, 0, 0);
            }
            break;
        // S, s (y 축 음의 이동)
        case 115:
        case 83:
            if (structNum == 1) {
                data1->set_shift(0, -0.1, 0);
            } else if (structNum == 2) {
                data2->set_shift(0, -0.1, 0);
            } else {
                data1->set_shift(0, -0.1, 0);
                data2->set_shift(0, -0.1, 0);
            }
            break;      
        // D, d (x 축 양의 이동)
        case 100:
        case 68:
        if (structNum == 1) {
            data1->set_shift(0.1, 0, 0);
        } else if (structNum == 2) {
            data2->set_shift(0.1, 0, 0);
        } else {
            data1->set_shift(0.1, 0, 0);
            data2->set_shift(0.1, 0, 0);
        }
            break;

        // X, x (x 축 중심 회전)
        case 120:
        case 88:
        if (structNum == 1) {
            data1->set_rotate(1, 0, 0);
        } else if (structNum == 2) {
            data2->set_rotate(1, 0, 0);
        } else {
            data1->set_rotate(1, 0, 0);
            data2->set_rotate(1, 0, 0);
        }
            break;  
        // Y, y (y 축 중심 회전)
        case 121:
        case 89:
        if (structNum == 1) {
            data1->set_rotate(0, 1, 0);
        } else if (structNum == 2) {
            data2->set_rotate(0, 1, 0);
        } else {
            data1->set_rotate(0, 1, 0);
            data2->set_rotate(0, 1, 0);
        }
            break;  
        // Z, z (z 축 중심 회전)
        case 122:
        case 90:
        if (structNum == 1) {
            data1->set_rotate(0, 0, 1);
        } else if (structNum == 2) {
            data2->set_rotate(0, 0, 1);
        } else {
            data1->set_rotate(0, 0, 1);
            data2->set_rotate(0, 0, 1);
        }
            break;  

        // R, R (줌 인)
        case 114:
        case 82:
            set_zoom_level(0.2);
            break;   
        // F, f (줌 아웃)
        case 102:
        case 70:
            set_zoom_level(-0.2);
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