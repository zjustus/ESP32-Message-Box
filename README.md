# ESP32-Message-Box
This is a picture messaging box concept project moddled after the [love note messenger box](https://en.lovebox.love/?gclid=Cj0KCQjw8p2MBhCiARIsADDUFVG-ykPWN5HD-qCWSHGmrksrYyajCpcGDbMe8eBeEUYvFspvNea1L3AaAqYOEALw_wcB) but this only costs 25 dollars. (some assembly required)  

# Hardware
- ESP32](https://www.amazon.com/gp/product/B0718T232Z/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1)  
- [ILI9341 touch screen](https://www.amazon.com/gp/product/B087C3PP9G/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1)  

# Tech
This project uses the hardware SPI pins on the ESP32 to quickly communicate information to the touch screen.  
when sending messages it will read the screen and send anything not blank over MQTT to its counterpart listing on the other end.  

The MQTT communication uses my own linked-list and base64 library developed for this project to send the information over the free [hive MQTT broker](https://www.hivemq.com/public-mqtt-broker/)  

The largest challenges of this project was to encode send and decode full pictures over the verry limited MQTT proptcalls that were not designed for large data transmittion as well as the verry limited memory capacity of the ESP32. The pictures had to be read from the screen line by line, encoded from hex values to base64 spanning an uneven bite array to save space then sending this as a text stream over MQTT then doing the reverse on the other side to recreate the immage.  

The interface is not pollished, Wifi needs to be configured in the code and there are 100 things I would like to do to this project to make it better, but I have no time and it does infact work and work well.  

Feel free to message me if you want better documentation about this project or want to pick it up where I left off. 
