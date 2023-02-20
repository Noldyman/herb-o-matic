# Herb-o-matic

---

The Herb-O-Matic is an automatic watering system for my precious basil and parsley plants. For this project I used a Wemos D1 mini. All the hardware was orderd at https://www.tinytronics.nl.

Every hour, the system measures the moistness of the soil in the planter. If the moistness is too low, it will pump water into the planter. After it performed these actions, it will send the data (soil moistness and weather the pump was active) to my home-assistant server over MQTT.
