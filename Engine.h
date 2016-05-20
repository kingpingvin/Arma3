#ifndef ENGINE_H
#define ENGINE_H

#include "main.h"
#include "Memory.h"

#pragma warning( disable : 4101 ) // UNREFERENCED PARAMATER

extern Memory*		m;
extern d3Console*	console;
extern LPD3DXFONT	font;

namespace A3
{

	namespace{
		DWORD		transOffsets[ 2 ]	= { 0x17C7C24, 0xE4 };
		DWORD		worldOffset			= 0x17B11D0; 
		DWORD		scoreboardOffset	= 0x17A0630;
		DWORD		weaponTableOffset	= 0xDAD7E8;
	}

	class ArmaString
	{
		DWORD address;

	public:
		ArmaString( DWORD dwAddress ){
			address = dwAddress;
		}

		int getLength(){
			if( !address )
				return 0;
			try
			{
				return m->read<int>( address + 0x4 );
			} catch (ERROR_MEM e) {
				console->sendInput("Unit table: " + to_string(e));
				return 0;
			}
		}
		std::string getString(){
			if( !address )
				return "";
		try {
			return m->readString( address + 0x8, this->getLength() );
		} catch (ERROR_MEM e) {
			console->sendInput("Unit table: " + to_string(e));
			return "";
		}
			
		}
		std::string getStringUnformated() {
			if (!address)
				return "";
			try {
				return m->readStringUnformated(address + 0x8, this->getLength());
			} catch (ERROR_MEM e) {
				console->sendInput("Unit table: " + to_string(e));
				return "";
			}

		}
		
		VOID setText(char* text, int length) {
			m->write<DWORD>( address + 0x4, length + 1 );
			m->write<char>( address + 0x8, text, length );
		}
	};

	class ScoreboardEntity{
		DWORD address;
	public:
		ScoreboardEntity( DWORD dwAddress ){
			address = dwAddress;
		}
		int getID(){
			try
			{
				return m->read<DWORD>( address + 0x4 );
			} catch ( ERROR_MEM e )
			{
				return 0;
			}
			
		}
		std::unique_ptr<ArmaString> getString(){
			try
			{
				return std::unique_ptr<ArmaString>( new ArmaString( m->read<DWORD>(address + 0xA0) ) );
			} catch (ERROR_MEM e )
			{
				console->sendInput("Scoreboard table: " + to_string(e));
				return std::unique_ptr<ArmaString>(new ArmaString(0));
			}
			
		}
	};

	class ScoreboardTable{
		DWORD address;
	public:
		ScoreboardTable( DWORD dwAddress ){
			address = dwAddress;
		}
		std::unique_ptr<ScoreboardEntity> getEntryById( DWORD id ){
			try
			{
				return std::unique_ptr<ScoreboardEntity>( new ScoreboardEntity( address + (id*0x120) ));
			} catch ( ERROR_MEM e )
			{
				return std::unique_ptr<ScoreboardEntity>(new ScoreboardEntity(0));
			}
			
		}
	};

	class Scoreboard{
		DWORD address;
	public:
		Scoreboard( DWORD dwAddress ){
			address = dwAddress;
		}

		std::unique_ptr<ArmaString> getPlayerByID(DWORD id)
		{
			auto _scoreboardTable = getScoreboardTable();
			for (INT _id = 0; _id < getTableSize(); _id++)
			{
				auto _entry = _scoreboardTable->getEntryById( _id );
				if (_entry->getID() == id) {
					return _entry->getString();
				}
			}

			return nullptr;
		}

		std::unique_ptr<ScoreboardTable> getScoreboardTable(){
			try
			{
				return std::unique_ptr<ScoreboardTable>( new ScoreboardTable( m->read<DWORD>( address + 0x18 )) );
			} catch ( ERROR_MEM e )
			{
				return std::unique_ptr<ScoreboardTable>(new ScoreboardTable(0));
			}
			
		}
		INT getTableSize(){
			try {
				return m->read<int>( address + 0x1C );
			} catch (ERROR_MEM e) {
				return 0;
			}
			
		}
	};

	class NetworkManager{
		DWORD address;
	private:

		// BASE ADDRESS + OFFSET
		static DWORD getBase(){
			return m->getProcessBaseAddress() + scoreboardOffset;
		}
	public:
		NetworkManager(){
			address = this->getBase();
		}
		static std::unique_ptr<NetworkManager> Singleton(){
			return std::unique_ptr<NetworkManager>( new NetworkManager() );
		}

		std::unique_ptr<Scoreboard> getScoreboard(){
			try
			{
				return std::unique_ptr<Scoreboard>( new Scoreboard( m->read<DWORD>( address + 0x24 ) ) );
			} catch (const std::exception&)
			{
				return std::unique_ptr<Scoreboard>(new Scoreboard(0));
			}
		}
	};

	class TransData
	{
	public:
		DWORD		address				= 0;
		D3DXVECTOR3 InvViewRight		= D3DXVECTOR3( 0, 0, 0 );
		D3DXVECTOR3 InvViewForward		= D3DXVECTOR3( 0, 0, 0 );
		D3DXVECTOR3 InvViewTranslation	= D3DXVECTOR3( 0, 0, 0 );
		D3DXVECTOR3 InvViewUp			= D3DXVECTOR3( 0, 0, 0 );

		
		D3DXVECTOR3 ViewPortMatrix		= D3DXVECTOR3( 0, 0, 0 );
		D3DXVECTOR3 Projection1			= D3DXVECTOR3( 0, 0, 0 );
		D3DXVECTOR3 Projection2			= D3DXVECTOR3( 0, 0, 0 );

	private:
		// + BASE ADDRES
		static DWORD getBase(){
			DWORD base = m->read<DWORD>( m->getProcessBaseAddress() + transOffsets[0] );
			return m->read<DWORD>( base + transOffsets[1] );
		}
	public:
		TransData(){
			address = this->getBase();
		}

		void refresh()
		{
			try
			{
				InvViewRight = m->read<D3DXVECTOR3>(address + 0x4);
				InvViewForward = m->read<D3DXVECTOR3>(address + 0x1C);
				InvViewTranslation = m->read<D3DXVECTOR3>(address + 0x28);
				InvViewUp = m->read<D3DXVECTOR3>(address + 0x10);

				ViewPortMatrix = m->read<D3DXVECTOR3>(address + 0x54);
				Projection1 = m->read<D3DXVECTOR3>(address + 0xCC);
				Projection2 = m->read<D3DXVECTOR3>(address + 0xD8);
			} catch (ERROR_MEM e )
			{
				return;
			}

		}

		D3DXVECTOR3 WorldToScreen( D3DXVECTOR3 curPos )
		{
			D3DXVECTOR3 temp( 0, 0, 0 );
			D3DXVec3Subtract( &temp, &curPos, &InvViewTranslation );

			float x = 0, y = 0, z = 0;

			z = D3DXVec3Dot( &temp, &InvViewForward );
			y = D3DXVec3Dot( &temp, &InvViewUp );
			x = D3DXVec3Dot( &temp, &InvViewRight );

			temp.y = 2 * ( ViewPortMatrix.y * ( 1 - ( y / Projection2.y / z ) ) ) / 2;
			temp.x = 2 * ( ViewPortMatrix.x * ( 1 + ( x / Projection1.x / z ) ) ) / 2;
			temp.z = z;

			return temp;
		}

		float Distance3D( D3DXVECTOR3 v1, D3DXVECTOR3 v2 )
		{
			D3DXVECTOR3 out( 0, 0, 0 );
			out = v1 - v2;

			return D3DXVec3Length( &out );
		}

		static std::unique_ptr<TransData> Singleton(){
			return std::unique_ptr<TransData>( new TransData( ) );
		}
			
	};

	class Entity{
		DWORD address;

	public:
		Entity( DWORD dwAddress ){
			address = dwAddress;
		}

		DWORD		getBase(){
			return address;
		}
		D3DXVECTOR3 getPos(){
			try
			{
				return m->read<D3DXVECTOR3>( address + 0x28 );
			} catch ( ERROR_MEM e )
			{
				return D3DXVECTOR3( 0,0,0 );
			}
		}
		D3DXVECTOR3 getAccerelation()
		{
			try
			{
				return m->read<D3DXVECTOR3>(address + 0x48);
			} catch (ERROR_MEM e)
			{
				return D3DXVECTOR3(0, 0, 0);
			}
		}
		float		getViewX(){
			try
			{
				return  m->read<float>( address + 0x1C );
			} catch (ERROR_MEM e )
			{
				return 0.0f;
			}
		}
		float		getViewY(){
			try
			{
				return  m->read<float>(address + 0x24);
			} catch (ERROR_MEM e)
			{
				return 0.0f;
			}
		}

		void		setPos( D3DXVECTOR3 vec ){
			m->write( address + 0x28, vec.x );
			m->write( address + 0x2C, vec.y );
			m->write( address + 0x30, vec.z );
		}
	};
	
	class Inventory{
		DWORD address;

	public:
		Inventory( DWORD dwAddress ){
			address = dwAddress;
		}

		DWORD getBase(){
			return address;
		}
		DWORD calculateDistance( SIZE_T id ){
			return address + ( id * 0x4 );
		}
	};

	class InventoryTable{
		DWORD address;

	public:
		InventoryTable( DWORD dwAddress ){
			address = dwAddress;
		}

		DWORD getTableSize(){
			return m->read<DWORD>( address + 0x68C );
		}
		DWORD getTableMaxSize(){
			return m->read<DWORD>( address + 0x690 );
		}

		DWORD getConsumableTableSize(){
			return m->read<DWORD>( address + 0x6A4 );
		}
		DWORD getConsumableTableMaxSize(){
			return m->read<DWORD>( address + 0x6A8 );
		}

		void  setTableSize( SIZE_T size ){
			m->write( address + 0x68C, size );
		}
		void  setConsumableTableSize( SIZE_T size ){
			m->write(address + 0x6A4, size);
		}
		
		std::unique_ptr<Inventory> getInventory(){
			return unique_ptr<Inventory>( new Inventory( m->read<DWORD>( address + 0x688 ) ) );
		}
		std::unique_ptr<Inventory> getConsumableInventory(){
			return unique_ptr<Inventory>( new Inventory( m->read<DWORD>( address + 0x6A0 ) ) );
		}
	};

	class EventHandler {
		DWORD address;

	public:
		EventHandler(DWORD dwAddress) {
			address = dwAddress;
		}

		DWORD		getEvent() {
			return m->read<DWORD>( address + 0x4 );
		}
		std::unique_ptr<ArmaString>	getEventText() {
			return std::unique_ptr<ArmaString>( new ArmaString( m->read<DWORD>( m->read<DWORD>( address + 0xC ) + 0x8 ) ) );
		}

		VOID		setEventText( string text ) {
			ArmaString aStr = m->read<DWORD>( m->read<DWORD>( address + 0xC ) + 0x8 );

			char* buffer = new char[ text.length() + 1 ];
			sprintf_s( buffer, text.length() + 1, "%s", text.c_str() );

			aStr.setText( buffer, text.length() + 1 );
		}
	};

	class EventHandlerTable {
		DWORD address;

	public:
		EventHandlerTable(DWORD dwAddress) {
			address = dwAddress;
		}

		DWORD						getTableSize() {
			return m->read<DWORD>( address + 0x4 );
		}
		std::vector<EventHandler>	getHandlers() {
			std::vector<EventHandler> handlers;
			DWORD base = m->read<DWORD>( address );

			for (DWORD i = 0; i < getTableSize(); i++)
			{
				handlers.push_back( EventHandler( base + i * 0x18 ) );
			}
			return handlers;
		}
		
	};

	class Unit
	{
		DWORD address;

	public:
		Unit( DWORD dwAddress ){
			address = dwAddress;
		}

		DWORD	getBase(){
			return address;
		}
		BYTE	isAlive(){
			try {
				return m->read<BYTE>( address + 0x330 );
			} catch (ERROR_MEM e) {
				return 0;
			}
		}
		DWORD	getID(){
			try {
				return m->read<DWORD>(address + 0x798);
			} catch (ERROR_MEM e) {
				return 0;
			}
		}
		DWORD	getTeam()
		{
			try {
				return m->read<DWORD>(address + 0x214);
			} catch (ERROR_MEM e) {
				return 0;
			}
		}
		INT		getWeaponID() {
			return m->read<DWORD>( address + 0x7CC );
		}

		string	getName()
		{
			auto _networkManager = NetworkManager::Singleton();

			auto _id = getID();
			if ( _id > 1 ) {
				auto name = _networkManager->getScoreboard()->getPlayerByID( _id );
				if( !name ) return "NULL";

				return name->getString();
			}

			return "AI";
		}

		std::unique_ptr<EventHandlerTable> getEventHandlerTable() {
			return std::unique_ptr<EventHandlerTable>(new EventHandlerTable(address + 0x41C));
		}
		std::unique_ptr<InventoryTable> getInventoryTable( ){
			return std::unique_ptr<InventoryTable>( new InventoryTable( address ) );
		}
		std::unique_ptr<Entity> getPlayer(){
			try
			{
				return std::unique_ptr<Entity>( new Entity( m->read<DWORD>( address + 0x68 ) ) );
			} catch (ERROR_MEM e)
			{
				return std::unique_ptr<Entity>(new Entity(0));
			}
			
		}
	};

	class Vehicle{
		DWORD address;
	public:
		Vehicle( DWORD dwAddress ){
			address = dwAddress;
		}

		D3DXVECTOR3		getPos(){
			try
			{
				return m->read<D3DXVECTOR3>( address + 0x28 );
			} catch (ERROR_MEM e)
			{
				return D3DXVECTOR3(0,0,0);
			}
		}
		D3DXVECTOR3		getAccerelation()
		{
			try
			{
				return m->read<D3DXVECTOR3>(address + 0x48);
			} catch (ERROR_MEM e)
			{
				return D3DXVECTOR3(0, 0, 0);
			}
		}

		void			setPos(D3DXVECTOR3 vec) {
			m->write(address + 0x28, vec.x);
			m->write(address + 0x2C, vec.y);
			m->write(address + 0x30, vec.z);
		}
		void			setAcceleration(D3DXVECTOR3 vec) {
			m->write(address + 0x48, vec.x);
			m->write(address + 0x4C, vec.y);
			m->write(address + 0x50, vec.z);
		}

	};

	class VehicleInfo{
		DWORD address;
	public:
		VehicleInfo( DWORD dwAddress ){
			address = dwAddress;
		}

		void			unlock(){
			m->write<int>( address + 0xB8C, 1 );
		}
		DWORD			getBase(){
			return address;
		}

		std::unique_ptr<ArmaString>	getEntityName(){
			try
			{
				DWORD base = m->read<DWORD>( m->read<DWORD>( address + 0xD8 ) + 0xA2C );
				return std::unique_ptr<ArmaString>( new ArmaString(base) );
			} catch (ERROR_MEM e)
			{
				return std::unique_ptr<ArmaString>(new ArmaString(0));
			}
		}

		std::unique_ptr<Unit> getDriver(){
			try {
				return std::unique_ptr<Unit>( new Unit( m->read<DWORD>( address + 0xBA4 ) ) );
			} catch (ERROR_MEM e) {
				console->sendInput("Unit table: " + to_string(e));
				return std::unique_ptr<Unit>(new Unit(0));
			}
			
		}
		std::unique_ptr<Vehicle> getVehicle(){
			try
			{
				return std::unique_ptr<Vehicle>( new Vehicle( m->read<DWORD>( address + 0x68 ) ) );
			} catch (ERROR_MEM e)
			{
				return std::unique_ptr<Vehicle>(new Vehicle(0));
			}
		}
	};

	class UnitInfo{
		DWORD address;
	public:
		UnitInfo( DWORD dwAddress ){
			address = dwAddress;
		}

		DWORD			getBase(){
			return address;
		}

		std::unique_ptr<Unit> getUnit(){
			try{
				return std::unique_ptr<Unit>( new Unit( m->read<DWORD>( address + 0x4 ) ) );
			} catch( ERROR_MEM e ){
				console->sendInput( "Unit table: " + to_string(e) );
				return std::unique_ptr<Unit>( new Unit( 0 ) );
			}
			
		}
		std::unique_ptr<VehicleInfo> getVehicle( ){
			try
			{
				return std::unique_ptr<VehicleInfo>( new VehicleInfo( m->read<DWORD>( address + 0x4 ) ) );
			} catch (ERROR_MEM e)
			{
				return std::unique_ptr<VehicleInfo>(new VehicleInfo(0));
			}
		}
	};

	class EntityTable{
		DWORD address;
	public:
		EntityTable( DWORD dwAddress ){
			address = dwAddress;
		}

		std::unique_ptr<UnitInfo> getUnitInfoById( DWORD id ){
			try {
				return std::unique_ptr<UnitInfo>( new UnitInfo( m->read<DWORD>( address + (id * 0x34) ) ) );
			} catch (ERROR_MEM e) {
				console->sendInput("UnitInfo table: " + to_string(e));
				return std::unique_ptr<UnitInfo>(new UnitInfo(0));
			}
			
		}
	};

	class ObjectList {
	public:
		std::vector<std::unique_ptr<Unit>>			players;
		std::vector<std::unique_ptr<VehicleInfo>>	vehPlayers;
		std::vector<std::unique_ptr<VehicleInfo>>	emptyVehicles;
		std::vector<std::unique_ptr<VehicleInfo>>	AIVehicles;
	};

	class EntityTablePtr{
		DWORD address;
	public:
		EntityTablePtr( DWORD dwAddress ){
			address = dwAddress;
		}

		std::unique_ptr<ObjectList>						getObjects()
		{
			std::unique_ptr<ObjectList> objects			= std::unique_ptr<ObjectList>( new ObjectList );

			auto _entityTable = getTable();
			for (DWORD i = 0; i < getTableSize(); i++)
			{
				auto _unit	= _entityTable->getUnitInfoById( i );
				auto _id	= _unit->getUnit()->getID();

				if ( _id == 1065353216 ) // Vehicle
				{
					auto _veh = _unit->getVehicle();
					if (!_veh->getDriver()->getBase()) // EMPTY CAR
					{
						objects->emptyVehicles.push_back( std::move(_veh) );
					}
					else if ( _veh->getDriver()->getID() > 1) // PLAYER INSIDE
					{
						objects->vehPlayers.push_back( std::move(_veh) );
					}
					else if (_veh->getDriver()->getID() == 1) // AI INSIDE
					{
						objects->AIVehicles.push_back( std::move(_veh) );
					}

					continue;
				} else if ( _unit->getUnit()->getID() > 1 ) //PLAYER
				{
					auto _entity = _unit->getUnit();
					objects->players.push_back( std::move(_entity) );
					continue;
				} else { // AI ON FOOT
					// CURRENTLY NO USE
				}
			}

			return objects;
		}

		DWORD							getTableSize(){
			try
			{
				return m->read<DWORD>( address + 0x4 );
			} catch (ERROR_MEM e)
			{
				return 0;
			}
		}
		std::unique_ptr<EntityTable>	getTable(){
			try
			{
				return std::unique_ptr<EntityTable>( new EntityTable( m->read<DWORD>(address + 0x0) ) );
			} catch (ERROR_MEM e)
			{
				return std::unique_ptr<EntityTable>(new EntityTable(0));
			}
		}
	};

	class Ammo{
		DWORD address;

	public:
		Ammo( DWORD dwAddress ){
			address = dwAddress;
		}

		DWORD					getBase(){
			return address;
		}
		DWORD					getStats(){
			return m->read<DWORD>( address + 0x314 );
		}
		D3DXVECTOR3				getPos(){
			return m->read<D3DXVECTOR3>( m->read<DWORD>( address + 0x68 ) + 0x28 );
		}

		void					setAcceleration(D3DXVECTOR3 vec) {
			m->write<D3DXVECTOR3>(m->read<DWORD>(address + 0x68) + 0x48, vec );
		}
		void					setPos( D3DXVECTOR3 vec ){
			m->write<D3DXVECTOR3>(m->read<DWORD>(address + 0x68) + 0x28, vec );
		}
		void					setOwner(DWORD owner)
		{
			m->write<DWORD>( address + 0x314, owner );
		}
	};

	class Ammunition{
		DWORD address;
		
	public:
		Ammunition( DWORD dwAddress ){
			address = dwAddress;
		}

		std::unique_ptr<Ammo>	getAmmoById( DWORD i ){
			return std::unique_ptr<Ammo>( new Ammo( m->read<DWORD>( address + ( i * 0x4 ) ) ) );
		}
	};

	class Munition {
		DWORD address;

	public:
		Munition(DWORD dwAddress) {
			address = dwAddress;
		}

		DWORD getTableSize() {
			return m->read<DWORD>(address + 0xCF4);
		}
		std::unique_ptr<Ammunition> getTable() {
			return std::unique_ptr<Ammunition>(new Ammunition(m->read<DWORD>(address + 0xCF0))); // B00
		}
	};

	class Weapon{
		DWORD address;

	public:
		Weapon( DWORD dwAddress ){
			address = dwAddress;
		}

		DWORD						getBase(){
			return address;
		}
		std::unique_ptr<ArmaString> getName(){
			return std::unique_ptr<ArmaString>( new ArmaString( m->read<DWORD>( address + 0x18 ) ) );
		}
		std::unique_ptr<ArmaString> getType(){
			return std::unique_ptr<ArmaString>( new ArmaString( m->read<DWORD>( address + 0x1C ) ) );
		}
	};

	class WeaponTable{
		DWORD address;

	private:
		static DWORD getBase(){
			return weaponTableOffset;
		}
	public:
		WeaponTable(){
			address = this->getBase();
		}

		DWORD getTableSize(){
			return m->read<DWORD>( address + 0x4 );
		}

		std::unique_ptr<Weapon> getEntryById( SIZE_T id ){
			return std::unique_ptr<Weapon>( new Weapon( m->read<DWORD>( m->read<DWORD>(address) + ( id * 0x4 ) ) ) );
		}

		static std::unique_ptr<WeaponTable> Singleton(){
			return std::unique_ptr<WeaponTable>( new WeaponTable() );
		}
	};

	class Consumable{
		DWORD address;

	public:
		Consumable( DWORD dwAddress ){
			address = dwAddress;
		}

		DWORD						getBase(){
			return address;
		}
		std::unique_ptr<ArmaString> getName(){
			return std::unique_ptr<ArmaString>( new ArmaString( m->read<DWORD>( address + 0x14 ) ) );
		}
		std::unique_ptr<ArmaString> getType(){
			return std::unique_ptr<ArmaString>( new ArmaString( m->read<DWORD>( address + 0x20 ) ) );
		}
	};

	class ConsumableTable{
		DWORD address;

	private:
		static DWORD getBase(){
			return weaponTableOffset;
		}
	public:
		ConsumableTable(){
			address = this->getBase();
		}

		DWORD getTableSize(){
			return m->read<DWORD>( address + 0xC4 );
		}

		std::unique_ptr<Consumable> getEntryById( SIZE_T id ){
			return std::unique_ptr<Consumable>( new Consumable( m->read<DWORD>( m->read<DWORD>( address + 0xC0 ) +( id * 0x4 ) ) ) );
		}
		static std::unique_ptr<ConsumableTable> Singleton( ){
			return std::unique_ptr<ConsumableTable>( new ConsumableTable( ) );
		}
	};

	class ScriptVM {
		DWORD address;
	public:
		ScriptVM(DWORD dwAddress) {
			address = dwAddress;
		}

		std::unique_ptr<ArmaString> getScriptName() {
			try {
				return std::unique_ptr<ArmaString>(new ArmaString(m->read<DWORD>(address + 0x134)));
			} catch (ERROR_MEM e) {
				console->sendInput("UnitInfo table: " + to_string(e));
				return std::unique_ptr<ArmaString>(new ArmaString(0));
			}
		}
		std::unique_ptr<ArmaString> getScriptText() {
			try {
				return std::unique_ptr<ArmaString>(new ArmaString(m->read<DWORD>(address + 0x138)));
			} catch (ERROR_MEM e) {
				console->sendInput("UnitInfo table: " + to_string(e));
				return std::unique_ptr<ArmaString>(new ArmaString(0));
			}
		}
		std::unique_ptr<ArmaString> getScriptSource() {
			try {
				return std::unique_ptr<ArmaString>(new ArmaString(m->read<DWORD>(address + 0x140)));
			} catch (ERROR_MEM e) {
				console->sendInput("UnitInfo table: " + to_string(e));
				return std::unique_ptr<ArmaString>(new ArmaString(0));
			}
		}
	};

	class ScriptTable {
		DWORD address;
	public:
		ScriptTable(DWORD dwAddress) {
			address = dwAddress;
		}

		DWORD	getTableSize()
		{
			return m->read<DWORD>( worldOffset + 0x71C );
		}
		std::unique_ptr<ScriptVM> getScriptByID(DWORD id) {
			try {
				return std::unique_ptr<ScriptVM>(new ScriptVM( m->read<DWORD>(address + (id * 0x8) )));
			} catch (ERROR_MEM e) {
				console->sendInput("Script table: " + to_string(e));
				return std::unique_ptr<ScriptVM>(new ScriptVM(0));
			}

		}
	};

	class World {
	private:

		// MODIFIED TO ADD BASE TO PTR
		static DWORD getBase() {
			try {
				return m->read<DWORD>(m->getProcessBaseAddress() + worldOffset);
			} catch (ERROR_MEM e) {
				console->sendInput("World: " + to_string(e));
				return 0;
			}
		}
		DWORD address;

	public:
		World() {
			address = this->getBase();
		}

		std::unique_ptr<EntityTablePtr> getEntityTable()
		{
			try {
				return std::unique_ptr<EntityTablePtr>(new EntityTablePtr(m->read<DWORD>(address + 0xA54)));
			} catch (ERROR_MEM e) {
				console->sendInput("Entity table: " + to_string(e));
				return std::unique_ptr<EntityTablePtr>(new EntityTablePtr(0));
			}
		}
		std::unique_ptr<UnitInfo>		getCameraOn() {
			try {
				return std::unique_ptr<UnitInfo>(new UnitInfo(m->read<DWORD>(address + 0x1920)));
			} catch (ERROR_MEM e) {
				console->sendInput("UnitInfo table: " + to_string(e));
				return std::unique_ptr<UnitInfo>(new UnitInfo(0));
			}

		}
		DWORD							getRealPlayer() {
			try {
				return m->read<DWORD>(address + 0x1930);
			} catch (ERROR_MEM e) {
				console->sendInput("RealPlayer table: " + to_string(e));
				return 0;
			}

		}
		std::unique_ptr<Munition>		getMunition()
		{
			try {
				return std::unique_ptr<Munition>(new Munition(address));
			} catch (ERROR_MEM e) {
				console->sendInput("Munition table: " + to_string(e));
				return std::unique_ptr<Munition>(new Munition(0));
			}
		}
		std::unique_ptr<ScriptTable>	getScriptTable()
		{
			try {
				return std::unique_ptr<ScriptTable>(new ScriptTable(address + 0x718));
			} catch (ERROR_MEM e) {
				console->sendInput("Munition table: " + to_string(e));
				return std::unique_ptr<ScriptTable>(new ScriptTable(0));
			}
		}

		static std::unique_ptr<World>	Singleton() {
			return std::unique_ptr<World>(new World());
		}
		DWORD							getWorldBase() {
			return address;
		}
	};
}

#endif