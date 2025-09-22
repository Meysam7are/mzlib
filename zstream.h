#ifndef MZ_STREAM_HEADER_FILE
#define MZ_STREAM_HEADER_FILE

#pragma once

#include "globals.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

/**
 * @file zstream.h
 * @brief Provides a unified stream API for file and string I/O in the mz library.
 *
 * This header defines:
 *   - mz::Stream: abstract base class for streams.
 *   - mz::FileStream: file-based stream implementation.
 *   - mz::StringStream: string-based stream implementation.
 *   - Serialization operators for trivially copyable types and std::vector.
 *
 * The design allows any class to load or save its data using a Stream pointer/reference,
 * without caring whether the underlying stream is a file or a string. This enables
 * flexible serialization, testing, and in-memory operations with a single API.
 *
 * Usage example:
 *   mz::FileStream fs("data.bin");
 *   mz::StringStream ss;
 *   MyType obj;
 *   obj.save(fs); // Save to file
 *   obj.save(ss); // Save to string
 *   obj.load(fs); // Load from file
 *   obj.load(ss); // Load from string
 */

namespace mz {

    /**
     * @brief Abstract base class for streams.
     *
     * Provides a unified interface for reading/writing trivially copyable types,
     * as well as file and string operations. All serialization and deserialization
     * code should use Stream& as the argument type, allowing seamless switching
     * between file and string streams.
     *
     * Derived classes must implement the low-level read_bytes and write_bytes methods.
     */
    class Stream {
    protected:
        // Low-level read/write (must be implemented by derived classes)
        virtual void read_bytes(char* ptr, arg_type size) noexcept = 0;
        virtual void write_bytes(const char* ptr, arg_type size) noexcept = 0;

    public:
        // Stream management and meta operations
        virtual Stream& clear() noexcept = 0;
        virtual Stream& operator=(const Stream& rhs) noexcept = 0;
        virtual Stream& operator<<(const Stream& rhs) noexcept = 0;
        virtual std::streambuf* rdbuf() const noexcept = 0;
        virtual bool empty() noexcept = 0;
        virtual void end() = 0;
        virtual void close() = 0;
        virtual void begin() = 0;
        virtual bool is_open() const noexcept = 0;
        virtual bool is_file() const noexcept = 0;
        virtual void save(const char* name) = 0;
        virtual void load(const char* name) = 0;
        virtual void flush() noexcept = 0;

        // Typed read/write for trivially copyable types
        template <class T>
            requires(std::is_trivially_copyable_v<T>)
        void read(T& x) noexcept { read_bytes(reinterpret_cast<char*>(&x), sizeof(T)); }

        template <class T>
            requires(std::is_trivially_copyable_v<T>)
        void write(const T& x) noexcept { write_bytes(reinterpret_cast<const char*>(&x), sizeof(T)); }

        template <class T>
            requires(std::is_trivially_copyable_v<T>)
        void read(T* ptr, int count) noexcept { read_bytes(reinterpret_cast<char*>(ptr), sizeof(T) * count); }

        template <class T>
            requires(std::is_trivially_copyable_v<T>)
        void write(const T* ptr, int count) noexcept { write_bytes(reinterpret_cast<const char*>(ptr), sizeof(T) * count); }

        // Virtual destructor
        virtual ~Stream() = 0;

        // Friend operators for stream << and >>
        template <typename T>
            requires(std::is_trivially_copyable_v<T>)
        friend Stream& operator<<(Stream& ss, const T& x) { ss.write(x); return ss; }

        template <typename T>
            requires(std::is_trivially_copyable_v<T>)
        friend Stream& operator>>(Stream& ss, T& x) { ss.read(x); return ss; }

        /**
         * @brief Reads a label value from the stream and checks if it matches the expected encoding.
         *
         * This function is used to identify and verify the type of a complex data structure
         * (such as a vector or custom object) during deserialization. The encoding value acts
         * as a type or version identifier. When reading, if the label in the stream does not
         * match the expected encoding, the function returns true to signal a mismatch.
         *
         * This mechanism helps ensure that the data being read matches the expected type,
         * preventing errors from reading incompatible or corrupted data.
         *
         * @param encoding Expected label value (type or version id).
         * @return true if the label does not match the expected encoding, false otherwise.
         *
         * Usage:
         *   // Before reading a vector, check its label:
         *   if (stream.read_label(VectorTypeId)) { throw std::runtime_error("Type mismatch"); }
         */
        bool read_label(uint64_t encoding = 0) noexcept {
            uint64_t x{ encoding - 1 };
            if (encoding) {
                read(x);
                return x != encoding;
            }
            return false;
        }

        /**
         * @brief Writes a label value to the stream to identify the type of the following data.
         *
         * This function is used to mark the beginning of a complex data structure
         * (such as a vector or custom object) during serialization. The encoding value acts
         * as a type or version identifier, allowing the reader to verify the type when
         * deserializing.
         *
         * This mechanism helps ensure that the data can be correctly identified and
         * validated during deserialization, supporting robust and extensible serialization
         * of heterogeneous data.
         *
         * @param encoding Label value (type or version id) to write.
         *
         * Usage:
         *   // Before writing a vector, write its label:
         *   stream.write_label(VectorTypeId);
         */
        void write_label(uint64_t encoding = 0) noexcept {
            if (encoding) { write(encoding); }
        }
    };

    inline Stream::~Stream() {}

    /**
     * @brief File-based stream implementation.
     *
     * Wraps std::fstream for file I/O. Implements all Stream virtual methods
     * for reading and writing to files. Can be used anywhere a Stream& is required.
     *
     * Usage:
     *   mz::FileStream fs("data.bin");
     *   obj.save(fs); // Save to file
     *   obj.load(fs); // Load from file
     */
    class FileStream final : public Stream {
        std::fstream file_handle_;
        std::string file_name_;

        void read_bytes(char* ptr, arg_type size) noexcept override final { file_handle_.read(ptr, size); }
        void write_bytes(const char* ptr, arg_type size) noexcept override final { file_handle_.write(ptr, size); }

    public:
        void flush() noexcept override final { file_handle_.flush(); }
        void end() override final { file_handle_.seekg(0, file_handle_.end); }
        void close() override final { file_handle_.close(); file_name_.clear(); }
        void begin() override final { file_handle_.seekg(std::ios_base::beg); }
        bool is_file() const noexcept override final { return true; }
        bool is_open() const noexcept override final { return file_handle_.is_open(); }
        bool empty() noexcept override final {
            return !file_handle_.is_open() || file_handle_.peek() == std::ifstream::traits_type::eof();
        }
        FileStream& clear() noexcept override final {
            file_handle_.close();
            file_handle_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
            return *this;
        }
        FileStream& operator=(const Stream& rhs) noexcept override final { return clear() << rhs; }
        FileStream& operator<<(const Stream& rhs) noexcept override final { file_handle_ << rhs.rdbuf(); return *this; }
        std::streambuf* rdbuf() const noexcept override final { return file_handle_.rdbuf(); }

        FileStream() = default;
        FileStream(std::string name) {
            file_name_ = std::move(name);
            file_handle_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
            if (!file_handle_.is_open()) {
                file_handle_.open(file_name_, std::ios::out | std::ios::binary);
                file_handle_.close();
                file_handle_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
                if (!file_handle_.is_open()) DOMAIN_ERROR_IF(true, "Error could not open {}", file_name_);
            }
        }

        /**
         * @brief Opens a file for reading.
         * @param name File name to open.
         */
        void open_for_read(std::string name) {
            ASSERT_IF(file_handle_.is_open(), "Cannot open stream for read {}. Handle is still open for {}\n", name, file_name_);
            file_handle_.open(name, std::ios::in | std::ios::binary);
            ASSERT_IF(!file_handle_.is_open(), "Cannot open stream for read: {} not found.\n", name);
            file_name_ = std::move(name);
        }

        ~FileStream() { file_handle_.close(); }
        void load(const char* /*name*/) override final {}
        void save(const char* /*name*/) override final {}
        FileStream& operator=(const FileStream& rhs) noexcept {
            if (this != &rhs) { clear(); file_handle_ << rhs.file_handle_.rdbuf(); }
            return *this;
        }
    };

    /**
     * @brief String-based stream implementation.
     *
     * Wraps std::stringstream for in-memory I/O. Implements all Stream virtual methods
     * for reading and writing to strings. Can be used anywhere a Stream& is required.
     *
     * Usage:
     *   mz::StringStream ss;
     *   obj.save(ss); // Save to string
     *   obj.load(ss); // Load from string
     */
    class StringStream final : public Stream {
        std::stringstream string_handle_;

        void read_bytes(char* ptr, arg_type size) noexcept override final { string_handle_.read(ptr, size); }
        void write_bytes(const char* ptr, arg_type size) noexcept override final { string_handle_.write(ptr, size); }

    public:
        bool empty() noexcept override final {
            return string_handle_.peek() == std::ifstream::traits_type::eof();
        }
        void flush() noexcept override final { string_handle_.flush(); }
        void end() override final { string_handle_.seekg(0, string_handle_.end); }
        void close() override final {}
        void begin() override final { string_handle_.seekg(std::ios_base::beg); }
        bool is_file() const noexcept override final { return false; }
        bool is_open() const noexcept override final { return true; }
        StringStream& clear() noexcept override final { string_handle_.str(std::string()); return *this; }
        StringStream& operator=(const Stream& rhs) noexcept override final { return clear() << rhs; }
        StringStream& operator<<(const Stream& rhs) noexcept override final { string_handle_ << rhs.rdbuf(); return *this; }
        std::streambuf* rdbuf() const noexcept override final { return string_handle_.rdbuf(); }
        StringStream() = default;

        /**
         * @brief Loads data from a file into the string stream.
         * @param fname File name to load.
         */
        void load(const char* fname) override final {
            std::fstream f(fname, std::ios::in | std::ios::binary);
            if (f.is_open()) string_handle_ << f.rdbuf();
            else DOMAIN_ERROR_IF(true, "Error cannot load {}", fname);
        }

        /**
         * @brief Saves data from the string stream to a file.
         * @param fname File name to save.
         */
        void save(const char* fname) override final {
            std::fstream f(fname, std::ios::out | std::ios::binary);
            if (f.is_open()) f << string_handle_.rdbuf();
            else DOMAIN_ERROR_IF(true, "Error cannot save {}", fname);
        }

        ~StringStream() = default;
        StringStream& operator=(const StringStream& rhs) noexcept {
            if (this != &rhs) { clear(); string_handle_ << rhs.string_handle_.rdbuf(); }
            return *this;
        }
    };

} // namespace mz

//-----------------------------------------------------------------------------
// Serialization operators for trivially copyable types and std::vector
//-----------------------------------------------------------------------------

/**
 * @brief Writes a trivially copyable type to a std::iostream.
 */
template <typename T>
    requires(std::is_trivially_copyable_v<T>)
inline std::iostream& operator<<(std::iostream& ss, const T& t) {
    ss.write(reinterpret_cast<const char*>(&t), sizeof(T));
    return ss;
}

/**
 * @brief Reads a trivially copyable type from a std::iostream.
 */
template <typename T>
    requires(std::is_trivially_copyable_v<T>)
inline std::iostream& operator>>(std::iostream& ss, T& t) {
    ss.read(reinterpret_cast<char*>(&t), sizeof(T));
    return ss;
}

/**
 * @brief Writes a std::vector of serializable elements to a std::iostream.
 */
template <class T>
    requires requires(std::iostream& ss, const T& x) { ss << x; }
std::iostream& operator<<(std::iostream& ss, const std::vector<T>& vec) {
    int length = static_cast<int>(vec.size());
    ss << length;
    const T* data = vec.data();
    for (int i = 0; i < length; ++i) ss << data[i];
    return ss;
}

/**
 * @brief Reads a std::vector of serializable elements from a std::iostream.
 */
template <class T>
    requires requires(std::iostream& ss, T& x) { ss >> x; }
std::iostream& operator>>(std::iostream& ss, std::vector<T>& vec) {
    int length;
    ss >> length;
    vec.resize(length);
    T* data = vec.data();
    for (int i = 0; i < length; ++i) ss >> data[i];
    return ss;
}

#endif // MZ_STREAM_HEADER_FILE