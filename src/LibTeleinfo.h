// **********************************************************************************
// Driver definition for French Teleinfo
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// For any explanation about teleinfo ou use , see my blog
// http://hallard.me/category/tinfo
//
// Code based on following datasheet
// http://www.erdf.fr/sites/default/files/ERDF-NOI-CPT_02E.pdf
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// Edit : Tab size set to 2 but I converted tab to sapces
//
// Modifié par Dominique DAMBRAIN 2017-07-10 (http://www.dambrain.fr)
//       Version 1.0.5
//       Librairie LibTeleInfo : Allocation statique d'un tableau de stockage 
//           des variables (50 entrées) afin de proscrire les malloc/free
//           pour éviter les altérations des noms & valeurs
//       Modification en conséquence des séquences de scanning du tableau
//       ATTENTION : Nécessite probablement un ESP-8266 type Wemos D1,
//        car les variables globales occupent 42.284 octets
//
// Modifié par marc Prieur 2019
//		-ajouté setReinit et getReinit.
//		-ajouté option de compilation HISTORIQUE.
//		-version standard a terminer.
// **********************************************************************************

#ifndef LibTeleinfo_h
#define LibTeleinfo_h

#ifdef __arm__
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define boolean bool 
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

// Using ESP8266 ?
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif

// Define this if you want library to be verbose
//#define TI_DEBUG

// I prefix debug macro to be sure to use specific for THIS library
// debugging, this should not interfere with main sketch or other 
// libraries


#ifdef TI_DEBUG
  #if ESP8266
    #define TI_Debug(x)    Serial1.print(x)
    #define TI_Debugln(x)  Serial1.println(x)
    #define TI_Debugf(...) Serial1.printf(__VA_ARGS__)
    #define TI_Debugflush  Serial1.flush
  #else
    #define TI_Debug(x)    Serial.print(x)
    #define TI_Debugln(x)  Serial.println(x)
    #define TI_Debugf(...) Serial.printf(__VA_ARGS__)
    #define TI_Debugflush  Serial.flush
  #endif
#else
  #define TI_Debug(x)    
  #define TI_Debugln(x)  
  #define TI_Debugf(...) 
  #define TI_Debugflush  
#endif

#ifdef ESP8266
  // For 4 bytes Aligment boundaries
  #define ESP8266_allocAlign(size)  ((size + 3) & ~((size_t) 3))
#endif
#define TAILLE_MAX_VALUE    98
#define TAILLE_MAX_NAME     16
// what we done with received value (also for callback flags)
#define TINFO_FLAGS_NONE     0x00
#define TINFO_FLAGS_NOTHING  0x01
#define TINFO_FLAGS_ADDED    0x02
#define TINFO_FLAGS_EXIST    0x04
#define TINFO_FLAGS_UPDATED  0x08
#define TINFO_FLAGS_ALERT    0x80 /* This will generate an alert */

// Local buffer for one line of teleinfo 
// maximum size, I think it should be enought
#define TINFO_BUFSIZE  64

// Teleinfo start and end of frame characters
#define TINFO_STX 0x02
#define TINFO_ETX 0x03 
#define TINFO_SGR '\n' // start of group  
#define TINFO_EGR '\r' // End of group    

#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary

// Linked list structure containing all values received
// Will be allocated statically 
typedef struct _ValueList ValueList;
struct _ValueList 
{
  ValueList *next;  // next element (for compatibility)
  char name[TAILLE_MAX_NAME];    // LABEL of value name
  //char value[16];   // value 
  char value[TAILLE_MAX_VALUE];   // //Changed for standard type
  uint8_t checksum; // checksum
  uint8_t flags;    // specific flags
  uint8_t free;		// checksum
  uint8_t filler;   // unused, for boundary
};
#pragma pack(pop) 
// Library state machine
enum _State_e {
  TINFO_INIT,     // We're in init
  TINFO_WAIT_STX, // We're waiting for STX
  TINFO_WAIT_ETX, // We had STX, We're waiting for ETX
  TINFO_READY     // We had STX AND ETX, So we're OK
};



class TInfo
{
  public:
    TInfo();
    void          init(boolean modeLinkyHistorique);
    void		  process (char c);
    void          attachADPS(void (*_fn_ADPS)(uint8_t phase));  
    void          attachData(void (*_fn_data)(ValueList * valueslist, uint8_t state));  
    void          attachNewFrame(void (*_fn_new_frame)(ValueList * valueslist));  
    void          attachUpdatedFrame(void (*_fn_updated_frame)(ValueList * valueslist));  
    ValueList *   addCustomValue(char * name, char * value, uint8_t * flags);
    ValueList *   getList(void);
    uint8_t       valuesDump(void);
    //char *        valueGet(char * name, char * value);
	char *        valueGet(const char * name, char * value);		//marc
    boolean       listDelete();
    unsigned char calcChecksum(char *etiquette, char *valeur) ;
	void setReinit();			//marc
	bool getReinit() const;			//marc

  private:
    void       clearBuffer();
    ValueList *   valueAdd (char * name, char * value, uint8_t checksum, uint8_t * flags);
    boolean       valueRemove (char * name);
    boolean       valueRemoveFlagged(uint8_t flags);
    int           labelCount();
    void          customLabel( char * plabel, char * pvalue, uint8_t * pflags) ;
    ValueList *   checkLine(char * pline) ;

    _State_e  _state; // Teleinfo machine state
    ValueList _valueslist;   // Linked list of teleinfo values
    char      _recv_buff[TINFO_BUFSIZE]; // line receive buffer
    uint8_t   _recv_idx;  // index in receive buffer
    boolean   _frame_updated; // Data on the frame has been updated
    void      (*_fn_ADPS)(uint8_t phase);
    void      (*_fn_data)(ValueList * valueslist, uint8_t state);
    void      (*_fn_new_frame)(ValueList * valueslist);
    void      (*_fn_updated_frame)(ValueList * valueslist);
	bool	  need_reinit = false;    //marc
	boolean modeLinkyHistorique;
    //volatile uint8_t *dcport;
    //uint8_t dcpinmask;
};

extern TInfo TINFO;		//marc

#endif
