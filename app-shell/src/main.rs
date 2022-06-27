use os_pipe::{PipeReader, PipeWriter};
use std::io::prelude::*;
use std::io::Result;

trait FromBeSlice {
    fn from_be_slice(buf: &[u8]) -> Self;
}

impl FromBeSlice for u32 {
    fn from_be_slice(buf: &[u8]) -> Self {
        let mut data = [0u8; 4];
        data.copy_from_slice(&buf[..4]);
        u32::from_be_bytes(data)
    }
}

struct SerialPort {
    r: PipeReader,
    w: PipeWriter,
}

impl SerialPort {
    fn pipe() -> Result<(SerialPort, SerialPort)> {
        let (read_a, write_b) = os_pipe::pipe()?;
        let (read_b, write_a) = os_pipe::pipe()?;
        Ok((
            SerialPort {
                r: read_a,
                w: write_a,
            },
            SerialPort {
                r: read_b,
                w: write_b,
            },
        ))
    }
}

type MessageType = u32;
type MessageLength = u32;

struct Message<'a> {
    t: MessageType,
    v: &'a [u8],
}

impl<'a> Message<'a> {
    const TYPE_SIZE: usize = (MessageType::BITS as usize) >> 3;
    const LENGTH_SIZE: usize = (MessageLength::BITS as usize) >> 3;
    const HEADER_SIZE: usize = Self::TYPE_SIZE + Self::LENGTH_SIZE;

    const PING: MessageType = 1;
    const PONG: MessageType = 2;
    const FLOOD: MessageType = 3;

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

struct MessagePort {
    port: SerialPort,
    buf: [u8; Self::BUFFER_SIZE],
    buf_len: usize,
}

impl MessagePort {
    const BUFFER_SIZE: usize = 8192;

    fn new(port: SerialPort) -> Self {
        Self {
            port: port,
            buf: [0; Self::BUFFER_SIZE],
            buf_len: 0,
        }
    }

    fn pipe() -> Result<(Self, Self)> {
        let (a, b) = SerialPort::pipe()?;
        Ok((Self::new(a), Self::new(b)))
    }

    fn next<'a>(&'a self) -> Option<Message<'a>> {
        Message::read(&self.buf[..self.buf_len])
    }

    fn advance(&mut self) -> Result<()> {
        // If there is a message in the buffer, shift the contents forward
        if let Some(msg) = self.next() {
            let msg_len = msg.len();
            let remaining_len = self.buf_len - msg_len;

            let mut temp_buf = [0u8; Self::BUFFER_SIZE];
            temp_buf[..remaining_len].copy_from_slice(&self.buf[msg_len..self.buf_len]);
            self.buf.fill(0);
            self.buf[..remaining_len].copy_from_slice(&temp_buf[..remaining_len]);
            self.buf_len = remaining_len;
        }

        // Make a single read call to fill more data
        let n = self.port.r.read(&mut self.buf[self.buf_len..])?;
        self.buf_len += n;

        Ok(())
    }

    fn write(&mut self, msg: &Message) -> Result<()> {
        self.port.w.write_all(&msg.header())?;
        self.port.w.write_all(msg.v)
    }
}

// Example 1: Ping/pong
/*
struct Ping {
    pong: MessagePort,
}

impl Ping {
    fn run(&mut self) {
        let mut count = 0u32;
        let ping_interval = std::time::Duration::from_secs(1);

        loop {
            std::thread::sleep(ping_interval);

            // Send a ping
            count += 1;
            let msg_data = count.to_be_bytes();

            println!("send ping: {}", count);
            self.pong
                .write(&Message {
                    t: Message::PING,
                    v: &msg_data,
                })
                .unwrap();

            // Read a pong
            self.pong.advance().unwrap();
            let pong = self.pong.next().unwrap();
            assert!(pong.t == Message::PONG);
            let count = u32::from_be_slice(pong.v);
            println!("recv pong: {}x", count);
        }
    }
}

struct Pong {
    ping: MessagePort,
}

impl Pong {
    fn run(&mut self) {
        loop {
            self.ping.advance().unwrap();
            let count = match self.ping.next() {
                None => continue,
                Some(ping) => {
                    assert!(ping.t == Message::PING);
                    u32::from_be_slice(ping.v)
                }
            };
            println!("recv ping: {}", count);

            self.ping
                .write(&Message {
                    t: Message::PONG,
                    v: &count.to_be_bytes(),
                })
                .unwrap();
            println!("send pong: {}x", count);
        }
    }
}

fn main() -> Result<()> {
    let (to_ping, to_pong) = MessagePort::pipe()?;

    let mut ping = Ping { pong: to_ping };
    let mut pong = Pong { ping: to_pong };

    let ping_thread = std::thread::spawn(move || ping.run());
    let pong_thread = std::thread::spawn(move || pong.run());

    ping_thread.join().unwrap();
    pong_thread.join().unwrap();

    Ok(())
}
*/

// Example 2: Flooding messages across a tree of processors
struct Flood {
    name: String,
    parent: MessagePort,
    children: Vec<MessagePort>,
}

impl Flood {
    fn new(name: &str, parent: MessagePort) -> Self {
        Self {
            name: String::from(name),
            parent: parent,
            children: Vec::new(),
        }
    }

    fn new_child(&mut self, name: &str) -> Result<Self> {
        let (to_parent, to_child) = MessagePort::pipe()?;
        self.children.push(to_parent);
        Ok(Self::new(name, to_child))
    }

    fn run(mut self) {
        loop {
            self.parent.advance().unwrap();
            let count = match self.parent.next() {
                None => continue,
                Some(msg) => {
                    assert!(msg.t == Message::FLOOD);
                    u32::from_be_slice(msg.v)
                }
            };

            println!("recv @ [{}]: {}", self.name, count);

            let count_data = (count + 1).to_be_bytes();
            let msg = Message {
                t: Message::FLOOD,
                v: &count_data,
            };

            for child in &mut self.children {
                child.write(&msg).unwrap();
            }
        }
    }
}

// Network -> Secure -> UI
//              |
//              V
//            Media
fn main() -> Result<()> {
    let (mut input, to_network) = MessagePort::pipe()?;
    let mut network_proc = Flood::new("Network", to_network);
    let mut secure_proc = network_proc.new_child("Secure")?;
    let media_proc = secure_proc.new_child("Media")?;
    let ui_proc = secure_proc.new_child("UI")?;

    let threads: Vec<_> = [network_proc, secure_proc, media_proc, ui_proc]
        .into_iter()
        .map(|p| std::thread::spawn(move || p.run()))
        .collect();

    let interval = std::time::Duration::from_secs(1);
    for i in 0u32..10 {
        std::thread::sleep(interval);
        println!("flooding {}", i);

        input
            .write(&Message {
                t: Message::FLOOD,
                v: &i.to_be_bytes(),
            })
            .unwrap();
    }

    for t in threads {
        t.join().unwrap();
    }

    Ok(())
}
