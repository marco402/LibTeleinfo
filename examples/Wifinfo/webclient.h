// **********************************************************************************
// ESP8266 Teleinfo WEB Client, routine include file
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use, see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-12-04 - First release
//
// All text above must be included in any redistribution.
//
// Modifi� par marc Prieur 2019
//		-V2.0.0:int�gr� le code dans la classe webClient webClient.cpp  webClient.h
//		-V2.0.2:ajout de TAILLEBUFEMONCMS
// Using library ESP8266HTTPClient version 1.1
//
// **********************************************************************************

#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include <Arduino.h> 
#include "Wifinfo.h"

#define TAILLETABNAMES 38
#define TAILLEBUFEMONCMS 400
class webClient
{
public:
	webClient(boolean modeLinkyHistorique);
	boolean emoncmsPost(void);
	boolean jeedomPost(void);
	boolean httpRequest(void);
	//boolean UPD_switch(void);
	String  build_emoncms_json(void);
	bool validate_value_name(String name);

	
private:
	boolean httpPost(char * host, uint16_t port, char * url);
	//List of authorized value names in Teleinfo, to detect polluted entries
		const String tabnames[TAILLETABNAMES] = {
	  "ADCO" , "OPTARIF" , "ISOUSC" , "BASE", "HCHC" , "HCHP",
	   "IMAX" , "IINST" , "PTEC", "PMAX", "PAPP", "HHPHC" , "MOTDETAT" , "PPOT",
	   "IINST1" , "IINST2" , "IINST3", "IMAX1" , "IMAX2" , "IMAX3" ,
	  "EJPHN" , "EJPHPM" , "BBRHCJB" , "BBRHPJB", "BBRHCJW" , "BBRHPJW" , "BBRHCJR" ,
	  "BBRHPJR" , "PEJP" , "DEMAIN" , "ADPS" , "ADIR1", "ADIR2" , "ADIR3","SINSTS" ,
	  "EAST","EASF01","EASF02"
	};
	boolean modeLinkyHistorique;

};
extern webClient WEBCLIENT;
#endif
