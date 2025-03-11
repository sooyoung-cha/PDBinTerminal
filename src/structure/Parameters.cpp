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
    chains = "";  // ⬅ 기본값 설정 (문제 1 해결)

    if (argc > 1) {
        in_file = argv[1];
    } else {
        std::cerr << "Need input file dir !!!" << std::endl;
        arg_okay = false;
        return;
    }

    for (int i = 2; i < argc; i++) {
        try {
            if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--format")) {
                if (i + 1 < argc && (!strcmp(argv[i+1], "pdb") || !strcmp(argv[i+1], "mmcif") ||
                                     !strcmp(argv[i+1], "mmtf") || !strcmp(argv[i+1], "mae") || 
                                     !strcmp(argv[i+1], "maegz"))) {
                    format = argv[i+1];
                    i++;
                } else {
                    throw std::runtime_error("Invalid format type.");
                }
            }
            else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--model")) {
                if (i + 1 < argc && isdigit(argv[i+1][0])) {  // ⬅ atoi() 실행 전 확인 (문제 2 해결)
                    model = atoi(argv[i+1]);
                    i++;
                } else {
                    throw std::runtime_error("Invalid or missing argument for -m / --model.");
                }
            }
            else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--chains")) {
                if (i + 1 < argc) {  
                    if (argv[i+1] == nullptr || strlen(argv[i+1]) == 0) {  // ✅ 비어 있는 값 체크
                        throw std::runtime_error("Error: Chains argument is empty!");
                    }
                    chains = argv[i+1];  
                    i++;

                    std::cout << "DEBUG: Parsed chains = '" << chains << "'" << std::endl; // ✅ 디버깅 출력 추가
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
    cout << "  in_file: " << in_file << endl;
    cout << "  format: " << format << endl;
    cout << "  model: " << model << endl;
    cout << "  chains: " << chains << endl;
    cout << "  width: " << width << endl;
    cout << "  height: " << height << endl;
    cout << "  show_structure: " << show_structure << endl;
    return;
}