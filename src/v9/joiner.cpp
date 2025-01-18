#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>

struct Table1Entry {
    std::vector<std::string> Bs;
    std::vector<std::string> Cs;
};

void parseLine(const std::string& line, char delimiter, std::string& part1, std::string& part2) {
    size_t delimPos = line.find(delimiter);
    if (delimPos != std::string::npos) {
        part1 = line.substr(0, delimPos);
        part2 = line.substr(delimPos + 1);
    }
}

int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    std::unordered_map<std::string, Table1Entry> table1; // Key: A, Values: Bs and Cs
    table1.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table3; // Key: A, Values: D's
    table3.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table4; // Key: D, Values: E's
    table4.reserve(20000000);


    
    // Read File1 (A,B)
    std::ifstream file1(path1);
    if (!file1.is_open()) {
        std::cerr << "Error opening File1\n";
        return -1;
    }
    std::string line, A, B;
    while (std::getline(file1, line)) {
        parseLine(line, ',', A, B);
        table1[A].Bs.push_back(B);
    }

    // Read File2 (A,C)
    std::ifstream file2(path2);
    if (!file2.is_open()) {
        std::cerr << "Error opening File2\n";
        return -1;
    }
    std::string C;
    while (std::getline(file2, line)) {
        parseLine(line, ',', A, C);
        table1[A].Cs.push_back(C);
    }

    // Read File3 (A,D)
    std::ifstream file3(path3);
    if (!file3.is_open()) {
        std::cerr << "Error opening File3\n";
        return -1;
    }
    std::string D;
    while (std::getline(file3, line)) {
        parseLine(line, ',', A, D);
        table3[A].push_back(D);
    }

    // Read File4 (D,E)
    std::ifstream file4(path4);
    if (!file4.is_open()) {
        std::cerr << "Error opening File4\n";
        return -1;
    }
    std::string E;
    while (std::getline(file4, line)) {
        parseLine(line, ',', D, E);
        table4[D].push_back(E);
    }

    const size_t BUFFER_SIZE = 1024 * 1024 * 1024; // 1 GB
    std::vector<char> outputBuffer(BUFFER_SIZE);
    size_t bufferPos = 0; // Tracks the current position in the buffer

    // Perform the join
    for (const auto& [A, entry] : table1) {
        if (table3.find(A) != table3.end()) {
            for (const auto& D : table3[A]) {
                if (table4.find(D) != table4.end()) {
                    for (const auto& E : table4[D]) {
                        for (const auto& B : entry.Bs) {
                            for (const auto& C : entry.Cs) {
                                // Calculate the size of the current record
                                size_t recordSize = D.size() + A.size() + B.size() + C.size() + E.size() + 5; // 4 commas and 1 newline

                                // Check if the buffer has enough space
                                if (bufferPos + recordSize > BUFFER_SIZE) {
                                    // Write the buffer to stdout and reset
                                    std::cout.write(outputBuffer.data(), bufferPos);
                                    bufferPos = 0; // Reset buffer position
                                }

                                // Copy data into the buffer
                                std::memcpy(&outputBuffer[bufferPos], D.data(), D.size());
                                bufferPos += D.size();
                                outputBuffer[bufferPos++] = ',';
                                std::memcpy(&outputBuffer[bufferPos], A.data(), A.size());
                                bufferPos += A.size();
                                outputBuffer[bufferPos++] = ',';
                                std::memcpy(&outputBuffer[bufferPos], B.data(), B.size());
                                bufferPos += B.size();
                                outputBuffer[bufferPos++] = ',';
                                std::memcpy(&outputBuffer[bufferPos], C.data(), C.size());
                                bufferPos += C.size();
                                outputBuffer[bufferPos++] = ',';
                                std::memcpy(&outputBuffer[bufferPos], E.data(), E.size());
                                bufferPos += E.size();
                                outputBuffer[bufferPos++] = '\n';
                            }
                        }
                    }
                }
            }
        }
    }
    if (bufferPos > 0) {
        std::cout.write(outputBuffer.data(), bufferPos);
    }


    return 0;
}

int join(const std::string path1, const std::string path2, const std::string path3, const std::string path4) {
    int code = hashJoin(path1, path2, path3, path4);
    return code;
}