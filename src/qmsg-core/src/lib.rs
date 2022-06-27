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

pub struct Message<'a> {
    pub t: MessageType,
    pub v: &'a [u8],
}

impl<'a> Message<'a> {
    const TYPE_SIZE: usize = (MessageType::BITS as usize) >> 3;
    const LENGTH_SIZE: usize = (MessageLength::BITS as usize) >> 3;
    const HEADER_SIZE: usize = Self::TYPE_SIZE + Self::LENGTH_SIZE;

    fn read(buf: &'a [u8]) -> Option<Self> {
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

        Some(Self {
            t: msg_type,
            v: &buf[payload_start..payload_end],
        })
    }

    fn len(&self) -> usize {
        Self::HEADER_SIZE + self.v.len()
    }

    fn header(&self) -> [u8; Message::HEADER_SIZE] {
        let mut data = [0; Self::HEADER_SIZE];
        data[0..Self::TYPE_SIZE].copy_from_slice(&self.t.to_be_bytes());

        let len = self.v.len() as MessageLength;
        data[Self::TYPE_SIZE..Self::HEADER_SIZE].copy_from_slice(&len.to_be_bytes());

        data
    }
}

const READ_BUFFER_SIZE: usize = 8192;

pub struct MessageReader<'a, T>
where
    T: Read,
{
    reader: &'a mut T,
    buf: [u8; READ_BUFFER_SIZE],
    buf_len: usize,
}

impl<'a, T> MessageReader<'a, T>
where
    T: Read,
{
    pub fn new(reader: &'a mut T) -> Self {
        Self {
            reader: reader,
            buf: [0; READ_BUFFER_SIZE],
            buf_len: 0,
        }
    }

    pub fn next<'b>(&'b self) -> Option<Message<'b>> {
        Message::read(&self.buf[..self.buf_len])
    }

    pub fn advance(&mut self) -> Result<()> {
        // If there is a message in the buffer, shift the contents forward
        if let Some(msg) = self.next() {
            let msg_len = msg.len();
            let remaining_len = self.buf_len - msg_len;

            let mut temp_buf = [0u8; READ_BUFFER_SIZE];
            temp_buf[..remaining_len].copy_from_slice(&self.buf[msg_len..self.buf_len]);
            self.buf.fill(0);
            self.buf[..remaining_len].copy_from_slice(&temp_buf[..remaining_len]);
            self.buf_len = remaining_len;
        }

        // Make a single read call to fill more data
        let n = self.reader.read(&mut self.buf[self.buf_len..])?;
        self.buf_len += n;

        Ok(())
    }
}

impl<'a> MessageReader<'a, File> {
    pub fn ready<'b>(&'b mut self, wait: Duration) -> Option<Message<'b>> {
        if nonblocking::ready(self.reader, wait) {
            return None;
        }

        if self.advance().is_err() {
            return None;
        }

        self.next()
    }
}

pub struct MessageWriter<'a, T>
where
    T: Write,
{
    writer: &'a mut T,
}

impl<'a, T> MessageWriter<'a, T>
where
    T: Write,
{
    pub fn new(writer: &'a mut T) -> Self {
        Self { writer: writer }
    }

    pub fn write(&mut self, msg: &Message) -> Result<()> {
        self.writer.write_all(&msg.header())?;
        self.writer.write_all(msg.v)
    }
}
