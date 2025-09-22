#include "Camera.hpp"

Camera::Camera(const std::string dir, const int width, const height){
    camera_dir = dir;
}

void Camera::screenshot(const std::vector<RenderPoint>& screenPixels){
    PngImage target = renderPoint2png(screenPixels);
    save_png()target;
}

PngImage Camera::renderPoint2png(const std::vector<RenderPoint>& screenPixels){

}

void Camera::save_png(PngImage target){

}