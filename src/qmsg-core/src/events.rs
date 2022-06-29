use super::Message;
use tls_codec::*;

pub enum Event {
    JoinRequest(JoinRequest),
    Welcome(Welcome),
    MLSMessage(MLSMessage),
    ASCIIMessage(ASCIIMessage),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq)]
pub struct JoinRequest {
    pub marker: u8,
    pub device_id: u32,
    pub bytes: TlsByteVecU32,
}

pub struct Welcome;

pub struct MLSMessage;

pub struct ASCIIMessage;
