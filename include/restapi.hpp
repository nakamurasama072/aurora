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

#ifndef JUSTORE_RESTAPI_HPP
#define JUSTORE_RESTAPI_HPP

#include <fprocess.hpp>
#include <sysutils.hpp>
#include <external/crow/crow_all.h>

// Validate listening port of the service (range: [0, 65535])
inline bool validate_listening_port(const uint16_t listening_port) {
    return listening_port > 0 && listening_port < 65535;
}

inline constexpr uint32_t kFailReturnCode = 0x03fac1;

// The Rest API class for this project, using Crow Lib.
class CrowRestAPI {
public:
    explicit CrowRestAPI(crow::SimpleApp& crowapp,
                         const uint16_t listening_port = 18230) :
    app_(crowapp),
    port_(listening_port) {
        if (!validate_listening_port(listening_port)) { // Check listening port
            std::cerr << "Invalid listening port " << listening_port << " specified. Please assign another one.";
            exit(kFailReturnCode);
        }
        // crowapp.port(listening_port).multithreaded();
        register_api_routes();
    };

    ~CrowRestAPI() = default;

    // Run the app as required, not created
    void run() const {
        app_.port(port_).multithreaded().run();
    }
private:
    crow::SimpleApp& app_;
    uint16_t port_;

    // Request handlers (Add more if you wish)
    static crow::response handle_file_browse(const crow::request& request);
    static crow::response handle_storage();

    // register REST API
    void register_api_routes() const;
    static crow::json::wvalue create_fentry_json(const FileEntry& fentry);
};

// Handle file browse requests
inline crow::response CrowRestAPI::handle_file_browse(const crow::request & request) {
    // request pattern: /api/files?path=foo/bar
    const auto path_params = request.url_params.get("path");
    crow::json::wvalue response_json;
    // crow::response response;

    // Resolve and validate the requested path
    const std::string request_path = path_params ? path_params : "";
    const auto resolved_req_path = resolve_path(request_path);
    if (!resolved_req_path) { // is nullptr
        response_json["success"] = false;
        response_json["message"] = "Forbidden operation (path traversal attack) detected";
        return {403, response_json};
    }

    // If resolve succeeded, read the directory
    const auto files = get_dir_content(*resolved_req_path);
    std::cout << "Number of files found: "
          << files.size()
          << "\n";
    response_json["success"] = true;
    response_json["path"] = request_path;
    response_json["message"] = "Fetch success";

    crow::json::wvalue::list entries;
    for (const auto& file : files) {
        entries.push_back(create_fentry_json(file));
    }
    response_json["entries"] = std::move(entries);

    return {200, response_json};
}

// Handle storage requests
inline crow::response CrowRestAPI::handle_storage() {
    // request pattern: /api/system/storage
    crow::json::wvalue response_json;
    auto fspaceinfo = get_disk_space();
    if (!fspaceinfo) {
        std::cerr << "Errors occurred when attempting to get disk space information.\n";
        response_json["success"] = false;
        response_json["message"] = "Server processing failure, please check server status";
        return {500, response_json};
    }
    std::cout << "Converting DiskSpace struct to JSON...\n";
    response_json["success"] = true;
    response_json["total"] = fspaceinfo->total;
    response_json["available"] = fspaceinfo->available;
    response_json["message"] = "Fetch success";

    return {200, response_json};
}

// Register REST API Routes that is available
inline void CrowRestAPI::register_api_routes() const {
    // List files
    CROW_ROUTE(app_, "/api/files")
    ([](const crow::request& request) {
        return handle_file_browse(request);
    });

    // Get disk space
    CROW_ROUTE(app_, "/api/system/storage")
    ([](const crow::request& request) {
        return handle_storage();
    });
    // Add more REST API router here...
}

// Create a json containing metadata of a given file
inline crow::json::wvalue CrowRestAPI::create_fentry_json(const FileEntry &fentry) {
    std::cout << "Creating JSON for: "
              << fentry.fname
              << "\n";
    crow::json::wvalue fjson;

    std::cout << "name detected...\n";
    fjson["name"] = fentry.fname;
    std::cout << "file type detected...\n";
    fjson["type"] = fentry.ftype;
    std::cout << "file size detected...\n";
    fjson["size"] = fentry.fsize;
    std::cout << "last modified time detected...\n";
    fjson["last_modified"] = fentry.last_modified;
    std::cout << "is_directory detected...\n";
    fjson["directory"] = fentry.is_directory;
    std::cout << "is_symlink detected...\n";
    fjson["symlink"] = fentry.is_symlink;
    std::cout << "is_hard_link detected...\n";
    fjson["hard_link"] = fentry.is_hard_link;
    // link target may be nullptr
    if (fentry.link_target) {
        std::cout << "hard link target detected...\n";
        fjson["target"] = *fentry.link_target;
    }
    std::cout << "JSON creation succeeded. Finalizing results...\n";
    return fjson;
}

#endif //JUSTORE_RESTAPI_HPP
