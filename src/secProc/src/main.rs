use qmsg_core::*;
use std::fs::File;
use std::time::Duration;

struct SecurityProcessor<'a> {
    to_network: MessageWriter<'a, File>,
    from_network: MessageReader<'a, File>,
    to_ui: MessageWriter<'a, File>,
    from_ui: MessageReader<'a, File>,
}

impl<'a> SecurityProcessor<'a> {
    fn run(mut self) {
        let poll_wait = Duration::from_millis(10);

        loop {
            if let Some(_msg) = self.from_network.ready(poll_wait) {
                // TODO actions based on network messages
            }

            if let Some(_msg) = self.from_ui.ready(poll_wait) {
                // TODO actions based on UI messages
            }
        }
    }
}

fn main() {
    // Communications with the network processor
    let mut s2n = nonblocking::open("/tmp/pipe-s2n");
    let mut n2s = nonblocking::open("/tmp/pipe-n2s");

    // Communications with the UI processor
    let mut s2u = nonblocking::open("/tmp/pipe-s2u");
    let mut u2s = nonblocking::open("/tmp/pipe-u2s");

    let sec_proc = SecurityProcessor {
        to_network: MessageWriter::new(&mut s2n),
        from_network: MessageReader::new(&mut n2s),
        to_ui: MessageWriter::new(&mut s2u),
        from_ui: MessageReader::new(&mut u2s),
    };

    sec_proc.run();
}
