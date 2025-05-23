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

#include "score/mw/log/detail/file_logging/text_message_builder.h"

#include "score/os/utils/high_resolution_steady_clock.h"
#include "score/mw/log/detail/file_logging/text_format.h"

#include <chrono>
#include <iostream>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

namespace
{

/* KW_SUPPRESS_START: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */
constexpr std::size_t kMaxHeaderSize = 512UL;
/* KW_SUPPRESS_END: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */

/* KW_SUPPRESS_START: MISRA.FUNC.UNUSEDPAR: False positive. All parameters used. */
void LogLevelToString(VerbosePayload& payload, const LogLevel level) noexcept
/* KW_SUPPRESS_END: MISRA.FUNC.UNUSEDPAR: False positive. All parameters used. */
{
    switch (level)
    {
        case LogLevel::kOff:
            detail::TextFormat::Log(payload, std::string_view{"off"});
            break;
        case LogLevel::kFatal:
            detail::TextFormat::Log(payload, std::string_view{"fatal"});
            break;
        case LogLevel::kError:
            detail::TextFormat::Log(payload, std::string_view{"error"});
            break;
        case LogLevel::kWarn:
            detail::TextFormat::Log(payload, std::string_view{"warn"});
            break;
        case LogLevel::kInfo:
            detail::TextFormat::Log(payload, std::string_view{"info"});
            break;
        case LogLevel::kDebug:
            detail::TextFormat::Log(payload, std::string_view{"debug"});
            break;
        case LogLevel::kVerbose:
            detail::TextFormat::Log(payload, std::string_view{"verbose"});
            break;
        default:
            detail::TextFormat::Log(payload, std::string_view{"undefined"});
            break;
    }
}
inline std::uint32_t TimeStamp() noexcept
{
    const std::uint32_t timestamp = std::chrono::duration_cast<std::chrono::duration<uint32_t, std::ratio<1, 10000>>>(
                                        score::os::HighResolutionSteadyClock::now().time_since_epoch())
                                        .count();
    return timestamp;
}

}  // namespace

/* KW_SUPPRESS_START: MISRA.LINKAGE.EXTERN: False positive. These are not declarations, these are definitions. */
TextMessageBuilder::TextMessageBuilder(const std::string_view ecu_id) noexcept
    : IMessageBuilder(),
      header_payload_(kMaxHeaderSize, header_memory_),
      parsing_phase_{ParsingPhase::kHeader},
      ecu_id_{ecu_id}
{
}

/* KW_SUPPRESS_START: MISRA.MEMB.NON_STATIC: False positive. Method uses class fields. */
void TextMessageBuilder::SetNextMessage(LogRecord& log_record) noexcept
/* KW_SUPPRESS_END: MISRA.MEMB.NON_STATIC: False positive. Method uses class fields. */
{
    log_record_ = log_record;

    const auto& log_entry = log_record_.value().get().getLogEntry();
    detail::TextFormat::PutFormattedTime(header_payload_);
    detail::TextFormat::Log(header_payload_, TimeStamp());
    detail::TextFormat::Log(header_payload_, std::string_view{"000"});
    detail::TextFormat::Log(header_payload_, ecu_id_.GetStringView());
    detail::TextFormat::Log(header_payload_, log_entry.app_id.GetStringView());
    detail::TextFormat::Log(header_payload_, log_entry.ctx_id.GetStringView());
    detail::TextFormat::Log(header_payload_, std::string_view{"log"});
    LogLevelToString(header_payload_, log_entry.log_level);
    detail::TextFormat::Log(header_payload_, std::string_view{"verbose"});
    detail::TextFormat::Log(header_payload_, log_entry.num_of_args);
    parsing_phase_ = ParsingPhase::kHeader;
}

/* KW_SUPPRESS_START: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */
/* KW_SUPPRESS_START: MISRA.MEMB.NON_STATIC: False positive. Method uses class fields. */
score::cpp::optional<score::cpp::span<const std::uint8_t>> TextMessageBuilder::GetNextSpan() noexcept
/* KW_SUPPRESS_END: MISRA.MEMB.NON_STATIC: False positive. Method uses class fields. */
/* KW_SUPPRESS_END: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */
{
    /* KW_SUPPRESS_START: MISRA.LOGIC.NOT_BOOL: False positive. It returns boolean. */
    if (!log_record_.has_value())
    /* KW_SUPPRESS_END: MISRA.LOGIC.NOT_BOOL: False positive. It returns boolean. */
    {
        return {};
    }

    score::cpp::optional<score::cpp::span<const std::uint8_t>> return_result = {};

    /* KW_SUPPRESS_START:MISRA.VAR.MIN.VIS: False positive. Variable used across multiple switch cases. */
    detail::VerbosePayload& verbose_payload = log_record_.value().get().getVerbosePayload();
    /* KW_SUPPRESS_END:MISRA.VAR.MIN.VIS */
    switch (parsing_phase_)  // LCOV_EXCL_BR_LINE: exclude the "default" branch.
    {
        case ParsingPhase::kHeader:
            parsing_phase_ = ParsingPhase::kPayload;
            return_result = header_payload_.GetSpan();
            break;
        case ParsingPhase::kPayload:
            parsing_phase_ = ParsingPhase::kReinitialize;
            detail::TextFormat::TerminateLog(verbose_payload);
            return_result = log_record_.value().get().getVerbosePayload().GetSpan();
            break;
        case ParsingPhase::kReinitialize:
            parsing_phase_ = ParsingPhase::kHeader;
            header_payload_.Reset();
            verbose_payload.Reset();
            log_record_.reset();
            break;
            // LCOV_EXCL_START
            // This part of the code should never be reached due to design of the switch state processing logic.
        default:
            std::cerr << "Enum ParsingPhase contains wrong value\n";
            return_result = score::cpp::nullopt;
            break;
            // LCOV_EXCL_STOP
    }
    return return_result;
}
/* KW_SUPPRESS_END: MISRA.LINKAGE.EXTERN: False positive. These are not declarations, these are definitions. */

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
