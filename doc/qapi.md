# API Messages sent across UI-Sec-Net processes for qmsg


This is a dataflow based API. The messages passed are binary encoded with TLV+Checksum format.

All times are absolute times in ms since the unix epoch


# Message Send and Receive

## Send Ascii Message

Allows ASCII characters 0x20 to 0x7E. Note this does not include CR or
LF so messages meant to display on different lines need to be sent as
multiple messages.

### UI -> Sec
EventType: Ascii_Message

Data:
* uint32:  team
* uint32:  channel
* opaque value<0..2^32-1>: plaintext // [4B length + uint8_t bytes]


### Sec -> Net
EventType: Ascii_Message

Data:
* uint32:  org 
* uint32:  team 
* uint32:  channel
* uint16:  device_id // maps to mls index
* uint32:  msg_id
* opaque value<0..2^32-1>: ciphertext // [4B length + uint8_t bytes]


## Receive Ascii Message 

### Net -> Sec
EventType: Ascii_Message

Data:
* uint32:  team
* uint32:  channel
* uint16:  device_id
* opaque value<0..2^32-1>: ciphertext // [4B length + uint8_t bytes]


### Sec->UI
EventType: Ascii_Message

Data:
* uint32: team
* uint32: channel
* uint16:  device_id // maps to mls index
* opaque value<0..2^32-1> // [4B length + uint8_t bytes]


# Watch and Unwatch channels
These messages are used to listen to and stop listening for messages from a given channel

## Watch

### UI -> Sec
EventType: Watch_Channel

Data:
* uint32: team
* uint32: channel

### Sec -> Net
EventType: Watch_Devices

Data:
* uint32: team
* uint32: channel
* uint16: num_device_id
* uint16[]: device_id_list

## Unwatch
Cancels any watches for a given channel

### UnWatch (UI -> Sec)
EventType: Unwatch_Channel

Data:
* int32: team
* int32: channel

## UnWatch (Sec->Net)
EventType: Unwatch_Devices

Data:
* int32: team
* int32: channel
* uint16: num_device_id
* uint16[]: device_id


# MLS Group Management Messages
This section covers message related to setting up MLS crypto state

## SignatureHash (UI -> Sec / Sec -> Net)
Identfier for a given user's device

EventType: SignatureHash

Data:
* uint32: team
* opaque vector<0..2^16-1>: key-package hash bytes or equivalent to it



## KeyPackage (Sec -> Net)
This event reports a Keypackage for the user/device to the network process on bootup
in order to be transmitted to the MLS leader for joining the group

EventType: Keypackage

Data:
* uint32: team
* opaque vector<0..2^32-1>: key-package bytes
* opaque vector<0..2^32-1>: key-package-hash bytes

## Add KeyPackage (Net -> Sec)
This event indicates to the MLS leader that a user/device wants to 
join the MLS group for a given team and has corresponding KeyPackage

EventType: Keypackage

Data:
* uint32: team
* opaque vector<0..2^32-1>: key-package bytes


## MLS Welcome (Sec -> Net / Net -> Sec)

This event indicates a new user was successfully added to the 
MLS group by the leader for a given team. MLS Welcome 
message is deilvered to the user attempting to join.

EventType: MLSWelcome

Data:
* uint32: team
* opaque vector<0..2^32-1>: mls-welcome bytes

## MLS Commit (Sec -> Net, Net -> Sec)
This event indicates group state was successfully updated  
by the leader for a given team. MLS Commit 
messages are deilvered to the group

EventType: MLSCommit

Data:
* uint32: team
* opaque vector<0..2^32-1>: mls-commit bytes


## DeviceInfo (Sec -> Net)

This message is sent to indicate MLS device details 
to the network process after the user 
joins a MLS group successfully.

EventType: DeviceInfo

Data:
* uint32: team
* uint16: device_id

# Miscellenous Messages

## Unlock (UI->Sec)

When the UI Process starts, it should send an unlock messages. 

Data:
* int32: PIN

## Is locked 
Flows from Secure to UI anytime the secure changes to locked state or any time a message is received when it is in the locked state. 

