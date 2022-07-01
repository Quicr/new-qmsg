// This is a mock-up of the security processor half of the Security -- UI interface.  It does the
// following:
//
// * Attach to two named pipes "sec_to_ui" and "ui_to_sec"
// * Read lines from stdin and write them as AsciiMessage on "sec_to_ui"
// * After each message, listen for any events from "ui_to_sec" and log them

use qmsg_core::events::*;
use qmsg_core::*;
use std::io::{stdin, Result};
use std::time::Duration;

struct MockSecurityProcessor<T, U>
where
    T: MessageWrite,
    U: MessageRead,
{
    to_ui: T,
    from_ui: U,
}

impl<T, U> MockSecurityProcessor<T, U>
where
    T: MessageWrite,
    U: MessageRead + MessageReadReady + Send + 'static,
{
    fn run(mut self) {
        // Read and print messages on a thread
        std::thread::spawn(move || loop {
            const RESPONSE_WAIT: Duration = Duration::from_millis(100);
            while !self.from_ui.ready(RESPONSE_WAIT) {}

            let msg = self.from_ui.next().unwrap();
            let event: SecurityToUiEvent = msg.to_tls().unwrap();
            println!("Received event: {:?}", event);
        });

        loop {
            // Read a line from stdin
            let mut buffer = String::new();
            stdin().read_line(&mut buffer).unwrap();

            // Send it to the UI processor as AsciiMessage
            let ascii = AsciiMessage {
                team: 1,
                channel: 2,
                device_id: 3,
                ascii: buffer.as_bytes().to_vec().into(),
            };
            let event = SecurityToUiEvent::AsciiMessage(ascii);
            let msg = Message::from_tls(&event).unwrap();
            self.to_ui.write(&msg).unwrap();
            println!("Sent event: {:?}", event);
        }
    }
}

fn main() -> Result<()> {
    let sec_to_ui = nonblocking::open("sec_to_ui");
    let ui_to_sec = nonblocking::open("ui_to_sec");

    let mock_sec_proc = MockSecurityProcessor {
        to_ui: sec_to_ui,
        from_ui: ui_to_sec,
    };

    mock_sec_proc.run();

    Ok(())
}
