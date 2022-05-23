# CAO-project
Automatic door bell with ESP32-cam

## Project Description
  In this I use an ESP32 micro-controller to create an automatic door bell 
  with face recognition capabilities.
## Hardware Used
- ESP32-cam
- FTDI board
- LEDs
- Breadboard
- 5V power supply
## IDE Used
  In this project I have used Arduino IDE. You may also use
  platform.io with VS-code if you want to utilize the VS-code
  environment
## Getting started
1. First you need to install Arduino ide on your machine.

2. Once you are done, you must install the esp32-tensorflow
library. Go to Sketch-> Include library-> Manage libraries. 
Search for esp32 tensorflow and install it.

3. Next install the Eloquent Vision libary according to above 
instructions as well. This libary provides and easy interface to
use to esp32's camera.

4. Next you need to build a tensorflow model in keras. I have created
a model using google colab. You may create and train the model on your
local computer.

5. Feed the model your facial data and train it.

6. Once the model has trained, convert the model into a tf-lite
model with the following command:

    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()

7. Now convert this model into a C++ array so it can be 
read by our micro-controller.

    !apt-get -qq install xxd        (Get the xxd command) 
    !xxd -i model.tflite > model.cc

8. Create a sketch, and add the model.cc file to it.
9. Use the esp32 tensorflow library to run inference.
10. Congratulations you have succesfully deployed an 
tensorflow model on a micro-controller.

  #converter = tf.lite.TFLiteConverter.from_keras_model(model)
  #tflite_model = converter.convert()
