/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Copyright (c) 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Most classes in this file and its implementations are borrowed from
// Chromium/net/third_party/quiche/src/quic/tools/quic_transport_simple_server_session.cc
// with modifications.
#include "impl/quic_transport_owt_server_session.h"
#include <memory>
#include "impl/quic_transport_stream_impl.h"
#include "net/third_party/quiche/src/quic/core/quic_buffer_allocator.h"
#include "net/third_party/quiche/src/quic/core/quic_types.h"
#include "net/third_party/quiche/src/quic/core/quic_versions.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_flags.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_logging.h"
#include "net/third_party/quiche/src/quic/quic_transport/quic_transport_protocol.h"
#include "net/third_party/quiche/src/quic/quic_transport/quic_transport_stream.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace owt {
namespace quic {

QuicTransportOwtServerSession::QuicTransportOwtServerSession(
    ::quic::QuicConnection* connection,
    bool owns_connection,
    QuicSession::Visitor* owner,
    const ::quic::QuicConfig& config,
    const ::quic::ParsedQuicVersionVector& supported_versions,
    const ::quic::QuicCryptoServerConfig* crypto_config,
    ::quic::QuicCompressedCertsCache* compressed_certs_cache,
    std::vector<url::Origin> accepted_origins,
    base::TaskRunner* runner)
    : QuicTransportServerSession(connection,
                                 owner,
                                 config,
                                 supported_versions,
                                 crypto_config,
                                 compressed_certs_cache,
                                 this),
      owns_connection_(owns_connection),
      accepted_origins_(accepted_origins),
      visitor_(nullptr),
      runner_(runner) {}

QuicTransportOwtServerSession::~QuicTransportOwtServerSession() {
  if (owns_connection_) {
    DeleteConnection();
  }
}

const char* QuicTransportOwtServerSession::ConnectionId() {
  const std::string& connection_id_str = connection_id().ToString();
  char* id = new char[connection_id_str.size() + 1];
  strcpy(id, connection_id_str.c_str());
  return id;
}

QuicTransportStreamInterface*
QuicTransportOwtServerSession::CreateBidirectionalStream() {
  std::unique_ptr<::quic::QuicTransportStream> stream =
      std::make_unique<::quic::QuicTransportStream>(
          GetNextOutgoingUnidirectionalStreamId(), this, this);
  std::unique_ptr<QuicTransportStreamImpl> stream_impl =
      std::make_unique<QuicTransportStreamImpl>(stream.get(), runner_);
  ActivateStream(std::move(stream));
  QuicTransportStreamImpl* stream_ptr(stream_impl.get());
  streams_.push_back(std::move(stream_impl));
  LOG(INFO) << "Create bidirectional stream.";
  return stream_ptr;
}

void QuicTransportOwtServerSession::OnIncomingDataStream(
    ::quic::QuicTransportStream* stream) {
  if (visitor_) {
    std::unique_ptr<QuicTransportStreamImpl> stream_impl =
        std::make_unique<QuicTransportStreamImpl>(stream, runner_);
    QuicTransportStreamImpl* stream_ptr(stream_impl.get());
    streams_.push_back(std::move(stream_impl));
    visitor_->OnIncomingStream(stream_ptr);
  }
}

void QuicTransportOwtServerSession::OnCanCreateNewOutgoingStream(
    bool unidirectional) {}

void QuicTransportOwtServerSession::SetVisitor(
    owt::quic::QuicTransportSessionInterface::Visitor* visitor) {
  visitor_ = visitor;
}

bool QuicTransportOwtServerSession::CheckOrigin(url::Origin origin) {
  DLOG(INFO) << "CheckOrigin";
  if (accepted_origins_.empty()) {
    return true;
  }

  for (const url::Origin& accepted_origin : accepted_origins_) {
    if (origin.IsSameOriginWith(accepted_origin)) {
      return true;
    }
  }
  return false;
}

bool QuicTransportOwtServerSession::ProcessPath(const GURL& url) {
  DLOG(INFO) << "ProcessPath: " << url.path();
  if (url.path() == "/echo" || url.path() == "/") {
    return true;
  }

  LOG(WARNING) << "Unknown path requested: " << url.path();
  return false;
}

void QuicTransportOwtServerSession::OnMessageReceived(
    quiche::QuicheStringPiece message) {
  LOG(INFO) << "Received message.";
}
}  // namespace quic
}  // namespace owt
