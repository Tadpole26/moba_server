#ifndef JSON_VERSION_H_INCLUDED
#define JSON_VERSION_H_INCLUDED

// Note: version must be updated in three places when doing a release. This
// annoying process ensures that amalgamate, CMake, and meson all report the
// correct version.
// 1. /meson.build
// 2. /include/json/version.h
// 3. /CMakeLists.txt
// IMPORTANT: also update the SOVERSION!!

#define JSONCPP_VERSION_STRING "1.9.5"
#define JSONCPP_VERSION_MAJOR 1
#define JSONCPP_VERSION_MINOR 9
#define JSONCPP_VERSION_PATCH 5
#define JSONCPP_VERSION_QUALIFIER
#define JSONCPP_VERSION_HEXA                                                   \
  ((JSONCPP_VERSION_MAJOR << 24) | (JSONCPP_VERSION_MINOR << 16) |             \
   (JSONCPP_VERSION_PATCH << 8))

#ifdef JSONCPP_USING_SECURE_MEMORY
#undef JSONCPP_USING_SECURE_MEMORY
#endif
#define JSONCPP_USING_SECURE_MEMORY 0
// If non-zero, the library zeroes any memory that it has allocated before
// it frees its memory.

#endif // JSON_VERSION_H_INCLUDED


// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_CONFIG_H_INCLUDED
#define JSON_CONFIG_H_INCLUDED
#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>

// If non-zero, the library uses exceptions to report bad input instead of C
// assertion macros. The default is to use exceptions.
#ifndef JSON_USE_EXCEPTION
#define JSON_USE_EXCEPTION 1
#endif

// Temporary, tracked for removal with issue #982.
#ifndef JSON_USE_NULLREF
#define JSON_USE_NULLREF 1
#endif

/// If defined, indicates that the source file is amalgamated
/// to prevent private header inclusion.
/// Remarks: it is automatically defined in the generated amalgamated header.
// #define JSON_IS_AMALGAMATION

// Export macros for DLL visibility
#if defined(JSON_DLL_BUILD)
#if defined(_MSC_VER) || defined(__MINGW32__)
#define JSON_API __declspec(dllexport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#elif defined(__GNUC__) || defined(__clang__)
#define JSON_API __attribute__((visibility("default")))
#endif // if defined(_MSC_VER)

#elif defined(JSON_DLL)
#if defined(_MSC_VER) || defined(__MINGW32__)
#define JSON_API __declspec(dllimport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#endif // if defined(_MSC_VER)
#endif // ifdef JSON_DLL_BUILD

#if !defined(JSON_API)
#define JSON_API
#endif

#if defined(_MSC_VER) && _MSC_VER < 1800
#error                                                                         \
    "ERROR:  Visual Studio 12 (2013) with _MSC_VER=1800 is the oldest supported compiler with sufficient C++11 capabilities"
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
// As recommended at
// https://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
extern JSON_API int msvc_pre1900_c99_snprintf(char* outBuf, size_t size,
    const char* format, ...);
#define jsoncpp_snprintf msvc_pre1900_c99_snprintf
#else
#define jsoncpp_snprintf std::snprintf
#endif

// If JSON_NO_INT64 is defined, then Json only support C++ "int" type for
// integer
// Storages, and 64 bits integer support is disabled.
// #define JSON_NO_INT64 1

// JSONCPP_OVERRIDE is maintained for backwards compatibility of external tools.
// C++11 should be used directly in JSONCPP.
#define JSONCPP_OVERRIDE override

#ifdef __clang__
#if __has_extension(attribute_deprecated_with_message)
#define JSONCPP_DEPRECATED(message) __attribute__((deprecated(message)))
#endif
#elif defined(__GNUC__) // not clang (gcc comes later since clang emulates gcc)
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define JSONCPP_DEPRECATED(message) __attribute__((deprecated(message)))
#elif (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define JSONCPP_DEPRECATED(message) __attribute__((__deprecated__))
#endif                  // GNUC version
#elif defined(_MSC_VER) // MSVC (after clang because clang on Windows emulates
                        // MSVC)
#define JSONCPP_DEPRECATED(message) __declspec(deprecated(message))
#endif // __clang__ || __GNUC__ || _MSC_VER

#if !defined(JSONCPP_DEPRECATED)
#define JSONCPP_DEPRECATED(message)
#endif // if !defined(JSONCPP_DEPRECATED)

#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ >= 6))
#define JSON_USE_INT64_DOUBLE_CONVERSION 1
#endif

#if !defined(JSON_IS_AMALGAMATION)

#endif // if !defined(JSON_IS_AMALGAMATION)

namespace Json {
    using Int = int;
    using UInt = unsigned int;
#if defined(JSON_NO_INT64)
    using LargestInt = int;
    using LargestUInt = unsigned int;
#undef JSON_HAS_INT64
#else                 // if defined(JSON_NO_INT64)
    // For Microsoft Visual use specific types as long long is not supported
#if defined(_MSC_VER) // Microsoft Visual Studio
    using Int64 = __int64;
    using UInt64 = unsigned __int64;
#else                 // if defined(_MSC_VER) // Other platforms, use long long
    using Int64 = int64_t;
    using UInt64 = uint64_t;
#endif                // if defined(_MSC_VER)
    using LargestInt = Int64;
    using LargestUInt = UInt64;
#define JSON_HAS_INT64
#endif // if defined(JSON_NO_INT64)

    template <typename T>
    using Allocator =
        typename std::conditional<JSONCPP_USING_SECURE_MEMORY, SecureAllocator<T>,
        std::allocator<T>>::type;
    using String = std::basic_string<char, std::char_traits<char>, Allocator<char>>;
    using IStringStream =
        std::basic_istringstream<String::value_type, String::traits_type,
        String::allocator_type>;
    using OStringStream =
        std::basic_ostringstream<String::value_type, String::traits_type,
        String::allocator_type>;
    using IStream = std::istream;
    using OStream = std::ostream;
} // namespace Json

// Legacy names (formerly macros).
using JSONCPP_STRING = Json::String;
using JSONCPP_ISTRINGSTREAM = Json::IStringStream;
using JSONCPP_OSTRINGSTREAM = Json::OStringStream;
using JSONCPP_ISTREAM = Json::IStream;
using JSONCPP_OSTREAM = Json::OStream;

#endif // JSON_CONFIG_H_INCLUDED




// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_ALLOCATOR_H_INCLUDED
#define JSON_ALLOCATOR_H_INCLUDED

#include <cstring>
#include <memory>

#pragma pack(push)
#pragma pack()

namespace Json {
    template <typename T> class SecureAllocator {
    public:
        // Type definitions
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        /**
         * Allocate memory for N items using the standard allocator.
         */
        pointer allocate(size_type n) {
            // allocate using "global operator new"
            return static_cast<pointer>(::operator new(n * sizeof(T)));
        }

        /**
         * Release memory which was allocated for N items at pointer P.
         *
         * The memory block is filled with zeroes before being released.
         */
        void deallocate(pointer p, size_type n) {
            // memset_s is used because memset may be optimized away by the compiler
            memset_s(p, n * sizeof(T), 0, n * sizeof(T));
            // free using "global operator delete"
            ::operator delete(p);
        }

        /**
         * Construct an item in-place at pointer P.
         */
        template <typename... Args> void construct(pointer p, Args&&... args) {
            // construct using "placement new" and "perfect forwarding"
            ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
        }

        size_type max_size() const { return size_t(-1) / sizeof(T); }

        pointer address(reference x) const { return std::addressof(x); }

        const_pointer address(const_reference x) const { return std::addressof(x); }

        /**
         * Destroy an item in-place at pointer P.
         */
        void destroy(pointer p) {
            // destroy using "explicit destructor"
            p->~T();
        }

        // Boilerplate
        SecureAllocator() {}
        template <typename U> SecureAllocator(const SecureAllocator<U>&) {}
        template <typename U> struct rebind { using other = SecureAllocator<U>; };
    };

    template <typename T, typename U>
    bool operator==(const SecureAllocator<T>&, const SecureAllocator<U>&) {
        return true;
    }

    template <typename T, typename U>
    bool operator!=(const SecureAllocator<T>&, const SecureAllocator<U>&) {
        return false;
    }

} // namespace Json

#pragma pack(pop)

#endif // JSON_ALLOCATOR_H_INCLUDED



// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_ASSERTIONS_H_INCLUDED
#define JSON_ASSERTIONS_H_INCLUDED

#include <cstdlib>
#include <sstream>

#if !defined(JSON_IS_AMALGAMATION)
#endif // if !defined(JSON_IS_AMALGAMATION)

/** It should not be possible for a maliciously designed file to
 *  cause an abort() or seg-fault, so these macros are used only
 *  for pre-condition violations and internal logic errors.
 */
#if JSON_USE_EXCEPTION

 // @todo <= add detail about condition in exception
#define JSON_ASSERT(condition)                                                 \
  do {                                                                         \
    if (!(condition)) {                                                        \
      Json::throwLogicError("assert json failed");                             \
    }                                                                          \
  } while (0)

#define JSON_FAIL_MESSAGE(message)                                             \
  do {                                                                         \
    OStringStream oss;                                                         \
    oss << message;                                                            \
    Json::throwLogicError(oss.str());                                          \
    abort();                                                                   \
  } while (0)

#else // JSON_USE_EXCEPTION

#define JSON_ASSERT(condition) assert(condition)

 // The call to assert() will show the failure message in debug builds. In
 // release builds we abort, for a core-dump or debugger.
#define JSON_FAIL_MESSAGE(message)                                             \
  {                                                                            \
    OStringStream oss;                                                         \
    oss << message;                                                            \
    assert(false && oss.str().c_str());                                        \
    abort();                                                                   \
  }

#endif

#define JSON_ASSERT_MESSAGE(condition, message)                                \
  do {                                                                         \
    if (!(condition)) {                                                        \
      JSON_FAIL_MESSAGE(message);                                              \
    }                                                                          \
  } while (0)

#endif // JSON_ASSERTIONS_H_INCLUDED




// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_FORWARDS_H_INCLUDED
#define JSON_FORWARDS_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#endif // if !defined(JSON_IS_AMALGAMATION)

namespace Json {

    // writer.h
    class StreamWriter;
    class StreamWriterBuilder;
    class Writer;
    class FastWriter;
    class StyledWriter;
    class StyledStreamWriter;

    // reader.h
    class Reader;
    class CharReader;
    class CharReaderBuilder;

    // json_features.h
    class Features;

    // value.h
    using ArrayIndex = unsigned int;
    class StaticString;
    class Path;
    class PathArgument;
    class Value;
    class ValueIteratorBase;
    class ValueIterator;
    class ValueConstIterator;

} // namespace Json

#endif // JSON_FORWARDS_H_INCLUDED



// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_JSON_H_INCLUDED
#define JSON_JSON_H_INCLUDED

#endif // JSON_JSON_H_INCLUDED




// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_FEATURES_H_INCLUDED
#define JSON_FEATURES_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#endif // if !defined(JSON_IS_AMALGAMATION)

#pragma pack(push)
#pragma pack()

namespace Json {

    /** \brief Configuration passed to reader and writer.
     * This configuration object can be used to force the Reader or Writer
     * to behave in a standard conforming way.
     */
    class JSON_API Features {
    public:
        /** \brief A configuration that allows all features and assumes all strings
         * are UTF-8.
         * - C & C++ comments are allowed
         * - Root object can be any JSON value
         * - Assumes Value strings are encoded in UTF-8
         */
        static Features all();

        /** \brief A configuration that is strictly compatible with the JSON
         * specification.
         * - Comments are forbidden.
         * - Root object must be either an array or an object value.
         * - Assumes Value strings are encoded in UTF-8
         */
        static Features strictMode();

        /** \brief Initialize the configuration like JsonConfig::allFeatures;
         */
        Features();

        /// \c true if comments are allowed. Default: \c true.
        bool allowComments_{ true };

        /// \c true if root must be either an array or an object value. Default: \c
        /// false.
        bool strictRoot_{ false };

        /// \c true if dropped null placeholders are allowed. Default: \c false.
        bool allowDroppedNullPlaceholders_{ false };

        /// \c true if numeric object key are allowed. Default: \c false.
        bool allowNumericKeys_{ false };
    };

} // namespace Json

#pragma pack(pop)

#endif // JSON_FEATURES_H_INCLUDED



// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_READER_H_INCLUDED
#define JSON_READER_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <deque>
#include <iosfwd>
#include <istream>
#include <stack>
#include <string>

// Disable warning C4251: <data member>: <type> needs to have dll-interface to
// be used by...
#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif // if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)

#pragma pack(push)
#pragma pack()

namespace Json {

    /** \brief Unserialize a <a HREF="http://www.json.org">JSON</a> document into a
     * Value.
     *
     * \deprecated Use CharReader and CharReaderBuilder.
     */

    class JSON_API Reader {
    public:
        using Char = char;
        using Location = const Char*;

        /** \brief An error tagged with where in the JSON text it was encountered.
         *
         * The offsets give the [start, limit) range of bytes within the text. Note
         * that this is bytes, not codepoints.
         */
        struct StructuredError {
            ptrdiff_t offset_start;
            ptrdiff_t offset_limit;
            String message;
        };

        /** \brief Constructs a Reader allowing all features for parsing.
          * \deprecated Use CharReader and CharReaderBuilder.
         */
        Reader();

        /** \brief Constructs a Reader allowing the specified feature set for parsing.
          * \deprecated Use CharReader and CharReaderBuilder.
         */
        Reader(const Features& features);

        /** \brief Read a Value from a <a HREF="http://www.json.org">JSON</a>
         * document.
         *
         * \param      document        UTF-8 encoded string containing the document
         *                             to read.
         * \param[out] root            Contains the root value of the document if it
         *                             was successfully parsed.
         * \param      collectComments \c true to collect comment and allow writing
         *                             them back during serialization, \c false to
         *                             discard comments.  This parameter is ignored
         *                             if Features::allowComments_ is \c false.
         * \return \c true if the document was successfully parsed, \c false if an
         * error occurred.
         */
        bool parse(const std::string& document, Value& root,
            bool collectComments = true);

        /** \brief Read a Value from a <a HREF="http://www.json.org">JSON</a>
         * document.
         *
         * \param      beginDoc        Pointer on the beginning of the UTF-8 encoded
         *                             string of the document to read.
         * \param      endDoc          Pointer on the end of the UTF-8 encoded string
         *                             of the document to read.  Must be >= beginDoc.
         * \param[out] root            Contains the root value of the document if it
         *                             was successfully parsed.
         * \param      collectComments \c true to collect comment and allow writing
         *                             them back during serialization, \c false to
         *                             discard comments.  This parameter is ignored
         *                             if Features::allowComments_ is \c false.
         * \return \c true if the document was successfully parsed, \c false if an
         * error occurred.
         */
        bool parse(const char* beginDoc, const char* endDoc, Value& root,
            bool collectComments = true);

        /// \brief Parse from input stream.
        /// \see Json::operator>>(std::istream&, Json::Value&).
        bool parse(IStream& is, Value& root, bool collectComments = true);

        /** \brief Returns a user friendly string that list errors in the parsed
         * document.
         *
         * \return Formatted error message with the list of errors with their
         * location in the parsed document. An empty string is returned if no error
         * occurred during parsing.
         * \deprecated Use getFormattedErrorMessages() instead (typo fix).
         */
        JSONCPP_DEPRECATED("Use getFormattedErrorMessages() instead.")
            String getFormatedErrorMessages() const;

        /** \brief Returns a user friendly string that list errors in the parsed
         * document.
         *
         * \return Formatted error message with the list of errors with their
         * location in the parsed document. An empty string is returned if no error
         * occurred during parsing.
         */
        String getFormattedErrorMessages() const;

        /** \brief Returns a vector of structured errors encountered while parsing.
         *
         * \return A (possibly empty) vector of StructuredError objects. Currently
         * only one error can be returned, but the caller should tolerate multiple
         * errors.  This can occur if the parser recovers from a non-fatal parse
         * error and then encounters additional errors.
         */
        std::vector<StructuredError> getStructuredErrors() const;

        /** \brief Add a semantic error message.
         *
         * \param value   JSON Value location associated with the error
         * \param message The error message.
         * \return \c true if the error was successfully added, \c false if the Value
         * offset exceeds the document size.
         */
        bool pushError(const Value& value, const String& message);

        /** \brief Add a semantic error message with extra context.
         *
         * \param value   JSON Value location associated with the error
         * \param message The error message.
         * \param extra   Additional JSON Value location to contextualize the error
         * \return \c true if the error was successfully added, \c false if either
         * Value offset exceeds the document size.
         */
        bool pushError(const Value& value, const String& message, const Value& extra);

        /** \brief Return whether there are any errors.
         *
         * \return \c true if there are no errors to report \c false if errors have
         * occurred.
         */
        bool good() const;

    private:
        enum TokenType {
            tokenEndOfStream = 0,
            tokenObjectBegin,
            tokenObjectEnd,
            tokenArrayBegin,
            tokenArrayEnd,
            tokenString,
            tokenNumber,
            tokenTrue,
            tokenFalse,
            tokenNull,
            tokenArraySeparator,
            tokenMemberSeparator,
            tokenComment,
            tokenError
        };

        class Token {
        public:
            TokenType type_;
            Location start_;
            Location end_;
        };

        class ErrorInfo {
        public:
            Token token_;
            String message_;
            Location extra_;
        };

        using Errors = std::deque<ErrorInfo>;

        bool readToken(Token& token);
        void skipSpaces();
        bool match(const Char* pattern, int patternLength);
        bool readComment();
        bool readCStyleComment();
        bool readCppStyleComment();
        bool readString();
        void readNumber();
        bool readValue();
        bool readObject(Token& token);
        bool readArray(Token& token);
        bool decodeNumber(Token& token);
        bool decodeNumber(Token& token, Value& decoded);
        bool decodeString(Token& token);
        bool decodeString(Token& token, String& decoded);
        bool decodeDouble(Token& token);
        bool decodeDouble(Token& token, Value& decoded);
        bool decodeUnicodeCodePoint(Token& token, Location& current, Location end,
            unsigned int& unicode);
        bool decodeUnicodeEscapeSequence(Token& token, Location& current,
            Location end, unsigned int& unicode);
        bool addError(const String& message, Token& token, Location extra = nullptr);
        bool recoverFromError(TokenType skipUntilToken);
        bool addErrorAndRecover(const String& message, Token& token,
            TokenType skipUntilToken);
        void skipUntilSpace();
        Value& currentValue();
        Char getNextChar();
        void getLocationLineAndColumn(Location location, int& line,
            int& column) const;
        String getLocationLineAndColumn(Location location) const;
        void addComment(Location begin, Location end, CommentPlacement placement);
        void skipCommentTokens(Token& token);

        static bool containsNewLine(Location begin, Location end);
        static String normalizeEOL(Location begin, Location end);

        using Nodes = std::stack<Value*>;
        Nodes nodes_;
        Errors errors_;
        String document_;
        Location begin_{};
        Location end_{};
        Location current_{};
        Location lastValueEnd_{};
        Value* lastValue_{};
        String commentsBefore_;
        Features features_;
        bool collectComments_{};
    }; // Reader

    /** Interface for reading JSON from a char array.
     */
    class JSON_API CharReader {
    public:
        virtual ~CharReader() = default;
        /** \brief Read a Value from a <a HREF="http://www.json.org">JSON</a>
         * document. The document must be a UTF-8 encoded string containing the
         * document to read.
         *
         * \param      beginDoc Pointer on the beginning of the UTF-8 encoded string
         *                      of the document to read.
         * \param      endDoc   Pointer on the end of the UTF-8 encoded string of the
         *                      document to read. Must be >= beginDoc.
         * \param[out] root     Contains the root value of the document if it was
         *                      successfully parsed.
         * \param[out] errs     Formatted error messages (if not NULL) a user
         *                      friendly string that lists errors in the parsed
         *                      document.
         * \return \c true if the document was successfully parsed, \c false if an
         * error occurred.
         */
        virtual bool parse(char const* beginDoc, char const* endDoc, Value* root,
            String* errs) = 0;

        class JSON_API Factory {
        public:
            virtual ~Factory() = default;
            /** \brief Allocate a CharReader via operator new().
             * \throw std::exception if something goes wrong (e.g. invalid settings)
             */
            virtual CharReader* newCharReader() const = 0;
        }; // Factory
    };   // CharReader

    /** \brief Build a CharReader implementation.
     *
     * Usage:
     *   \code
     *   using namespace Json;
     *   CharReaderBuilder builder;
     *   builder["collectComments"] = false;
     *   Value value;
     *   String errs;
     *   bool ok = parseFromStream(builder, std::cin, &value, &errs);
     *   \endcode
     */
    class JSON_API CharReaderBuilder : public CharReader::Factory {
    public:
        // Note: We use a Json::Value so that we can add data-members to this class
        // without a major version bump.
        /** Configuration of this builder.
         * These are case-sensitive.
         * Available settings (case-sensitive):
         * - `"collectComments": false or true`
         *   - true to collect comment and allow writing them back during
         *     serialization, false to discard comments.  This parameter is ignored
         *     if allowComments is false.
         * - `"allowComments": false or true`
         *   - true if comments are allowed.
         * - `"allowTrailingCommas": false or true`
         *   - true if trailing commas in objects and arrays are allowed.
         * - `"strictRoot": false or true`
         *   - true if root must be either an array or an object value
         * - `"allowDroppedNullPlaceholders": false or true`
         *   - true if dropped null placeholders are allowed. (See
         *     StreamWriterBuilder.)
         * - `"allowNumericKeys": false or true`
         *   - true if numeric object keys are allowed.
         * - `"allowSingleQuotes": false or true`
         *   - true if '' are allowed for strings (both keys and values)
         * - `"stackLimit": integer`
         *   - Exceeding stackLimit (recursive depth of `readValue()`) will cause an
         *     exception.
         *   - This is a security issue (seg-faults caused by deeply nested JSON), so
         *     the default is low.
         * - `"failIfExtra": false or true`
         *   - If true, `parse()` returns false when extra non-whitespace trails the
         *     JSON value in the input string.
         * - `"rejectDupKeys": false or true`
         *   - If true, `parse()` returns false when a key is duplicated within an
         *     object.
         * - `"allowSpecialFloats": false or true`
         *   - If true, special float values (NaNs and infinities) are allowed and
         *     their values are lossfree restorable.
         * - `"skipBom": false or true`
         *   - If true, if the input starts with the Unicode byte order mark (BOM),
         *     it is skipped.
         *
         * You can examine 'settings_` yourself to see the defaults. You can also
         * write and read them just like any JSON Value.
         * \sa setDefaults()
         */
        Json::Value settings_;

        CharReaderBuilder();
        ~CharReaderBuilder() override;

        CharReader* newCharReader() const override;

        /** \return true if 'settings' are legal and consistent;
         *   otherwise, indicate bad settings via 'invalid'.
         */
        bool validate(Json::Value* invalid) const;

        /** A simple way to update a specific setting.
         */
        Value& operator[](const String& key);

        /** Called by ctor, but you can use this to reset settings_.
         * \pre 'settings' != NULL (but Json::null is fine)
         * \remark Defaults:
         * \snippet src/lib_json/json_reader.cpp CharReaderBuilderDefaults
         */
        static void setDefaults(Json::Value* settings);
        /** Same as old Features::strictMode().
         * \pre 'settings' != NULL (but Json::null is fine)
         * \remark Defaults:
         * \snippet src/lib_json/json_reader.cpp CharReaderBuilderStrictMode
         */
        static void strictMode(Json::Value* settings);
    };

    /** Consume entire stream and use its begin/end.
     * Someday we might have a real StreamReader, but for now this
     * is convenient.
     */
    bool JSON_API parseFromStream(CharReader::Factory const&, IStream&, Value* root,
        String* errs);

    /** \brief Read from 'sin' into 'root'.
     *
     * Always keep comments from the input JSON.
     *
     * This can be used to read a file into a particular sub-object.
     * For example:
     *   \code
     *   Json::Value root;
     *   cin >> root["dir"]["file"];
     *   cout << root;
     *   \endcode
     * Result:
     * \verbatim
     * {
     * "dir": {
     *    "file": {
     *    // The input stream JSON would be nested here.
     *    }
     * }
     * }
     * \endverbatim
     * \throw std::exception on parse error.
     * \see Json::operator<<()
     */
    JSON_API IStream& operator>>(IStream&, Value&);

} // namespace Json

#pragma pack(pop)

#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(pop)
#endif // if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)

#endif // JSON_READER_H_INCLUDED
