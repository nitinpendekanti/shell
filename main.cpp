#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <unordered_map>
#include <filesystem>

namespace nsh {
    void loop();
    
    void read_line(std::string& line);
    std::vector<std::string> split_line(const std::string& line);

    int launch(std::vector<std::string>& args);
    int execute(std::vector<std::string>& args);

    // Builtin shell commands
    int cd(std::vector<std::string>& args);
    int help(std::vector<std::string>& args);
    int exit(std::vector<std::string>& args);
    int pwd(std::vector<std::string>& args);
}

// Mapping from builtin shell command to function
std::unordered_map<std::string, std::function<int(std::vector<std::string>&)>> builtinCommandsMap = {
    {"pwd", &nsh::pwd},
    {"cd", &nsh::cd},
    {"help", &nsh::help},
    {"exit", &nsh::exit}
};

int nsh::pwd(std::vector<std::string>& args) {
    std::cout << std::filesystem::current_path().string() << '\n';
    return 1;
}

int nsh::cd(std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "nsh: expected argument for \"cd\"" << '\n';
        return 1;
    }

    if (chdir(args[1].c_str()) != 0) {
        std::cerr << "Error with chdir(): " << strerror(errno) << '\n';
    }

    return 1;
}

int nsh::help(std::vector<std::string>& args) {
    std::cout << "Nitin's SHell (nsh)" << '\n';
    std::cout << "Simple shell implementation where you input a command, arguments, and press enter\n\n";

    std::cout << "Builtin commands: \n"; 
    for (const auto& pair : builtinCommandsMap) {
        std::cout << "\t-" << pair.first << '\n';
    }

    return 1;
}

int nsh::exit(std::vector<std::string>& args) {
    return 0;
}

// Note: The read_line() function can be replaced with the geline() standard C++ function
void nsh::read_line(std::string &line) { 
    line = ""; 
    int c{0};

    while (true) {
        c = getchar();

        if (c == EOF || c == '\n') { // new line character or at end of file
            line += '\0';
            return;
        } else {
            line += c;
        }
    }
}

std::vector<std::string> nsh::split_line(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token{""};
    int pos{0};

    while (line[pos] != '\0') {
        if (char(line[pos]) == ' ') {
            if (token != "")
                tokens.push_back(token);

            token = "";
        } else {
            token += line[pos];
        }   

        pos += 1;
    }

    if (token != "")
        tokens.push_back(token);

    return tokens;
}

std::vector<char*> formatArgs(std::vector<std::string>& args) {
    std::vector<char*> cstrings{};

    for(auto& string : args)
        cstrings.push_back(&string.front());
    
    return cstrings;
}

int nsh::launch(std::vector<std::string>& args) {
    pid_t pid{-1}, wpid{-1};
    int status{-1};

    pid = fork();
    if (pid == 0) {
        // Child process
        std::vector<char*> cargs = formatArgs(args);
        cargs.push_back(nullptr);

        if (execvp(args[0].c_str(), cargs.data()) == -1) {
            std::cerr << "Error calling execvp(): " << strerror(errno) << '\n';
        }
        std::exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent Process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    } else {
        // Error
        std::cerr << "Error calling fork(): " << strerror(errno) << '\n';
    }

    return 1;
}

int nsh::execute(std::vector<std::string>& args) {
    if (args.size() == 0)
        return 1;
    
    if (builtinCommandsMap.find(args[0]) != builtinCommandsMap.end()) {
        return builtinCommandsMap[args[0]](args);
    }

    return nsh::launch(args);
}

void nsh::loop() {
    std::string line{""};
    std::vector<std::string> args;
    int status{-1};

    do {
        std::cout << "> ";
        nsh::read_line(line);
        args = nsh::split_line(line);
        status = nsh::execute(args);
    } while(status);
}

int main(int argc, char* argv[]) {
    nsh::loop();
    return 0;
}