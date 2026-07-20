//
// Created by NakamuraSama072 on 2026/7/11.
//

#ifndef JUSTORE_FPROCESS_HPP
#define JUSTORE_FPROCESS_HPP

#include <config.h>
#include <map>
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
inline std::map<std::string, std::string> extensions_map = {
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
    std::optional<std::string> link_target = nullptr; // target of a SYMLINK

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
        // Symbolic Link
        if (fs::is_symlink(fpath)) {
            return FileType::kSymLink;
        }

        // Regular File and Hard Link
        if (fs::is_regular_file(fpath)) {
            if (fs::hard_link_count(fpath) > 1) {
                // Hard Link
                return FileType::kLink;
            }
            // Regular File
            return FileType::kRegular;
        }

        // Directory
        if (fs::is_directory(fpath)) {
            return FileType::kDir;
        }

        // Executable
        const std::string fext = fpath.extension().string();
        // For Windows, the executable file is "xxx.exe"
        if (fext == ".exe") return FileType::kExecutable;
        if (fext.empty()) {
            // On UNIX-like systems, executables may come without an extension
            if (const auto permissions = fs::status(fpath).permissions();
                (permissions & fs::perms::owner_exec) != fs::perms::none ||
                (permissions & fs::perms::group_exec) != fs::perms::none ||
                (permissions & fs::perms::others_exec) != fs::perms::none) {
                return FileType::kExecutable;
            }
            return FileType::kUnrecognized;
        }
    } catch ([[maybe_unused]] const fs::filesystem_error& error) {
        return FileType::kFailed;
    }
    return FileType::kUnrecognized; // fallback
}

// Returns the description of given file according to its extension.
inline std::string get_file_type_desc(const fs::path& fpath) {
    try {
        const std::string fext = fpath.extension().string();
        std::string query_res = fext.substr(1) + " File";
        // Find description in the map
        if (const auto fiter = extensions_map.find(fext);
            fiter != extensions_map.end())
            query_res = fiter->second;
        return query_res;
    } catch ([[maybe_unused]] const fs::filesystem_error& error) {
        return "Process Failed";
    }
}

// Return a std::vector of all files under a directory
inline std::vector<FileEntry> get_dir_content(const fs::path& dirpath) {
    std::vector<FileEntry> file_entries;
    try {
        // Check existence
        if (!fs::exists(dirpath) || !fs::is_directory(dirpath)) {
            std::cerr << "The path is not a directory or does not exist.\n";
            return file_entries;
        }

        // List all file entries
        fs::directory_iterator fiter(dirpath);
        std::cout << "File Path\tFile Type\n";
        for (const auto& fentry : fiter) {
            try {
                FileEntry fentry_stat;

                // Get file name and type
                fentry_stat.fname = fentry.path().filename().string();
                // I think there is no need to add comments for this part though!
                switch (get_generic_file_type(fentry.path())) {
                    case FileType::kDir: {
                        fentry_stat.ftype = "Directory";
                        fentry_stat.is_directory = true;
                        break;
                    }
                    case FileType::kSymLink: {
                        fentry_stat.ftype = "Symbolic Link";
                        fentry_stat.is_symlink = true;
                        try {
                            fentry_stat.link_target = fs::read_symlink(fentry.path()).string();
                        } catch (...) {
                            throw std::runtime_error("Failed to get target of symlink");
                        }
                        break;
                    }
                    case FileType::kExecutable: {
                        fentry_stat.ftype = "Executable File";
                        break;
                    }
                    case FileType::kLink: {
                        fentry_stat.ftype = "Hard Link";
                        fentry_stat.is_hard_link = true;
                        break;
                    }
                    case FileType::kUnrecognized: {
                        fentry_stat.ftype = "Unrecognized";
                        break;
                    }
                    case FileType::kFailed: {
                        throw std::runtime_error("Failed to process file metadata!");
                    }
                    case FileType::kRegular: {
                        auto type_desc = get_file_type_desc(fentry.path());
                        if (type_desc == "Process Failed")
                            throw std::runtime_error("Failed to get type description!");
                        fentry_stat.ftype = type_desc;
                        break;
                    }
                }

                // Get file size
                // On UNIX systems, folders will take a block
                if (fs::is_directory(fentry.path()))
                    fentry_stat.fsize = 4096;
                else
                    fentry_stat.fsize = fs::file_size(fentry.path());

                // Get last modified time
                auto ftime = fs::last_write_time(fentry.path());
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
                    );
                std::time_t tt = std::chrono::system_clock::to_time_t(sctp);
                tm *local_time = std::localtime(&tt);
                // Convert to string
                std::ostringstream time_oss;
                time_oss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
                fentry_stat.last_modified = time_oss.str();

                // last of assembly line: insert
                file_entries.push_back(fentry_stat);
            } catch (...) {
                // std::cerr << "Failed to list file entry, passing it.\n";
                // continue;
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

#endif //JUSTORE_FPROCESS_HPP
