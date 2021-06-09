/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Copyright (c) 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Most classes in this file and its implementations are borrowed from
// Chromium/net/third_party/quiche/src/quic/tools/quic_transport_simple_server_dispatcher.h
// with modifications.

#ifndef OWT_QUIC_QUIC_TRANSPORT_QUIC_TRANSPORT_OWT_SERVER_DISPATCHER_H_
#define OWT_QUIC_QUIC_TRANSPORT_QUIC_TRANSPORT_OWT_SERVER_DISPATCHER_H_

#include "base/single_thread_task_runner.h"
#include "net/third_party/quiche/src/quic/core/quic_dispatcher.h"
#include "url/origin.h"

namespace owt {
namespace quic {
class QuicTransportOwtServerSession;
class QuicTransportOwtServerDispatcher : public ::quic::QuicDispatcher {
 public:
  class Visitor {
   public:
    virtual ~Visitor() = default;
    virtual void OnSession(QuicTransportOwtServerSession*) = 0;
  };
  QuicTransportOwtServerDispatcher(
      const ::quic::QuicConfig* config,
      const ::quic::QuicCryptoServerConfig* crypto_config,
      ::quic::QuicVersionManager* version_manager,
      std::unique_ptr<::quic::QuicConnectionHelperInterface> helper,
      std::unique_ptr<::quic::QuicCryptoServerStreamBase::Helper>
          session_helper,
      std::unique_ptr<::quic::QuicAlarmFactory> alarm_factory,
      uint8_t expected_server_connection_id_length,
      std::vector<url::Origin> accepted_origins,
      base::SingleThreadTaskRunner* task_runner,
      base::SingleThreadTaskRunner* event_runner);
  void SetVisitor(Visitor* visitor);

  ~QuicTransportOwtServerDispatcher() override;

 protected:
  std::unique_ptr<::quic::QuicSession> CreateQuicSession(
      ::quic::QuicConnectionId server_connection_id,
      const ::quic::QuicSocketAddress& self_address,
      const ::quic::QuicSocketAddress& peer_address,
      quiche::QuicheStringPiece alpn,
      const ::quic::ParsedQuicVersion& version) override;

  std::vector<url::Origin> accepted_origins_;
  Visitor* visitor_;
  base::SingleThreadTaskRunner* runner_;
  base::SingleThreadTaskRunner* event_runner_;
};
}  // namespace quic
}  // namespace owt

#endif