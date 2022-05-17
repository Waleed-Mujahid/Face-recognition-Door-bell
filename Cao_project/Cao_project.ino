#include "camera.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include "esp_camera.h"

void setup() {
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);
  
  //Initialize the camera  
  camera_config_t config;
  Serial.print("Initializing the camera module...");
  configInitCamera(config);
  Serial.println("Ok!");
}

void loop() {
  //Take and Save Photo
  //takeSavePhoto();

  camera_fb_t  * fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  else
  {
    Serial.println("Camera captured");
  }
  delay(3000); 
}
