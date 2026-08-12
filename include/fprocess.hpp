//
// Created by NakamuraSama072 on 2026/7/11.
//

#ifndef JUSTORE_FPROCESS_HPP
#define JUSTORE_FPROCESS_HPP

#include <config.h>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <optional>
#include <chrono>
#include <ctime>

// The Generic File Type enum class.
enum class FileType {
    kFailed = 255, // Errors occurred
    kSymLink = 0, // Symbolic Link
    kLink, // HARD LINK!
    kDir, // Directory
    kRegular, // Regular File
    kExecutable, // Executable File
    kUnrecognized, // Unrecognized Types
    // Add custom types here...
};

// A map of extensions and corresponding file types
inline const std::unordered_map<std::string, std::string> extensions_map = {
    // Compressed Archives
    {".zip", "ZIP Compressed File"},
    {".tar","Tarball Archive"},
    {".tar.gz", "GZ Compressed Tarball"},
    {".tar.bz2","BZ2 Compressed Tarball"},
    {".tar.xz","XZ Compressed Tarball"},
    {".tar.zst", "Z-Standard Compressed Tarball"},
    // Plain Texts
    {".txt", "Text File"},
    {".md", "Markdown File"},
    // Developers
    {".sh", "Bash Script"},
    {".py", "Python Source File"},
    {".h", "C Header File"},
    {".hpp", "C++ Header File"},
    {".c", "C Source File"},
    {".cpp", "C++ Source File"},
    {".cxx", "C++ Source File"},
    {".java", "Java Source File"},
    {".rust", "Rust Source File"},
    {".kt", "Kotlin Source File"},
    {".js", "JavaScript Source File"},
    {".ts", "TypeScript Source File"},
    {".htm", "HTML File"},
    {".html", "HTML File"},
    {".css", "CSS File"},
    {".yaml", "YAML File"},
    {".yml","YAML File"},
    // ISO Images
    {".iso", "ISO Image"},
    {".img", "Disk Image"},
    // Microsoft Office and OpenDocument Formats
    {".doc", "Microsoft Office 2003 Word Document"},
    {".xls", "Microsoft Office 2003 Excel Sheet"},
    {".ppt", "Microsoft Office 2003 PowerPoint Slide"},
    {".docx", "Microsoft Office Word Document"},
    {".xlsx", "Microsoft Office Excel Sheet"},
    {".pptx", "Microsoft Office PowerPoint Slide"},
    {".odt", "OpenDocument Format Text"},
    {".ods", "OpenDocument Format Sheet"},
    {".odp", "OpenDocument Format Slide"},
    {".odg", "OpenDocument Format Image"},
    // Documents
    {".pdf", "PDF Document"},
    {".json", "JSON File"},
    {".xml", "XML File"},
    // Images
    {".jpg", "JPG Image"},
    {".jpeg", "JPEG Image"},
    {".png", "PNG Image"},
    {".bmp", "Bitmap Image"},
    {".gif", "GIF Animation"},
    {".svg", "Vector Image"},
    {".HEIC", "Apple Image"},
    // Media
    {".mp3", "MPEG3 Audio Format"},
    {".ogg", "OggVorbis Audio"},
    {".wav", "Windows WaveForm Audio"},
    {".flac", "Free Lossless Audio Codec"},
    {".mp4", "MPEG4 Video Format"},
    {".avi", "Audio Video Interleaved File"},
    {".mkv", "Matroska Video File"},
    {".mov", "Apple QuickTime Encapsulation Video"}
};

// The reason why I replaced class with struct is that according to Google Style,
// struct is preferred for just storing values.
struct FileEntry {
    std::string fname; // file name
    std::string ftype; // file type
    uint64_t fsize = 0; // file size (in Bytes)
    std::string last_modified; // last modified time
    bool is_directory = false; // flag to mark if the file is a directory
    bool is_symlink = false; // flag to mark if the file is a symlink
    bool is_hard_link = false; // flag to mark if the file is a hard link
    std::optional<std::string> link_target; // target of a SYMLINK

    // Overloading "<" operator (dir-type first, dict order afterward)
    bool operator<(const FileEntry& other) const {
        if (is_directory ^ other.is_directory) // not equal to
            return is_directory > other.is_directory;
        return fname < other.fname;
    }
};

// Returns the GENERIC type of given file.
inline FileType get_generic_file_type(const fs::path& fpath) {
    try {
        std::cout << "Trying to look at " << fpath << "...\n";
        // Symbolic Link
        if (fs::is_symlink(fpath)) {
            std::cout << "Symlink detected.\n";
            return FileType::kSymLink;
        }

        // Directory
        if (fs::is_directory(fpath)) {
            std::cout << "The file is in fact a directory.\n";
            return FileType::kDir;
        }

        // Regular File and Hard Link
        if (fs::is_regular_file(fpath)) {
            if (fs::hard_link_count(fpath) > 1) {
                std::cout << "Hard link detected.\n";
                // Hard Link
                return FileType::kLink;
            }

            // Executable
            const std::string fext = fpath.extension().string();
            // For Windows, the executable file is "xxx.exe"
            if (fext == ".exe") {
                std::cout << "Windows executable file detected.\n";
                return FileType::kExecutable;
            } if (fext.empty()) {
                // On UNIX-like systems, executables may come without an extension
                if (const auto permissions = fs::status(fpath).permissions();
                    (permissions & fs::perms::owner_exec) != fs::perms::none ||
                    (permissions & fs::perms::group_exec) != fs::perms::none ||
                    (permissions & fs::perms::others_exec) != fs::perms::none) {
                    std::cout << "*nix executable file detected.\n";
                    return FileType::kExecutable;
                }
            }

            // Regular File
            std::cout << "This may be a regular file. Further investigations will be conducted.\n";
            return FileType::kRegular;
        }
    } catch ([[maybe_unused]] const fs::filesystem_error& error) {
        std::cerr << "Failed to process path. Skipping it.\n";
        return FileType::kFailed;
    }
    std::cerr << "Unable to recognize file type.\n";
    return FileType::kUnrecognized; // fallback
}

// Returns the description of given file according to its extension.
inline std::string get_file_type_desc(const fs::path& fpath) {
    try {
        std::cout << "Trying to look at " << fpath << " for file type description...\n";
        std::string matched_ext;
        const std::string filename = fpath.filename().string();

        // Perform LONGEST matching rule, not using .extension() function.
        for (const auto& [ext_key, desc_val] : extensions_map) {
            if (filename.size() >= ext_key.size() && !filename.compare(
                    filename.size() - ext_key.size(),
                    ext_key.size(), ext_key)) {
                // longest extension (like .tar.gz, .tar.xz, etc.)
                if (ext_key.size() > matched_ext.size())
                    matched_ext = ext_key;
            }
        }

        std::string query_res;
        // Found extension, then for its matching description
        if (!matched_ext.empty()) {
            // Find description in the map
            if (const auto fiter = extensions_map.find(matched_ext);
                fiter != extensions_map.end()) {
                query_res = fiter->second;
                std::cout << "The file extension is: " << matched_ext << "\n";
                std::cout << "Attempting to map executable with stored map...\n";
                std::cout << "Type found! That is " << query_res << ".\n";
            }
        } else {
            // Fallback part for files without a recorded/recognized extension
            const std::string fext = fpath.extension().string();
            std::string prefix;

            // Cannot recognize extension, fallback
            if (fext.empty()) {
                std::cout << "File extension temporarily not available.\n";
                prefix = "Regular";
            } else {
                prefix = fext.substr(1);
                prefix[0] -= 32; // Make the first letter uppercase
                std::cout << "Extension \"" << fext << "\" is not recognized. "
                             "The format will be \"" << prefix << " File\" instead.\n";
            }
            query_res = prefix + " File";
        }
        return query_res;
    } catch ([[maybe_unused]] const fs::filesystem_error& error) {
        std::cerr << "Failed to process path. Skipping it.\n";
        return "Process Failed";
    }
}

// Return a std::vector of all files under a directory
inline std::vector<FileEntry> get_dir_content(const fs::path& dirpath) {
    std::vector<FileEntry> file_entries;
    try {
        std::cout << "Detecting " << dirpath << "...\n";
        // Check existence
        if (!fs::exists(dirpath) || !fs::is_directory(dirpath)) {
            std::cerr << "The path is not a directory or does not exist.\n";
            return file_entries;
        }
        std::cout << "Path existence validation succeeded. Trying to list all file entries...\n";

        // List all file entries
        fs::directory_iterator fiter(dirpath);
        std::cout << "Directory Iterator created. Proceeding...\n";
        //std::cout << "File Path\tFile Type\n";
        for (const auto& fentry : fiter) {
            std::cout << "Entered one iteration. Now trying to fetch file metadata...\n";
            try {
                FileEntry fentry_stat;

                // Get file name and type
                std::cout << "Fetching file name...\n";
                fentry_stat.fname = fentry.path().filename().string();
                std::cout << "File name is " << fentry_stat.fname << "\n";
                std::cout << "Trying to get file type...\n";
                // I think there is no need to add comments for this part though!
                switch (get_generic_file_type(fentry.path())) {
                    case FileType::kDir: {
                        std::cout << "The file is a directory\n";
                        fentry_stat.ftype = "Directory";
                        fentry_stat.is_directory = true;
                        break;
                    }
                    case FileType::kSymLink: {
                        fentry_stat.ftype = "Symbolic Link";
                        fentry_stat.is_symlink = true;
                        try {
                            fentry_stat.link_target = fs::read_symlink(fentry.path()).string();
                            std::cout << "The file is a symlink. Target: " << fentry_stat.link_target.value_or("N/A") << "\n";
                        } catch (...) {
                            throw std::runtime_error("Failed to get target of symlink");
                        }
                        break;
                    }
                    case FileType::kExecutable: {
                        std::cout << "The file is an executable file\n";
                        fentry_stat.ftype = "Executable File";
                        break;
                    }
                    case FileType::kLink: {
                        std::cout << "The file is a hard link\n";
                        fentry_stat.ftype = "Hard Link";
                        fentry_stat.is_hard_link = true;
                        break;
                    }
                    case FileType::kUnrecognized: {
                        std::cout << "The file type is not yet recognized...\n";
                        fentry_stat.ftype = "Unrecognized";
                        break;
                    }
                    case FileType::kFailed: {
                        std::cerr << "Failed to process file metadata!\n";
                        throw std::runtime_error("Failed to process file metadata!");
                    }
                    case FileType::kRegular: {
                        std::cout << "The file is a regular file. Detecting its specific type...\n";
                        auto type_desc = get_file_type_desc(fentry.path());
                        if (type_desc == "Process Failed") {
                            std::cerr << "Failed to get type description!\n";
                            throw std::runtime_error("Failed to get type description!");
                        }
                        fentry_stat.ftype = type_desc;
                        std::cout << "Specific file type detected: " << fentry_stat.ftype << "\n";
                        break;
                    }
                }

                // Get file size
                // On UNIX systems, folders will take a block, but recognized as a size of 0
                std::cout << "";
                if (fs::is_directory(fentry.path()))
                    fentry_stat.fsize = 0;
                else
                    fentry_stat.fsize = fs::file_size(fentry.path());

                // Get last modified time
                auto ftime = fs::last_write_time(fentry.path());
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
                    );
                std::time_t tt = std::chrono::system_clock::to_time_t(sctp);
                std::tm local_time;
                if (localtime_r(&tt, &local_time) == nullptr) {
                    throw std::runtime_error("Failed to get local time!");
                }
                // Convert to string
                std::ostringstream time_oss;
                time_oss << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
                fentry_stat.last_modified = time_oss.str();

                // last of assembly line: insert
                file_entries.push_back(fentry_stat);
            } catch (const std::runtime_error& rerr) {
                std::cerr << "Failed to list file entry, passing it.\n";
                std::cerr << "Error message: " << rerr.what() << "\n";
                continue;
            }
        }

        // Directory first, then sort in dictionary order
        std::sort(file_entries.begin(), file_entries.end(),
            [](const FileEntry& fobj_a, const FileEntry& fobj_b) {
                return fobj_a < fobj_b;
            });
    } catch ([[maybe_unused]] const fs::filesystem_error& error) {
        std::cerr << "Errors occurred when trying to parse.\n";
    }
    return file_entries;
}

// Resolve user path
inline std::optional<fs::path> resolve_path(const std::string& request_path) {
    try {
        std::cout << "Currently resolving path: " << request_path << "...\n";
        fs::path root_path = fs::weakly_canonical(kNetDiskRoot);
        std::cout << "Root path is: " << root_path << "\n";
        fs::path target_path = fs::weakly_canonical(root_path / request_path);
        std::cout << "Expected (in theory) target path is: " << target_path << "\n";

        // Validation: Make sure no path traversal attack happened
        fs::path relative_path_to_root = fs::relative(target_path, root_path);
        std::cout << "Attempting to print relative path: " << relative_path_to_root << "\n";
        if (!relative_path_to_root.empty() && *relative_path_to_root.begin() == "..") {
            std::cerr << "There is a path traversal attack. Please report it to the security personnel.\n";
            throw std::runtime_error("Path traversal attack detected: " + request_path + "\n");
        }
        return target_path;
    } catch (fs::filesystem_error& ferror) {
        std::cerr << "Filesystem Errors occurred when resolving path: " << ferror.what() << "\n";
    } catch (std::exception& std_err) {
        std::cerr << "Errors occurred when resolving path: " << std_err.what() << "\n";
    }
    return std::nullopt;
}

#endif //JUSTORE_FPROCESS_HPP
