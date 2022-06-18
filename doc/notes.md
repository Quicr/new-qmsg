
# Project Notes

This is an unstructured and informal notes document on thoughts/updates.

## Initial Summer Coding Exercise

Not all that has been defined in the over all architecture and protocol would need to be completed.  Only a subset of core functionality is needed.

### Acceptance Criteria

* Endpoints signal other endpoints to join a communication stream.
* Two endpoints initially join and begin to exchange messages bidirectionally. Endpoints can be in same region.
* Two additional endpoints join and begin to exchange messages bidirectionally. Endpoints should be in different regions.
* Latency end-to-end via the global network should be less than 100ms within the same region and no more than 250ms between regions

## Thoughts

* Endpoint subscription must expire at some point.  
* Endpoint subscription must be access controlled/authorized and should limit auto subscriptions to
  sub-channels/streams.
* Caches should cache for a short period of time, enough to retransmit and handle
  disconnect/reconnects, but short enough to drop messages that are considered too old.
  Cache age is best to be at the stream/channel/topic level
* Endpoints start up cycle connects first to the local relay to advertise it self and to subscribe
  to streams.
* Each endpoint subscribes and produces to a series of streams for control signaling
  * Endpoints are advertise to other endpoints via one or more group registration streams. Multiple
    streams are used to support the use-case of limiting what some endpoints can see. For example,
    org level stream.
  * Endpoints subscribe to relay network notifications.  This includes details about the Local Relay
    the endpoint and other profile level settings for endpoints that use the local relay. For
    example, max allowed age of messages, rate limits, ...
  * Endpoints produce/advertise themselves to the registration streams that they are instructed to
    via the notification stream
  * Endpoints produce/advertise notifications to the relay network. For example, instruct the relay
    network to handle its messages a specific way, add streams, etc.
* Use groups for 1:1 instead of direct addressing so that anyone else
  can join in
* Endpoint registration includes the ER it's attached. This directly drives how other ERs can optimize paths. 

* protocol specific; a stream is within a message channel (e.g., topic).  Multiple streams can exist, but they may be forwarded via the same channel.  When they are forwarded via the same channel, the endpoint and relays demux to specific endpoints
* A channel defines cache retention, optimized forwarding paths, general authorization, ...
