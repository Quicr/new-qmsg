# API Secure to Network

This defines the API from the Secure Processor to the Network processor.

This is a dataflow based API. The messages passed are binary encoded with TLV+Checksum format.

All times are absolute times in ms since the unix epoch


## DeviceInfo ( Sec -> Net )

This also publishes the Key Package. Sent at startup. 

Data:
* string: org domain
* int32: org ID
* int32: device ID
* string: MLS Key Package 
* int64: fingerprint of Key Package 

## Time ( Net -> Sec )

Sent at startup and peridically every few minutes after that. 

Data:
* int64: current time in ms since unix epoch


## MLS Commit ( Net -> Sec ) 

Data: 
* string:  MLS commit message 

## WatchTeam ( Sec -> Net ) 

Will cause Security to get appropriate welcome and commit messages for
specified team and publication of MLS Key package to that team as well
as any messages to channel 0 from device 0. 

Data: 
* int32: team num 


## MLS Welcome ( Net -> Sec ) 

Data: 
* string:  MLS welcome message 


## Send Ascii Message ( Sec -> Net )

Any messages that update the  team name are sent to channel
number 0. 

Data:
* string: encrypted MLS message with data for a messages
* int64: expiry time as absolute time in ms since unix epoch
* int32: team
* int32: channel

The information in the encrypted messages includes:
* ASCII message
* int64: creation time in ms since unix epoch
* int32: channel 
* int32: team 
* bool: updates team name
* bool: updates channel name 


## Received Ascii Message ( Sec -> Net )

Data:
* string: encrypted MLS message with data for a messages
* int32: claimed team
* int32: claimed channel
* int32: claimed sender device

The information in the encrypted messages is the same as the send. 


## GetChannels  (Sec -> Net)

Ask for all info on all channels in a team. The network code can find
this by looking at all channels that have at least one message from
device 0. Device 0 creates this message as the name update to channel
when it is created. 

Data:
* int32: team 


## Watch ( Sec -> Net )

Creates a subscription to get any new messages received for a given
channel from the specified device. Does not get all the of the old messages.

Data:
* int32: team
* int32: channel 
* int32: sender device 

## UnWatch  ( Sec -> Net )

Cancels any watches for a given channel

Data:
* int32: team
* int32: channel
* int32: sender device 

## Request Messages ( Sec -> Net )

Request a set of messages be sent to the UI. Request the most recent X messages on a given channel that all occurred before a specific time.

Data:
* int32: team
* int32: channel
* int32: sender device 
* int64: before time
* int16: num messages

