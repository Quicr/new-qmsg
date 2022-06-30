// This is the pong half of a ping/pong example.  See ping.rs for further details.

use qmsg_core::*;
use std::fs::OpenOptions;
use std::io::Result;

struct Pong<T, U>
where
    T: MessageWrite,
    U: MessageRead,
{
    to_ping: T,
    from_ping: U,
}

impl<T, U> Pong<T, U>
where
    T: MessageWrite,
    U: MessageRead,
{
    fn run(mut self) {
        loop {
            let ping = self.from_ping.next().unwrap();
            let count = u32::from_be_slice(ping.data());
            println!("recv ping: {}", count);

            self.to_ping.write(&ping).unwrap();
            println!("send pong: {}x", count);
        }
    }
}

fn main() -> Result<()> {
    let ping_to_pong = OpenOptions::new().read(true).open("ping_to_pong")?;
    let pong_to_ping = OpenOptions::new().write(true).open("pong_to_ping")?;

    let pong = Pong {
        to_ping: pong_to_ping,
        from_ping: ping_to_pong,
    };

    pong.run();

    Ok(())
}
