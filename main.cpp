#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <unordered_map>
#include <filesystem>
#include <unordered_set>

/*
* To Do:
    * Autocomplete for tabs
    * Piping
    * Quoting and backslash escaping
    * Implement whatever globbing is
    * Increase builtins
*/

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
    int ls(std::vector<std::string>& args);
}

// Mapping from builtin shell command to function
std::unordered_map<std::string, std::function<int(std::vector<std::string>&)>> builtin_commands_map = {
    {"pwd", &nsh::pwd},
    {"cd", &nsh::cd},
    {"help", &nsh::help},
    {"exit", &nsh::exit},
    {"ls", &nsh::ls}
};

std::unordered_set<std::string> parse_flags(std::vector<std::string>& args) {
    std::unordered_set<std::string> flags{};

    for (int i = 1; i < args.size(); ++i) {
        if (args[i][0] == '-')
            flags.insert(args[i]);
    }

    return flags;
}

std::string path_to_file(std::string& path) {
    int pos = path.find_last_of("/");

    if (pos != std::string::npos) {
        return path.substr(pos + 1, path.size());
    }

    return path;
}

int nsh::ls(std::vector<std::string>& args) {
    const std::filesystem::path current_path{std::filesystem::current_path()};
    std::unordered_set<std::string> flags = parse_flags(args);

    for (auto const& dir_entry : std::filesystem::directory_iterator{current_path}) {
        std::string path = dir_entry.path();
        std::string file_name = path_to_file(path);

        if (file_name[0] == '.' && flags.find("-a") == flags.end() && flags.find("--all") == flags.end())
            continue;
        
        std::cout << file_name << "        ";
    }
    std::cout << '\n';

    return 1;
}

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
    for (const auto& pair : builtin_commands_map) {
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

std::vector<char*> format_args(std::vector<std::string>& args) {
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
        std::vector<char*> cargs = format_args(args);
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

    if (builtin_commands_map.find(args[0]) != builtin_commands_map.end()) {
        return builtin_commands_map[args[0]](args);
    }

    return nsh::launch(args);
}

void nsh::loop() {
    std::string line{""};
    std::vector<std::string> args;
    int status{1};

    do {
        std::cout << "nsh > ";
        nsh::read_line(line);
        args = nsh::split_line(line);
        status = nsh::execute(args);
    } while(status);
}

int main(int argc, char* argv[]) {
    nsh::loop();
    return 0;
}