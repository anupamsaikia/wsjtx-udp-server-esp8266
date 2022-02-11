#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <secrets.h>

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

WiFiUDP Udp;
unsigned int localUdpPort = 2237;     // local port to listen on
uint8_t WSJTX_incomingByteArray[255]; // buffer for incoming packets
size_t WSJTX_currentIndex = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

// WSJTX helper functions
uint8 readuInt8()
{
  uint8 val;
  memcpy(&val, &WSJTX_incomingByteArray[WSJTX_currentIndex], sizeof(val));
  WSJTX_currentIndex += 1;
  return val;
}

uint32 readuInt32()
{
  uint32 bigEndianValue;
  memcpy(&bigEndianValue, &WSJTX_incomingByteArray[WSJTX_currentIndex], sizeof(bigEndianValue));
  WSJTX_currentIndex += 4;
  uint32 theUnpackedValue = ntohl(bigEndianValue);
  return theUnpackedValue;
}

int32 readInt32()
{
  int32 bigEndianValue;
  memcpy(&bigEndianValue, &WSJTX_incomingByteArray[WSJTX_currentIndex], sizeof(bigEndianValue));
  WSJTX_currentIndex += 4;
  int32 theUnpackedValue = ntohl(bigEndianValue);
  return theUnpackedValue;
}

uint64 readuInt64()
{
  uint64 bigEndianValue;
  memcpy(&bigEndianValue, &WSJTX_incomingByteArray[WSJTX_currentIndex], sizeof(bigEndianValue));
  WSJTX_currentIndex += 8;
  uint64 theUnpackedValue = __builtin_bswap64(bigEndianValue);
  return theUnpackedValue;
}

bool readBool()
{
  bool val;
  memcpy(&val, &WSJTX_incomingByteArray[WSJTX_currentIndex], sizeof(val));
  WSJTX_currentIndex += 1;
  return val;
}

void loop()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    unsigned long now = millis();

    // receive incoming UDP packets
    int len = Udp.read(WSJTX_incomingByteArray, 255);
    if (len > 0)
    {
      WSJTX_currentIndex = 8; // skip packet header

      // Packet Type
      uint32 WSJTX_packetType = readuInt32();
      if (WSJTX_packetType == 1)
      {
        //--------------------------------------------------------------------//
        // Client id
        int32 WSJTX_clientIdLength = readInt32();
        char WSJTX_clientId[WSJTX_clientIdLength + 1];
        for (int32 i = 0; i < WSJTX_clientIdLength; i++)
        {
          WSJTX_clientId[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_clientId[WSJTX_clientIdLength] = 0;

        //--------------------------------------------------------------------//
        // Dial Frequency
        uint64 WSJTX_dialFrequency = readuInt64();

        //--------------------------------------------------------------------//
        // Mode
        int32 WSJTX_modeLength = readInt32();
        char WSJTX_mode[WSJTX_modeLength + 1];
        for (int32 i = 0; i < WSJTX_modeLength; i++)
        {
          WSJTX_mode[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_mode[WSJTX_modeLength] = 0;

        //--------------------------------------------------------------------//
        // DX Call
        int32 WSJTX_dxCallLength = readInt32();
        char WSJTX_dxCall[WSJTX_dxCallLength + 1];
        for (int32 i = 0; i < WSJTX_dxCallLength; i++)
        {
          WSJTX_dxCall[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_dxCall[WSJTX_dxCallLength] = 0;

        //--------------------------------------------------------------------//
        // Report
        int32 WSJTX_reportLength = readInt32();
        char WSJTX_report[WSJTX_reportLength + 1];
        for (int32 i = 0; i < WSJTX_reportLength; i++)
        {
          WSJTX_report[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_report[WSJTX_reportLength] = 0;

        //--------------------------------------------------------------------//
        // Tx mode
        int32 WSJTX_txModeLength = readInt32();
        char WSJTX_txMode[WSJTX_txModeLength + 1];
        for (int32 i = 0; i < WSJTX_txModeLength; i++)
        {
          WSJTX_txMode[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_txMode[WSJTX_txModeLength] = 0;

        //--------------------------------------------------------------------//
        // Tx Enabled
        bool WSJTX_txEnabled = readBool();

        //--------------------------------------------------------------------//
        // Transmitting
        bool WSJTX_transmitting = readBool();

        //--------------------------------------------------------------------//
        // Decoding
        bool WSJTX_decoding = readBool();

        //--------------------------------------------------------------------//
        // Rx DF
        uint32 WSJTX_rxDrift = readuInt32();

        //--------------------------------------------------------------------//
        // Tx DF
        uint32 WSJTX_txDrift = readuInt32();

        //--------------------------------------------------------------------//
        // DE call
        int32 WSJTX_deCallLength = readInt32();
        char WSJTX_deCall[WSJTX_deCallLength + 1];
        for (int32 i = 0; i < WSJTX_deCallLength; i++)
        {
          WSJTX_deCall[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_deCall[WSJTX_deCallLength] = 0;

        //--------------------------------------------------------------------//
        // DE grid
        int32 WSJTX_deGridLength = readInt32();
        char WSJTX_deGrid[WSJTX_deGridLength + 1];
        for (int32 i = 0; i < WSJTX_deGridLength; i++)
        {
          WSJTX_deGrid[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_deGrid[WSJTX_deGridLength] = 0;

        //--------------------------------------------------------------------//
        // DX grid
        int32 WSJTX_dxGridLength = readInt32();
        char WSJTX_dxGrid[WSJTX_dxGridLength + 1];
        for (int32 i = 0; i < WSJTX_dxGridLength; i++)
        {
          WSJTX_dxGrid[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_dxGrid[WSJTX_dxGridLength] = 0;

        //--------------------------------------------------------------------//
        // Tx Watchdog
        bool WSJTX_txWatchdog = readBool();

        //--------------------------------------------------------------------//
        // Sub-mode
        int32 WSJTX_subModeLength = readInt32();
        char WSJTX_subMode[WSJTX_subModeLength + 1];
        for (int32 i = 0; i < WSJTX_subModeLength; i++)
        {
          WSJTX_subMode[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_subMode[WSJTX_subModeLength] = 0;

        //--------------------------------------------------------------------//
        // Fast mode
        bool WSJTX_fastMode = readBool();

        //--------------------------------------------------------------------//
        // Special Operation Mode
        uint8 WSJTX_specialOpMode = readuInt8();

        // Frequency Tolerance
        uint32 WSJTX_frequencyToletance = readuInt32();

        //--------------------------------------------------------------------//
        // T/R Period
        uint32 WSJTX_txrxPeriod = readuInt32();

        //--------------------------------------------------------------------//
        // Configuration Name
        int32 WSJTX_configNameLength = readInt32();
        char WSJTX_configName[WSJTX_configNameLength + 1];
        for (int32 i = 0; i < WSJTX_configNameLength; i++)
        {
          WSJTX_configName[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_configName[WSJTX_configNameLength] = 0;

        //--------------------------------------------------------------------//
        // Tx Message
        int32 WSJTX_txMessageLength = readInt32();
        char WSJTX_txMessage[WSJTX_txMessageLength + 1];
        for (int32 i = 0; i < WSJTX_txMessageLength; i++)
        {
          WSJTX_txMessage[i] = WSJTX_incomingByteArray[WSJTX_currentIndex];
          WSJTX_currentIndex += 1;
        }
        WSJTX_txMessage[WSJTX_txMessageLength] = 0;

        Serial.printf("txOn: %s, freq: %lld, msg: %s\n", WSJTX_transmitting ? "true" : "false", WSJTX_dialFrequency, WSJTX_txMessage);
        Serial.printf("Time taken : %ld\n", millis() - now);
      }
    }
  }
}
