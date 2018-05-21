# Lora CHAT ESP32 OLED : A autonomous Lora Chat 

---
![Image screen shot](./lora_esp.jpg)
---

## Context

A tweaked out version from : https://github.com/unprovable/LoRaChat

Just add some buttons, and code to make it autonomous

2 red buttons for direction : Left and Right
1 blue button to choose a letter
1 green button to validate pseudo / messages

Then, each arrived or sent messages are displayed with the pseudo at the beginning 

and Left button again to come back to the edit message screen 

Anf that's it !

## Technical details

The board is a : [Heltec ESP32 LORA](https://robotzero.one/heltec-wifi-lora-32/)

It's NOT LORAWAN compliant ... In fact it's more a Proof of Concept than a real project ...

But it works ! And i made a few tests of distance coverage, i can confirm that it's quite impressive ! 

With more options, a clean code and some final design... it can make a decent "off the grid" pager ;-)

See yeah
