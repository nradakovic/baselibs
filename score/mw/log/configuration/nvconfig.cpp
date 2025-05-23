/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#include "nvconfig.h"

#include "score/json/json_parser.h"

#include <string_view>

namespace score
{
namespace mw
{
namespace log
{

namespace
{
constexpr mw::log::LogLevel kDefaultLogLevel = mw::log::LogLevel::kInfo;
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from GetMsgDescriptor as it declared as noexcept.
// coverity[autosar_cpp14_a15_5_3_violation]
config::NvMsgDescriptor GetMsgDescriptor(const std::uint32_t id,
                                         const score::json::Object& object_array_value,
                                         const score::json::Object::const_iterator& object_appid_iterator,
                                         const score::json::Object::const_iterator& object_ctxid_iterator) noexcept
{
    const auto& object_loglevel_iterator = object_array_value.find("loglevel");
    /* KW_SUPPRESS_START: MISRA.VAR.MIN.VIS: False positive, */
    /* log_level visibility is correct, since it is used in the NvMsgDescriptor. */
    auto log_level = kDefaultLogLevel;
    /* KW_SUPPRESS_END: MISRA.VAR.MIN.VIS */
    if (object_loglevel_iterator != object_array_value.end())
    {
        log_level = mw::log::TryGetLogLevelFromU8(object_loglevel_iterator->second.As<std::uint8_t>().value())
                        .value_or(kDefaultLogLevel);
    }

    auto object_appid_result = object_appid_iterator->second.As<std::string_view>();
    /* KW_SUPPRESS_START: MISRA.INIT.BRACES: False positive, */
    /* Braces are correctly placed. */
    const score::mw::log::detail::LoggingIdentifier appid(object_appid_result.value());
    /* KW_SUPPRESS_END: MISRA.INIT.BRACES */
    auto object_ctxid_result = object_ctxid_iterator->second.As<std::string_view>();
    /* KW_SUPPRESS_START: MISRA.INIT.BRACES: False positive, */
    /* Braces are correctly placed. */
    const score::mw::log::detail::LoggingIdentifier ctxid(object_ctxid_result.value());
    /* KW_SUPPRESS_END: MISRA.INIT.BRACES */
    /* KW_SUPPRESS_START: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive, */
    /* It's initialization list, not variables declaring. */
    /* KW_SUPPRESS_START: MISRA.NULL.STMT: False positive: No null statement here. */
    return config::NvMsgDescriptor{id, appid, ctxid, log_level};
    /* KW_SUPPRESS_END: MISRA.NULL.STMT */
    /* KW_SUPPRESS_END: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE */
}
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from HandleParseResult as it declared as noexcept.
// coverity[autosar_cpp14_a15_5_3_violation]
INvConfig::ReadResult HandleParseResult(const score::json::Object& parse_result, NvConfig::typemap_t& typemap) noexcept
{
    for (auto& result_iterator : parse_result)
    {
        auto object_array_result = result_iterator.second.As<score::json::Object>();
        /* KW_SUPPRESS_START: MISRA.LOGIC.NOT_BOOL: False positive. */
        if (!object_array_result.has_value())
        /* KW_SUPPRESS_END: MISRA.LOGIC.NOT_BOOL: False positive. */
        {
            return INvConfig::ReadResult::kERROR_PARSE;
        }

        auto& object_array_value = object_array_result.value().get();
        const auto& object_ctxid_iterator = object_array_value.find("ctxid");
        if (object_ctxid_iterator == object_array_value.end())
        {
            return INvConfig::ReadResult::kERROR_CONTENT;
        }

        const auto& object_id_iterator = object_array_value.find("id");
        if (object_id_iterator == object_array_value.end())
        {
            return INvConfig::ReadResult::kERROR_CONTENT;
        }

        const auto& object_appid_iterator = object_array_value.find("appid");
        if (object_appid_iterator == object_array_value.end())
        {
            return INvConfig::ReadResult::kERROR_CONTENT;
        }

        auto id = object_id_iterator->second.As<std::uint32_t>();
        /* KW_SUPPRESS_START: MISRA.STMT.COND.NOT_BOOLEAN: It is correct boolean statement. */
        /* KW_SUPPRESS_START: MISRA.LOGIC.NOT_BOOL: It is correct boolean statement. */
        if (id.has_value() == false)
        /* KW_SUPPRESS_END: MISRA.LOGIC.NOT_BOOL: It is correct boolean statement. */
        /* KW_SUPPRESS_END: MISRA.STMT.COND.NOT_BOOLEAN: It is correct boolean statement. */
        {
            return INvConfig::ReadResult::kERROR_CONTENT;
        }

        auto object_name = result_iterator.first.GetAsStringView();
        typemap[object_name.data()] =
            GetMsgDescriptor(id.value(), object_array_value, object_appid_iterator, object_ctxid_iterator);
    }
    return INvConfig::ReadResult::kOK;
}

}  // namespace

/* KW_SUPPRESS_START: MISRA.LINKAGE.EXTERN: False positive. These are not declarations, these are definitions. */
NvConfig::NvConfig(const std::string& file_path) : INvConfig(), json_path_(file_path), typemap_{} {}

/* KW_SUPPRESS_START: MISRA.MEMB.NON_STATIC: False positive. Method uses class fields. */
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from parseFromJson as it declared as noexcept.
// coverity[autosar_cpp14_a15_5_3_violation]
INvConfig::ReadResult NvConfig::parseFromJson() noexcept
/* KW_SUPPRESS_END: MISRA.MEMB.NON_STATIC: False positive. Method uses class fields. */
/* KW_SUPPRESS_END: MISRA.LINKAGE.EXTERN: False positive. These are not declarations, these are definitions. */
{
    const score::json::JsonParser json_parser_obj;
    // FromFile() is safe, if the JSON file is stored in stored on qtsafefs (integrity protection).
    // See broken_link_c/item/13777082 and broken_link_c/issue/6576406
    // NOLINTNEXTLINE(score-banned-function) - Argumentation is above.
    auto root = json_parser_obj.FromFile(json_path_);
    /* KW_SUPPRESS_START: MISRA.STMT.COND.NOT_BOOLEAN: It is correct boolean statement. */
    /* KW_SUPPRESS_START: MISRA.LOGIC.NOT_BOOL: It is correct boolean statement. */
    if (!root.has_value())
    /* KW_SUPPRESS_END: MISRA.LOGIC.NOT_BOOL: It is correct boolean statement. */
    /* KW_SUPPRESS_END: MISRA.STMT.COND.NOT_BOOLEAN: It is correct boolean statement. */
    {
        return INvConfig::ReadResult::kERROR_PARSE;
    }

    auto parse_result = root.value().As<score::json::Object>();
    /* KW_SUPPRESS_START: MISRA.LOGIC.NOT_BOOL: It is correct boolean statement. */
    if (!parse_result.has_value())
    /* KW_SUPPRESS_END: MISRA.LOGIC.NOT_BOOL: It is correct boolean statement. */
    {
        return INvConfig::ReadResult::kERROR_PARSE;
    }

    return HandleParseResult(parse_result.value(), typemap_);
}

/* KW_SUPPRESS_START: MISRA.LINKAGE.EXTERN: False positive, is declared in header */
/* KW_SUPPRESS_START: MISRA.MEMB.NON_STATIC: False positive, uses non-static member 'typemap' */
/* KW_SUPPRESS_START: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */
const config::NvMsgDescriptor* NvConfig::getDltMsgDesc(const std::string& typeName) const noexcept
/* KW_SUPPRESS_END: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */
/* KW_SUPPRESS_END: MISRA.MEMB.NON_STATIC: False positive, uses non-static member 'typemap' */
/* KW_SUPPRESS_END: MISRA.LINKAGE.EXTERN: False positive, is declared in header */
{
    auto desc = typemap_.find(typeName);

    if (desc != typemap_.end())
    {
        return &desc->second;
    }
    else
    {
        return nullptr;
    }
}

}  // namespace log
}  // namespace mw
}  // namespace score
