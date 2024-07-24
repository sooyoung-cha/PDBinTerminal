#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <cmath>

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 24;
const float FOV = 90.0;
const float ASPECT_RATIO = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
const float PI = 3.14159265359;

struct Point3D {
    float x, y, z;
    char symbol;
};

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

void drawScreen(const std::vector<std::vector<char> >& screen, int currentLine) {
    clearScreen();
    std::cout << "Currently controlling line: " << currentLine << "\n";
    for (const auto& row : screen) {
        for (const auto& cell : row) {
            std::cout << cell;
        }
        std::cout << "\n";
    }
}

void projectAndDraw(Point3D points[], int currentLine) {
    float fovRad = 1.0 / tan(FOV * 0.5 / 180.0 * PI);

    // Create screen buffer
    std::vector<std::vector<char> > screen(SCREEN_HEIGHT, std::vector<char>(SCREEN_WIDTH, ' '));

    // Project and draw each line
    for (int i = 0; i < 3; ++i) {
        float x = points[i].x;
        float y = points[i].y;
        float z = points[i].z;
        float projectedX = (x / z) * fovRad * ASPECT_RATIO;
        float projectedY = (y / z) * fovRad;
        int screenX = (int)((projectedX + 1.0) * 0.5 * SCREEN_WIDTH);
        int screenY = (int)((1.0 - projectedY) * 0.5 * SCREEN_HEIGHT);

        // Project origin (0, 0, 0)
        float originProjectedX = (0.0f / z) * fovRad * ASPECT_RATIO;
        float originProjectedY = (0.0f / z) * fovRad;
        int originScreenX = (int)((originProjectedX + 1.0) * 0.5 * SCREEN_WIDTH);
        int originScreenY = (int)((1.0 - originProjectedY) * 0.5 * SCREEN_HEIGHT);

        // Draw line from origin to point
        drawLine(screen, originScreenX, originScreenY, screenX, screenY);

        // Draw the point character
        screen[screenY][screenX] = points[i].symbol;
    }

    drawScreen(screen, currentLine);
}

int main() {
    // Initialize points
    Point3D points[3] = {
        { 0.5f, 0.5f, 3.0f, '1' },
        { -0.5f, 0.5f, 3.0f, '2' },
        { 0.0f, -0.5f, 3.0f, '3' }
    };

    int currentLine = 1;

    bool running = true;
    while (running) {
        projectAndDraw(points, currentLine);

        char input = getch();
        switch (input) {
            case 'w': points[currentLine - 1].y += 0.1f; break;  // Move up
            case 's': points[currentLine - 1].y -= 0.1f; break;  // Move down
            case 'a': points[currentLine - 1].x -= 0.1f; break;  // Move left
            case 'd': points[currentLine - 1].x += 0.1f; break;  // Move right
            case 't': points[currentLine - 1].z -= 0.1f; break;  // Move forward
            case 'g': points[currentLine - 1].z += 0.1f; break;  // Move backward
            case '1': currentLine = 1; break;  // Switch to line 1
            case '2': currentLine = 2; break;  // Switch to line 2
            case '3': currentLine = 3; break;  // Switch to line 3
            case 'q': running = false; break;  // Quit
        }
        usleep(100000); // Sleep for 100 milliseconds
    }

    return 0;
}
