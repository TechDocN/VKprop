# VKprop
# A Voight-Kampff styled prop for a Blade Runner-themed TTRPG
I am completely new to the world of Table-top Role Playing Games (TTRPG), but I am a huge fan of the movie Blade Runner. So when a Blade Runner TTRPG was announced to much fanfare on Kickstarter, I thought it would be a great way to get introduced to this type of gaming. As I began to learn about the game, I thought there might be some fun things I could do with electronics and simple code to augment the gameplay and add to the overall user experience. I decided on making an electronic and interactive prop that could be integrated into the gameplay to perform certain tasks, like a Voight-Kampff (VK) test, or rolling a die. I wanted this prop to live inside an enclosure that looks enough like a VK machine that it's easily recognized, but also a shape that is simple enough to 3D print with basic equipment. And finally, I wanted it to have a little bit of a "Wow!" factor, so I had to include some cool lights, sounds and graphics too.

![71461867472__C3A6EE0E-0C6C-421F-8617-02FF87353984](https://github.com/TechDocN/VKprop/assets/130190014/b265b4bc-3af8-457b-b3c2-dfc09468581b)

What does it do?
- When you turn on the power, all 3 screens display the VL logo and you are instructed to press the "VK" button.
- When you press the "VK" button, the screens will cycle through a boot sequence.
- After the boot sequence the large LCD screen will display the main menu, and the OLED screens will display two different animated graphics.
- The first menu choice runs a simulated VK test by displaying several images then displays one of 3 randomly selected results (human, replicant or unsure).
- The second and third menu items simulate a 10 or 12 sided die roll, respectively.


What is it made from?
- 1 Arduino Nano
- 2 Seeeduino Xiao
- 1 LCD display, with built-in microSD card reader
- 2 OLED displays, 0.96" 128x64
- 4 momentary push buttons
- 1 SPST rocker switch
- 1 passive buzzer
- 1 9V rechargeable battery
- 1 9V battery connector
- 3D printing filament of your choice (I used dark gray PLA)
- Optional red and green LEDs
