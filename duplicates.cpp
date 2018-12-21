#include "duplicates.h"
#include <fstream>
#include <map>
#include <algorithm>
#include "sha256.h"

namespace fs = std::experimental::filesystem;

void getListFiles(std::vector<fs::path> &paths, const std::string &directory)
{
    for(const auto &p: fs::recursive_directory_iterator(directory)) {
        if (!fs::is_directory(p.path()) && !fs::is_symlink(p.path())) {
            paths.push_back(p.path());
        }
    }
}

void sortBySize(const std::vector<fs::path> &paths, std::map<long long, std::vector<fs::path>> &sameSizeFiles)
{
    for (const auto &p : paths) {
        auto fileSize = fs::file_size(p);
        sameSizeFiles[fileSize].push_back(p);
    }
}

void sortByBeginEndBytes(const std::vector<fs::path> &paths, std::map<std::tuple<unsigned int, unsigned int, unsigned int>, std::vector<fs::path>> &sameBeginEndBytesFiles, long long size)
{
    std::ifstream file;
    for (const auto &p : paths) {
        file.open(p);
        int n = 4;
        if (size < n) {
            n = (int) size;
        }
        std::vector<char> bufBegin(n, 0), bufMid(n, 0), bufEnd(n, 0);
        file.read(bufBegin.data(), n);
        file.seekg(size - n, std::ios_base::beg);
        file.read(bufEnd.data(), n);
        long long mid = size / 2;
        if (mid + n > size) {
            n = (int)(size - mid);
        }
        file.seekg(mid, std::ios_base::beg);
        file.read(bufMid.data(), n);
        unsigned int beginOfFile = 0, midOfFile = 0, endOfFile = 0;
        for (int i = 0; i < n; i++) {
            beginOfFile = (beginOfFile << 8) + bufBegin[i];
        }
        for (int i = 0; i < n; i++) {
            midOfFile = (midOfFile << 8) + bufMid[i];
        }
        for (int i = 0; i < n; i++) {
            endOfFile = (endOfFile << 8) + bufEnd[i];
        }
        sameBeginEndBytesFiles[{beginOfFile, midOfFile, endOfFile}].push_back(p);
        file.close();
    }
}

const size_t sizeBuf = 4000000;

void sortBySHA256(const std::vector<fs::path> &paths, std::map<std::string, std::vector<fs::path>> &sameSHA256Files)
{
    if (paths.size() == 1) {
        sameSHA256Files[""].push_back(paths[0]); //unnecessary hash
        return;
    }
    std::ifstream file;
    for (const auto &p : paths) {
        file.open(p);
        char buf[sizeBuf];
        SHA256_CTX ctx;
        unsigned char hash[32];
        std::string hashStr;
        SHA256Init(&ctx);
        do {
            file.read(buf, sizeof(buf));
            SHA256Update(&ctx, (unsigned char*) buf, sizeBuf);
        } while (file);
        SHA256Final(&ctx, hash);
        char s[3];
        for (unsigned char i : hash) {
            sprintf(s, "%02x", i);
            hashStr += s;
        }
        sameSHA256Files[hashStr].push_back(p);
        file.close();
    }
}

void duplicates::setDirectory(std::string &directory)
{
    sameFiles.clear();
    std::vector<fs::path> paths;
    getListFiles(paths, directory);
    std::map<long long, std::vector<fs::path>> sameSizeFiles;
    sortBySize(paths, sameSizeFiles);
    for (const auto &groupSize : sameSizeFiles) {
        std::map<std::tuple<unsigned int, unsigned int, unsigned int>, std::vector<fs::path>> sameBeginEndBytesFiles;
        sortByBeginEndBytes(groupSize.second, sameBeginEndBytesFiles, groupSize.first);
        for (const auto &groupBeginEndBytes : sameBeginEndBytesFiles) {
            std::map<std::string, std::vector<fs::path>> sameSHA256Files;
            sortBySHA256(groupBeginEndBytes.second, sameSHA256Files);
            for (const auto &groupSHA256 : sameSHA256Files) {
                if (groupSHA256.second.size() > 1) {
                    sameFiles.push_back(std::vector<std::string>());
                    for (const auto &p : groupSHA256.second) {
                        sameFiles.back().push_back(p.string());
                        indexInSameFiles[p.string()] = sameFiles.size() - 1;
                    }
                }
            }
        }
    }
}
