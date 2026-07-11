//
// Created by NakamuraSama072 on 2026/7/11.
//

#ifndef JUSTORE_FPROCESS_HPP
#define JUSTORE_FPROCESS_HPP

#include <config.h>
#include <map>
#include <string>
#include <vector>

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
inline std::map<std::string, std::string> extensions = {
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

class FileEntry {
public:

private:

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
        std::string fext = fpath.extension().string();
        // For Windows, the executable file is "xxx.exe"
        if (fext == ".exe") return FileType::kExecutable;
        else if (fext.empty()) {
            // On UNIX, executables may not come with an extension
            auto permissions = fs::status(fpath).permissions();
            if ((permissions & fs::perms::owner_exec) != fs::perms::none ||
                (permissions & fs::perms::group_exec) != fs::perms::none ||
                (permissions & fs::perms::others_exec) != fs::perms::none) {
                return FileType::kExecutable;
            }
            return FileType::kUnrecognized;
        }
    } catch (const fs::filesystem_error& error) {
        return FileType::kFailed;
    }
    return FileType::kUnrecognized; // fallback
}

// Returns the description of given file according to its extension.
inline std::string get_file_type_desc(const fs::path& fpath) {
    try {
        std::string fext = fpath.extension().string();
        std::string query_res = fext.substr(1) + " File";
        // Find description in the map
        auto fiter = extensions.find(fext);
        if (fiter != extensions.end()) query_res = fiter->second;
        return query_res;
    } catch (const fs::filesystem_error& error) {
        return "Process Failed";
    }
}

// List all files under a directory
inline std::vector<FileEntry> list_dir_content(const fs::path& fpath) {
    // TODO: Complete this function
    return std::vector<FileEntry> {};
}

#endif //JUSTORE_FPROCESS_HPP
