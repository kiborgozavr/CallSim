# CallSim State Machine

## Overview

CallSim manages client states on both the client and server sides.

The client implements an object-oriented finite state machine. Each client has its own current state and transitions to another state when it receives an event.

The server maintains the state of each connected client. It processes incoming events, updates client states, and sends corresponding events to the affected clients.

The two sides work together to coordinate registration, call setup, call rejection, and conversation termination.

## Client State Machine

The client state machine is implemented using object-oriented design. Each state encapsulates the behavior associated with that state.

**States** are written in uppercase, while **events** are written in lowercase.

| Current State | Event | Next State |
|---|---|---|
| `CONNECTED` | `registered` | `REGISTERED` |
| `REGISTERED` | `calling` | `CALLING` |
| `REGISTERED` | `answering` | `ANSWERING` |
| `CALLING` | `accepted` | `TALKING` |
| `ANSWERING` | `accepted` | `TALKING` |
| `CALLING` | `rejected` | `REGISTERED` |
| `ANSWERING` | `rejected` | `REGISTERED` |
| `TALKING` | `ended` | `REGISTERED` |

### State Descriptions

- **`CONNECTED`** — The client has connected to the server but has not completed registration.
- **`REGISTERED`** — The client is registered and available to initiate or receive a call.
- **`CALLING`** — The client has initiated an outgoing call and is waiting for the recipient's response.
- **`ANSWERING`** — The client has received an incoming call and can accept or reject it.
- **`TALKING`** — The call has been accepted, and both participants can exchange text messages.

## Server State Management

The server maintains a state for each connected client and manages transitions using a transition table.

Unlike the client state machine, which handles events to change its own state, the server coordinates state changes across multiple clients.

When the server receives an event, it processes the corresponding transition, updates the state of the affected client or clients, and sends the appropriate event or events.

### Server Transition Table

The numbers `(1)` and `(2)` identify the first and second participants involved in a call.

| Current State | Incoming Event | State Update | Events Sent |
|---|---|---|---|
| `CONNECTED` | `register` | Client → `REGISTERED` | `registered` to the registering client |
| `REGISTERED` | `call` | Caller → `CALLING`; recipient → `ANSWERING` | `calling` to participant (1); `answering` to participant (2) |
| `ANSWERING` | `accept` | Both participants → `TALKING` | `accepted` to participants (1) and (2) |
| `ANSWERING` | `reject` | Both participants → `REGISTERED` | `rejected` to participants (1) and (2) |
| `TALKING` | `end` | Both participants → `REGISTERED` | `ended` to participants (1) and (2) |

### Call Handling

A typical call follows these steps:

1. The caller sends a `call` event to the server.
2. The server changes the caller's state to `CALLING` and the recipient's state to `ANSWERING`.
3. The server notifies both participants of their respective state transitions.
4. The recipient responds with `accept` or `reject`.
5. If the call is accepted, both clients transition to `TALKING`. If it is rejected, both return to `REGISTERED`.
6. When either participant ends the conversation, the server notifies both clients with the `ended` event and restores their states to `REGISTERED`.

![CallSim State Machine](CallSim State Machine.png)