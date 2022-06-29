/*

use super::{Message, SubEvent};
use std::convert::{TryFrom, TryInto};
use tls_codec::*;
use tls_codec_derive::*;

// SubEvent captures the critical properties of a sub-event:
// * They have a u32 event type
// * They can serialize and deserialize to TLS syntax

// Sub-event types
mod event_type {
    pub const JOIN_REQUEST: u32 = 1;
    pub const WELCOME: u32 = 1;
    pub const MLS_MESSAGE: u32 = 1;
    pub const ASCII_MESSAGE: u32 = 1;
}

#[derive(TlsSerialize, TlsDeserialize)]
pub struct JoinRequest {
    pub key_package: TlsByteVecU32,
}

impl SubEvent for JoinRequest {
    const EVENT_TYPE: u32 = event_type::JOIN_REQUEST;
}

#[derive(TlsSerialize, TlsDeserialize)]
pub struct Welcome {
    pub welcome: TlsByteVecU32,
}

impl SubEvent for Welcome {
    const EVENT_TYPE: u32 = event_type::WELCOME;
}

#[derive(TlsSerialize, TlsDeserialize)]
pub struct MlsMessage {
    pub mls_message: TlsByteVecU32,
}

impl SubEvent for MlsMessage {
    const EVENT_TYPE: u32 = event_type::MLS_MESSAGE;
}

#[derive(TlsSerialize, TlsDeserialize)]
pub struct AsciiMessage {
    pub ascii: TlsByteVecU32,
}

impl SubEvent for AsciiMessage {
    const EVENT_TYPE: u32 = event_type::ASCII_MESSAGE;
}

// Unions of sub-event types for the various interfaces
pub enum Event {
    JoinRequest(JoinRequest),
    Welcome(Welcome),
    MLSMessage(MlsMessage),
    ASCIIMessage(AsciiMessage),
}

*/
