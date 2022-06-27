use qmsg_core::*;
use std::fs::OpenOptions;
use std::io::{Read, Result, Write};

pub const PING: MessageType = 1;
pub const PONG: MessageType = 2;

struct Ping<'a, T, U>
where
    T: Write,
    U: Read,
{
    to_pong: MessageWriter<'a, T>,
    from_pong: MessageReader<'a, U>,
}

impl<'a, T, U> Ping<'a, T, U>
where
    T: Write,
    U: Read,
{
    fn run(mut self) {
        let mut count = 0u32;
        let ping_interval = std::time::Duration::from_secs(1);

        loop {
            std::thread::sleep(ping_interval);

            // Send a ping
            count += 1;
            let msg_data = count.to_be_bytes();

            println!("send ping: {}", count);
            self.to_pong
                .write(&Message {
                    t: PING,
                    v: &msg_data,
                })
                .unwrap();

            // Read a pong
            self.from_pong.advance().unwrap();
            let pong = self.from_pong.next().unwrap();
            assert!(pong.t == PONG);
            let count = u32::from_be_slice(pong.v);
            println!("recv pong: {}x", count);
        }
    }
}

fn main() -> Result<()> {
    let mut ping_to_pong = OpenOptions::new().write(true).open("ping_to_pong")?;
    let mut pong_to_ping = OpenOptions::new().read(true).open("pong_to_ping")?;

    let ping = Ping {
        to_pong: MessageWriter::new(&mut ping_to_pong),
        from_pong: MessageReader::new(&mut pong_to_ping),
    };

    ping.run();

    Ok(())
}
