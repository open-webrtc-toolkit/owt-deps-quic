/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef OWT_QUIC_TRANSPORT_QUIC_TRANSPORT_FACTORY_H_
#define OWT_QUIC_TRANSPORT_QUIC_TRANSPORT_FACTORY_H_

#include "owt/quic/export.h"
#include "owt/quic/quic_transport_client_interface.h"

namespace owt {
namespace quic {

class QuicTransportServerInterface;
class QuicTransportClientInterface;

class OWT_EXPORT QuicTransportFactory {
 public:
  virtual ~QuicTransportFactory() = default;

  /// Create a QuicTransportFactory.
  static QuicTransportFactory* Create();
  /// Create a QuicTransportFactory for testing. It will not initialize
  /// AtExitManager since testing tools will initialize one.
  static QuicTransportFactory* CreateForTesting();
  // `accepted_origins` is removed at this time because ABI compatible issue.
  // Ownership of returned value is moved to caller. Returns nullptr if creation
  // is failed.
  virtual QuicTransportServerInterface* CreateQuicTransportServer(
      int port,
      const char* cert_path,
      const char* key_path,
      const char* secret_path/*,
      std::vector<std::string> accepted_origins*/) = 0;
  virtual QuicTransportServerInterface* CreateQuicTransportServer(
      int port,
      const char* pfx_path,
      const char* password) = 0;
  virtual void ReleaseQuicTransportServer(
      const QuicTransportServerInterface* server) = 0;
  virtual QuicTransportClientInterface* CreateQuicTransportClient(
      const char* url) = 0;
  virtual QuicTransportClientInterface* CreateQuicTransportClient(
      const char* url,
      const QuicTransportClientInterface::Parameters& parameters) = 0;
};
}  // namespace quic
}  // namespace owt

#endif