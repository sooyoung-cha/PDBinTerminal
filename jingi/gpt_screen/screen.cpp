#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <cmath>

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 24;
const float FOV = 90.0;
const float ASPECT_RATIO = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
const float NEAR_PLANE = 0.1;
const float FAR_PLANE = 100.0;
const float PI = 3.14159265359;

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}

void drawLine(std::vector<std::vector<char> >& screen, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    
    while (true) {
        if (x0 >= 0 && x0 < SCREEN_WIDTH && y0 >= 0 && y0 < SCREEN_HEIGHT) {
            screen[y0][x0] = '*';
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void drawScreen(const std::vector<std::vector<char> >& screen) {
    clearScreen();
    for (const auto& row : screen) {
        for (const auto& cell : row) {
            std::cout << cell;
        }
        std::cout << "\n";
    }
}

void projectAndDraw(float x, float y, float z) {
    float fovRad = 1.0 / tan(FOV * 0.5 / 180.0 * PI);
    float projectedX = (x / z) * fovRad * ASPECT_RATIO;
    float projectedY = (y / z) * fovRad;
    int screenX = (int)((projectedX + 1.0) * 0.5 * SCREEN_WIDTH);
    int screenY = (int)((1.0 - projectedY) * 0.5 * SCREEN_HEIGHT);

    // Project origin (0, 0, 0)
    float originProjectedX = (0.0f / z) * fovRad * ASPECT_RATIO;
    float originProjectedY = (0.0f / z) * fovRad;
    int originScreenX = (int)((originProjectedX + 1.0) * 0.5 * SCREEN_WIDTH);
    int originScreenY = (int)((1.0 - originProjectedY) * 0.5 * SCREEN_HEIGHT);

    // Create screen buffer
    std::vector<std::vector<char> > screen(SCREEN_HEIGHT, std::vector<char>(SCREEN_WIDTH, ' '));
    
    // Draw line from origin to star
    drawLine(screen, originScreenX, originScreenY, screenX, screenY);
    
    // Draw the origin and star characters
    screen[originScreenY][originScreenX] = '$';
    screen[screenY][screenX] = '*';
    
    drawScreen(screen);
}

int main() {
    float x = 0.0f, y = 0.0f, z = 3.0f;

    bool running = true;
    while (running) {
        projectAndDraw(x, y, z);

        char input = getch();
        switch (input) {
            case 'w': y += 0.1f; break;  // Move up
            case 's': y -= 0.1f; break;  // Move down
            case 'a': x -= 0.1f; break;  // Move left
            case 'd': x += 0.1f; break;  // Move right
            case 't': z -= 0.1f; break;  // Move forward
            case 'g': z += 0.1f; break;  // Move backward
            case 'q': running = false; break;  // Quit
        }
        usleep(100000); // Sleep for 100 milliseconds
    }

    return 0;
}
