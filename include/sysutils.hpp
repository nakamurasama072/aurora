/**
 * Aurora Drive - a simple, lightweight and high-performance personal cloud storage solution.
 * Copyright (C) 2026  nakamurasama072

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
        std::cout << "Detecting the total and available space of path " << kNetDiskRoot << "...\n";
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
