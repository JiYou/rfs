#include "split_path.h"

void split(const std::string& file_path, std::string* dir, std::string* file) {
    dir->clear();
    file->clear();

    auto pos = file_path.rfind('/');
    // if not found
    if (pos == string::npos) {
        return;
    }

    // alloc the memory size first.
    dir->resize(pos);
    file->resize(file_path.length() - pos);

    int i = 0;
    while (i < pos) {
        (*dir)[i] = file_path[i];
        i++;
    }

    // skip '/'
    i++;

    while (i < file_path.length()) {
        (*file)[i - pos - 1] = file_path[i];
        i++;
    }
}