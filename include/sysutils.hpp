//
// Created by nakamurasama072 on 2026/8/15.
//

#ifndef JUSTORE_SYSUTILS_HPP
#define JUSTORE_SYSUTILS_HPP

#include <config.h>
#include <optional>

// The Disk Space structure object.
struct DiskSpace {
    uint64_t total; // disk capacity
    uint64_t available; // available disk space
};

// Return the total and available disk space (in Bytes) where the netdisk root path is in.
inline std::optional<DiskSpace> get_disk_space() {
    // total and available
    try {
        DiskSpace space_info{};
        std::cout << "Detecting the total and available space of path \"" << kNetDiskRoot << "\"...\n";
        fs::space_info fsiobj = fs::space(kNetDiskRoot);
        space_info.total = fsiobj.capacity;
        std::cout << "The disk capacity is " << fsiobj.capacity << " Bytes\n";
        space_info.available = fsiobj.available;
        std::cout << "The available disk space is " << fsiobj.available << " Bytes\n";
        std::cout << "Process completed. Returning values...\n";
        return space_info;
    } catch (const fs::filesystem_error& fserr) {
        std::cerr << "Failed to retrieve disk space information. Error message: " << fserr.what() << "\n";
        return std::nullopt;
    }
}

#endif //JUSTORE_SYSUTILS_HPP
