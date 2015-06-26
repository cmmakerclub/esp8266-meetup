#include "WiFiHelper.h"
#include <HttpClient.h>

typedef enum {
    eIdle,
    eRequestStarted,
    eRequestSent,
    eReadingStatusCode,
    eStatusCodeRead,
    eReadingContentLength,
    eSkipToEndOfHeader,
    eLineStartingCRFound,
    eReadingBody
} tHttpState;

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

WiFiHelper::WiFiHelper(const char* ssid, const char* password)
{
    init_config(ssid, password);

}

void WiFiHelper::init_config(const char* ssid, const char* password)
{
    prev_millis = millis();

    _ssid = String(ssid);
    _password = String(password);

}


WiFiHelper::~WiFiHelper()
{

}

String WiFiHelper::get( const char *_host, const char *_path )
{
  int err = 0;
  int response_code = 0;  

  DEBUG_PRINT( "Connecting to " );
  DEBUG_PRINTLN( _host );

  WiFiClient client;
  DEBUG_PRINT( "Requesting URL: " );
  DEBUG_PRINT( _host );
  DEBUG_PRINTLN( _path ); 

  String response = "";

  HttpClient http(client);
  err = http.get(_host, _path);
  if (err == 0)
  {
      DEBUG_PRINT("Got status code: ");
      DEBUG_PRINTLN(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get
      response_code = http.responseStatusCode();
      
      DEBUG_PRINT("Got response code: ");
      DEBUG_PRINTLN(response_code);
      
      err = http.skipResponseHeaders();
      if (err >= 0)
      {   
        int bodyLen = http.contentLength();
        DEBUG_PRINT("Content length is: ");
        DEBUG_PRINTLN(bodyLen);
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        int iChunkState = eReadingContentLength;
        int iChunkLength = 0;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                if( http.isChunk ) {
                    switch(iChunkState)
                    {
                    case eReadingContentLength:
                        if (isdigit(c))
                        {
                            iChunkLength = iChunkLength*16 + (c - '0');
                        }
                        else if( c >= 'a' && c <= 'f' )
                        {
                            iChunkLength = iChunkLength*16 + (c - 'a' + 10);
                        }
                        else if( c >= 'A' && c <= 'F' )
                        {
                            iChunkLength = iChunkLength*16 + (c - 'A' + 10);
                        }
                        else if( c == '\r' ) {
                        }
                        else
                        {
                            DEBUG_PRINT( "Chunk length: " );
                            DEBUG_PRINTLN( iChunkLength );
                            iChunkState = eReadingBody;
                        }
                        break;
                    case eReadingBody:
                        if( iChunkLength > 0 ) {
                            response = response + c;
                            iChunkLength--;
                            if( iChunkLength == 0 )
                                iChunkState = eReadingContentLength;
                        }
                        break;
                    }
                    
                }
                // Print out this character
                DEBUG_PRINT(c);
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }        
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  DEBUG_PRINT( "\r\nResponse:\r\n" );
  DEBUG_PRINT( response );
  DEBUG_PRINTLN( "---\r\n" );

  return response;
}
