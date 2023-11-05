#include "HTTP2Task.h"
#include <Wire.h>


void HTTP2Task::setup(){
HTTPTask::setup();

server->on("/post", HTTP_ANY, std::bind(&HTTP2Task::handleW2A, this, std::placeholders::_1));
server->on("/getdata", HTTP_ANY, std::bind(&HTTP2Task::handleA2W, this, std::placeholders::_1));
server->begin();
};




void HTTP2Task::loop(){
 HTTPTask::loop();
}
    



void HTTP2Task::handleW2A(AsyncWebServerRequest * request)
{
	
	uint8_t params = request->params();
	if (params<1 || !(request->getParam(0)->name()).equals(F("page"))){
			request->send(500, F("text/plain"),F("ERROR PAGE PARAMETR")); // Oтправляем ответ No Reset
			return;
	}

    if (request->getParam(0)->value().equals(F("main"))){  
    	for (uint8_t i = 1; i < params; i++)
    	{
        	var(request->getParam(i)->name(), request->getParam(i)->value());
    	}
	} else if (request->getParam(0)->value().equals(F("log"))){  
    	for (uint8_t i = 1; i < params; i++)
    	{
        	//var_log(request->getParam(i)->name(), request->getParam(i)->value());
    	}
	}
    request->send(200, F("text/plain"), F("OK"));
}

void HTTP2Task::var(String n, String v)
{
     event_t ev;
	 uint8_t h=9,m=50,d=3,nn=0;
     ev.state=WEB_EVENT;
  	if (n.equals("BTN1"))
	{
		ev.button=1;
		//Serial.println(v);
		h=v.substring(0,v.indexOf(':')).toInt();
		m=v.substring(v.indexOf(':')+1,v.indexOf('*')).toInt();
		d=v.substring(v.indexOf('*')+1,v.indexOf('-')).toInt();
		nn=v.substring(v.indexOf('-')+1).toInt();
		ev.alarm.hour=h;
		ev.alarm.minute=m;
		ev.alarm.period=(period_t)d;
		ev.alarm.action=nn;
	}
	else if (n.equals("BTN2"))
	{
		ev.button=2;
		//=makeAlarm(20,d,h,m); 
		//ev.count=v.equals(F("true"));
	}
	else if (n.equals("BTN3"))
	{
		ev.button=3;
		//=makeAlarm(20,d,h,m); 
		//ev.count=v.equals(F("true"));
	}
	else if (n.equals("BTN4"))
	{
		ev.button=4;
		//=makeAlarm(20,d,h,m); 
		//ev.count=v.equals(F("true"));
	}
	else if (n.equals("REL1"))
	{
		ev.button=11;
		ev.count=v.equals("true");
	}
	else if (n.equals("REL2"))
	{
		ev.button=12;
		ev.count=v.equals("true");
	}
	else if (n.equals("REL3"))
	{
		ev.button=13;
		ev.count=v.equals("true");
	}
	else if (n.equals("REL4"))
	{
		ev.button=14;
		ev.count=v.equals("true");
	}
	else if (n.equals("FUNC1"))
	{
		ev.state=PULT_BUTTON;
		ev.button=5;
		ev.count=IR_DEVICE;
		
	}
	else if (n.equals("LIGHT_R"))
	{
		ev.button=21;
		ev.count=v.toInt();
		
	}
	else if (n.equals("LIGHT_G"))
	{
		ev.button=22;
		ev.count=v.toInt();
	}
	else if (n.equals("LIGHT_B"))
	{
		ev.button=23;
		ev.count=v.toInt();
	}
	else if (n.equals("LEDMODE_R"))
	{
		ev.button=31;
		ev.count=v.toInt();
		
	}
	else if (n.equals("LEDMODE_G"))
	{
		ev.button=32;
		ev.count=v.toInt();
	}
	else if (n.equals("LEDMODE_B"))
	{
		ev.button=33;
		ev.count=v.toInt();
	}
	 xQueueSend(que,&ev,portMAX_DELAY);
}


String HTTP2Task::getI2Cdevices(){
    int error;
	String res="I2C device found at address<ul>";
	
	
	uint8_t count=0;
    for (uint8_t address = 1; address < 127; address++ )  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)    {
		res+="<li>";
		res+=String(address,16);
		res+="</li>";
		count++;
      
    }
    }
    res+="<li><b>Ttal devices ";
	res+=String(count);
	res+="</b></li></ul>";
	return res;
}


void HTTP2Task::handleA2W(AsyncWebServerRequest * request)
{
	if (request->params()<1 || !(request->getParam(0)->name()).equals("page")){
			request->send(500, F("text/plain"),F("ERROR PAGE PARAMETR")); // Oтправляем ответ No Reset
			return;
	}
	String str = F("{");
	SystemState_t st;
	event_t ev;
	if (request->getParam(0)->value().equals(F("log"))){
		//String str = F("{\"logdata\":\"<ul>")+logg.getAll2Web()+F("</ul>\"}");
		str+=F("\"logdata\":\"<ul>");
		//str+=logg.getAll2Web();
		str+=F("</ul>\"}");
		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	}else if (request->getParam(0)->value().equals(F("main"))){
		ev.button=199;
		ev.state=MEM_EVENT;
		xQueueSend(que,&ev,portMAX_DELAY);
		vTaskDelay(pdTICKS_TO_MS(100));
		//if (xMessageBufferReceive(web_mess,&st,SSTATE_LENGTH,portMAX_DELAY)==SSTATE_LENGTH){
		//if (xMessageBufferReceive(web_mess,&st,SSTATE_LENGTH,3000)==SSTATE_LENGTH){
		if (xMessageBufferReceive(web_mess,&st,SSTATE_LENGTH,100)==SSTATE_LENGTH){
		uint8_t i;	
      	// for (i=0;i<RELAYS_COUNT;i++)
		// {
		// 	str+=(i==0?"\"REL":",\"REL");
		// 	str+=String(i+1);
		// 	str+=("\":");
		// 	str+=String(st.rel[i]?1:0);
		// }
		// //str+=(",\"LIGHT_R\":");
		// for (i=0;i<LEDS_COUNT;i++)
		// {
		// 	str+=(",\"LIGHT_");
		// 	str+=i==0?"R":i==1?"G":"B";
		// 	str+=("\":\"");
		// 	str+=String(st.br[i].value);
		// 	str+=("\"");
		// }
		// for (i=0;i<LEDS_COUNT;i++)
		// {
		// 	str+=(",\"LEDMODE_");
		// 	str+=((i==0)?"R\":\"":(i==1)?"G\":\"":"B\":\"");
		// 	//str+=("\":\"");
		// 	str+=String(st.br[i].stste);
		// 	str+=("\"");
		// }
		//str+=F(",");
		for (i=0;i<ALARMS_COUNT;i++)
		{
    		str+=(",\"ALRM");
			str+=String(i+1);
			str+=("\":\"");
			if (st.alr[i].active)
			{
			 str+=(st.alr[i].hour>9?String(st.alr[i].hour):"0"+String(st.alr[i].hour));
			 str+=("-");
			 str+=(st.alr[i].minute>9?String(st.alr[i].minute):"0"+String(st.alr[i].minute));
			 str+=(" Per=");
			 str+=String(st.alr[i].period);
			 str+=(" WD=");
			 str+=String(st.alr[i].wday);
			}
			else{
			 str+=("NONE");
			}
			str+=("\"");
		}
		str+=F("}");
    	}
		// for (uint8_t i=0;i<4;i++)
		// {
		// 	str+=F("\"REL");
		// 	str+=String(i+1);
		// 	str+=F("\":");
		// 	//str+=String(data->isOn(i)?1:0);
		// 	str+=F(",");
		// }
		// str+=F("\"BAND_CW\":");
		// str+=String(128);
		// str+=F(",");
		// str+=F("\"BAND_NW\":");
		// str+=String(128);
		// str+=F(",");
		// str+=F("\"BAND_WW\":");
		// str+=String(128);
		// str+=F(",");
		// str+=F("\"DEVSHOW\":");
		// str+="\"HUI\"";
		// str+=F("}");

		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	}else if (request->getParam(0)->value().equals(F("main1"))){
		str+=F("\"DEVSHOW\":\"");
		str+=getI2Cdevices();
		//str+="AAS";
		str+=F("\"}");
		//Serial.println(str);
		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	
	}else if (request->getParam(0)->value().equals(F("update"))){
		
		str+=F("\"ALL\":");
		
		// if (Update.isRunning()){
		// 	str+=String(Update.size());
		// 	str+=F(",\"PROGRESS\":");
		// 	str+=String(Update.progress());
		// 	str+=F(",\"WORK\":1,\"ERROR\":\"");
		// }
		// else
		// {
		// 	str+=F("0,\"PROGRESS\":0,\"WORK\":0,\"ERROR\":\"");
		// }
		
		// if (Update.hasError())
		// {
		// 	str+=Update.errorString();
		// }
		// else{
		// 	str+=F("OK");
		// }
		str+=F("}");

		//logg.logging(str);
		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	}
}

