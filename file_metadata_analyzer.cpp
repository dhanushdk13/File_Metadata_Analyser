#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <iomanip>
#include <sys/stat.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <ctime>
#include <chrono>
#include <sndfile.h>
#include <type_traits>

using namespace std;

// Define a pair for magic number and file type
using MagicPair = pair<string, string>;

// Magic Number Checker
struct MagicChecker {
    template<typename CharType, typename MpType>
    static string check(const CharType* buffer, size_t size, const MpType& mp) {
        for (const auto& pair : mp) {
            const auto& magic_sequence = pair.first;
            size_t magic_size = magic_sequence.size();
            if (magic_size <= size) {
                // Compare magic sequence with the beginning of the buffer
                if (memcmp(buffer, magic_sequence.data(), magic_size) == 0) {
                    cout << "File type: " << pair.second << endl;
                    return pair.second;
                }
            }
        }
        cout << "No match";
        return "";
    }
};

//placeholder classes
class image {};
class audio {};
class text {};

template<typename FileType>
struct FileTypeTraits {
    static constexpr bool is_image = std::is_same_v<FileType, image>;
    static constexpr bool is_audio = std::is_same_v<FileType, audio>;
    static constexpr bool is_text = std::is_same_v<FileType, text>;
    // Add more traits as needed
};



void printTextFileMetadata(const std::string& filename) {
    // Get file size
    struct stat file_stat;
    if (stat(filename.c_str(), &file_stat) != 0) {
        std::cerr << "Error getting file information: " << filename << std::endl;
        return;
    }
    std::cout << "File Size: " << file_stat.st_size << " bytes" << std::endl;

    // Get file creation time
    std::time_t creation_time = file_stat.st_ctime;
    std::cout << "Creation Time: ";
    std::cout << std::put_time(std::localtime(&creation_time), "%c") << std::endl;

    // Get file modification time
    std::time_t modification_time = file_stat.st_mtime;
    std::cout << "Modification Time: ";
    std::cout << std::put_time(std::localtime(&modification_time), "%c") << std::endl;
}



// Base template for file categorizer
template <typename FileType>
class file_categorizer {
public:
    void categorize(FileType file, const string& filename, const string& filetype) {
        std::cout << "This File type is not Categorized yet\n";
    }
};

// Specialization for images
template <>
class file_categorizer<image> {
public:
    void categorize(const image& file, const std::string& filename, const std::string& filetype) {
        if (FileTypeTraits<image>::is_image) {
            stbi_set_flip_vertically_on_load(false);
            int width, height, channels;
            unsigned char *image_data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_default);
            // Print image dimensions

            if(image_data){
                std::cout << "Image dimensions: " << width << " x " << height << std::endl;
                
                // Print color space
                std::string color_space;
                switch (channels) {
                    case 1:
                        color_space = "Grayscale";
                        break;
                    case 2:
                        color_space = "Grayscale with alpha";
                        break;
                    case 3:
                        color_space = "RGB";
                        break;
                    case 4:
                        color_space = "RGBA";
                        break;
                    default:
                        color_space = "Unknown";
                        break;
                }
                std::cout << "Color space: " << color_space << std::endl;
                stbi_image_free(image_data);
            } else {
                // Error getting image info
                std::cerr << "Error getting image info: " << filename << std::endl;
            }
        } else {
            std::cerr << "Invalid file type for image categorization" << std::endl;
        }
    }
};

// Specialization for audio
template <>
class file_categorizer<audio> {
public:
    void categorize(audio file, const std::string& filename, const std::string& filetype) {
        if (FileTypeTraits<audio>::is_audio) {
            std::cout << "Categorizing audio file: " << filename << std::endl;

            // Get file information
            struct stat file_stat;
            if (stat(filename.c_str(), &file_stat) != 0) {
                std::cerr << "Error getting file information: " << filename << std::endl;
                return;
            }

            // Print file size
            std::cout << "File Size: " << file_stat.st_size << " bytes" << std::endl;

            // Get file creation time
            std::time_t creation_time = file_stat.st_ctime;
            std::cout << "Creation Time: ";
            std::cout << std::put_time(std::localtime(&creation_time), "%c") << std::endl;

            // Get file modification time
            std::time_t modification_time = file_stat.st_mtime;
            std::cout << "Modification Time: ";
            std::cout << std::put_time(std::localtime(&modification_time), "%c") << std::endl;
        } else {
            std::cerr << "Invalid file type for audio categorization" << std::endl;
        }
    }
};

// Specialization for text files
template <>
class file_categorizer<text> {
public:
    void categorize(text file, const string& filename, const string& filetype) {
        if (FileTypeTraits<text>::is_text) {
            std::cout << "Categorizing text file\n";
            printTextFileMetadata(filename);
        } else {
            std::cerr << "Invalid file type for text categorization" << std::endl;
        }
    }
};


// Specialization for compressed files
// template <>
// class file_categorizer<compressed> {
// public:
//     void categorize(compressed file, const string& filename, const string& filetype) {
//         std::cout << "Categorizing compressed file\n";
//     }
// };




class FileAnalyzer {
public:
    // Lambda Template for File Reading
    template<typename MpType>
    void read_file(const char* filename) {
        auto file_reader = [this](const char* filename) {
            ifstream file(filename, ios::binary);
            if (!file) {
                cerr << "Error opening file: " << filename << endl;
                return;
            }
            file.seekg(0, ios::end);
            size_t size = file.tellg();
            file.seekg(0, ios::beg);
            vector<char> buffer(size);
            file.read(buffer.data(), size);
            file.close();
            MpType magic_numbers;
            initialize_magic_numbers(magic_numbers,
                std::make_pair("\xFF\xD8\xFF", "jpeg"),
                std::make_pair("\x89\x50\x4E\x47", "png"),
                std::make_pair("\x47\x49\x46", "gif"),
                std::make_pair("RIFF", "wav"),
                std::make_pair("OggS", "ogg"),
                std::make_pair("ID3", "mp3"),
                std::make_pair("\x25\x50\x44\x46", "pdf"),
                std::make_pair("\x50\x4B\x03\x04", "docx")
            );

            analyze_file(buffer.data(), size, magic_numbers, filename);
        };
        file_reader(filename);
    }

private:
    // Initialize Magic Numbers
    template<typename... Args>
    void initialize_magic_numbers(std::vector<MagicPair>& magic_numbers, Args&&... args) {
        // Use fold expressions to emplace each pair of magic numbers and file formats
        (magic_numbers.emplace_back(std::forward<Args>(args)), ...);
}

    // Analyze File Function
    template<typename MpType>
    void analyze_file(const char* buffer, size_t size, const MpType& magic_numbers, const string& filename) {
        if (size == 0) {
            cout << "File is empty" << endl;
            return;
        }
        cout << "Contents of the file:" << endl;
        for (size_t i = 0; i < 20; ++i) {
            cout << hex << setw(2) << setfill('0') << static_cast<int>(static_cast<unsigned char>(buffer[i])) << " ";
        }
        cout << endl;
        string file_type = MagicChecker::check(buffer, size, magic_numbers);
        if (!file_type.empty()) {
            categorize_file(file_type,filename);
        } else {
            cout << "Unknown file type" << endl;
        }
    }

    // Categorize File Function
    void categorize_file(const string& file_type, const string& filename) {
        if (file_type == "jpeg" || file_type == "png" || file_type == "gif") {
            file_categorizer<image> image_categorizer;
            image img;
            image_categorizer.categorize(img,filename,file_type);
        } else if (file_type == "wav" || file_type == "ogg" || file_type == "mp3") {
            file_categorizer<audio> audio_categorizer;
            audio aud;
            audio_categorizer.categorize(aud,filename,file_type);
        } else if (file_type == "pdf" || file_type == "docx") {
            file_categorizer<text> text_categorizer;
            text txt;
            text_categorizer.categorize(txt,filename,file_type);
        }
        // } else if (file_type == "zip") {
        //     file_categorizer<compressed> compressed_categorizer;
        //     compressed cmp;
        //     compressed_categorizer.categorize(cmp,filename,file_type);
        // } 
        else {
            file_categorizer<decltype(file_type)> default_categorizer;
            default_categorizer.categorize(file_type,filename,file_type);
        }

    }
};

int main() {
    FileAnalyzer file_analyzer;

    vector<MagicPair> magic_numbers;

    string filepath;
    while (true) {
        cout<<endl;
        cout << "Enter the file path or type 'exit' to quit: ";
        getline(cin, filepath);

        if (filepath == "exit") {
            break;
        }

        // Convert string to const char* for file_reader lambda function
        const char* filename = filepath.c_str();

        file_analyzer.read_file<decltype(magic_numbers)>(filename);
    }

    return 0;
}
