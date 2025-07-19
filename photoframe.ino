#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "JPEGDEC.h"

#define USE_VERTICAL 0

// WiFi settings
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Google Photos album share link
#if USE_VERTICAL
#define GOOGLE_PHOTOS_SHARE_LINK ""
#else
#define GOOGLE_PHOTOS_SHARE_LINK ""
#endif

// Display constants
#if USE_VERTICAL
#define DISPLAY_WIDTH EPD_7IN3F_HEIGHT  //480
#define DISPLAY_HEIGHT EPD_7IN3F_WIDTH  //800
#else
#define DISPLAY_WIDTH EPD_7IN3F_WIDTH   //800
#define DISPLAY_HEIGHT EPD_7IN3F_HEIGHT //480
#endif

// Google Photo constants
#define PHOTO_LIMIT 5
#define HTTP_WAIT_COUNT 50
#define SEEK_PATTERN "id=\"_ij\""
#define SEARCH_PATTERN "\",[\"https://lh3.googleusercontent.com/"
#define PHOTO_ID_SIZE 124  // the photo ID should be 123 charaters long and then add a zero-tail

// Certificate for HTTPS (may need to be updated)
const char* rootCACertificate =
"-----BEGIN CERTIFICATE-----\n"\
"MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw\n"\
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"\
"MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n"\
"MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"\
"Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA\n"\
"A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo\n"\
"27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w\n"\
"Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw\n"\
"TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl\n"\
"qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH\n"\
"szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8\n"\
"Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk\n"\
"MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92\n"\
"wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p\n"\
"aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN\n"\
"VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID\n"\
"AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E\n"\
"FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb\n"\
"C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe\n"\
"QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy\n"\
"h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4\n"\
"7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J\n"\
"ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef\n"\
"MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/\n"\
"Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT\n"\
"6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ\n"\
"0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm\n"\
"2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb\n"\
"bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n"\
"-----END CERTIFICATE-----\n";

// Add this array to store photo IDs
char photoUrlList[PHOTO_LIMIT][PHOTO_ID_SIZE];

// Add these constants for the download function
#define HTTP_TIMEOUT 30000           // 10 seconds timeout
#define MAX_PHOTO_SIZE (1024 * 1024)  // Reduced to 512KB max photo size

// Dynamic buffers
uint8_t* photoBufferJpeg = NULL;
uint32_t* photoBufferRaw = NULL;
uint8_t* photoBufferEink = NULL;

// JPEG decoder instance
JPEGDEC jpeg;

// Function declarations
int jpegDither(JPEGDRAW* pDraw);
void ditherPhoto(size_t photoSize);

void setup() {
  Serial.begin(115200);

  // Initialize PSRAM if available
  if (psramFound()) {
    Serial.println("PSRAM found and initialized");
    // Allocate buffers in PSRAM
    photoBufferJpeg = (uint8_t*)ps_malloc(MAX_PHOTO_SIZE);
    photoBufferRaw = (uint32_t*)ps_malloc(4 * DISPLAY_WIDTH * DISPLAY_HEIGHT);
    photoBufferEink = (uint8_t*)ps_malloc(EPD_7IN3F_WIDTH/2 * EPD_7IN3F_HEIGHT);
  } else {
    Serial.println("PSRAM not found, using DRAM");
    // Fallback to DRAM
    photoBufferJpeg = (uint8_t*)malloc(MAX_PHOTO_SIZE);
    photoBufferRaw = (uint32_t*)malloc(4 * DISPLAY_WIDTH * DISPLAY_HEIGHT);
    photoBufferEink = (uint8_t*)malloc(EPD_7IN3F_WIDTH/2 * EPD_7IN3F_HEIGHT);
  }

  if (!photoBufferJpeg || !photoBufferEink) {
    Serial.println("Failed to allocate memory!");
    while (1) delay(1000);
  }

  // // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Connect to WiFi
  Serial.printf("Connecting to WiFi...");
  // WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.printf(".");
  }
  Serial.printf("Connected to WiFi\n");

  // Grab photo URLs
  // getPhotoUrls();

  // // download a photo
  // size_t photoSize;
  // if (downloadPhoto(photoUrlList[0], &photoSize)) {
  //   // extract the jpeg photo
  //   extractJpegPhoto(photoSize);

  //   // dither the photo using given algorithm
  //   // ditherPhotoSierra();
  //   ditherPhotoFloydSteinberg();

  //   // setup display
  //   Serial.printf("EPD_7IN3F_test Demo\n");
  //   DEV_Module_Init();

  //   Serial.printf("e-Paper Init and Clear...\n");
  //   EPD_7IN3F_Init();
  //   // EPD_7IN3F_Clear(0xFF);  // Clear with white color (0xFF)

  //   Serial.printf("Displaying photo...\n");
  //   EPD_7IN3F_Display(photoBufferEink);
  // }

    //setup display
    Serial.printf("EPD_7IN3F_test Demo\n");
    DEV_Module_Init();

    Serial.printf("e-Paper Init and Clear...\n");
    EPD_7IN3F_Init();
    // EPD_7IN3F_Clear(0xFF);  // Clear with white color (0xFF)

  pinMode(0, INPUT);  // Set GPIO 0 as input
  randomSeed(analogRead(1)); // GPIO 1 is NC
}

void loop() {
  const unsigned long debounceDelay = 50; // 50 ms debounce
  static char curPhotoId[PHOTO_ID_SIZE] = "";
  static char newestPhotoId[PHOTO_ID_SIZE] = "";

  // You can add logic here to periodically fetch new photos or display them
  getPhotoUrls();

  // if the newest photo doesn't match what was previously grabbed then display it
  if(strcmp(newestPhotoId, photoUrlList[0]) != 0)
  {
    // set both the newest photo and the current photo to the first photo
    strcpy(newestPhotoId, photoUrlList[0]);
    strcpy(curPhotoId, photoUrlList[0]);
  }
  else
  {
    // otherwise, pick a random one that is not the newest and not the current photo
    int randIdx = random(1, PHOTO_LIMIT);
    while(strcmp(photoUrlList[randIdx], curPhotoId) == 0)
    {
      randIdx = random(1, PHOTO_LIMIT);
    }
    strcpy(curPhotoId, photoUrlList[randIdx]);
  }

  size_t photoSize;
  if (downloadPhoto(curPhotoId, &photoSize)) {
    // extract the jpeg photo
    extractJpegPhoto(photoSize);

    memset(photoBufferEink, 0, EPD_7IN3F_WIDTH/2 * EPD_7IN3F_HEIGHT);
    // dither the photo using given algorithm
    // ditherPhotoSierra();
    ditherPhotoFloydSteinberg();

    Serial.printf("Displaying photo...\n");
    EPD_7IN3F_Display(photoBufferEink);
  }

#if !USE_VERTICAL
  Serial.printf("Waiting for button press...\n");

  // Wait in a loop until the button (GPIO 0) is pressed (goes LOW)
  while (1) {

    int reading = digitalRead(0);
    if (reading == LOW) {
      delay(debounceDelay);
      reading = digitalRead(0);

      if (reading == LOW) {
        Serial.printf("Button pressed! GPIO 0 value: %d\n", reading);
        break;
      }
    }

    delay(1); // Small delay to avoid busy-waiting
  }
#endif
}

void getPhotoUrls() {
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  HTTPClient https;

  String currentUrl = GOOGLE_PHOTOS_SHARE_LINK;
  Serial.printf("Connecting to: %s\n", GOOGLE_PHOTOS_SHARE_LINK);
  if (!https.begin(client, GOOGLE_PHOTOS_SHARE_LINK)) {
    Serial.printf("HTTPS connection failed!\n");
    return;
  }

  https.setTimeout(HTTP_TIMEOUT);

  int httpCode = https.GET();
  for (int i = 0; i < 10; ++i) {
    Serial.printf("HTTP response code: %d\n", httpCode);
    delay(1000);
    if (httpCode != -1) {
      break;
    }
    httpCode = https.GET();
  }

  int redirectCount = 0;
  const int MAX_REDIRECTS = 5;  // Prevent infinite redirect loops

  while (httpCode == HTTP_CODE_FOUND || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    if (redirectCount >= MAX_REDIRECTS) {
      Serial.printf("Too many redirects, giving up\n");
      https.end();
      return;
    }

    String newUrl = https.getLocation();
    Serial.printf("Redirecting to: %s\n", newUrl.c_str());

    https.end();
    if (!https.begin(client, newUrl)) {
      Serial.printf("HTTPS connection failed on redirect!\n");
      return;
    }

    currentUrl = newUrl;
    httpCode = https.GET();
    redirectCount++;
  }

  if (httpCode == HTTP_CODE_OK) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("HTTP response received. Processing...\n");

    // Find photo IDs directly from the stream
    int photoCount = 0;
    int wait_count = 0;
    bool foundStartingPoint = false;
    WiFiClient* stream = https.getStreamPtr();
    stream->setTimeout(1000);

    while ((photoCount < PHOTO_LIMIT) && (wait_count < HTTP_WAIT_COUNT)) {
      if (!stream->available()) {
        Serial.printf(".");
        ++wait_count;
        delay(200);
      } else {
        if (!foundStartingPoint) {
          Serial.printf("Finding seek pattern: %s\n", SEEK_PATTERN);
          if (stream->find(SEEK_PATTERN)) {
            Serial.printf("Found seek pattern: %s\n", SEEK_PATTERN);
            foundStartingPoint = true;
          }
        } else {
          if (stream->find(SEARCH_PATTERN)) {
            int i = -1;
            char c = stream->read();
            char photoId[PHOTO_ID_SIZE];
            while (c != '\"' && i < PHOTO_ID_SIZE - 1) {  // Leave room for null terminator
              photoId[++i] = c;
              c = stream->read();
            }
            photoId[++i] = 0;  // null terminate

            // Check that all characters in this array are printable characters
            bool isPrintable = true;
            for (int j = 0; j < PHOTO_ID_SIZE-1 && photoId[j] != '\0'; j++) {
              if (photoId[j] < 32 || photoId[j] > 126) {
                Serial.printf("Non-printable character %d at index %d found in photo ID, skipping\n", photoId[j], j);
                isPrintable = false;
                break;
              }
            }

            // Check if this photo ID is already in the array
            bool isDuplicate = false;
            for (int j = 0; j < photoCount; j++) {
              if (strcmp(photoId, photoUrlList[j]) == 0) {
                Serial.printf("Duplicate photo ID found, skipping\n");
                isDuplicate = true;
                break;
              }
            }

            if (!isDuplicate && isPrintable) {
              // Copy the photo ID to the list
              strcpy(photoUrlList[photoCount], photoId);
              Serial.printf("Found photo id: %s\n", photoId);
              ++photoCount;
            }
          }
        }
      }

      // Allow other tasks to run
      delay(1);
    }

    Serial.printf("%d photo IDs added.\n", photoCount);
  } else {
    Serial.printf("HTTP GET failed with error code: %d\n", httpCode);
  }
  https.end();
}

// Function to download a photo from a URL
bool downloadPhoto(const char* photoId, size_t* outSize) {
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  HTTPClient https;

  // Create the full photo URL
  String photoUrl = "https://lh3.googleusercontent.com/" + String(photoId) + "=w" + String(DISPLAY_WIDTH) + "-h" + String(DISPLAY_HEIGHT) + "-c";

  Serial.printf("Downloading photo from: %s\n", photoUrl.c_str());

  if (!https.begin(client, photoUrl.c_str())) {
    Serial.printf("HTTPS connection failed!\n");
    return false;
  }

  https.setTimeout(HTTP_TIMEOUT);
  Serial.printf("Sending GET request...\n");
  int httpCode = https.GET();

  Serial.printf("HTTP response code: %d\n", httpCode);

  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("HTTP GET failed, error: %s\n", https.errorToString(httpCode).c_str());
    https.end();
    return false;
  }

  // Get the content length
  int contentLength = https.getSize();
  Serial.printf("Content length: %d bytes\n", contentLength);

  if (contentLength <= 0) {
    Serial.printf("Unknown content size: %d\n", contentLength);
    https.end();
    return false;
  }

  if (contentLength > MAX_PHOTO_SIZE) {
    Serial.printf("Content too large: %d bytes (max: %d)\n", contentLength, MAX_PHOTO_SIZE);
    https.end();
    return false;
  }

  // Get the TCP stream
  WiFiClient* stream = https.getStreamPtr();

  // Read the photo data
  size_t bytesRead = 0;
  while (bytesRead < contentLength) {
    size_t available = stream->available();
    if (available) {
      size_t toRead = min(available, (size_t)(contentLength - bytesRead));
      size_t read = stream->readBytes(photoBufferJpeg + bytesRead, toRead);
      bytesRead += read;

      // Serial.printf("Read %d/%d bytes (%.1f%%)\n",
      //               bytesRead, contentLength,
      //               (float)bytesRead / contentLength * 100);
    }
    delay(1);  // Allow other tasks to run
  }

  Serial.printf("Photo download complete: %d bytes\n", bytesRead);
  https.end();

  // Return the photo size
  if (outSize) {
    *outSize = bytesRead;
  }

  return true;
}

// colors
/*            {"name": "black", "hex": "#000000", "rgb": (0, 0, 0)},
            {"name": "white", "hex": "#FFFFFF", "rgb": (255, 255, 255)},
            {"name": "green", "hex": "#008000", "rgb": (0, 255, 0)},
            {"name": "blue", "hex": "#0000FF", "rgb": (0, 0, 255)},
            {"name": "red", "hex": "#FF0000", "rgb": (255, 0, 0)},
            {"name": "yellow", "hex": "#FFFF00", "rgb": (255, 255, 0)},
            {"name": "orange", "hex": "#FFA500", "rgb": (255, 165, 0)}
*/

// From https://github.com/cnlohr/epaper_projects/blob/master/atmega168pb_waveshare_color/tools/converter/converter.c
// uint8_t palette[7][3] = {
// 	{46, 44, 58},   //black
// 	{255, 255, 255},  //white
// 	{97, 116, 67},   //green
// 	{74, 74, 98},   //blue
// 	{184, 74, 59},   //red
// 	{195, 175, 54},  //yellow
// 	{159, 74, 45},   //orange
// };
uint8_t palette[7][3] = {
	{0,0,0},   //black
	{255,255,255},  //white
	{115, 140, 74},   //green
	{87, 86, 119},   //blue
	{221, 78, 56},   //red
	{230, 204, 0},  //yellow
	{195, 83, 37},   //orange
};


static void getRgbErr(int&rErr, int&gErr, int&bErr, uint8_t stdCol[3]) {
  rErr -= (stdCol[0]);  // Red component
  gErr -= (stdCol[1]);  // Green component
  bErr -= (stdCol[2]);  // Blue component
}

// Return the square error of {r, g, b},
// that means how far them are from standard color stdCol
static int getSquareErr(int r, int g, int b, uint8_t stdCol[3]) {
  getRgbErr(r, g, b, stdCol);
  return r * r + g * g + b * b;
}


// Return the index of current palette color which is
// nearest to the {r, g, b}
static int getNear(int r, int g, int b) {
  int ind = 0;
  int minErr = INT_MAX;

  for (int i = 0; i < 7; ++i) {
    double cur = getSquareErr(r, g, b, palette[i]);
    if (cur < minErr) {
      minErr = cur;
      ind = i;
    }
  }

  return ind;
}

uint32_t applyError(uint32_t pixel, int rErr, int gErr, int bErr, int distribution, int divisor) {
  
  int b = (pixel >> 16) & 0xFF;
  int g = (pixel >> 8) & 0xFF;
  int r = (pixel >> 0) & 0xFF;

  double distribution_factor = (double)distribution / divisor;

  r += rErr * distribution_factor;
  g += gErr * distribution_factor;
  b += bErr * distribution_factor;

  if(r < 0) r = 0;
  else if(r > 255) r = 255;

  if(g < 0) g = 0;
  else if(g > 255) g = 255;

  if(b < 0) b = 0;
  else if(b > 255) b = 255;

  return (b&0xFF) << 16 | (g&0xFF) << 8 | (r&0xFF);
}

void applyPixelError(int x, int y, int rErr, int gErr, int bErr, int distribution, int divisor) {
  if(x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) {
    return;
  }
  photoBufferRaw[y * DISPLAY_WIDTH + x] = applyError(photoBufferRaw[y * DISPLAY_WIDTH + x], rErr, gErr, bErr, distribution, divisor);
}

void ditherPhotoFloydSteinberg() {
  for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
    for (int x = 0; x < DISPLAY_WIDTH; ++x) {
      uint8_t b = (photoBufferRaw[y * DISPLAY_WIDTH + x] >> 16) & 0xFF;
      uint8_t g = (photoBufferRaw[y * DISPLAY_WIDTH + x] >> 8) & 0xFF;
      uint8_t r = (photoBufferRaw[y * DISPLAY_WIDTH + x] >> 0) & 0xFF;
      int near = getNear(r, g, b);

      if (x == 0 && y == 0) Serial.printf("r: %d, g: %d, b: %d, near: %d\n", r, g, b, near);

#if USE_VERTICAL
      if (y % 2 == 0) {
        photoBufferEink[(DISPLAY_WIDTH-x) * EPD_7IN3F_WIDTH/2 + y/2] |= (uint8_t)((near & 0xF) << 4);
      } else {
        photoBufferEink[(DISPLAY_WIDTH-x) * EPD_7IN3F_WIDTH/2 + y/2] |= (uint8_t)(near & 0xF);
      }
#else
      if (x % 2 == 0) {
        photoBufferEink[y * DISPLAY_WIDTH/2 + x/2] |= (uint8_t)((near & 0xF) << 4);
      } else {
        photoBufferEink[y * DISPLAY_WIDTH/2 + x/2] |= (uint8_t)(near & 0xF);
      }
#endif

      int rErr = r, gErr = g, bErr = b;
      getRgbErr(rErr, gErr, bErr, palette[near]);


      // apply error to pixels
      applyPixelError(x+1, y,   rErr, gErr, bErr, 7, 16);
      applyPixelError(x-1, y+1, rErr, gErr, bErr, 3, 16);
      applyPixelError(x  , y+1, rErr, gErr, bErr, 5, 16);
      applyPixelError(x+1, y+1, rErr, gErr, bErr, 1, 16);

    }
  }
}

void ditherPhotoSierra() {
  for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
    for (int x = 0; x < DISPLAY_WIDTH; ++x) {
      uint8_t b = (photoBufferRaw[y * DISPLAY_WIDTH + x] >> 16) & 0xFF;
      uint8_t g = (photoBufferRaw[y * DISPLAY_WIDTH + x] >> 8) & 0xFF;
      uint8_t r = (photoBufferRaw[y * DISPLAY_WIDTH + x] >> 0) & 0xFF;
      int near = getNear(r, g, b);


      if (x % 2 == 0) {
        photoBufferEink[y * DISPLAY_WIDTH/2 + x/2] = (uint8_t)((near & 0xF) << 4);
      } else {
        photoBufferEink[y * DISPLAY_WIDTH/2 + x/2] |= (uint8_t)(near & 0xF);
      }

      int rErr = r, gErr = g, bErr = b;
      getRgbErr(rErr, gErr, bErr, palette[near]);


      // apply error to pixels
      applyPixelError(x+1, y,   rErr, gErr, bErr, 5, 32);
      applyPixelError(x+2, y,   rErr, gErr, bErr, 3, 32);
      applyPixelError(x-2, y+1, rErr, gErr, bErr, 2, 32);
      applyPixelError(x-1, y+1, rErr, gErr, bErr, 4, 32);
      applyPixelError(x  , y+1, rErr, gErr, bErr, 5, 32);
      applyPixelError(x+1, y+1, rErr, gErr, bErr, 4, 32);
      applyPixelError(x+2, y+1, rErr, gErr, bErr, 2, 32);
      applyPixelError(x-1, y+2, rErr, gErr, bErr, 2, 32);
      applyPixelError(x  , y+2, rErr, gErr, bErr, 3, 32);
      applyPixelError(x+1, y+2, rErr, gErr, bErr, 2, 32);
    }
  }
}

int jpegRead(JPEGDRAW* pDraw) {
  // Serial.printf("Reading photo x: %d, y: %d, width: %d, height: %d, bpp: %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->iBpp);
  uint32_t* pPixels = reinterpret_cast<uint32_t*>(pDraw->pPixels);
  for (int i = 0; i < pDraw->iHeight; ++i) {
    for (int j = 0; j < pDraw->iWidth; ++j) {
      photoBufferRaw[(pDraw->y + i) * DISPLAY_WIDTH + (pDraw->x + j)] = pPixels[i * pDraw->iWidth + j];
    }
  }
  return 1;
}

void extractJpegPhoto(size_t photoSize) {
  if (jpeg.openRAM(photoBufferJpeg, photoSize, jpegRead)) {
    jpeg.setPixelType(RGB8888);
    Serial.printf("Decoding photo\n");
    jpeg.decode(0, 0, 0);
    Serial.printf("Decoded photo\n");
    jpeg.close();
  }
}

void cleanup() {
  if (photoBufferJpeg) {
    free(photoBufferJpeg);
    photoBufferJpeg = NULL;
  }
  if (photoBufferEink) {
    free(photoBufferEink);
    photoBufferEink = NULL;
  }
}