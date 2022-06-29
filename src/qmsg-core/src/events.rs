use tls_codec::*;
use tls_codec_derive::*;

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct JoinRequest {
    pub team: u32,
    pub key_package: TlsByteVecU32,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct MlsWelcome {
    pub team: u32,
    pub welcome: TlsByteVecU32,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct MlsCommit {
    pub team: u32,
    pub commit: TlsByteVecU32,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct AsciiMessage {
    pub team: u32,
    pub channel: u32,
    pub device_id: u16,
    pub ascii: TlsByteVecU32,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct WatchDevices {
    pub team: u32,
    pub channel: u32,
    pub device_ids: TlsVecU16<u16>,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct UnwatchDevices {
    pub team: u32,
    pub channel: u32,
    pub device_ids: TlsVecU16<u16>,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct WatchChannel {
    pub team: u32,
    pub channel: u32,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct UnwatchChannel {
    pub team: u32,
    pub channel: u32,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct DeviceInfo {
    pub team: u32,
    pub device_id: u16,
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
    MlsWelcome(MlsWelcome),

    #[tls_codec(discriminant = 3)]
    MlsCommit(MlsCommit),

    #[tls_codec(discriminant = 4)]
    AsciiMessage(AsciiMessage),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum SecurityToNetworkEvent {
    #[tls_codec(discriminant = 1)]
    JoinRequest(JoinRequest),

    #[tls_codec(discriminant = 2)]
    MlsWelcome(MlsWelcome),

    #[tls_codec(discriminant = 3)]
    MlsCommit(MlsCommit),

    #[tls_codec(discriminant = 4)]
    AsciiMessage(AsciiMessage),

    #[tls_codec(discriminant = 5)]
    WatchDevices(WatchDevices),

    #[tls_codec(discriminant = 6)]
    UnwatchDevices(UnwatchDevices),

    #[tls_codec(discriminant = 9)]
    DeviceInfo(DeviceInfo),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum UiToSecurityEvent {
    #[tls_codec(discriminant = 4)]
    AsciiMessage(AsciiMessage),

    #[tls_codec(discriminant = 7)]
    WatchChannel(WatchChannel),

    #[tls_codec(discriminant = 8)]
    UnwatchChannel(UnwatchChannel),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum SecurityToUiEvent {
    #[tls_codec(discriminant = 4)]
    AsciiMessage(AsciiMessage),
}
