use std::fs::File;
use std::io::{Read, Result, Write};
use std::time::Duration;

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

pub struct Message {
    pub t: MessageType,
    pub v: Vec<u8>,
}

impl Message {
    const TYPE_SIZE: usize = (MessageType::BITS as usize) >> 3;
    const LENGTH_SIZE: usize = (MessageLength::BITS as usize) >> 3;
    const HEADER_SIZE: usize = Self::TYPE_SIZE + Self::LENGTH_SIZE;

    fn parse(buf: &[u8]) -> Option<(Self, usize)> {
        if buf.len() < Self::HEADER_SIZE {
            return None;
        }

        let msg_type = MessageType::from_be_slice(&buf[0..Self::TYPE_SIZE]);
        let msg_len = MessageLength::from_be_slice(&buf[Self::TYPE_SIZE..Self::HEADER_SIZE]);
        let msg_len = msg_len as usize;

        if buf.len() < Self::HEADER_SIZE + msg_len {
            return None;
        }

        let payload_start = Self::HEADER_SIZE;
        let payload_end = payload_start + msg_len;

        let msg = Self {
            t: msg_type,
            v: buf[payload_start..payload_end].to_vec(),
        };

        Some((msg, payload_end))
    }

    fn header(&self) -> [u8; Message::HEADER_SIZE] {
        let mut data = [0; Self::HEADER_SIZE];
        data[0..Self::TYPE_SIZE].copy_from_slice(&self.t.to_be_bytes());

        let len = self.v.len() as MessageLength;
        data[Self::TYPE_SIZE..Self::HEADER_SIZE].copy_from_slice(&len.to_be_bytes());

        data
    }
}

pub trait MessageRead {
    fn next(&mut self) -> Result<Message>;
}

impl<T> MessageRead for T
where
    T: Read,
{
    // Reads the next whole message, blocking until a whole message has been received or a read
    // returns an error.
    fn next(&mut self) -> Result<Message> {
        let mut msg_buf = Vec::new();
        let mut read_buf = [0u8; 1024];
        loop {
            // Read into the buffer
            let n = self.read(&mut read_buf)?;
            msg_buf.extend_from_slice(&read_buf[..n]);

            // Attempt to parse a message
            if let Some((msg, msg_len)) = Message::parse(&read_buf) {
                msg_buf.drain(..msg_len);
                return Ok(msg);
            };
        }
    }
}

pub trait MessageReadReady {
    fn ready(&mut self, wait: Duration) -> bool;
}

impl MessageReadReady for File {
    fn ready(&mut self, wait: Duration) -> bool {
        nonblocking::ready(self, wait)
    }
}

pub trait MessageWrite {
    fn write(&mut self, msg: &Message) -> Result<()>;
}

impl<T> MessageWrite for T
where
    T: Write,
{
    fn write(&mut self, msg: &Message) -> Result<()> {
        self.write_all(&msg.header())?;
        self.write_all(&msg.v)
    }
}
