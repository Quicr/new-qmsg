use qmsg_core::*;
use std::fs::OpenOptions;
use std::io::Result;

const PING: MessageType = 1;
const PONG: MessageType = 2;

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
            assert!(ping.t == PING);
            let count = u32::from_be_slice(&ping.v);
            println!("recv ping: {}", count);

            self.to_ping
                .write(&Message {
                    t: PONG,
                    v: count.to_be_bytes().to_vec(),
                })
                .unwrap();
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
