# ilifev7s-rest-remote

Rest remote for Chuwi iLife v7s built on ESP8266


## Hardware
- [ESP8266](https://www.amazon.it/versione-internet-sviluppo-wireless-Micropython/dp/B0791FJB62) (~ 5.00€)
- IR led (~ 0.60€)


## Usage

- `http GET http://{ip}:80` debug endpoint just to see if it is working

- `http POST http://{ip}:80/ilifev7s action={action}` execute an action - action is one of {up, clean, down, left, right, spot, home, edge}

- `http GET http://{ip}:80/ilifev7s` to see the previous received action


## Most important dependencies

- [ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer) Simple web server that knows how to handle HTTP requests
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) C++ JSON library for IoT. Simple and efficient
- [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266) Infrared remote library for ESP8266: send and receive infrared signals with multiple protocols


## Thanks to
- [Rest server example](https://gist.github.com/mancusoa74/9450227d1251e0a527965e858cf6eebd#file-simplehttprest-ino) The code that I used to start my project
- [IR codes](https://github.com/z3t0/Arduino-IRremote/issues/354) The issue where I take the IR codes without pain :p

