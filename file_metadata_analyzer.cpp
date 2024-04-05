#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>

using namespace std;

// Custom Map Implementation
template<typename T, typename U>
class Mp {
private:
    vector<T> keys;
    vector<U> values;

public:
    void add(T key, U value) {
        auto it = lower_bound(keys.begin(), keys.end(), key);
        if (it != keys.end() && *it == key) {
            values[it - keys.begin()] = value;
        }
        else {
            int index = it - keys.begin();
            keys.insert(it, key);
            values.insert(values.begin() + index, value);
        }
    }

    typename vector<T>::const_iterator begin_keys() const {
        return keys.begin();
    }

    typename vector<T>::const_iterator end_keys() const {
        return keys.end();
    }

    typename vector<U>::const_iterator begin_values() const {
        return values.begin();
    }

    typename vector<U>::const_iterator end_values() const {
        return values.end();
    }

    void display() const {
        for (size_t i = 0; i < keys.size(); ++i) {
            cout << keys[i] << " : " << values[i] << endl;
        }
    }

    U& operator[](const T& key) {
        auto it = lower_bound(keys.begin(), keys.end(), key);
        if (it != keys.end() && *it == key) {
            return values[it - keys.begin()];
        }
        else {
            throw out_of_range("Key not found");
        }
    }

    void del(T ele) {
        auto it = lower_bound(keys.begin(), keys.end(), ele);
        if (it != keys.end() && *it == ele) {
            int index = it - keys.begin();
            keys.erase(keys.begin() + index);
            values.erase(values.begin() + index);
        }
        else {
            throw out_of_range("Key not found");
        }
    }

    U find(T key) const {
        auto it = lower_bound(keys.begin(), keys.end(), key);
        if (it != keys.end() && *it == key) {
            return values[it - keys.begin()];
        }
        else {
            throw out_of_range("Key not found");
        }
    }

    size_t size() const {
        return keys.size();
    }
};

// Magic Number Checker
template<typename MpType>
struct MagicChecker {
    template<typename CharType>
    static string check(const CharType* buffer, size_t size, const MpType& mp) {
        for (auto it = mp.begin_keys(); it != mp.end_keys(); ++it) {
            const auto& magic_sequence = *it;
            if (magic_sequence.size() <= size) {
                if (memcmp(buffer, magic_sequence.data(), magic_sequence.size()) == 0) {
                    return mp.find(*it);
                }
            }
        }
        return "";
    }
};

// File Analyzer
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
            initialize_magic_numbers(magic_numbers);
            analyze_file(buffer.data(), size, magic_numbers);
        };
        file_reader(filename);
    }

private:
    // Initialize Magic Numbers
    template<typename MpType>
    void initialize_magic_numbers(MpType& magic_numbers) {
        // Define known file signatures (magic numbers)
        magic_numbers.add("\xFF\xD8\xFF", "JPEG"); // JPEG/JFIF
        magic_numbers.add("\x89\x50\x4E\x47", "PNG"); // PNG
        magic_numbers.add("\x47\x49\x46", "GIF"); // GIF
    }

    // Analyze File Function
    template<typename MpType>
    void analyze_file(const char* buffer, size_t size, const MpType& magic_numbers) {
        cout << "Contents of the file:" << endl;
         for (size_t i = 0; i < 20; ++i) {
            cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(buffer[i])) << " ";
        }
        cout << endl;
        string file_type = MagicChecker<MpType>::check(buffer, size, magic_numbers);
        if (!file_type.empty()) {
            categorize_file(magic_numbers, file_type);
        } else {
            cout << "Unknown file type" << endl;
        }
    }

    // Categorize File Function
    template<typename MpType>
    void categorize_file(const MpType& magic_numbers, const string& file_type) {
        if (file_type == "JPEG" || file_type == "PNG" || file_type == "GIF") {
            cout << "File belongs to Image category, Format: " << file_type << endl;
        } else {
            cout << "File belongs to Unknown category" << endl;
        }
    }

};

// Usage
int main() {
    FileAnalyzer file_analyzer;

    Mp<string, string> magic_numbers;

    // Define known file signatures (magic numbers)
    magic_numbers.add("\xFF\xD8\xFF", "JPEG"); // JPEG/JFIF
    magic_numbers.add("\x89\x50\x4E\x47", "PNG"); // PNG
    magic_numbers.add("\x47\x49\x46", "GIF"); // GIF

    string filepath;
    while (true) {
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
