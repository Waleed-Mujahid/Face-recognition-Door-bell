#define CAMERA_MODEL_AI_THINKER

#include "camera.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include "esp_camera.h"
#include "model.h"
#include "EloquentVision.h"
#include "WiFi.h"
#include "EloquentVision.h"
#include "ESP32CameraHTTPVideoStreamingServer.h"

#define FRAME_SIZE FRAMESIZE_QVGA  // Image is taken in QVGA quality
#define SOURCE_WIDTH 320           // QVGA = 320x240
#define SOURCE_HEIGHT 240
#define CHANNELS 1
#define DEST_WIDTH 32              // Target resolution is 32x24
#define DEST_HEIGHT 24


// Usage of eloquent library to resize image
using namespace Eloquent::Vision;
using namespace Eloquent::Vision::Camera;
using namespace Eloquent::Vision::ImageProcessing;
using namespace Eloquent::Vision::ImageProcessing::Downscale;
using namespace Eloquent::Vision::ImageProcessing::DownscaleStrategies;

ESP32Camera camera;
HTTPVideoStreamingServer server(81);          // the buffer to store the downscaled version of the image
uint8_t resized[DEST_HEIGHT][DEST_WIDTH];     // the downscaler algorithm
Cross<SOURCE_WIDTH, SOURCE_HEIGHT, DEST_WIDTH, DEST_HEIGHT> crossStrategy;                               // the downscaler container
float img[DEST_HEIGHT][DEST_WIDTH];
Downscaler<SOURCE_WIDTH, SOURCE_HEIGHT, CHANNELS, DEST_WIDTH, DEST_HEIGHT> downscaler(&crossStrategy);   // the motion detection algorithm
MotionDetection<DEST_WIDTH, DEST_HEIGHT> motion;

void setup() {
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);
  camera.begin(FRAMESIZE_QVGA, PIXFORMAT_GRAYSCALE);
  camera_fb_t *frame = camera.capture();
  downscaler.downscale(frame->buf, resized);
  // 32x24 image is returned in RESIZE array

  // Now we normalize to so we can feed data to machine learning model
  for (int i =0;i<DEST_HEIGHT;i++){
    for (int j =0;j<DEST_WIDTH;j++){
      img[i][j] = (float)resized[i][j]/255.0;
      Serial.print(img[i][j]);
      Serial.print(" ");
      }
      Serial.print("line is at: ");
      Serial.println(i);
    }
}

void loop() {

}
