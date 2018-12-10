//! Provide a UART receive interface that can receive until terminator.

use core::cell::Cell;

use kernel::common::cells::OptionalCell;
use kernel::hil;
use kernel::ReturnCode;

pub trait UartReceiveUntilTerminator: hil::uart::UART {
    fn receive_until_terminator(&self, rx_buffer: &'static mut [u8], terminator: u8);
}

#[derive(Clone, Copy, PartialEq)]
enum State {
    Normal,
    TerminatorReceive {
        index: usize,
        terminator: u8,
        first_byte: u8,
    },
}

pub struct UartTerminatorReceiver<'a, U: hil::uart::UART> {
    uart: &'a U,
    client: OptionalCell<&'a hil::uart::Client>,
    state: Cell<State>,
}

impl<U: hil::uart::UART> UartTerminatorReceiver<'a, U> {
    pub fn new(uart: &'a U) -> UartTerminatorReceiver<'a, U> {
        UartTerminatorReceiver {
            uart: uart,
            client: OptionalCell::empty(),
            state: Cell::new(State::Normal),
        }
    }
}

impl<U: hil::uart::UART> hil::uart::UART for UartTerminatorReceiver<'a, U> {
    fn set_client(&self, client: &'static hil::uart::Client) {
        self.client.set(client);
    }

    fn configure(&self, parameters: hil::uart::UARTParameters) -> ReturnCode {
        self.uart.configure(parameters)
    }

    fn transmit(&self, tx_data: &'static mut [u8], tx_len: usize) {
        self.uart.transmit(tx_data, tx_len)
    }

    fn receive(&self, rx_buffer: &'static mut [u8], rx_len: usize) {
        self.uart.receive(rx_buffer, rx_len)
    }

    fn abort_receive(&self) {
        self.uart.abort_receive()
    }
}

impl<U: hil::uart::UART> UartReceiveUntilTerminator for UartTerminatorReceiver<'a, U> {
    fn receive_until_terminator(&self, rx_buffer: &'static mut [u8], terminator: u8) {
        self.state.set(State::TerminatorReceive {
            index: 0,
            terminator: terminator,
            first_byte: 0,
        });

        self.uart.receive(rx_buffer, 10);
    }
}

impl<U: hil::uart::UART> hil::uart::Client for UartTerminatorReceiver<'a, U> {
    fn transmit_complete(&self, buffer: &'static mut [u8], error: hil::uart::Error) {
        self.client.map(move |client| {
            client.transmit_complete(buffer, error);
        });
    }

    fn receive_complete(&self, buffer: &'static mut [u8], rx_len: usize, error: hil::uart::Error) {
        // We need to check if this is a normal receive or a
        // receive_until_terminator. On a normal receive we just pass the
        // callback through, on a receive_until_terminator we need to check if
        // we got the terminator.

        match self.state.get() {
            State::Normal => {
                self.client.map(move |client| {
                    client.receive_complete(buffer, rx_len, error);
                });
            }
            State::TerminatorReceive {
                index,
                terminator,
                first_byte,
            } => {
                // // We are doing a trick with the first byte. Since the receive will
                // // always overwrite the first byte in the buffer, we keep that
                // // separate and that way we don't need an additional buffer.
                // if index == 0 {
                //     // Check if we got the terminator on the first byte.
                //     if buffer[0] == terminator {
                //         self.state.set(State::Normal);
                //         self.client.map(move |client| {
                //             client.receive_complete(buffer, 1, error);
                //         });
                //     } else {
                //         // We need to do another read.
                //         self.state.set(State::TerminatorReceive {
                //             index: 1,
                //             terminator,
                //             first_byte: buffer[0],
                //         });
                //         self.uart.receive(buffer, 1);
                //     }
                // } else {
                //     // Otherwise we do the normal operation where we wait
                //     // to receive the terminator.

                //     // Put the received byte in the correct place.
                //     if buffer.len() > index {
                //         buffer[index] = buffer[0];
                //     }

                //     // Check if we are done.
                //     if buffer[0] == terminator {
                //         self.state.set(State::Normal);
                //         // Replace the correct first byte.
                //         buffer[0] = first_byte;
                //         // Signal completion.
                //         self.client.map(move |client| {
                //             client.receive_complete(buffer, index + 1, error);
                //         });
                //     } else {
                //         // We need to do another read.
                //         self.state.set(State::TerminatorReceive {
                //             index: index + 1,
                //             terminator,
                //             first_byte,
                //         });
                //         self.uart.receive(buffer, 1);
                //     }
                // }

                for i in 0..10 {
                    if buffer[i] == terminator {
                        self.state.set(State::Normal);
                        self.client.map(move |client| {
                            client.receive_complete(buffer, i + 1, error);
                        });
                        break;
                    }
                }


                // // We are doing a trick with the first byte. Since the receive will
                // // always overwrite the first byte in the buffer, we keep that
                // // separate and that way we don't need an additional buffer.
                // if index == 0 {
                //     // Check if we got the terminator on the first byte.
                //     if buffer[0] == terminator {
                //         self.state.set(State::Normal);
                //         self.client.map(move |client| {
                //             client.receive_complete(buffer, 1, error);
                //         });
                //     } else {
                //         // We need to do another read.
                //         self.state.set(State::TerminatorReceive {
                //             index: 1,
                //             terminator,
                //             first_byte: buffer[0],
                //         });
                //         self.uart.receive(buffer, 1);
                //     }
                // } else {
                //     // Otherwise we do the normal operation where we wait
                //     // to receive the terminator.

                //     // Put the received byte in the correct place.
                //     if buffer.len() > index {
                //         buffer[index] = buffer[0];
                //     }

                //     // Check if we are done.
                //     if buffer[0] == terminator {
                //         self.state.set(State::Normal);
                //         // Replace the correct first byte.
                //         buffer[0] = first_byte;
                //         // Signal completion.
                //         self.client.map(move |client| {
                //             client.receive_complete(buffer, index + 1, error);
                //         });
                //     } else {
                //         // We need to do another read.
                //         self.state.set(State::TerminatorReceive {
                //             index: index + 1,
                //             terminator,
                //             first_byte,
                //         });
                //         self.uart.receive(buffer, 1);
                //     }
                // }
            }
        }
    }
}
