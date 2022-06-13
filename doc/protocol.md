# QMessage Protocol

Each message gets a unique name and is published at that name. Clients subscribe to a wild carded subset of the name to receive the desired messages.

## Names

Names follow the form:

```
qmsg://<orgin-domain>/<version>/<org>/<team>/<channel>/<device>/<msgNum>
```

* origin domain: DNS domain name of origin server
* version: protocol version number 
* org:  number allocated by the origin domain for each organization using this origin 
* team: number allocated by org for each team in the org 
* channel: number allocated by team owner for each channel in the team 
* device: number allocated by the team for each device in the team 
* msgNum: number allocated by the device for each message in this channel from this device

### Short Names

originID: 48 bits.  SHA1 hash of origin-domain
appID: 48 bits. low 2 bits of version, 20 bit org, 15 bit team, 11 bit channel 
groupIID: 16 bits. device 
objIID: 16 bits. msgNum.

msg ID is 128 bit number formed by concatenating originID, appID, groupID, objID

channel ID is 96 bit number formed by  concatenating originID, appID 

## Messages

Each message has a:
* sender identity
* encrypted payload
* signature
* expiry time 

## Payload

Each payload has:
* mime message type
* message data
* creation time
* optional reply to device/msg
* optional replaces device/msg

To create a threat, the reply to is set to identify the first message of the thread. Threads can not be nested.

To edit a message, a new message is sent with a replaces that identifies the message being edited.

## mine types

### text/ascii-printable

Allows ASCII characters 0x20 to 0x7E. Note this does not include CR or LF so messages meant to display on different lines need to be sent as multiple messages. 




