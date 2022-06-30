use env_logger;
use log::{error, info, warn};
use openmls::prelude::*;
use openmls_rust_crypto::OpenMlsRustCrypto;
use qmsg_core::events::*;
use qmsg_core::*;
use std::cell::*;
use std::collections::HashMap;
use std::time::Duration;
use std::{env, str};
use tls_codec::*;

struct SecurityProcessor<R, W>
where
    R: MessageRead,
    W: MessageWrite,
{
    to_network: W,
    from_network: R,
    to_ui: W,
    from_ui: R,

    backend: OpenMlsRustCrypto,
    groups: HashMap<u32, RefCell<MlsGroup>>,
    our_kp: KeyPackage,
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
                let msg = self.from_network.next().unwrap();
                match msg.to_tls::<NetworkToSecurityEvent>().unwrap() {
                    NetworkToSecurityEvent::JoinRequest(jr) => {
                        let kp = jr.key_package.unwrap();
                        let mut group = match self.groups.get(&jr.team) {
                            Some(group) => group.borrow_mut(),
                            None => {
                                warn!("Got KeyPackage without corresponding group for team {}, ignoring", jr.team);
                                continue;
                            }
                        };
                        let (commit, welcome) = group.add_members(&self.backend, &[kp]).unwrap();
                        group.merge_pending_commit().unwrap();
                        self.to_network
                            .write(
                                &Message::from_tls(&SecurityToNetworkEvent::MlsCommitOut(
                                    MlsCommitOut {
                                        team: jr.team,
                                        commit: TlsSerialized::new(commit),
                                    },
                                ))
                                .unwrap(),
                            )
                            .unwrap();
                        self.to_network
                            .write(
                                &Message::from_tls(&SecurityToNetworkEvent::MlsWelcome(
                                    MlsWelcome {
                                        team: jr.team,
                                        welcome: TlsSerialized::new(welcome),
                                    },
                                ))
                                .unwrap(),
                            )
                            .unwrap();
                        info!("Successfully added member to group, sent out commit and welcome");
                    }
                    NetworkToSecurityEvent::MlsWelcome(w) => {
                        let welcome = w.welcome.unwrap();
                        let group_config = &MlsGroupConfig::builder()
                            .use_ratchet_tree_extension(true)
                            .build();
                        let group = match MlsGroup::new_from_welcome(
                            &self.backend,
                            group_config,
                            welcome,
                            None,
                        ) {
                            Ok(group) => group,
                            Err(e) => {
                                error!("Failed to set up group from welcome: {}", e);
                                continue;
                            }
                        };
                        self.groups.insert(w.team, RefCell::new(group));
                    }
                    NetworkToSecurityEvent::MlsCommit(c) => {
                        let mut group = match self.groups.get(&c.team) {
                            Some(group) => group.borrow_mut(),
                            None => {
                                warn!(
                                    "Got Commit without corresponding group for team {}, ignoring",
                                    c.team
                                );
                                continue;
                            }
                        };
                        match self.verify_mls_message(&mut group, c.commit.unwrap()) {
                            Ok(res) => {
                                if let ProcessedMessage::StagedCommitMessage(_) = res {
                                    if let Err(e) = group.merge_pending_commit() {
                                        warn!("Failed to merge commit: {}", e);
                                        continue;
                                    }
                                    info!("Successfully applied incoming commit");
                                } else {
                                    warn!("Got non-commit message in MlsCommit event");
                                    continue;
                                }
                            }
                            Err(e) => {
                                warn!("Failed to verify incoming commit: {}", e);
                                continue;
                            }
                        }
                    }
                    NetworkToSecurityEvent::EncryptedAsciiMessage(am) => {
                        let mut group = match self.groups.get(&am.team) {
                            Some(group) => group.borrow_mut(),
                            None => {
                                warn!(
                                    "Got AsciiMessage without corresponding group for team {}, ignoring",
                                    am.team
                                );
                                continue;
                            }
                        };
                        match self.verify_mls_message(&mut group, am.ciphertext.unwrap()) {
                            Ok(res) => {
                                if let ProcessedMessage::ApplicationMessage(msg) = res {
                                    let msg_bytes = msg.into_bytes();
                                    let out = AsciiMessage {
                                        team: am.team,
                                        channel: am.channel,
                                        device_id: Self::get_device_id(&group, &self.our_kp)
                                            .unwrap(),
                                        ascii: TlsByteVecU32::from_slice(msg_bytes.as_slice()),
                                    };
                                    self.to_ui
                                        .write(
                                            &Message::from_tls(&SecurityToUiEvent::AsciiMessage(
                                                out,
                                            ))
                                            .unwrap(),
                                        )
                                        .unwrap();
                                    info!(
                                        "Got message with content \"{}\" from network, sent to UI process",
                                        str::from_utf8(msg_bytes.as_slice()).unwrap()
                                    );
                                } else {
                                    warn!("Got non-application message in AsciiMessage event");
                                    continue;
                                }
                            }
                            Err(e) => {
                                warn!("Failed to verify incoming message: {}", e);
                                continue;
                            }
                        }
                    }
                }
            }

            if self.from_ui.ready(poll_wait) {
                let msg = self.from_ui.next().unwrap();
                match msg.to_tls::<UiToSecurityEvent>().unwrap() {
                    UiToSecurityEvent::WatchChannel(w) => {
                        let out = WatchDevices {
                            team: w.team,
                            channel: w.channel,
                            // Temporary: simply watch device IDs [0, 10)
                            device_ids: TlsVecU16::new(Vec::from_iter(0..10)),
                        };
                        self.to_network
                            .write(
                                &Message::from_tls(&SecurityToNetworkEvent::WatchDevices(out))
                                    .unwrap(),
                            )
                            .unwrap();
                    }
                    UiToSecurityEvent::UnwatchChannel(u) => {
                        let out = UnwatchDevices {
                            team: u.team,
                            channel: u.channel,
                            // Temporary: simply unwatch device IDs [0, 10)
                            device_ids: TlsVecU16::new(Vec::from_iter(0..10)),
                        };
                        self.to_network
                            .write(
                                &Message::from_tls(&SecurityToNetworkEvent::UnwatchDevices(out))
                                    .unwrap(),
                            )
                            .unwrap();
                    }
                    UiToSecurityEvent::AsciiMessage(am) => {
                        let mut group = match self.groups.get(&am.team) {
                            Some(group) => group.borrow_mut(),
                            None => {
                                warn!(
                                    "Tried to send AsciiMessage without corresponding group for team {}, ignoring",
                                    am.team
                                );
                                continue;
                            }
                        };

                        let message = match group.create_message(&self.backend, am.ascii.as_slice())
                        {
                            Ok(out) => out,
                            Err(e) => {
                                error!("Failed to create message: {}", e);
                                continue;
                            }
                        };
                        let out = EncryptedAsciiMessageOut {
                            team: am.team,
                            channel: am.channel,
                            ciphertext: TlsSerialized::new(message),
                        };
                        self.to_network
                            .write(
                                &Message::from_tls(&SecurityToNetworkEvent::EncryptedAsciiMessage(
                                    out,
                                ))
                                .unwrap(),
                            )
                            .unwrap();
                        info!(
                            "Created message with content \"{}\", sent to network",
                            str::from_utf8(am.ascii.as_slice()).unwrap()
                        );
                    }
                }
            }
        }
    }

    fn verify_mls_message(
        &self,
        group: &mut MlsGroup,
        message: MlsMessageIn,
    ) -> Result<ProcessedMessage, String> {
        let unverified = group
            .parse_message(message, &self.backend)
            .map_err(|e| format!("failed to parse message: {}", e))?;
        group
            .process_unverified_message(unverified, None, &self.backend)
            .map_err(|e| format!("failed to verify message: {}", e))
    }

    fn get_device_id(group: &MlsGroup, key_package: &KeyPackage) -> Result<u16, String> {
        group
            .export_ratchet_tree()
            .iter()
            .filter_map(|n| match n {
                Some(node) => match node {
                    Node::LeafNode(leaf) => Some(leaf),
                    _ => None,
                },
                _ => None,
            })
            .position(|leaf| leaf.key_package().eq(&key_package))
            .map(|u| u as u16)
            .ok_or("KeyPackage not in team".into())
    }
}

fn main() {
    env_logger::init();

    // Communications with the network processor
    let s2n = nonblocking::open("/tmp/pipe-s2n");
    let n2s = nonblocking::open("/tmp/pipe-n2s");

    // Communications with the UI processor
    let s2u = nonblocking::open("/tmp/pipe-s2u");
    let u2s = nonblocking::open("/tmp/pipe-u2s");

    let backend = OpenMlsRustCrypto::default();

    // Set up our own credentials
    let credential_bundle = CredentialBundle::new(
        env::var("MLS_ID").unwrap().as_bytes().to_vec(),
        CredentialType::Basic,
        SignatureScheme::ED25519,
        &backend,
    )
    .unwrap();
    let cred = credential_bundle.credential().clone();
    backend
        .key_store()
        .store(
            &cred.signature_key().tls_serialize_detached().unwrap(),
            &credential_bundle,
        )
        .unwrap();

    let key_package_bundle = KeyPackageBundle::new(
        &[Ciphersuite::MLS_128_DHKEMX25519_AES128GCM_SHA256_Ed25519],
        &credential_bundle,
        &backend,
        vec![],
    )
    .unwrap();
    let key_package = key_package_bundle.key_package().clone();
    backend
        .key_store()
        .store(
            key_package.hash_ref(backend.crypto()).unwrap().value(),
            &key_package_bundle,
        )
        .unwrap();

    let mut groups = HashMap::new();

    if match env::var("LEADER") {
        Ok(v) => v == "1",
        Err(_) => false,
    } {
        let group_config = &MlsGroupConfig::builder()
            .use_ratchet_tree_extension(true)
            .build();
        let g = MlsGroup::new(
            &backend,
            group_config,
            GroupId::from_slice(b"Static team test group"),
            key_package.hash_ref(backend.crypto()).unwrap().value(),
        )
        .unwrap();
        groups.insert(123, RefCell::new(g));
    }

    let sec_proc = SecurityProcessor {
        to_network: s2n,
        from_network: n2s,
        to_ui: s2u,
        from_ui: u2s,
        backend: backend,
        groups: groups,
        our_kp: key_package,
    };

    sec_proc.run();
}
