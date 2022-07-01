use std::fs::File;
use std::io::{Read, Write};
use std::time::Duration;
use tls_codec::*;

pub mod events;
pub mod nonblocking;

pub trait FromBeSlice {
    fn from_be_slice(buf: &[u8]) -> Self;
}

impl FromBeSlice for u32 {
    fn from_be_slice(buf: &[u8]) -> Self {
        let mut data = [0u8; 4];
        data.copy_from_slice(&buf[..4]);
        u32::from_be_bytes(data)
    }
}

pub type MessageType = u32;
pub type MessageLength = u32;

// Message represents a TLV that is passed between processors
#[derive(Debug, PartialEq, Eq)]
pub struct Message(Vec<u8>);

impl Message {
    const HEADER_SIZE: usize = (MessageLength::BITS as usize) >> 3;

    fn parse(buf: &[u8]) -> Option<(Self, usize)> {
        if buf.len() < Self::HEADER_SIZE {
            return None;
        }

        let msg_len = MessageLength::from_be_slice(&buf[..Self::HEADER_SIZE]);
        let msg_len = msg_len as usize;
        let payload_end = Self::HEADER_SIZE + msg_len;

        if buf.len() < payload_end {
            return None;
        }

        let msg = Self(buf[Self::HEADER_SIZE..payload_end].to_vec());
        Some((msg, payload_end))
    }

    fn header(&self) -> [u8; Message::HEADER_SIZE] {
        (self.0.len() as u32).to_be_bytes()
    }

    pub fn data(&self) -> &Vec<u8> {
        &self.0
    }

    pub fn from_tls<T: Serialize>(obj: &T) -> Result<Message, tls_codec::Error> {
        Ok(Message(obj.tls_serialize_detached()?))
    }

    pub fn to_tls<T: Deserialize>(&self) -> Result<T, tls_codec::Error> {
        T::tls_deserialize(&mut self.0.as_slice())
    }
}

impl From<Vec<u8>> for Message {
    fn from(vec: Vec<u8>) -> Self {
        Message(vec)
    }
}

// MessageRead is a trait that augments Read objects with the ability to read Messages
pub trait MessageRead {
    fn next(&mut self) -> std::io::Result<Message>;
}

impl<T> MessageRead for T
where
    T: Read,
{
    // Reads the next whole message, blocking until a whole message has been received or a read
    // returns an error.
    fn next(&mut self) -> std::io::Result<Message> {
        let mut msg_buf = Vec::new();
        let mut read_buf = [0u8; 1024];
        loop {
            // Read into the buffer
            let n = self.read(&mut read_buf)?;
            msg_buf.extend_from_slice(&read_buf[..n]);

            // Attempt to parse a message
            if let Some((msg, msg_len)) = Message::parse(&msg_buf) {
                msg_buf.drain(..msg_len);
                return Ok(msg);
            };
        }
    }
}

// For non-blocking readers, MessageReadReady lets the reader probe whether the reader is ready to
// produce data.  Note that it does not guarantee that it is ready to produce a whole message.
pub trait MessageReadReady {
    fn ready(&mut self, wait: Duration) -> bool;
}

impl MessageReadReady for File {
    fn ready(&mut self, wait: Duration) -> bool {
        nonblocking::ready(self, wait)
    }
}

// MessageRead is a trait that augments Read objects with the ability to read Messages
pub trait MessageWrite {
    fn write(&mut self, msg: &Message) -> std::io::Result<()>;
}

impl<T> MessageWrite for T
where
    T: Write,
{
    fn write(&mut self, msg: &Message) -> std::io::Result<()> {
        self.write_all(&msg.header())?;
        self.write_all(msg.data())
    }
}

#[cfg(test)]
mod test {
    use super::*;
    use tls_codec_derive::*;

    #[test]
    fn test_message() {
        // Deserialize / serialize
        let buffer = [0u8, 0, 0, 4, 1, 2, 3, 4, 0xA0, 0xA0];
        let (msg, msg_len) = Message::parse(&buffer).unwrap();
        assert_eq!(msg, Message(vec![1, 2, 3, 4]));
        assert_eq!(msg_len, 8);
        assert_eq!(msg.header(), [0, 0, 0, 4]);
        assert_eq!(msg.data(), &[1, 2, 3, 4]);
    }

    #[test]
    fn test_message_tls() {
        #[derive(TlsSerialize, TlsDeserialize, TlsSize, PartialEq, Eq, Debug)]
        #[repr(u32)]
        enum Event {
            #[tls_codec(discriminant = 1)]
            Simple(u32),

            #[tls_codec(discriminant = 2)]
            Compound(u16, u32, u8),
        }

        let event = Event::Compound(1, 2, 3);
        let msg = Message::from_tls(&event).unwrap();
        assert_eq!(msg, Message(vec![0, 0, 0, 2, 0, 1, 0, 0, 0, 2, 3]));

        let event_parsed: Event = msg.to_tls().unwrap();
        assert_eq!(event_parsed, event);
    }

    #[test]
    fn test_message_read() {
        let buffer = [0u8, 0, 0, 4, 1, 2, 3, 4, 0xA0, 0xA0];
        let mut slice = &buffer[..];

        let msg = MessageRead::next(&mut slice).unwrap();
        assert_eq!(msg, Message(vec![1, 2, 3, 4]));
    }

    #[test]
    fn test_message_write() {
        let mut buffer = [0xA0u8; 10];
        let mut slice = &mut buffer[..];
        let msg = Message(vec![1, 2, 3, 4]);

        MessageWrite::write(&mut slice, &msg).unwrap();
        assert_eq!(buffer, [0, 0, 0, 4, 1, 2, 3, 4, 0xA0, 0xA0]);
    }
}
