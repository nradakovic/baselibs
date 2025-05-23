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
#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_BAD_STRING_STREAM_COLLECTION_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_BAD_STRING_STREAM_COLLECTION_H

#include "score/filesystem/filestream/i_string_stream_collection.h"

namespace score
{
namespace filesystem
{

class BadStringStreamCollection : public IStringStreamCollection
{
  public:
    score::Result<std::reference_wrapper<std::stringstream>> OpenStringStream(const Path&) noexcept override;
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_BAD_STRING_STREAM_COLLECTION_H
