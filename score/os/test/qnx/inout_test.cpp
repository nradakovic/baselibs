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
#include "score/os/qnx/inout_impl.h"
#include <sys/mman.h>
#include <sys/neutrino.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class InoutTestFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        unit_ = std::make_unique<score::os::qnx::InOutQnx>();

        // Set I/O privileges for the calling thread
        ThreadCtl(_NTO_TCTL_IO, 0);

        address_ = ::mmap_device_io(1, 0x60);
    }

    std::unique_ptr<score::os::qnx::InOutQnx> unit_{};
    uintptr_t address_{};
};

TEST_F(InoutTestFixture, in8TestToReturnNoErrorIfpassValidAddress)
{
    const auto val = unit_->in8(address_);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0x0);
}

TEST_F(InoutTestFixture, in16ReturnsNoErrorIfPassValidAddress)
{
    const auto val = unit_->in16(address_);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0x0);
}

TEST_F(InoutTestFixture, in32ReturnNoErrorIfPassValidAddress)
{
    const auto val = unit_->in32(address_);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0x0);
}

TEST_F(InoutTestFixture, out8ReturnNoErrorIfPassValidAddress)
{
    const auto val = unit_->out8(address_, 0xAA);
    EXPECT_TRUE(val.has_value());
}

TEST_F(InoutTestFixture, out16ReturnNoErrorIfPassValidAddress)
{
    const auto val = unit_->out16(address_, 0xAA);
    EXPECT_TRUE(val.has_value());
}

TEST_F(InoutTestFixture, out32ReturnNoErrorIfPassValidAddress)
{
    const auto val = unit_->out32(address_, 0xAA);
    EXPECT_TRUE(val.has_value());
}
