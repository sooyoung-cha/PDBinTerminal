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
    zoom_level1 = 3;
    zoom_level2 = 3;
}

Screen::~Screen() {
    for (Protein* p : data) {
        delete p;
    }
    data.clear(); 
}

void Screen::set_protein(const std::string& in_file, const std::string& target_chains, const bool& show_structure) {
    Protein* protein = new Protein(in_file, target_chains, show_structure);
    data.push_back(protein);
}

void Screen::normalize_proteins(){
    for (auto* p : data) {
        BoundingBox bb = p->get_bounding_box();
        global_bb = global_bb + bb; // + 연산자 오버로드를 통한 통합
    }

    for (auto* p : data) {
        p->set_bounding_box(global_bb);
        p->load_data();
    }
}

void Screen::set_utmatrix(int protein_idx, const std::string& umatrix, const std::string& tmatrix) {
    float matrix1[3][3];
    size_t start = 0;
    size_t end;
    int idx = 0;
    while ((end = umatrix.find(',', start)) != std::string::npos && idx < 9) {
        matrix1[idx / 3][idx % 3] = std::stof(umatrix.substr(start, end - start));
        start = end + 1;
        idx++;
    }

    if (idx < 9)
        matrix1[idx / 3][idx % 3] = std::stof(umatrix.substr(start));

    size_t start2 = 0;
    size_t end2;
    int idx2 = 0;
    float (matrix2)[3];
    while ((end2 = tmatrix.find(',', start)) != std::string::npos && idx < 3) {
        matrix2[idx2++] = std::stof(tmatrix.substr(start2, end2 - start));
        start2 = end2 + 1;
    }

    if (idx2 < 3)
        matrix2[idx2++] = std::stof(tmatrix.substr(start2));

    data[protein_idx]->do_rotation(matrix1);
    //TODO
    // data2->do_shift(matrix2);
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

void Screen::assign_colors_to_points(std::vector<RenderPoint>& points, int protein_idx) {
    initscr();
    if (!has_colors() || !can_change_color()) {
        endwin();
        std::cerr << "Terminal does not support colors!" << std::endl;
        return;
    }

    start_color();
    use_default_colors();

    if (screen_mode == "default") {
        for (auto& pt : points) {
            init_pair(protein_idx+1, unrainbow_ids[protein_idx], -1);
            pt.color_id = protein_idx + 1;
        }
    }

    else if (screen_mode == "chain") {
        char cur_chain = '-';
        int color_index = 0;
        int num_colors = sizeof(unrainbow_ids) / sizeof(int);

        for (auto& pt : points) {
            char cID = pt.chainID;
            if (cID != cur_chain) {
                color_index++;
                cur_chain = cID;
                int col = unrainbow_ids[(color_index - 1) % num_colors];
                init_pair(color_index, col, -1);
            }
            pt.color_id = color_index;
        }
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
    float adjustedFOV1 = FOV / zoom_level1;
    float fovRad1 = 1.0 / tan(adjustedFOV1 * 0.5 / 180.0 * PI);
    float adjustedFOV2 = FOV / zoom_level2;
    float fovRad2 = 1.0 / tan(adjustedFOV2 * 0.5 / 180.0 * PI);
    float focal_offset = 10.0f;

    std::vector<RenderPoint> finalPoints;

    // project dots and connect them into line
    int protein_idx = 0;
    for (const auto& [chainID, chain_atoms] : data[0]->get_atoms()) {
        if (chain_atoms.size() == 0) continue;

        int num_atoms = data[0]->get_chain_length(chainID);
        std::vector<RenderPoint> chainPoints;

        int prevScreenX = -1, prevScreenY = -1;
        float prevZ = -1.0f;

        for (int i = 0; i < num_atoms; ++i) {
            float* position = chain_atoms[i].get_position();
            float x = position[0];
            float y = position[1];
            float z = position[2] + focal_offset;
            char structure = chain_atoms[i].get_structure();

            float projectedX = (x / z) * fovRad1;
            float projectedY = (y / z) * fovRad1;
            int screenX = (int)((projectedX + 1.0) * 0.5 * screen_width);
            int screenY = (int)((1.0 - projectedY) * 0.5 * screen_height);

            if (prevScreenX != -1 && prevScreenY != -1) {
                drawLine(chainPoints, prevScreenX, prevScreenY, screenX, screenY, prevZ, z, chainID, structure);
            }
            
            if (screenX >= 0 && screenX < screen_width && screenY >= 0 && screenY < screen_height) {
                chainPoints.push_back({screenX, screenY, z, getPixelCharFromDepth(z), chainID, structure});
            }

            prevScreenX = screenX;
            prevScreenY = screenY;
            prevZ = z;
        }
        finalPoints.insert(finalPoints.end(), chainPoints.begin(), chainPoints.end());
    }
    assign_colors_to_points(finalPoints, protein_idx); 
    protein_idx++;

    for (const auto& pt : finalPoints) {
        int idx = pt.y * screen_width + pt.x;
        if (pt.z < screenPixels[idx].depth) {
            screenPixels[idx].depth = pt.z;
            screenPixels[idx].pixel = pt.pixel;
            screenPixels[idx].color_id = pt.color_id;
        }
    }
    for (const auto& [chainID, chain_atoms] : data[1]->get_atoms()) {
        if (chain_atoms.size() == 0) continue;

        int num_atoms = data[1]->get_chain_length(chainID);
        std::vector<RenderPoint> chainPoints;

        int prevScreenX = -1, prevScreenY = -1;
        float prevZ = -1.0f;

        for (int i = 0; i < num_atoms; ++i) {
            float* position = chain_atoms[i].get_position();
            float x = position[0];
            float y = position[1];
            float z = position[2] + focal_offset;
            char structure = chain_atoms[i].get_structure();

            float projectedX = (x / z) * fovRad2;
            float projectedY = (y / z) * fovRad2;
            int screenX = (int)((projectedX + 1.0) * 0.5 * screen_width);
            int screenY = (int)((1.0 - projectedY) * 0.5 * screen_height);

            if (prevScreenX != -1 && prevScreenY != -1) {
                drawLine(chainPoints, prevScreenX, prevScreenY, screenX, screenY, prevZ, z, chainID, structure);
            }
            
            if (screenX >= 0 && screenX < screen_width && screenY >= 0 && screenY < screen_height) {
                chainPoints.push_back({screenX, screenY, z, getPixelCharFromDepth(z), chainID, structure});
            }

            prevScreenX = screenX;
            prevScreenY = screenY;
            prevZ = z;
        }
        finalPoints.insert(finalPoints.end(), chainPoints.begin(), chainPoints.end());
    }
    assign_colors_to_points(finalPoints, protein_idx); 
    protein_idx++;

    for (const auto& pt : finalPoints) {
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
    if (structNum == 0) {
        if ((zoom_level1 + zoom > 1)&&(zoom_level1 + zoom < 50)){
            zoom_level1 += zoom;
        }
    } else if (structNum == 1) {
        if ((zoom_level2 + zoom > 1)&&(zoom_level2 + zoom < 50)){
            zoom_level2 += zoom;
        }
    } else {
        if ((zoom_level1 + zoom > 1)&&(zoom_level1 + zoom < 50)){
            zoom_level1 += zoom;
        }
        if ((zoom_level2 + zoom > 1)&&(zoom_level2 + zoom < 50)){
            zoom_level2 += zoom;
        }
    }
}

bool Screen::handle_input(){
    bool keep_show = true;
    int key = getch();
    switch(key){
        // select protein
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
            if (key == 48){
                structNum = -1;
            }
            else if (key - 48 <= data.size()) {
                structNum = key - 49;
            }
            break;
        // A, a (x 축 음의 이동)
        case 65:
        case 97:
            if (structNum != -1) {
                data[structNum]->set_shift(-0.1, 0, 0);
            } else {
                for (int i = 0; i < data.size(); i++){
                    data[i]->set_shift(-0.1, 0, 0);
                }
            }
            break;
        // D, d (x 축 양의 이동)
        case 68:
        case 100:
            if (structNum != -1) {
                data[structNum]->set_shift(0.1, 0, 0);
            } else {
                for (int i = 0; i < data.size(); i++){
                    data[i]->set_shift(0.1, 0, 0);
                }
            }
            break;
        // S, s (y 축 음의 이동)
        case 83:
        case 115:
            if (structNum != -1) {
                data[structNum]->set_shift(0, -0.1, 0);
            } else {
                for (int i = 0; i < data.size(); i++){
                    data[i]->set_shift(0, -0.1, 0);
                }
            }
            break;      
        // W, w (y 축 양의 이동)
        case 87:
        case 119:
            if (structNum != -1) {
                data[structNum]->set_shift(0, 0.1, 0);
            } else {
                for (int i = 0; i < data.size(); i++){
                    data[i]->set_shift(0, 0.1, 0);
                }
            }
            break;

        // X, x (x 축 중심 회전)
        case 88:
        case 120:
            if (structNum != -1) {
                data[structNum]->set_rotate(1, 0, 0);
            } else {
                for (int i = 0; i < data.size(); i++){
                    data[i]->set_rotate(1, 0, 0);
                }
            }
            break;  
        // Y, y (y 축 중심 회전)
        case 89:
        case 121:
            if (structNum != -1) {
                data[structNum]->set_rotate(0, 1, 0);
            } else {
                for (int i = 0; i < data.size(); i++){
                    data[i]->set_rotate(0, 1, 0);
                }
            }
            break;  
        // Z, z (z 축 중심 회전)
        case 90:
        case 122:
            if (structNum != -1) {
                data[structNum]->set_rotate(0, 0, 1);
            } else {
                for (int i = 0; i < data.size(); i++){
                    data[i]->set_rotate(0, 0, 1);
                }
            }
            break;  

        // F, f (줌 아웃)
        case 70:
        case 102:
            set_zoom_level(-0.5);
            break;   
        // R, R (줌 인)
        case 82:
        case 114:
            set_zoom_level(0.5);
            break;   

        // Q, q
        case 81:
        case 113:
            keep_show = false;
            break;

        default:
            break;       
    }     

    return keep_show;
}