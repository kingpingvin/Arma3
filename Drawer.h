#ifndef DRAWER_HEADER
#define DRAWER_HEADER

#include "main.h"
#include "Engine.h"

void render( LPVOID args );
void render2( LPVOID args );
void cleanUP( LPVOID args );
void findItem( string data );
void setRange( LPVOID args );
void teleport( LPVOID args );
void clearFile( LPVOID args );
void setPlayer( LPVOID args );
void setDamage( LPVOID args );
void setWeapon( LPVOID args );
void unlockCar( LPVOID args );
void killPlayer( LPVOID args );
void listWeapon( string data );
void resetStats( LPVOID args );
void findWeapon( string data );
void setIndirect( LPVOID args );
void framePlayer( LPVOID args );
void spawnWeapon( LPVOID args );
void setAmmunation( LPVOID args );
void setConsumable( LPVOID args );
void displayScripts( LPVOID args );
void findConsumable( string data );
void listConsumable( string data );
void teleportUnsafe( LPVOID args );
void spawnAmmunation( LPVOID args );
void getScriptConsole( INT eventID );
void listPlayersConsole( string comp );
void spawnWeaponConsole( int id, int amount );
void hijackVehicle( INT targetID, INT timeOut );
void giveCarConsole( INT targetID, INT distance );
void spawnConsumableConsole( int id, int amount );
void setScriptConsole( INT eventID, string text, INT timeOut );
void accelerateCarConsole( INT targetID, INT distance, bool* run );
void killPlayerConsole(vector<int> targetList, INT frameIndex, bool* run );

#endif