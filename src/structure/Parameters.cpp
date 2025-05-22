#include "Parameters.hpp"

bool Parameters::is_valid_number(const std::string& str, int min, int max) {
    try {
        // 입력값이 숫자인지 확인
        for (char c : str) {
            if (!std::isdigit(c)) return false;
        }

        // 문자열을 정수로 변환
        int value = std::stoi(str);

        // 범위 확인
        if (value < min || value > max) {
            return false;
        }

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

Parameters::Parameters(int argc, char* argv[]) {
    arg_okay = true;
    chains1 = "";  // ⬅ 기본값 설정 (문제 1 해결)
    unsigned int start;
    if (argc > 1) {
        in_file = argv[1];
        if (argv[2][0] != '-') {
            in_file2 = argv[2];
            start = 3;
        } else {
            in_file2 = "none";
            start = 2;
        }
    } else {
        std::cerr << "Need input file dir !!!" << std::endl;
        arg_okay = false;
        return;
    }

    for (int i = start; i < argc; i++) {
        try {
            if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--mode")) {  // ✅ mode 옵션 추가
                if (i + 1 < argc) {
                    std::string val(argv[i + 1]);
                    std::transform(val.begin(), val.end(), val.begin(), ::tolower); // 소문자로 변환
                    if (val == "chain" || val == "rainbow" || val == "default") {
                        mode = val;
                        i++;
                    } else {
                        throw std::runtime_error("Invalid value for --mode. Use 'chain' or 'rainbow'.");
                    }
                } else {
                    throw std::runtime_error("Missing value for -m / --mode.");
                }
            }
            else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--chains")) {
                if (i + 1 < argc) {  
                    if (argv[i+1] == nullptr || strlen(argv[i+1]) == 0) {  // ✅ 비어 있는 값 체크
                        throw std::runtime_error("Error: Chains argument is empty!");
                    }
                    chains1 = argv[i+1];  
                    i++;

                    std::cout << "DEBUG: Parsed chains = '" << chains1 << "'" << std::endl; // ✅ 디버깅 출력 추가
                } else {
                    throw std::runtime_error("Missing argument for -c / --chains.");
                }
            }
            else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--width")) {
                if (i + 1 < argc) {
                    if (is_valid_number(argv[i + 1], 1, 5)) {
                        width = std::stoi(argv[i + 1]) * 40 + 40;
                        ++i; 
                    } else {
                        throw std::runtime_error("Error: Parameter must be an integer between 1 and 5.");
                    }
                } else {
                    throw std::runtime_error("Error: Missing value for -w / --width.");
                }
            }
            else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--height")) {
                if (i + 1 < argc) {
                    if (is_valid_number(argv[i + 1], 1, 5)) {
                        height = std::stoi(argv[i + 1]) * 10 + 30;
                        ++i; 
                    } else {
                        throw std::runtime_error("Error: Parameter must be an integer between 1 and 5.");
                    }
                } else {
                    throw std::runtime_error("Error: Missing value for -h / --height.");
                }
            }
            else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--structure")) {
                show_structure = true;
            }
            else {
                throw std::runtime_error("Unknown parameter: " + std::string(argv[i])); // ⬅ throw 수정 (문제 3 해결)
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Wrong input parameters: " << e.what() << std::endl;
            std::cerr << "Error at argument: " << argv[i] << std::endl;
            arg_okay = false;
            return;
        }
    }
}

void Parameters::print_args() {
    cout << "Input parameters >> " << endl;
    cout << "  in_file1: " << in_file << endl;
    cout << "  in_file2: " << in_file2 << endl;
    cout << "  format: " << format << endl;
    cout << "  mode: " << mode << endl;
    cout << "  chains1: " << chains1 << endl;
    cout << "  chains2: " << chains2 << endl;
    cout << "  width: " << width << endl;
    cout << "  height: " << height << endl;
    cout << "  show_structure: " << show_structure << endl;
    return;
}