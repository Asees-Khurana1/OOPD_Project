#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <thread>
#include <utime.h>
#include <cstring>
#include <ctime>
#include <chrono>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>

using namespace std;
using namespace chrono;

class Shell{

private:
    string current_dir;
    string previous_dir;

    void change_directory(const vector<string>& input_tokens){

        if (input_tokens.size() < 2) {
            cout<<"Invalid command"<<endl;
            return;
        }

        string destination = input_tokens[1];

        if(destination == "-"){
            swap(current_dir, previous_dir);
        } else if (destination == "-h" || destination == "-help"){
            printChangeDirectoryHelp();
            return;
        } else if (destination == "..") {
            // Move up one directory level
            size_t lastSlash = current_dir.find_last_of('\\');
            if (lastSlash != string::npos) {
                destination = current_dir.substr(0, lastSlash);
            } else {
                cout << "Cannot move up from the root directory." << endl;
                return;
            }
        } else if (destination == "/") {
            // Change to the root directory
            destination = "/";
        } else {
            // Combine with current directory
            destination = current_dir + "/" + destination;
        }

        if (chdir(destination.c_str()) != 0) {
            cout << "Directory not found: " << destination << endl;
        } else {
            char buffer[PATH_MAX];
            getcwd(buffer, PATH_MAX);
            previous_dir = current_dir;
            current_dir = buffer;
        }
    }

    void printChangeDirectoryHelp() {
        cout << "cd - Change the current working directory." << endl;
        cout << "Usage: [options]" << endl;
        cout << "Options:" << endl;
        cout << "-h, --help     Display this help message." << endl;
        cout << "..             Move up one directory level." << endl;
        cout << "/              Change to root directory." << endl;

    }

    void move_file(const vector<string>& input_tokens) {
        if (input_tokens.size() < 3) {
            cout << "Invalid command" << endl;
            return;
        }

        string source = input_tokens[1];
        string destination = input_tokens[2];

        // Check for options
        bool interactive = false;
        bool update = false;
        bool backup = false;
        bool no_clobber = false;

        for (size_t i = 3; i < input_tokens.size(); ++i) {
            string option = input_tokens[i];
            if (option == "-i") {
                interactive = true;
            } else if (option == "-u") {
                update = true;
            } else if (option == "-b") {
                backup = true;
            } else if (option == "-h" || option == "-help"){
                printMoveFileHelp();
                return;
            } else if (option == "-n") {
                no_clobber = true;
            }
        }

        // Check if destination exists
        bool destination_exists = (access(destination.c_str(), F_OK) == 0);

        // Implement option behavior
        if (no_clobber && destination_exists) {
            cout << "mv: Cannot overwrite existing file '" << destination << "' (use -n to override)." << endl;
            return;
        }

        if (update) {
            // Check if source is newer than destination or destination is missing
            bool moveFile = false;
            if (!destination_exists) {
                moveFile = true;
            } else {
                time_t sourceTime = getLastModificationTime(source);
                time_t destTime = getLastModificationTime(destination);
                moveFile = (sourceTime > destTime);
            }

            if (!moveFile) {
                cout << "mv: '" << source << "' is not newer than '" << destination << "'." << endl;
                return;
            }
        }

        if (backup && destination_exists) {
            createBackup(destination);
        }

        if (interactive) {
            cout << "mv: overwrite '" << destination << "'? (y/n): ";
            char response;
            cin >> response;
            if (response != 'y' && response != 'Y') {
                cout << "mv: File not moved." << endl;
                return;
            }
        }

        // Perform the move operation
        if (rename(source.c_str(), destination.c_str()) != 0) {
            cout << "Error moving file." << endl;
        } else {
            cout << "Moved " << source << " to " << destination << "." << endl;
        }
    }

    void printMoveFileHelp() {
        cout << "mv - Move or rename files and directories." << endl;
        cout << "Options:" << endl;
        cout << "-h, --help     Display this help message." << endl;
        cout << "-u             Update option executed" << endl;
        cout << "-b             Backup option executed" << endl;
        cout << "-i             Interactive option executed" << endl;

    }

    static time_t getLastModificationTime(const string& filePath) {
        struct stat fileInfo;
        if (stat(filePath.c_str(), &fileInfo) != 0) {
            return 0;
        }
        return fileInfo.st_mtime;
    }

    void createBackup(const string& filePath) {
        string backupFileName = filePath + ".bak";
        ifstream source(filePath, ios::binary);
        ofstream backup(backupFileName, ios::binary);
        backup << source.rdbuf();
        cout << "Backup created: " << backupFileName << endl;
    }

    void remove_file(const vector<string>& tokens) {

        auto startTime = high_resolution_clock::now();

        if (tokens.size() < 2) {
            cout << "Invalid command" << endl;
            return;
        }

        string file = tokens[1];

        // Check for options
        bool interactive = false;
        bool recursive = false;
        bool force = false;
        bool verbose = false;

        for (size_t i = 1; i < tokens.size(); ++i) {
            string option = tokens[i];
            if (option == "-i") {
                interactive = true;
            } else if (option == "-r") {
                recursive = true;
            } else if (option == "-f") {
                force = true;
            } else if (option == "-h" || option == "--help") {
                printRemoveFileHelp();
                return;
            } else if (option == "-v") {
                verbose = true;
            }
        }

        // Implement option behavior
        if (interactive) {
            // Prompt before each removal
            cout << "Remove " << file << "? (y/n): ";
            char response;
            cin >> response;
            if (response != 'y' && response != 'Y') {
                cout << "File not removed." << endl;
                return;
            }
        }

        if (recursive && force) {
            cout << "rm: Options -r and -f cannot be used together." << endl;
            return;
        }

        if (force) {
            // Remove files forcefully without prompting and suppress error messages
            if (removeFileForce(file, recursive, verbose) == 0) {
                cout << "Removed " << file << "." << endl;
            }
        } else {
            // Default behavior (prompting before removal)
            if (remove_func(file.c_str(), recursive, verbose) == 0) {
                cout << "Removed " << file << "." << endl;
            }
        }

        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(endTime - startTime);

        cout << "Deletion completed in " << duration.count() << " microseconds." << endl;
    }

    void printRemoveFileHelp() {
        cout << "rm - Remove files or directories." << endl;
        cout << "Options:" << endl;
        cout << "  -h, --help     Display this help message." << endl;
        cout << "-i             Interactive option executed" << endl;
        cout << "-r             Recursive option executed" << endl;
        cout << "-f             Force option executed" << endl;

    }

    int removeFileForce(const string& file, bool recursive, bool verbose) {
        // Remove files forcefully without prompting and suppress error messages
        string command = "rm -rf";
        if (verbose) {
            command += "v";
        }
        command += " " + file;

        int result = system(command.c_str());
        return result;
    }

    int remove_func(const char* filename, bool recursive, bool verbose) {
        int result;
        if (recursive) {
            // Remove directories and their contents recursively
            string command = "rm -r " + string(filename);
            result = system(command.c_str());
        } else {
            // Default behavior (prompt before removal)
            result = remove(filename);
        }

        return result;
    }

    void list_directory(const vector<string>& tokens) {
        // Determine the path to list (default to current directory if not specified)
        string path = (tokens.size() > 1) ? tokens[1] : ".";

        // Open the directory
        DIR* dir = opendir(path.c_str());
        if (dir == nullptr) {
            cout << "Directory not found." << endl;
            return;
        }

        for (const string& token : tokens) {
            if (token == "-h" || token == "--help") {
                printListDirectoryHelp();
                return;
            }
        }

        // Read all entries into a vector for sorting
        vector<string> entries;
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            entries.push_back(entry->d_name);
        }

        // Close the directory
        closedir(dir);

        // Sort entries based on options
        sortEntries(entries, tokens);

        // Print the sorted entries
        printEntries(entries, path, tokens);
    }

    void printListDirectoryHelp() {
        cout << "ls - List directory contents." << endl;
        cout << "Options:" << endl;
        cout << "  -h, --help     Display this help message." << endl;
    }

    void sortEntries(vector<string>& entries, const vector<string>& tokens) {
         //Sorting based on options
        bool sortTime = false;

        for (const string& token : tokens) {
            if (token == "-t") {
                sortTime = true;
            }
        }

        if (sortTime) {
            sort(entries.begin(), entries.end(), compareModificationTime);
        } else {
            sort(entries.begin(), entries.end());
        }
    }

    static bool compareModificationTime(const string& entry1, const string& entry2) {
        // Compare modification times for sorting
        struct stat stat1, stat2;
        stat(entry1.c_str(), &stat1);
        stat(entry2.c_str(), &stat2);
        return stat1.st_mtime > stat2.st_mtime;
    }

    void printEntries(const vector<string>& entries, const string& path, const vector<string>& tokens) {
        // Print detailed information about files
        bool longFormat = false;
        bool showHidden = false;
        bool humanReadable = false;

        for (const string& token : tokens) {
            if (token == "-l") {
                longFormat = true;
            } else if (token == "-a") {
                showHidden = true;
            } else if (token == "-h") {
                humanReadable = true;
            }
        }

        cout << (longFormat ? "Detailed information" : "Listing files") << " in directory: " << path << endl;

        for (const string& entry : entries) {
            // Skip hidden files if not specified by the -a option
            if (!showHidden && entry[0] == '.') {
                continue;
            }

            if (longFormat) {
                printLongFormat(entry, path, humanReadable);
            } else {
                cout << entry << endl;
            }
        }
    }

    void printLongFormat(const string& entry, const string& path, bool humanReadable) {
        // Print detailed information about a file
        struct stat fileStat;
        string fullPath = path + "/" + entry;
        if (stat(fullPath.c_str(), &fileStat) != 0) {
            cout << "Error reading file information for: " << entry << endl;
            return;
        }

        // File type and permissions
        // cout << (S_ISDIR(fileStat.st_mode) ? "d" : "-")
        //      << (fileStat.st_mode & S_IRUSR ? "r" : "-")
        //      << (fileStat.st_mode & S_IWUSR ? "w" : "-")
        //      << (fileStat.st_mode & S_IXUSR ? "x" : "-")
        //      << (fileStat.st_mode & S_IRGRP ? "r" : "-")
        //      << (fileStat.st_mode & S_IWGRP ? "w" : "-")
        //      << (fileStat.st_mode & S_IXGRP ? "x" : "-")
        //      << (fileStat.st_mode & S_IROTH ? "r" : "-")
        //      << (fileStat.st_mode & S_IWOTH ? "w" : "-")
        //      << (fileStat.st_mode & S_IXOTH ? "x" : "-");

        // Number of hard links
        cout << " " << fileStat.st_nlink;

        // Owner and group
        // struct passwd* ownerInfo = getpwuid(fileStat.st_uid);
        // struct group* groupInfo = getgrgid(fileStat.st_gid);
        // cout << " " << ownerInfo->pw_name << " " << groupInfo->gr_name;

        // File size
        if (humanReadable) {
            printHumanReadableSize(fileStat.st_size);
        } else {
            cout << " " << fileStat.st_size;
        }

        // Modification time
        char timeBuffer[20];
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localtime(&fileStat.st_mtime));
        cout << " " << timeBuffer;

        // File name
        cout << " " << entry << endl;
    }

    void printHumanReadableSize(off_t size) {
        // Print file size in human-readable format
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;

        while (size >= 1024 && unitIndex < 4) {
            size /= 1024;
            unitIndex++;
        }

        cout << " " << size << " " << units[unitIndex];
    }

    void copyFile(const vector<string>& tokens) {

        auto startTime = high_resolution_clock::now();

        if (tokens.size() < 3) {
            cout << "Invalid command" << endl;
            return;
        }

        // Parse options
        bool interactive = false;
        bool recursive = false;
        bool update = false;
        bool preserve = false;

        for (size_t i = 3; i < tokens.size(); ++i) {
            string option = tokens[i];
            if (option == "-i") {
                interactive = true;
            } else if (option == "-r") {
                recursive = true;
            } else if (option == "-u") {
                update = true;
            } else if (option == "-p") {
                preserve = true;
            }
        }

        // Check if the destination file already exists
        struct stat destStat;
        if (stat(tokens[2].c_str(), &destStat) == 0) {
            // Destination file exists
            if (interactive) {
                cout << "Do you want to overwrite " << tokens[2] << "? (y/n): ";
                char response;
                cin >> response;
                if (response != 'y' && response != 'Y') {
                    cout << "File not copied." << endl;
                    return;
                }
            }
            if (update) {
                // Check modification times to decide whether to copy or not
                struct stat sourceStat;
                if (stat(tokens[1].c_str(), &sourceStat) == 0) {
                    if (sourceStat.st_mtime <= destStat.st_mtime) {
                        cout << "Source file is not newer than the destination file. File not copied." << endl;
                        return;
                    }
                }
            }
        }

        // Open the source file in binary mode for reading
        ifstream source(tokens[1], ios::binary);

        // Check if the source file is successfully opened
        if (!source.is_open()) {
            cout << "Error opening source file: " << tokens[1] << endl;
            return;
        }

        // Open the destination file in binary mode for writing
        ofstream destination(tokens[2], ios::binary);

        // Check if the destination file is successfully opened
        if (!destination.is_open()) {
            cout << "Error opening destination file: " << tokens[2] << endl;
            source.close();  // Close the source file if destination file opening fails
            return;
        }

        // Copy the contents of the source file to the destination file
        destination << source.rdbuf();

        // Preserve file attributes if the -p option is specified
        if (preserve) {
            if (preserveAttributes(tokens[1], tokens[2]) != 0) {
                cout << "Error preserving file attributes." << endl;
            }
        }

        // Close the file streams
        source.close();
        destination.close();

        cout << "Copied " << tokens[1] << " to " << tokens[2] << "." << endl;

        // Copy directories recursively if the -r option is specified
        if (recursive) {
            copyDirectoryRecursive(tokens[1], tokens[2], preserve, update, interactive);
        }

        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(endTime - startTime);

        cout << "Copy completed in " << duration.count() << " microseconds." << endl;
    }

    int preserveAttributes(const string& source, const string& destination) {
        // Preserve file attributes (timestamps and permissions)
        struct stat sourceStat;
        if (stat(source.c_str(), &sourceStat) != 0) {
            return -1;
        }

        // Preserve timestamps
        struct utimbuf fileTimes = {sourceStat.st_atime, sourceStat.st_mtime};
        if (utime(destination.c_str(), &fileTimes) != 0) {
            return -1;
        }

        // Preserve permissions
        if (chmod(destination.c_str(), sourceStat.st_mode) != 0) {
            return -1;
        }

        return 0;
    }

    void copyDirectoryRecursive(const string& source, const string& destination, bool preserve, bool update, bool interactive) {
        // Open the source directory
        DIR* dir = opendir(source.c_str());
        if (!dir) {
            cout << "Error opening source directory: " << source << endl;
            return;
        }

        // Create the destination directory
        if (mkdir(destination.c_str()) != 0) {
            cout << "Error creating destination directory: " << destination << endl;
            closedir(dir);
            return;
        }

        // Read the contents of the source directory
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] == '.') {
                // Skip "." and ".." entries
                continue;
            }

            // Construct full paths for the source and destination entries
            string sourcePath = source + "/" + entry->d_name;
            string destPath = destination + "/" + entry->d_name;

            // Check if the entry is a directory
            struct stat entryStat;
            if (stat(sourcePath.c_str(), &entryStat) != 0) {
                cout << "Error getting information for: " << sourcePath << endl;
                continue;
            }

            if (S_ISDIR(entryStat.st_mode)) {
                // Recursive call for directories
                copyDirectoryRecursive(sourcePath, destPath, preserve, update, interactive);
            } else {
                // Copy file
                copyFileInternal(sourcePath, destPath, preserve, update, interactive);
            }
        }

        // Close the source directory
        closedir(dir);
    }

    void printCopyFileHelp() {
        cout << "cp - Copy files and directories." << endl;
        cout << "Options:" << endl;
        cout << "  -h, --help     Display this help message." << endl;
    }

    void copyFileInternal(const string& source, const string& destination, bool preserve, bool update, bool interactive) {
        // Open the source file in binary mode for reading
        ifstream sourceFile(source, ios::binary);

        // Check if the source file is successfully opened
        if (!sourceFile.is_open()) {
            cout << "Error opening source file: " << source << endl;
            return;
        }

        // Check if the destination file already exists
        struct stat destStat;
        if (stat(destination.c_str(), &destStat) == 0) {
            // Destination file exists
            if (interactive) {
                cout << "Do you want to overwrite " << destination << "? (y/n): ";
                char response;
                cin >> response;
                if (response != 'y' && response != 'Y') {
                    cout << "File not copied." << endl;
                    sourceFile.close();
                    return;
                }
            }
            if (update) {
                // Check modification times to decide whether to copy or not
                struct stat sourceStat;
                if (stat(source.c_str(), &sourceStat) == 0) {
                    if (sourceStat.st_mtime <= destStat.st_mtime) {
                        cout << "Source file is not newer than the destination file. File not copied." << endl;
                        sourceFile.close();
                        return;
                    }
                }
            }
        }

        // Open the destination file in binary mode for writing
        ofstream destFile(destination, ios::binary);

        // Check if the destination file is successfully opened
        if (!destFile.is_open()) {
            cout << "Error opening destination file: " << destination << endl;
            sourceFile.close();
            return;
        }

        // Copy the contents of the source file to the destination file
        destFile << sourceFile.rdbuf();

        // Preserve file attributes if the -p option is specified
        if (preserve) {
            if (preserveAttributes(source, destination) != 0) {
                cout << "Error preserving file attributes." << endl;
            }
        }

        // Close the file streams
        sourceFile.close();
        destFile.close();

        cout << "Copied " << source << " to " << destination << "." << endl;
    }

public:

    Shell() {

        char buffer[PATH_MAX];
        if (getcwd(buffer, PATH_MAX) != nullptr) {
            current_dir = buffer;
            previous_dir = current_dir;
        } else {
            cerr << "Error getting initial working directory." << endl;
        }
    }

    void run_command(const string& input_command) {

        istringstream string_stream(input_command);

        vector<string> input_tokens{istream_iterator<string>{string_stream},
                                    istream_iterator<string>{}};

        if (input_tokens.empty()) {
            return;
        }

        if(input_tokens[0] == "cd"){
            change_directory(input_tokens);
        }else if(input_tokens[0] == "ls"){
            list_directory(input_tokens);
        }else if(input_tokens[0] == "mv"){
            move_file(input_tokens);
        }else if(input_tokens[0] == "rm"){
            remove_file(input_tokens);
        }else if(input_tokens[0] == "cp"){
            copyFile(input_tokens);
        }else{
            cout <<"Command not recognized."<<endl;
        }
    }

    string get_current_directory(){
        return current_dir;
    }

};

int main() {
    Shell shell;

    while (true) {
        cout << shell.get_current_directory() << " $ ";
        string user_input;
        getline(cin, user_input);

        if (user_input == "exit") {
            break;
        }

        shell.run_command(user_input);
    }

    return 0;
}
