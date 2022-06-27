use proc_shell::*;
use std::fs::OpenOptions;
use std::io::{Read, Result, Write};

struct Pong<'a, T, U>
where
    T: Write,
    U: Read,
{
    to_ping: MessageWriter<'a, T>,
    from_ping: MessageReader<'a, U>,
}

impl<'a, T, U> Pong<'a, T, U>
where
    T: Write,
    U: Read,
{
    fn run(mut self) {
        loop {
            self.from_ping.advance().unwrap();
            let count = match self.from_ping.next() {
                None => continue,
                Some(ping) => {
                    assert!(ping.t == PING);
                    u32::from_be_slice(ping.v)
                }
            };
            println!("recv ping: {}", count);

            self.to_ping
                .write(&Message {
                    t: PONG,
                    v: &count.to_be_bytes(),
                })
                .unwrap();
            println!("send pong: {}x", count);
        }
    }
}

fn main() -> Result<()> {
    let mut ping_to_pong = OpenOptions::new().read(true).open("ping_to_pong")?;
    let mut pong_to_ping = OpenOptions::new().write(true).open("pong_to_ping")?;

    let pong = Pong {
        to_ping: MessageWriter::new(&mut pong_to_ping),
        from_ping: MessageReader::new(&mut ping_to_pong),
    };

    pong.run();

    Ok(())
}
