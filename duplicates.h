#ifndef DUPLICATES_H
#define DUPLICATES_H

#include <string>
#include <vector>
#include <map>
#include <experimental/filesystem>

class duplicates
{
public:
  static duplicates& instance()
  {
    static duplicates singleton;
    return singleton;
  }

  void setDirectory(std::string &directory);

  std::vector<std::vector<std::string>> sameFiles;
  std::map<std::string, size_t> indexInSameFiles;

// Other non-static member functions
private:
  duplicates() {}                                  // Private constructor
  ~duplicates() {}
  duplicates(const duplicates&);                 // Prevent copy-construction
  duplicates& operator=(const duplicates&);      // Prevent assignment
};

#endif // DUPLICATES_H
