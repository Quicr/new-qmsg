use tls_codec::*;
use tls_codec_derive::*;

#[derive(TlsSerialize, TlsDeserialize)]
pub struct JoinRequest {
    pub key_package: TlsByteVecU32,
}

#[derive(TlsSerialize, TlsDeserialize)]
pub struct Welcome {
    pub welcome: TlsByteVecU32,
}

#[derive(TlsSerialize, TlsDeserialize)]
pub struct MlsMessage {
    pub mls_message: TlsByteVecU32,
}

#[derive(TlsSerialize, TlsDeserialize)]
pub struct AsciiMessage {
    pub ascii: TlsByteVecU32,
}

// Unions of sub-event types for the various interfaces
//
// The "discriminant" value represents the "type" value in the documentation.  These need to be
// kept consistent with the values in the C++ code.
#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum NetworkToSecurityEvent {
    #[tls_codec(discriminant = 1)]
    JoinRequest(JoinRequest),

    #[tls_codec(discriminant = 2)]
    Welcome(Welcome),

    #[tls_codec(discriminant = 3)]
    MLSMessage(MlsMessage),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum SecurityToNetworkEvent {
    #[tls_codec(discriminant = 1)]
    JoinRequest(JoinRequest),

    #[tls_codec(discriminant = 2)]
    Welcome(Welcome),

    #[tls_codec(discriminant = 3)]
    MLSMessage(MlsMessage),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum UiToSecurityEvent {
    #[tls_codec(discriminant = 4)]
    ASCIIMessage(AsciiMessage),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum SecurityToUiEvent {
    #[tls_codec(discriminant = 4)]
    ASCIIMessage(AsciiMessage),
}
