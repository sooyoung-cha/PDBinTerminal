#include "Camera.hpp"

Camera::Camera(const std::string dir, const int width, const int height){
    camera_dir = dir;
    camera_width = width;
    camera_height = height;
    screenshot_idx = 0;

    try {
        std::filesystem::create_directories(camera_dir);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to create screenshot directory: " << e.what() << std::endl;
    }
}

void Camera::screenshot(const std::vector<RenderPoint>& screenPixels){    
    std::vector<RGBA> screenImage;
    screenImage.assign(camera_width * camera_height * 3, RGBA{0,0,0,0});
    renderPoint2image(screenPixels, screenImage);
    if(save_image(screenImage)){
        std::cout << "\nScreenshot saved in " << camera_dir << std::endl;
    } else{
        std::cout << "\nFailed to save screenshot in " << camera_dir << std::endl;
    }
}

void Camera::renderPoint2image(const std::vector<RenderPoint>& screenPixels,
                               std::vector<RGBA>& screenImage){
    float min_depth = std::numeric_limits<float>::max();
    float max_depth = std::numeric_limits<float>::lowest();
    for (int y = 0; y < camera_height; y++) {
        for (int x = 0; x < camera_width; x++) {
            screenImage[((y * 3) + 0) * camera_width + x] = Palettes::ID2RGBA[Palettes::RAINBOW[screenPixels[y * camera_width + x].color_id]];
            screenImage[((y * 3) + 1) * camera_width + x] = Palettes::ID2RGBA[Palettes::RAINBOW[screenPixels[y * camera_width + x].color_id]];
            screenImage[((y * 3) + 2) * camera_width + x] = Palettes::ID2RGBA[Palettes::RAINBOW[screenPixels[y * camera_width + x].color_id]];

            min_depth = std::min(min_depth, screenPixels[y * camera_width + x].depth);
            if (screenPixels[y * camera_width + x].depth != std::numeric_limits<float>::infinity())
                max_depth = std::max(max_depth, screenPixels[y * camera_width + x].depth);
        }
    }
    for (int y = 0; y < camera_height; y++) {
        for (int x = 0; x < camera_width; x++) {
            screenImage[((y * 3) + 0) * camera_width + x].a = get_alpha_from_depth(screenPixels[y * camera_width + x].depth, min_depth, max_depth);
            screenImage[((y * 3) + 1) * camera_width + x].a = get_alpha_from_depth(screenPixels[y * camera_width + x].depth, min_depth, max_depth);
            screenImage[((y * 3) + 2) * camera_width + x].a = get_alpha_from_depth(screenPixels[y * camera_width + x].depth, min_depth, max_depth);
        }
    }

    return;
}

bool Camera::save_image(std::vector<RGBA>& screenImage){
    std::string screenshot_dir = camera_dir + std::to_string(screenshot_idx++) + ".png"; 
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(screenImage.data());
    unsigned err = lodepng_encode32_file(screenshot_dir.c_str(), bytes, camera_width, camera_height * 3);
    if (err) { return false; }
    else { return true; }
}

int Camera::get_alpha_from_depth(float z, float min_z, float max_z) {
    float zn = (z - min_z) / (max_z - min_z);

    if (zn < 0.1) return 255;
    else if (zn < 0.26) return 215;
    else if (zn < 0.42) return 175;
    else if (zn < 0.58) return 135;
    else if (zn < 0.72) return 95;
    else if (zn < 0.9) return 55;
    else if (zn < 1.0) return 15;
    else return 0;
}