// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include "analyzer/protocol/tcp/TCP.h"

ZEEK_FORWARD_DECLARE_NAMESPACED(NVT_Analyzer, zeek, analyzer::login);

namespace zeek::analyzer::ftp {

class FTP_Analyzer final : public zeek::analyzer::tcp::TCP_ApplicationAnalyzer {
public:
	explicit FTP_Analyzer(zeek::Connection* conn);

	void Done() override;
	void DeliverStream(int len, const u_char* data, bool orig) override;

	static zeek::analyzer::Analyzer* Instantiate(zeek::Connection* conn)
		{
		return new FTP_Analyzer(conn);
		}

protected:
	zeek::analyzer::login::NVT_Analyzer* nvt_orig;
	zeek::analyzer::login::NVT_Analyzer* nvt_resp;
	uint32_t pending_reply;	// code associated with multi-line reply, or 0
	std::string auth_requested;	// AUTH method requested
};

/**
 * Analyzes security data of ADAT exchanges over FTP control session (RFC 2228).
 * Currently only the GSI mechanism of GSSAPI AUTH method is understood.
 * The ADAT exchange for GSI is base64 encoded TLS/SSL handshake tokens.  This
 * analyzer just decodes the tokens and passes them on to the parent, which must
 * be an SSL analyzer instance.
 */
class FTP_ADAT_Analyzer final : public zeek::analyzer::SupportAnalyzer {
public:
	FTP_ADAT_Analyzer(zeek::Connection* conn, bool arg_orig)
	    : SupportAnalyzer("FTP_ADAT", conn, arg_orig),
	      first_token(true) { }

	void DeliverStream(int len, const u_char* data, bool orig) override;

protected:
	// Used by the client-side analyzer to tell if it needs to peek at the
	// initial context token and do sanity checking (i.e. does it look like
	// a TLS/SSL handshake token).
	bool first_token;
};

} // namespace zeek::analyzer::ftp

namespace analyzer::ftp {

using FTP_Analyzer [[deprecated("Remove in v4.1. Use zeek::analyzer::ftp::FTP_Analyzer.")]] = zeek::analyzer::ftp::FTP_Analyzer;
using FTP_ADAT_Analyzer [[deprecated("Remove in v4.1. Use zeek::analyzer::ftp::FTP_ADAT_Analyzer.")]] = zeek::analyzer::ftp::FTP_ADAT_Analyzer;

} // namespace analyzer::ftp
