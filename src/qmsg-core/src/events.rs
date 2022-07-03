use std::io::{Read, Write};
use tls_codec::*;

// TlsSerialized automatically wraps TLS-serializable things in a TLS byte vector, including their
// serialization / deserialization in that of an overall object.
type TlsSerializedVector = TlsByteVecU32;

#[derive(Debug)]
pub struct TlsSerialized<T>
where
    T: Serialize + Deserialize + Size,
{
    inner: T,
}

impl<T> TlsSerialized<T>
where
    T: Serialize + Deserialize + Size,
{
    pub fn new(inner: T) -> Self {
        Self { inner: inner }
    }

    pub fn unwrap(self) -> T {
        self.inner
    }
}

impl<T> Size for TlsSerialized<T>
where
    T: Serialize + Deserialize + Size,
{
    fn tls_serialized_len(&self) -> usize {
        // Hard-coding as 4 until a bug in tls_codec is fixed where
        // TlsByteVecU{16/32}::len_len() returns 1 (instead of 2 and 4, respectively).
        // TlsSerializedVector::len_len() + self.inner.tls_serialized_len()
        4 + self.inner.tls_serialized_len()
    }
}

impl<T> Serialize for TlsSerialized<T>
where
    T: Serialize + Deserialize + Size,
{
    fn tls_serialize<W: Write>(&self, writer: &mut W) -> Result<usize, Error> {
        let inner = self.inner.tls_serialize_detached()?;
        let vec = TlsSerializedVector::new(inner);
        vec.tls_serialize(writer)
    }
}

impl<T> Deserialize for TlsSerialized<T>
where
    T: Serialize + Deserialize + Size,
{
    fn tls_deserialize<R: Read>(bytes: &mut R) -> Result<Self, Error> {
        let vec = TlsSerializedVector::tls_deserialize(bytes)?;
        let inner = T::tls_deserialize(&mut vec.as_slice())?;
        Ok(Self::new(inner))
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_tls_serialized() {
        #[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
        struct Tuple(u16, u32, u8);

        let original = TlsSerialized::new(Tuple(1, 2, 3));

        let serialized = original.tls_serialize_detached().unwrap();
        assert_eq!(serialized, [0, 0, 0, 7, 0, 1, 0, 0, 0, 2, 3]);

        let deserialized: TlsSerialized<Tuple> =
            TlsSerialized::tls_deserialize(&mut serialized.as_slice()).unwrap();
        assert_eq!(deserialized, original);
    }
}

use openmls::prelude::{KeyPackage, MlsMessageIn, MlsMessageOut, Welcome};

// Specific event types
#[derive(TlsSerialize, TlsDeserialize, TlsSize, Debug)]
pub struct MlsKeyPackage {
    pub team: u32,
    pub key_package: TlsSerialized<KeyPackage>,
    pub hash_bytes: TlsByteVecU32,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, Debug)]
pub struct MlsAddKeyPackage {
    pub team: u32,
    pub key_package: TlsSerialized<KeyPackage>,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, Debug)]
pub struct MlsWelcome {
    pub team: u32,
    pub welcome: TlsSerialized<Welcome>,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, Debug)]
pub struct MlsCommitIn {
    pub team: u32,
    pub commit: TlsSerialized<MlsMessageIn>,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, Debug)]
pub struct MlsCommitOut {
    pub team: u32,
    pub commit: TlsSerialized<MlsMessageOut>,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, Debug)]
pub struct EncryptedAsciiMessageIn {
    pub team: u32,
    pub channel: u32,
    pub ciphertext: TlsSerialized<MlsMessageIn>,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, Debug)]
pub struct EncryptedAsciiMessageOut {
    pub team: u32,
    pub channel: u32,
    pub ciphertext: TlsSerialized<MlsMessageOut>,
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
    pub device_ids: TlsVecU16<u32>,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct UnwatchDevices {
    pub team: u32,
    pub channel: u32,
    pub device_ids: TlsVecU16<u32>,
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
pub struct MlsSignatureHash {
    pub team: u32,
    pub hash_bytes: TlsByteVecU32,
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
#[derive(TlsSerialize, TlsDeserialize, TlsSize, Debug)]
#[repr(u32)]
pub enum NetworkToSecurityEvent {
    #[tls_codec(discriminant = 6)]
    MlsKeyPackage(MlsKeyPackage),

    #[tls_codec(discriminant = 7)]
    MlsAddKeyPackage(MlsAddKeyPackage),

    #[tls_codec(discriminant = 8)]
    MlsWelcome(MlsWelcome),

    #[tls_codec(discriminant = 9)]
    MlsCommit(MlsCommitIn),

    #[tls_codec(discriminant = 2)]
    EncryptedAsciiMessage(EncryptedAsciiMessageIn),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, Debug)]
#[repr(u32)]
pub enum SecurityToNetworkEvent {
    #[tls_codec(discriminant = 7)]
    JoinRequest(MlsAddKeyPackage),

    #[tls_codec(discriminant = 8)]
    MlsWelcome(MlsWelcome),

    #[tls_codec(discriminant = 9)]
    MlsCommitOut(MlsCommitOut),

    #[tls_codec(discriminant = 1)]
    EncryptedAsciiMessage(EncryptedAsciiMessageOut),

    #[tls_codec(discriminant = 3)]
    WatchDevices(WatchDevices),

    #[tls_codec(discriminant = 4)]
    UnwatchDevices(UnwatchDevices),

    #[tls_codec(discriminant = 5)]
    MlsSignatureHash(MlsSignatureHash),

    #[tls_codec(discriminant = 10)]
    DeviceInfo(DeviceInfo),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum UiToSecurityEvent {
    #[tls_codec(discriminant = 1)]
    AsciiMessage(AsciiMessage),

    #[tls_codec(discriminant = 5)]
    MlsSignatureHash(MlsSignatureHash),

    #[tls_codec(discriminant = 11)]
    WatchChannel(WatchChannel),

    #[tls_codec(discriminant = 12)]
    UnwatchChannel(UnwatchChannel),
}

#[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum SecurityToUiEvent {
    #[tls_codec(discriminant = 2)]
    AsciiMessage(AsciiMessage),
}
