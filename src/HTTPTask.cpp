#include "HTTPTask.h"
#include <Update.h>

void HTTPTask::cleanup(){
	server->end();
	SPIFFS.end();
	delete server;
};
void HTTPTask::setup(){
SPIFFS.begin();
server = new AsyncWebServer(80);
if (!server){
    Serial.println("Error creating HTTP server");
    return;
}
server->on("/", std::bind(&HTTPTask::handleRoot, this, std::placeholders::_1));
server->on("/upd", std::bind(&HTTPTask::handleUpd, this, std::placeholders::_1));
server->on("/log", std::bind(&HTTPTask::handleUpd, this, std::placeholders::_1));

//server->on("/main", std::bind(&HttpHelper::handleMainFile, this, std::placeholders::_1));
server->onNotFound(std::bind(&HTTPTask::handleNotFound, this, std::placeholders::_1));
//server->on("/css/bootstrap.min.css", std::bind(&HTTPTask::handleBootstrapCss, this, std::placeholders::_1));
//server->on("/css/font-awesome.min.css", std::bind(&HTTPTask::handleFontAwesomeCss, this, std::placeholders::_1));
//server->on("/css/radio.css", std::bind(&HTTPTask::handleRadioCss, this, std::placeholders::_1));
//server->on("/fonts/fontawesome-webfont.woff2", std::bind(&HTTPTask::handleFontAwesomeFontsWoff, this, std::placeholders::_1));
//server->on("/js/bootstrap.min.js", std::bind(&HTTPTask::handleBootstrapJs, this, std::placeholders::_1));
//server->on("/js/jquery.min.js", std::bind(&HTTPTask::handleJqueryJs, this, std::placeholders::_1));
server->serveStatic("/css/bootstrap.min.css",SPIFFS,"/css/bootstrap.min.css");
server->serveStatic("/js/jquery.min.js",SPIFFS,"/js/jquery.min.js");
server->serveStatic("/js/bootstrap.min.js",SPIFFS,"/js/bootstrap.min.js");
server->serveStatic("/css/font-awesome.min.css",SPIFFS,"/css/font-awesome.min.css");
server->serveStatic("/css/radio.css",SPIFFS,"/css/radio.css");
server->serveStatic("/fonts/fontawesome-webfont.woff2",SPIFFS,"/fonts/fontawesome-webfont.woff2");
	
server->on(
	"/update", 
	HTTP_POST, 
	[](AsyncWebServerRequest *request){
	  request->redirect("/");
    }, 
	std::bind(&HTTPTask::handleUpdate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)
	);

server->on(
	"/spiffs", 
	HTTP_POST, 
	[](AsyncWebServerRequest *request){
	    request->send(200);
    }, std::bind(&HTTPTask::handleSpiffs, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)
	);



server->begin();

};


void HTTPTask::loop(){
   delay(10); 
   
}
    

void HTTPTask::handleRoot(AsyncWebServerRequest * request) {
	
	if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/index.htm","text/html", request);
}


void HTTPTask::handleLog(AsyncWebServerRequest * request) {
	
	if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/log.htm","text/html", request);
}

void HTTPTask::handleFile(String path,String type, AsyncWebServerRequest *request){
	request->send(SPIFFS,path,type);
}

void HTTPTask::handleNotFound(AsyncWebServerRequest * request) {
	request->send(200, "text/plain", "404 PAGE NOT FOUND!!!");
}

// void HTTPTask::handleBootstrapCss(AsyncWebServerRequest * request) {
// 		handleFile("/css/bootstrap.min.css","text/css",request);
// }

// void HTTPTask::handleRadioCss(AsyncWebServerRequest * request) {
// 		handleFile("/css/radio.css","text/css",request);
// }
// void HTTPTask::handleFontAwesomeCss(AsyncWebServerRequest * request) {
// 		handleFile("/css/font-awesome.min.css","text/css",request);
// }
// void  HTTPTask::handleFontAwesomeFontsWoff(AsyncWebServerRequest * request){
// 	handleFile("/fonts/fontawesome-webfont.woff","font/woff",request);
// }

// void HTTPTask::handleJqueryJs(AsyncWebServerRequest * request) {
//     		handleFile("/js/jquery.min.js","application/javascript",request);
// }

// void HTTPTask::handleBootstrapJs(AsyncWebServerRequest * request) {
// 		handleFile("/js/bootstrap.min.js","application/javascript",request);
// }

void HTTPTask::handleUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
 uint32_t free_space = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
 uint32_t result;
  if (!index){
	request->redirect("/");
    if (!Update.begin(free_space,U_FLASH)) {
    }
	else{
		result=800;
		xQueueSend(que,&result,portMAX_DELAY);
	}
  }

  if (Update.write(data, len) != len) {
  }else{
	  
  }

  if (final) {
    if (!Update.end(true)){
	  
    } else {
		ESP.restart();
    }
  }
}

void HTTPTask::handleSpiffs(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
  uint32_t result;
  if (!index){
	request->redirect("/");
    if (!Update.begin(UPDATE_SIZE_UNKNOWN,U_SPIFFS)) {
    }
	else{
		result=800;
		xQueueSend(que,&result,portMAX_DELAY);
	}
  }
  if (Update.write(data, len) != len) {
  }else{
	//  if (counter++==9) {Serial.print(".");counter=0;}
  }
  if (final) {
    if (!Update.end(true)){
    } else {
	  ESP.restart();
    }
  }
}


void HTTPTask::handleUpd(AsyncWebServerRequest * request) {
    if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/upd.htm","text/html",request);
	
}



