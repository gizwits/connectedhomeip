/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/* This file contains a definition for a class that implements OTADownloader and downloads CHIP OTA images using the BDX protocol.
 * It should not execute any logic that is application specific.
 */

// TODO: unit tests

#pragma once

#include "OTADownloader.h"

#include <lib/core/CHIPError.h>
#include <protocols/bdx/BdxTransferSession.h>
#include <system/SystemPacketBuffer.h>
#include <transport/raw/MessageHeader.h>

namespace chip {

// Constants for BDX URI parsing
constexpr uint8_t bdxPrefix[]         = { 'b', 'd', 'x', ':', '/', '/' };
constexpr uint8_t bdxSeparator[]      = { '/' };
constexpr uint8_t kValidBdxUriMinLen  = 24;
constexpr uint8_t kNodeIdHexStringLen = 16;
constexpr size_t kUriMaxLen           = 256;

class BDXDownloader : public chip::OTADownloader
{
public:
    // A delegate for passing messages to/from BDXDownloader and some messaging layer. This is mainly to make BDXDownloader more
    // easily unit-testable.
    class MessagingDelegate
    {
    public:
        virtual CHIP_ERROR SendMessage(const chip::bdx::TransferSession::OutputEvent & msgEvent) = 0;
        virtual ~MessagingDelegate() {}
    };

    BDXDownloader() : chip::OTADownloader() {}

    // To be called when there is an incoming message to handle (of any protocol type)
    void OnMessageReceived(const chip::PayloadHeader & payloadHeader, chip::System::PacketBufferHandle msg);

    void SetMessageDelegate(MessagingDelegate * delegate) { mMsgDelegate = delegate; }

    // Initialize a BDX transfer session but will not proceed until OnPreparedForDownload() is called.
    CHIP_ERROR SetBDXParams(const chip::bdx::TransferSession::TransferInitData & bdxInitData);

    // OTADownloader Overrides
    CHIP_ERROR BeginPrepareDownload() override;
    CHIP_ERROR OnPreparedForDownload(CHIP_ERROR status) override;
    void OnDownloadTimeout() override;
    // BDX does not provide a mechanism for the driver of a transfer to gracefully end the exchange, so it will abort the transfer
    // instead.
    void EndDownload(CHIP_ERROR reason = CHIP_NO_ERROR) override;
    CHIP_ERROR FetchNextData() override;
    // TODO: override SkipData

    /**
     * Validate the URI and parse the BDX URI for various fields
     */
    CHIP_ERROR ParseBdxUri(CharSpan uri, NodeId & nodeId, MutableCharSpan fileDesignator);

private:
    void PollTransferSession();
    CHIP_ERROR HandleBdxEvent(const chip::bdx::TransferSession::OutputEvent & outEvent);

    chip::bdx::TransferSession mBdxTransfer;
    MessagingDelegate * mMsgDelegate;
};

} // namespace chip