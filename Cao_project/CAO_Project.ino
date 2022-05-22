/*
 *  Project: Automatic door bell with esp32-cam. Uses tflite to Recognise face.
 *  Project by: Muhammad Waleed Mujahid
 *  Libaries used: Tesnorflow-Esp32 to run model
 *  Eloquent-Vision: to resize image
*/

// Model name
#define CAMERA_MODEL_AI_THINKER

// Including dependencies
#include <TensorFlowLite_ESP32.h>
#include "driver/rtc_io.h"
#include "esp_camera.h"
#include "model.h"
#include "EloquentVision.h"
#include "tensorflow/lite/experimental/micro/kernels/micro_ops.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/experimental/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#define FRAME_SIZE FRAMESIZE_QVGA  // Image is taken in QVGA quality
#define SOURCE_WIDTH 320           // QVGA = 320x240
#define SOURCE_HEIGHT 240
#define CHANNELS 1
#define DEST_WIDTH 72              // Model expects 72 x 72 input 
#define DEST_HEIGHT 72  
#define LED_PIN 12                 // ESP32 pin GIOP12, which connected to led


// Usage of eloquent library to resize image
using namespace Eloquent::Vision;
using namespace Eloquent::Vision::Camera;
using namespace Eloquent::Vision::ImageProcessing;
using namespace Eloquent::Vision::ImageProcessing::Downscale;
using namespace Eloquent::Vision::ImageProcessing::DownscaleStrategies;
ESP32Camera camera;              // Object of esp32 Camera class
camera_fb_t *frame;              // Pointer to the image

float threshold = 0.6;                        // Setting the threshold for the model according to requirements
uint8_t resized[DEST_HEIGHT][DEST_WIDTH];     // Array to store resized image
Cross<SOURCE_WIDTH, SOURCE_HEIGHT, DEST_WIDTH, DEST_HEIGHT> crossStrategy; // the downscaler container
Downscaler<SOURCE_WIDTH, SOURCE_HEIGHT, CHANNELS, DEST_WIDTH, DEST_HEIGHT> downscaler(&crossStrategy);   

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
constexpr int kTensorArenaSize = 70  * 1024;             // An area of memory to use for input, output, and intermediate arrays.
static uint8_t* tensor_arena = (uint8_t*) malloc(kTensorArenaSize * sizeof(uint8_t)); 
}  // Dynamic memory used, due to lack of static memory

void setup() {
  Serial.begin(115200);
  pinMode (LED_PIN, OUTPUT);                              //Specify that LED pin is output
  camera.begin(FRAMESIZE_QVGA, PIXFORMAT_GRAYSCALE);      // Settings for the camera
  // Grayscale image of 320 x 240 pixels

  // Creating an error reporter object
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;
 
  // Map the model into a usable data structure.
  model = tflite::GetModel(cao_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Calling in all the TensorFlow operations required in the project
  static tflite::MicroMutableOpResolver micro_mutable_op_resolver;
  
  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_CONV_2D,
      tflite::ops::micro::Register_CONV_2D());
    
  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_MAX_POOL_2D,
      tflite::ops::micro::Register_MAX_POOL_2D());
     
  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_AVERAGE_POOL_2D,
      tflite::ops::micro::Register_AVERAGE_POOL_2D());

  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_RESHAPE,
      tflite::ops::micro::Register_RESHAPE());
  
  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_LOGISTIC,
      tflite::ops::micro::Register_LOGISTIC());
      
  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_FULLY_CONNECTED,
      tflite::ops::micro::Register_FULLY_CONNECTED());
    
  static tflite::MicroInterpreter static_interpreter(
      model, micro_mutable_op_resolver, tensor_arena, kTensorArenaSize,
      error_reporter);
  interpreter = &static_interpreter;       // Creating a Interpreter object to run inference

  interpreter->AllocateTensors();         // Necessary to allocate tensors before running the Tf-lite model
  input = interpreter->input(0);          // initializing the input pointer
}

void loop() {
  frame = camera.capture();                           // Captures the image
  downscaler.downscale(frame->buf, resized);          // Downscales the image to 72 x 72 so it can be fed to the Tf-lite model

  // Now we normalize to so we can feed data to machine learning model
  int post = 0;
  for (int i =0;i<DEST_HEIGHT;i++){
    for (int j =0;j<DEST_WIDTH;j++){
      input->data.f[post] = (float)resized[i][j]/255.0;
      post++;
    }
  }
  
  interpreter->Invoke();     // Runs inferece
  
  TfLiteTensor* output = interpreter->output(0); // Getting the ouput pointer
  float y_val = output->data.f[0];               // Storing the float value returned by the model

  Serial.print("Y_pred = ");
  Serial.println(y_val);        // Output the predicted value
  if (y_val > threshold)        // Model turns light on if prediction is above threshold
  {
    Serial.println("You are waleed. LED is on");
    digitalWrite(LED_PIN, HIGH);
  }
  else{
    Serial.println("You are NOT waleed. Go away");
  }
  delay(3000);
  digitalWrite(LED_PIN, LOW);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13,1); // Setting up the RTC to turn on the esp32 when person rings bell
  esp_deep_sleep_start();                      // Putting the esp32 to sleep
}
