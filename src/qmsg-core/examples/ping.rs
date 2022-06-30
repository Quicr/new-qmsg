// This example opens two named pipes, "ping_to_pong" and "pong_to_ping".  Every second, it writes
// a u32 counter to "ping_to_pong", and awaits that counter being echoed back by the "pong"
// example.  To set this up, you will need to `mkfifo ping_to_pong` and `mkfifo pong_to_ping`.

use qmsg_core::*;
use std::fs::OpenOptions;
use std::io::Result;

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
            let ping = Message::from(count.to_be_bytes().to_vec());
            self.to_pong.write(&ping).unwrap();

            // Read a pong
            let pong = self.from_pong.next().unwrap();
            assert_eq!(pong, ping);
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
