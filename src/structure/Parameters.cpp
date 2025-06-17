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
    
    if (argc <= 1) {
        std::cerr << "Need input file dir" << std::endl;
        arg_okay = false;
        return;
    }

    for (int i = 1; i < argc; i++) {
        try {
            if (fs::exists(argv[i]) && fs::is_regular_file(argv[i]) && in_file.size() < 6){
                in_file.push_back(argv[i]);
            }
            else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--mode")) {  // ✅ mode 옵션 추가
                if (i + 1 < argc) {
                    std::string val(argv[i + 1]);
                    std::transform(val.begin(), val.end(), val.begin(), ::tolower); // 소문자로 변환
                    if (val == "chain" || val == "rainbow" || val == "default") {
                        mode = val;
                        i++;
                    } else {
                        throw std::runtime_error("Error: Invalid value for --mode. Use 'chain' or 'rainbow'.");
                    }
                } else {
                    throw std::runtime_error("Error: Missing value for -m / --mode.");
                }
            }
            else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--chains")) {
                if (i + 1 < argc) {  
                    if (argv[i+1] == nullptr || strlen(argv[i+1]) == 0) {  // ✅ 비어 있는 값 체크
                        throw std::runtime_error("Error: Chains argument is empty.");
                    }
                    while(in_file.size() - 1 != chains.size()){
                        chains.push_back("-");
                    }
                    chains.push_back(argv[i+1]);  
                    i++;
                } else {
                    throw std::runtime_error("Error: Missing argument for -c / --chains.");
                }
            }
            else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--width")) {
                if (i + 1 < argc) {
                    if (is_valid_number(argv[i + 1], 1, 5)) {
                        width = std::stoi(argv[i + 1]);
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
                        height = std::stoi(argv[i + 1]);
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
            else if (!strcmp(argv[i], "-u") || !strcmp(argv[i], "--umatrix")) {
                if (i + 1 < argc) {
                    umatrix = argv[i + 1];
                    i++;
                }else {
                    throw std::runtime_error("Error: Missing value for -u / --umatrix.");
                }
            }
            else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--tmatrix")) {
                if (i + 1 < argc) {
                    tmatrix = argv[i + 1];
                    i++;
                }else {
                    throw std::runtime_error("Error: Missing value for -t / --tmatrix.");
                }
            }
            else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--ut_target_idx")) {
                if (i + 1 < argc) {
                    if (is_valid_number(argv[i + 1], 1, 5)) {
                        ut_target = std::stoi(argv[i + 1])-1;
                        ++i; 
                    } else {
                        throw std::runtime_error("Error: Parameter must be an integer between 1 and 5.");
                    }
                } else {
                    throw std::runtime_error("Error: Missing value for -i / --ut_target_idx.");
                }
            }
            else {
                throw std::runtime_error("Error: Unknown parameter: " + std::string(argv[i])); // ⬅ throw 수정 (문제 3 해결)
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Wrong input parameters: " << e.what() << std::endl;
            std::cerr << "Error at argument: " << argv[i] << std::endl;
            arg_okay = false;
            return;
        }
    }

    while(in_file.size() != chains.size()){
        chains.push_back("-");
    }

    if (in_file.size() == 0){
        std::cerr << "Error: Need input file dir" << std::endl;
        arg_okay = false;
        return;
    }
    if (ut_target > in_file.size() && ut_target != -1){
        std::cerr << "Error: Invalid utmatrix target index." << std::endl;
        std::cerr << in_file.size() << " "  << ut_target << std::endl;
        arg_okay = false;
        return;
    }

    return;
}

void Parameters::print_args() {
    cout << "Input parameters >> " << endl;
    cout << "  in_file: " << endl;
    for (int i = 0; i < in_file.size(); i++) {
        std::cout << "\t" << in_file[i] << ": " << chains[i] << '\n'; 
    }
    cout << "  mode: " << mode << endl;
    cout << "  width: " << width << endl;
    cout << "  height: " << height << endl;
    cout << "  umatrix: " << umatrix << endl;
    cout << "  tmatrix: " << tmatrix << endl;
    cout << "  ut_target: " << ut_target << endl;
    cout << "  show_structure: " << show_structure << endl;
    return;
}