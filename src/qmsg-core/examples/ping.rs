use qmsg_core::*;
use std::fs::OpenOptions;
use std::io::Result;

pub const PING: MessageType = 1;
pub const PONG: MessageType = 2;

struct Ping<T, U>
where
    T: MessageWrite,
    U: MessageRead,
{
    to_pong: T,
    from_pong: U,
}

impl<T, U> Ping<T, U>
where
    T: MessageWrite,
    U: MessageRead,
{
    fn run(mut self) {
        let mut count = 0u32;
        let ping_interval = std::time::Duration::from_secs(1);

        loop {
            std::thread::sleep(ping_interval);

            // Send a ping
            count += 1;

            println!("send ping: {}", count);
            self.to_pong
                .write(&Message {
                    t: PING,
                    v: count.to_be_bytes().to_vec(),
                })
                .unwrap();

            // Read a pong
            let pong = self.from_pong.next().unwrap();
            assert!(pong.t == PONG);
            let count = u32::from_be_slice(&pong.v);
            println!("recv pong: {}x", count);
        }
    }
}

fn main() -> Result<()> {
    let ping_to_pong = OpenOptions::new().write(true).open("ping_to_pong")?;
    let pong_to_ping = OpenOptions::new().read(true).open("pong_to_ping")?;

    let ping = Ping {
        to_pong: ping_to_pong,
        from_pong: pong_to_ping,
    };

    ping.run();

    Ok(())
}
