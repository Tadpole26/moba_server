#pragma once

#include <chrono>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client;
class collection;
class database;

namespace options {

class MONGOCXX_API change_stream {
    public:
    change_stream();

    change_stream& full_document(bsoncxx::string::view_or_value full_doc);

    const bsoncxx::stdx::optional<bsoncxx::string::view_or_value>& full_document() const;

    change_stream& batch_size(std::int32_t batch_size);

    const stdx::optional<std::int32_t>& batch_size() const;

    change_stream& resume_after(bsoncxx::document::view_or_value resume_after);

    const stdx::optional<bsoncxx::document::view_or_value>& resume_after() const;

    change_stream& start_after(bsoncxx::document::view_or_value token);

    const stdx::optional<bsoncxx::document::view_or_value>& start_after() const;

    change_stream& collation(bsoncxx::document::view_or_value collation);

    const stdx::optional<bsoncxx::document::view_or_value>& collation() const;

    change_stream& max_await_time(std::chrono::milliseconds max_time);

    const stdx::optional<std::chrono::milliseconds>& max_await_time() const;

    change_stream& start_at_operation_time(bsoncxx::types::b_timestamp timestamp);

   private:
    friend class ::mongocxx::client;
    friend class ::mongocxx::collection;
    friend class ::mongocxx::database;

    bsoncxx::document::value as_bson() const;
    stdx::optional<bsoncxx::string::view_or_value> _full_document;
    stdx::optional<std::int32_t> _batch_size;
    stdx::optional<bsoncxx::document::view_or_value> _collation;
    stdx::optional<bsoncxx::document::view_or_value> _resume_after;
    stdx::optional<bsoncxx::document::view_or_value> _start_after;
    stdx::optional<std::chrono::milliseconds> _max_await_time;

    bsoncxx::types::b_timestamp _start_at_operation_time;
    bool _start_at_operation_time_set = false;
};
}
MONGOCXX_INLINE_NAMESPACE_END
}

#include <mongocxx/config/postlude.hpp>
