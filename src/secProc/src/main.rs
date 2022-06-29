use qmsg_core::*;
use std::time::Duration;

struct SecurityProcessor<R, W>
where
    R: MessageRead,
    W: MessageWrite,
{
    to_network: W,
    from_network: R,
    to_ui: W,
    from_ui: R,
}

impl<R, W> SecurityProcessor<R, W>
where
    R: MessageRead + MessageReadReady,
    W: MessageWrite,
{
    fn run(mut self) {
        let poll_wait = Duration::from_millis(10);

        loop {
            if self.from_network.ready(poll_wait) {
                let _msg = self.from_network.next().unwrap();
                // TODO actions based on network messages
            }

            if self.from_ui.ready(poll_wait) {
                let _msg = self.from_network.next().unwrap();
                // TODO actions based on UI messages
            }
        }
    }
}

fn main() {
    // Communications with the network processor
    let s2n = nonblocking::open("/tmp/pipe-s2n");
    let n2s = nonblocking::open("/tmp/pipe-n2s");

    // Communications with the UI processor
    let s2u = nonblocking::open("/tmp/pipe-s2u");
    let u2s = nonblocking::open("/tmp/pipe-u2s");

    let sec_proc = SecurityProcessor {
        to_network: s2n,
        from_network: n2s,
        to_ui: s2u,
        from_ui: u2s,
    };

    sec_proc.run();
}
