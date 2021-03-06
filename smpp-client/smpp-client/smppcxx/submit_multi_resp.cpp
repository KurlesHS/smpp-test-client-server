/*
 * SMPP Encoder/Decoder
 * Copyright (C) 2006 redtaza@users.sourceforge.net
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
/// @file submit_multi_resp.cpp
/// @brief Declares a submit_multi_resp

#include "submit_multi_resp.hpp"

/// @brief Default constructor.
/// note Sets all values to their defaults.
Smpp::SubmitMultiResp::SubmitMultiResp() :
    Response(CommandLength(MinLength), 
    CommandId(CommandId::SubmitMultiResp),
    CommandStatus(CommandStatus::ESME_ROK),
    SequenceNumber::Min)
{
}

/// @brief Constructor that requires values for all the mandatory parameters.
/// @param commandStatus The command status to use.
/// @param sequenceNumber The sequence number to use.
/// @param messageId The message id to use.
Smpp::SubmitMultiResp::SubmitMultiResp(
        const CommandStatus& commandStatus,
        const SequenceNumber& sequenceNumber,
        const MessageId& messageId) :
    Response(CommandLength(MinLength), 
             CommandId(CommandId::SubmitMultiResp), 
             commandStatus,
             sequenceNumber),
    message_id_(messageId)
{
    Header::update_length(message_id_.length());
}

/// @brief Construct from a buffer.
/// @param b The buffer (octet array).
Smpp::SubmitMultiResp::SubmitMultiResp(const Smpp::Uint8* b) :
    Response(CommandLength(MinLength),
              CommandId(CommandId::SubmitMultiResp),
              CommandStatus(CommandStatus::ESME_ROK),
              1)
{
    decode(b);
}

Smpp::SubmitMultiResp::~SubmitMultiResp()
{
}

/// @brief Encode the message into an octet array.
/// @return Pointer to the encoded message.
/// @note The Buffer length is the command_length.
const Smpp::Uint8*
Smpp::SubmitMultiResp::encode()
{
    buff_.reset(Header::command_length());

    Response::encode(buff_); // insert the header first
    buff_ += message_id_;
    buff_ += unsuccess_sme_;

    Header::encode_tlvs(buff_);

    return buff_.get();
}

/// @brief Decode the message from an octet array.
/// @param buff The octet array to decode.
/// @note The octet array maybe the data read from a socket.
void
Smpp::SubmitMultiResp::decode(const Smpp::Uint8* buff)
{
    Response::decode(buff);

    Smpp::Uint32 len = Response::command_length();
    Smpp::Uint32 offset = 16;
    const char* err = "Bad length in submit_multi_resp";
    if(len < offset)
        throw Error(err);
 
    const Smpp::Char* sptr = reinterpret_cast<const Smpp::Char*>(buff);
    message_id_ = &sptr[offset];
    offset += message_id_.length() + 1;
    if(len < offset)
        throw Error(err);

    unsuccess_sme_.decode(buff+offset, len - offset);
    offset += unsuccess_sme_.octet_count();
    if(len < offset)
        throw Error(err);
    
    Header::decode_tlvs(buff + offset, len - offset);
}

