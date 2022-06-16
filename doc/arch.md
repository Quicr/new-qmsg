# Architecture

Thy system consists of endpoints that users can use to send and receive
messages and a tree of relays that help distribute and cache the messages.

## Network Architecture

```mermaid
flowchart BT
    A(Alice Endpoint) <--> LR(Local Relay)
    B(Bob Endpoint) <--> LR
    LR <--> CR(Cloud Relay)
    CR <--> OR(Origin Relay)
    C(Charlie Endpoint) <---> CR
```

Endpoints can talk to a local relay or cloud relay.

The endpoints ask for given set of messages and receive those messages
from the relay and any future messages that match the request.

The relays form a tree rooted at a single logical Origin
Relay. internally the Origin Relay could be a distributed across many
servers.  Cloud relays are discovered by configuration. DNS
names, and/or anycast. Local Relays are discovered by MDNS.

## Terminology, Names, and Concepts

Users send and receive Messages.

Each User belongs to exactly one Organization. Each user has one or more
Devices to send and receive Messages.

Organization have many Teams. Each Team is owned by one Organization but
has a bunch of Users from one or more Organizations. Each team has
multiple Channels and each Channel has a set of Threads and each Thread
has a set of Messages.

## Endpoint Architecture

```mermaid
flowchart LR
    UI(UI Proc) <-- plain ---> SEC
    SEC(Secure Proc) <-- encrypted ---> NET
    NET(Network Proc)
    CACHE[(Cache Storage)] <--> NET
```

The UI Process takes care of all the UI. Mostly display of message and
creating new messages. It passes unencrypted messages to the Secure
Processor and receives unencrypted messages. The code is security
reviewed. It is in C++. 

The Secure process takes care of management of all keys as well as
encrypting and decrypting all messages. The code is developed securely
and is in Rust. It also keep track of all the teams and  a
device is a member of.

TODO - who keeps track of channels. 

The Network Process only hands encrypted messages and is not developed
security. It takes care of distribution of the messages over the network
and local caching of messages. It is in C++.  It implements the protocol
found in the the [TODO link protocol.md doc]. The low level portion of
the network process simply needs to be able to connect to a relay, and
send publish and subscribes to it.  This process also needs to be able
to find the time - for the initial implementation just the OS time can be
used but later implementations could subscribe to get the time. 


## Relays

Relays cache and distribute names chunks of data. Each chunk has a 128
bit name, an expiry time, and 1K or less of data. Clients that connect
to a relay can subscribe to a set of names they wish to receive data
for.  Clients can publish a chunk to a relay and the network of relays
will ensure that any relay that has a client subscribe to that name will
eventually receive that chunk.

The current implementation arranges all the relays into a tree and when any
relay receives new data data, it distributes it to all sub trees as well
as the node above it. It does not send it to the node it came from. Each
relay is configured with the node above it in the tree and the root node
can be found via DNS at the origin for the message name. 

## Tools


### Dump & Undump Tool

Takes subpath and dumps all messages and MLS message (commit, welcome,
and key-package)  under that path to individual files. The undump takes
that set of files from and publishes all the files

### Member Tool

CLI tool that run on osx that allows creation of orgs, teams, users,
devices

* provisioned with orgID and has pub/priv keys for admin user

* Keep track of current number of teams and epoch number 

* Can create a new team and add Admin user to team

* can get hKP as input and add that user to a team

* can dump all deviceID for all devices in a team



