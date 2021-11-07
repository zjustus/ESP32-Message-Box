#include <Arduino.h>
//#include <esp_spiffs.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <LinkedList.h>
#include "screen.h"

/*
NOTES:

  #Libraries
  [Adfruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
  [Adafruit_ILI9341](https://github.com/adafruit/Adafruit_ILI9341)
  [Better Screen driver](https://github.com/Bodmer/TFT_eSPI/)
  [XPT2046_Touchscreen](https://github.com/PaulStoffregen/XPT2046_Touchscreen)
  [Wifi.h](https://www.arduino.cc/en/Reference/WiFi)
  [PubSubClient.h](https://pubsubclient.knolleary.net/api#getBufferSize)
  [ESPAsyncWebServer.h](https://github.com/me-no-dev/ESPAsyncWebServer)

  #Articles
  [PlatformIO config](https://docs.platformio.org/en/latest/platforms/espressif32.html)
  
  ##File systems
  [Using spiffs](https://visualgdb.com/tutorials/esp32/spiffs/)
  [Upload to file system](https://diyprojects.io/esp8266-upload-data-folder-spiffs-littlefs-platformio/#.X8kYGbmSlhH)

  ##Wifi
  [connect to wifi](https://www.megunolink.com/articles/wireless/how-do-i-connect-to-a-wireless-network-with-the-esp32/)
  [make a wifi network](https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/#:~:text=The%20ESP32%20that%20creates%20its,Point%20(soft%2DAP).)
  
  ##Web Server
  [Make an async spiffs webserver](https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/)
  #Touch Notes
  DO - MISO
  DIN - MOSI
  IRQ - this will be used to wake up the display in the future

  # for emergencies | wipe the flash
  1. cd C:\Users\zachj\.platformio\packages\framework-arduinoespressif32\tools
  1. ./esptool.py --chip esp32 p com7 erase_flash
*/

/*
MAP:
- setup
  - Init storage
  - Init Touch
  - Init Screen
  - Init Wifi
  - Connect to MQTT?
    - subscribe - justus/loveBox/in
    - publish - justus/loveBox/out

*/

//screen class
Screen display;

//Storage config
// esp_vfs_spiffs_conf_t storage{
// 	.base_path = "/storage",
// 	.partition_label = "spiffs",
// 	.max_files = 3,
// 	.format_if_mount_failed = true,
// };

//wifi functions
void wifi_setup();
bool wifi_setup_noFile();
String SSID;
String wifiPassword;
bool internetConnection;

//Website config
AsyncWebServer server(80);
void serverSetup();
String htmlProcessor(const String &x); //might be old

//MQTT Vars
WiFiClient espClient;						   //the wifi
PubSubClient MQTTclient;					   //the mqtt client
const char *mqtt_server = "broker.hivemq.com"; //the mqtt server
const int mqtt_port = 1883;					   //the tcp port
String baseTopic = "justus/loveBox/";		   //the base nameing convention
String boxID = "2";							   //the box id ** This needs to be dynamic later **
String pairID = "";							   //this is the boxes matching pair ** This needs to be dynamic later **

String mqtt_publish = baseTopic + pairID + "/"; //the topic to push a message to
//mqtt_publish = mqtt_publish+"/";
String mqtt_subscribe = baseTopic + boxID + "/+/+"; //the toppic to recieve a message on

//MQTT Functions
void MQTTinit();												//this is called to init the whole jam
void MQTTreconnect();											//this is called to connect and subsribe to messages
void callback(char *topic, byte *payload, unsigned int length); //this is called every time a message is revieved

//debug functions
void myPrint(const char *x)
{
	Serial.println(x);
}

void setup()
{

	//setup the serial for debug
	Serial.begin(9600);
	myPrint("It Begins");

	//mount the file storage
	//esp_vfs_spiffs_register(&storage);
	//debug step, check if spiffs is mounted
	// if (esp_spiffs_mounted("spiffs"))
	// {
	// 	Serial.println("Files Mounted");
	// 	size_t size;
	// 	size_t used;
	// 	esp_spiffs_info("spiffs", &size, &used);
	// 	Serial.printf("Total Size: %i \n", size);
	// 	Serial.printf("Total Used: %i \n", used);
	// }

	if (!SPIFFS.begin(true))
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
	}
	else
	{
		Serial.println("Files Mounted");
		//esp_spiffs_info("spiffs", &size, &used);
		Serial.printf("Total Size: %i \n", SPIFFS.totalBytes());
		Serial.printf("Total Used: %i \n", SPIFFS.usedBytes());
	}

	//load the config settings
	File configFile = SPIFFS.open("/config.ini");

	//this chunk parses the config file
	if (configFile != false)
	{
		JustusDevTools::LinkedList<char> configList; //the File contents
		String varNameBuffer;						 //the var name read from file
		String varValueBuffer;						 //the var value read from file
		bool isColin = false;						 //is the line pre or post colin?

		//populates the config list with file contents
		Serial.println();
		Serial.println();
		while (configFile.available())
		{
			configList.add(configFile.read());
		}
		configFile.close();
		configList.goto_first(); //goes to the start of the list

		//parse the config file
		/* DEBUG Observations
			Im getting all the way till the end before if crashes ... Null pointer?
			its not catching EOL's or ':'? Why not? it can print them?
			THOUGHT!!! is it because im moving to the next char before finishing my compair? resulting in false negative and moving past EOL?

		*/
		for (int i = 0; i < configList.length(); i++)
		{	
			//Serial.print(configList.get_data());

			//if its not EOL or ':'
			if (configList.get_data() != '\n' && configList.get_data() != ':')
			{
				if (!isColin) varNameBuffer += configList.get_data(); //save to the name buffer if its the first half
				else varValueBuffer += configList.get_data(); //save to the var buffer if its the 2nd half
			}
			//if it is a ':' make isColine true
			else if (configList.get_data() == ':') {
				isColin = true; 
			}
			
			else //if it is EOL
			{
				if (varNameBuffer == "SSID") //populate the SSID
				{
					SSID = varValueBuffer;
					Serial.println(SSID);
				}
				else if (varNameBuffer == "password") //populate the password
				{
					wifiPassword = varValueBuffer;
					Serial.println(wifiPassword);
				}
				else if (varNameBuffer == "pairid") //populate the pair id
				{
					pairID = varValueBuffer;
					mqtt_publish = baseTopic + pairID + "/";
					Serial.println(pairID);
				}

				//clear buffs for next line
				varNameBuffer = "";
				varValueBuffer = "";
				isColin = false;
			}
			configList.move_forward();
		}
		
	}
	else
	{	/// if the file dosent exist
		//it might be smart to make the file here
	}

	//init the screen
	display.init(MQTTclient, baseTopic + pairID + "/");

	/** 
   * later make this return true or false for the mode
   * if returns true connect to MQTT, else skip to setup mode 
  **/
	internetConnection = wifi_setup_noFile();

	//connect to MQTT if wifi is connected
	if (internetConnection)
	{
		MQTTinit();
		if (!MQTTclient.connected())
		{ //make sure MQTT is connected
			MQTTreconnect();
		}
	}
	else
	{
		//start the web server here if it returns false
		serverSetup();
	}
}

/* Loop only vars here */
//int screenFlop = 0;

void loop(void)
{
	display.ScreenProgram();
	//serverLoop();
	if (internetConnection)
	{
		if (!MQTTclient.connected())
		{ //make sure MQTT is connected
			MQTTreconnect();
		}
		MQTTclient.loop();
	}
}

/** WIFI FUNCTIONS **/
void wifi_setup()
{
	//temp wifi settings, save to file / flash later
	const char *ssid = "Justus 5n";
	const char *password = "waterski";
	bool wifiConnect = 1;

	FILE *wifiCredentials;
	wifiCredentials = fopen("/wifi-credential.cfg", "r");

	//if the file credentials exist
	if (wifiCredentials != NULL)
	{
		myPrint("My file exists / reading file");

		char *wifiString;
		int ch;
		size_t length = 0;
		wifiString = (char *)realloc(NULL, sizeof(char *));

		while ((ch = fgetc(wifiCredentials)) != EOF)
		{
			if (length != 0)
				wifiString = (char *)realloc(wifiString, sizeof(char *) * (length + 1));
			wifiString[length++] = ch;
		}

		wifiString[length] = '\0';

		fclose(wifiCredentials);
		myPrint("Wifi File Read");

		myPrint(wifiString);
	}
	else
	{
		//wifi dosent exist
		myPrint("File not found");
		/** 
     * potentially make a file here and and save nothing to it to nulify
     * set wifiConnect to 0
    **/

		/* //to make the wifi file
		wifiCredentials = fopen("/storage/wifi-credential.cfg", "w");
		fputs("Hello World\n", wifiCredentials);
		myPrint("Wifi File created");
		fclose(wifiCredentials);
	*/
	}

	// //if wifi exists and can connect
	if (ssid != NULL && wifiConnect)
	{
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid, password);

		uint8_t timer = 0;
		Serial.printf("Connecting to Wifi: %s \n", ssid);

		//wait about 32 seconds for wifi
		while (WiFi.status() != WL_CONNECTED)
		{
			Serial.print(".");
			if (timer % 16 == 0 && timer > 15)
				Serial.printf("\n **Still trying to connect** , Timer: %i\n", timer);
			if (timer >= 64)
				break;
			timer++;
			delay(500);
		}
		//if no wifi is found, break and restart into host mode
		if (WiFi.status() != WL_CONNECTED)
		{
			wifiConnect = 0;
			//ESP.restart();
		}

		Serial.print("Connected, IP address:");
		Serial.print(WiFi.localIP());
		Serial.println();
	}
	//if wifi isint configured or connected
	if (!wifiConnect)
	{
		WiFi.mode(WIFI_MODE_AP);
		IPAddress local_ip(192, 198, 1, 1);
		IPAddress gateway(192, 198, 1, 1);
		IPAddress subnet(255, 255, 255, 0);

		WiFi.softAP("MessageBox", NULL); //sets the ssid and passowrd
		WiFi.softAPConfig(local_ip, gateway, subnet);
	}
}

bool wifi_setup_noFile()
{
	bool wifiConnect = 1;

	//if wifi exists and can connect
	if (SSID != NULL && wifiConnect)
	{
		WiFi.mode(WIFI_STA);
		WiFi.begin(SSID.c_str(), wifiPassword.c_str());

		uint8_t timer = 0;
		Serial.printf("Connecting to Wifi: %s \n", SSID.c_str());

		//wait about 32 seconds for wifi
		while (WiFi.status() != WL_CONNECTED)
		{
			Serial.print(".");
			if (timer % 16 == 0 && timer > 15)
				Serial.printf("\n **Still trying to connect** , Timer: %i\n", timer);
			if (timer >= 64)
				break;
			timer++;
			delay(500);
		}
		//if no wifi is found delete the config file and restart
		if (WiFi.status() != WL_CONNECTED)
		{
			//SPIFFS.remove("/config.ini");
			ESP.restart();
		}

		Serial.print("Connected, IP address:");
		Serial.print(WiFi.localIP());
		Serial.println();
		return true;
	}
	//if wifi isint configured or connected
	else
	{
		WiFi.mode(WIFI_MODE_AP);
		WiFi.softAP("MessageBox", NULL); //sets the ssid and passowrd
		IPAddress ipAddress = WiFi.softAPIP();
		Serial.println(ipAddress);
		//WiFi.softAPConfig(local_ip, gateway, subnet);
		return false;
	}
}

/** MQTT FUNCTIONS **/
//MQTT Init
void MQTTinit()
{
	MQTTclient.setClient(espClient);

	//tries to connect to the server
	/*
    bool sucess = false;
    while(!sucess){

        sucess = espClient.connect(mqtt_server, mqtt_port);
        if(sucess) Serial.print("Connected to Server\n");
        else{
            Serial.print("Server Connection Fail, retrying in 2 seconds\n");

            delay(2000);
        }
    }
    */

	MQTTclient.setServer(mqtt_server, mqtt_port);
	MQTTclient.setCallback(callback);
}

//MQTT Reconnect
void MQTTreconnect()
{
	// Loop until we're reconnected
	while (!MQTTclient.connected())
	{
		Serial.print("Attempting MQTT connection...");

		// Create a random client ID
		String clientId = "Justus_LoveBox-";
		clientId += String(random(0xffff), HEX);

		// Attempt to connect
		if (MQTTclient.connect(clientId.c_str()))
		{
			Serial.println("connected");

			// Once connected, publish an announcement...
			Serial.println("wifi: \t" + WiFi.localIP().toString());
			mqtt_publish = baseTopic + pairID + "/";
			MQTTclient.publish(mqtt_publish.c_str(), const_cast<char *>(WiFi.localIP().toString().c_str()));

			MQTTclient.subscribe(mqtt_subscribe.c_str());

			Serial.println("All Subscribed");
		}
		else
		{
			Serial.print("failed, rc=");
			Serial.print(MQTTclient.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

//MQTT if a message comes in on a subscribed toppic
/**
 * NOTE: Start here!!! 
 * make 2nd node test that message is being recieved with this code
 * break down topic into something that can be used
 * call the print screen from web function
 * 
**/
void callback(char *topic, byte *payload, unsigned int length)
{
	String message = "";
	String topic_str = "";

	//add the payload and topic to two seprate strings message and topic_str
	for (size_t i = 0; i < length; i++)
		message += (char)payload[i];
	for (size_t i = 0; i < strlen(topic); i++)
		topic_str += topic[i];

	//print message arived on x topic
	// Serial.print("Message arrived [");
	// Serial.print(topic);
	// Serial.print("] ");

	//print the message to terminal
	//Serial.println(message.c_str());

	// "justus/loveBox/boxID/{line}(i)/{part}(j)"

	//get the line number and part from the topic
	size_t delimit_count = 0;
	String lineNum_str = "";
	String partNum_str = "";
	size_t lineNum;
	size_t partNum;
	for (size_t i = 0; i < topic_str.length(); i++)
	{
		if (delimit_count == 3 && topic_str[i] != '/')
			lineNum_str += topic_str[i];
		else if (delimit_count == 4 && topic_str[i] != '/')
			partNum_str += topic_str[i];

		if (topic_str[i] == '/')
			delimit_count++;
	}
	sscanf(lineNum_str.c_str(), "%d", &lineNum);
	sscanf(partNum_str.c_str(), "%d", &partNum);

	if (lineNum > 30 && lineNum < 320 && partNum >= 0 && partNum < 4)
		display.printScreenFromInternet(message.c_str(), lineNum, partNum);
	else if (lineNum == 0 && partNum == 0)
		display.drawPaint();
}

void serverSetup()
{
	//serve the main files
	// server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
	// 	request->send(SPIFFS, "/index.html", String(), false, htmlProcessor);
	// 	Serial.println("Now Serving Tea");
	// });
	server.on("/", [](AsyncWebServerRequest *request) {
		String saveString = "";

		if (request->hasParam("ssid", true))
		{

			AsyncWebParameter p = *request->getParam("ssid", true);
			saveString += "SSID:";
			saveString += p.value();
			saveString += "\n";
		}
		if (request->hasParam("password", true))
		{
			AsyncWebParameter p = *request->getParam("password", true);
			saveString += "password:";
			saveString += p.value();
			saveString += "\n";
		}
		if (request->hasParam("pairid", true))
		{
			AsyncWebParameter p = *request->getParam("pairid", true);
			saveString += "pairid:";
			saveString += p.value();
			saveString += "\n";
		}

		//Serial.print(saveString);

		if (saveString != "")
		{
			File configFile = SPIFFS.open("/config.ini", "w");
			if (configFile != false)
			{
				configFile.print(saveString.c_str());
				configFile.close();
				Serial.println("Ive Been posted");
			}
			else Serial.println("Dosent Work");
		}
		request->send(SPIFFS, "/index.html", String(), false, htmlProcessor);
	});

	server.begin();
}

String htmlProcessor(const String &x)
{
	if (x == "boxID")
	{
		return boxID;
	}
	else
		return "UN-Defined";
}