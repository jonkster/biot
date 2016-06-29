# More complicated network stuff + oled

With 2 boards, send a udp message with a led colour from one board to the other

eg ```udp fe80::5842:2d4f:22f6:7e2e red```

The 2 boards should start responding to each other, changing led colours as
they go until communication is broken.

Some details of the communication are displayed in the OLED display.

