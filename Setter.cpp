#include "Drawer.h"

using namespace A3;

int  itemRef		= 0;

std::vector<DWORD> weapons;

void teleport( LPVOID args )
{
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*)item->arguments;

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();

	while( item->isEnabled )
	{
		if( GetAsyncKeyState( 0x57 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x + ( item->value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z + ( item->value*locPlayer->getViewY() );
			plyrPos.y = 0;

			locPlayer->setPos( plyrPos );
		} else if( GetAsyncKeyState( 0x53 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x - ( item->value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z - ( item->value*locPlayer->getViewY() );
			plyrPos.y = 0;

			locPlayer->setPos( plyrPos );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
	}
}

void teleportUnsafe( LPVOID args )
{
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*) item->arguments;

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();

	while( item->isEnabled )
	{
		if( GetAsyncKeyState( 0x57 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x + ( item->value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z + ( item->value*locPlayer->getViewY() );

			locPlayer->setPos( plyrPos );
		} else if( GetAsyncKeyState( 0x53 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x - ( item->value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z - ( item->value*locPlayer->getViewY() );

			locPlayer->setPos( plyrPos );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
	}
}

void unlockCar( LPVOID args ){
	D3D9_item*		item		= (D3D9_item*) args;
	Memory*			m			= (Memory*) item->arguments;

	item->isEnabled				= false;

	auto			world			= World::Singleton();
	auto			locPlayer		= world->getCameraOn()->getUnit()->getPlayer();
	auto			transData		= TransData::Singleton( );

	auto			entityTablePtr		= world->getEntityTable();
	auto			entityTable			= entityTablePtr->getTable();
	auto			entityTableSize		= entityTablePtr->getTableSize();

	for( DWORD i = 0; i < entityTableSize; i++ ){

		auto UnitInfo = entityTable->getUnitInfoById( i );
		auto Unit = UnitInfo->getUnit();
		auto UnitBase = Unit->getBase();

		if( UnitBase ){
			if( Unit->getID() == 1065353216 ){

				auto VehicleInfo	= UnitInfo->getVehicle();
				auto Vehicle		= VehicleInfo->getVehicle();
				if( transData->Distance3D( locPlayer->getPos(), Vehicle->getPos() ) < 15 ){
					VehicleInfo->unlock();
					break;
				}
			}
		}

	}
}

void listPlayersConsole( string comp )
{
	cout << "Searching for players that include: " << comp << endl;
	auto			world = World::Singleton();

	if (!world->getCameraOn()->getUnit()->getBase())
		return;

	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();
	if (!locPlayer->getBase())
		return;

	auto			entityTablePtr = world->getEntityTable();
	auto			entityTable = entityTablePtr->getTable();
	auto			entityTableSize = entityTablePtr->getTableSize();

	auto			networkMgr = NetworkManager::Singleton();
	auto			scoreboard = networkMgr->getScoreboard();
	auto			scoreboardSize = scoreboard->getTableSize();
	auto			scoreboardTable = scoreboard->getScoreboardTable();

	cout << "List of all players and populated vehicles in game" << endl;
	cout << "==================================================" << endl;

	for (DWORD i = 0; i < entityTableSize; i++) {

		auto UnitInfo = entityTable->getUnitInfoById(i);
		auto Unit = UnitInfo->getUnit();
		auto UnitBase = Unit->getBase();

		DWORD ID = Unit->getID();

		if (UnitBase) {
			if (ID > 1 && ID != 1065353216) { // PLAYER

				auto Entity = Unit->getPlayer();

				for (INT l = 0; l < scoreboardSize; l++) {
					auto scoreboardEntity = scoreboardTable->getEntryById(l);
					if (scoreboardEntity->getID() == ID) {
						if (comp.length() > 0 && scoreboardEntity->getString()->getString().find(comp) != string::npos) {
							string plyrName = scoreboardEntity->getString()->getString();
							cout << i << '\t' << plyrName << endl;
						} else if( comp.length() == 0 ) {
							string plyrName = scoreboardEntity->getString()->getString();
							cout << i << '\t' << plyrName << endl;
						}
						break;
					}
				}
			} else if (ID == 1065353216) {
				auto VehicleInfo = UnitInfo->getVehicle();
				auto Entity = VehicleInfo->getDriver();
				auto Vehicle = VehicleInfo->getVehicle();

					if (Entity->getBase()) {

						if (VehicleInfo->getBase() == locPlayer->getBase())
							continue;

						ID = Entity->getID();
						if (ID == 1) {
						} else {
							for (INT l = 0; l < scoreboardSize; l++) {
								auto scoreboardEntity = scoreboardTable->getEntryById(l);

								if (scoreboardEntity->getID() == ID) {
									if (comp.length() > 0 && scoreboardEntity->getString()->getString().find( comp ) != string::npos ) {
										string plyrName = scoreboardEntity->getString()->getString();
										cout << i << '\t' << "[CAR]" << '\t' << plyrName << endl;
									} else if( comp.length() == 0 ){
										string plyrName = scoreboardEntity->getString()->getString();
										cout << i << '\t' << "[CAR]" << '\t' << plyrName << endl;
									}
									break;
								}
							}
						}
					}
			}
		}
	}

	cout << "==================================================" << endl;
}

void killPlayerConsole(std::vector<int> targetList, INT frameIndex, bool* run) {

	if (targetList.size() == 0)
	{
		cout << "Target list array is empty, exiting!" << endl;
		return;
	}
	

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();
	auto			transData = TransData::Singleton();

	auto			entityTablePtr = world->getEntityTable();
	auto			entityTable = entityTablePtr->getTable();
	auto			entityTableSize = entityTablePtr->getTableSize();

	auto			networkMgr = NetworkManager::Singleton();
	auto			scoreboard = networkMgr->getScoreboard();
	auto			scoreboardSize = scoreboard->getTableSize();
	auto			scoreboardTable = scoreboard->getScoreboardTable();

	auto FrameInfo = entityTable->getUnitInfoById(frameIndex);
	auto Frame = FrameInfo->getUnit();
	auto FrameBase = Frame->getBase();
	auto FrameID = Frame->getID();
	auto FramePtr = FrameInfo->getBase();

	// DEFINING THEM HERE FOR AUTO TYPES
	auto TargetInfo = entityTable->getUnitInfoById(targetList.at(0));
	auto Target = TargetInfo->getUnit();
	auto TargetBase = Target->getBase();
	auto TargetID = Target->getID();

	
	for (INT targetIndex : targetList)
	{
		TargetInfo = entityTable->getUnitInfoById(targetIndex);
		Target = TargetInfo->getUnit();
		TargetBase = Target->getBase();
		TargetID = Target->getID();

		string targetName, frameName;

		// GETTING PLAYER NAMES
		if (TargetBase && FrameBase)
		{
			if (TargetID == 1065353216 || FrameID == 1065353216) // CAR
			{
				auto VehicleInfo = TargetInfo->getVehicle();
				auto Entity = VehicleInfo->getDriver();
				auto Vehicle = VehicleInfo->getVehicle();

				if (Entity->getBase()) { //PLAYER IS INSIDE

					if (VehicleInfo->getBase() == locPlayer->getBase())
						continue;

					DWORD ID = Entity->getID();
					if (ID == 1) {
						// BOT
						cout << "Target car contains a bot as a driver!" << endl;
					} else {

						for (INT l = 0; l < scoreboardSize; l++)
						{
							auto scoreboardEntity = scoreboardTable->getEntryById(l);

							if (TargetID == 1065353216) {
								if (scoreboardEntity->getID() == Entity->getID()) {
									targetName = scoreboardEntity->getString()->getString();
								}
							} else {
								if (scoreboardEntity->getID() == Target->getID()) {
									targetName = scoreboardEntity->getString()->getString();
								}
							}
							
							if (FrameID == 1065353216) {
								if (scoreboardEntity->getID() == FrameInfo->getVehicle()->getDriver()->getID()) {
									frameName = scoreboardEntity->getString()->getString();
								}
							} else {
								if (frameName.length() == 0 && scoreboardEntity->getID() == FrameID) {
									frameName = scoreboardEntity->getString()->getString();
								}
							}
						}
					}
				}
			} else { //PLAYER
				for (INT i = 0; i < scoreboardSize; i++)
				{
					auto scoreboardEntity = scoreboardTable->getEntryById(i);
					if (scoreboardEntity->getID() == TargetID) {
						targetName = scoreboardEntity->getString()->getString();
					} else if (frameName.length() == 0 && scoreboardEntity->getID() == FrameID) {
						frameName = scoreboardEntity->getString()->getString();
					}
				}
			}
		} else {
			cout << "Invalid target ID's passed! ID: " << targetIndex << endl;
			return;
		}

			cout << "Kill mode activated: TARGET:[" << targetName << "] FRAME:[" << frameName << "]" << endl;
	}

	
	TargetInfo = entityTable->getUnitInfoById(targetList.at(0));
	Target = TargetInfo->getUnit();
	TargetBase = Target->getBase();
	TargetID = Target->getID();

	bool killAll = false;
	if (targetList.at(0) == 0) {
		killAll = true;
		cout << "Kill all mode activated!" << endl;
	}

	// EVERYTHING WENT FINE, STARTING
	DWORD		targetCount = 1;
	while (*run)
	{
		if (killAll) {
			if (targetCount >= entityTableSize)
			{
				cout << "Reached the end of player table. Stopping." << endl;
				return;
			}

			for (targetCount; targetCount < entityTableSize; targetCount++)
			{
				TargetInfo = entityTable->getUnitInfoById( targetCount );
				Target = TargetInfo->getUnit();

				DWORD ID = Target->getID();
				if ( ID > 1 && ID != 1065353216 && ID != FrameID ) {

					if (Target->getPlayer()->getBase() == locPlayer->getBase())
						continue;

					if( Target->isAlive() )
						continue;

					TargetBase = Target->getBase();
					break;
				}
			}
			
			TargetBase = Target->getBase();
		} else if ( Target->isAlive() && targetCount != targetList.size()) {
			TargetInfo = entityTable->getUnitInfoById(targetList.at(targetCount));
			Target = TargetInfo->getUnit();

			if (Target->getID() == 1 || Target->getID() == 1065353216) {
				cout << "Target invalid or in car. Skipping." << endl;

				targetCount++;
				continue;
			}

			TargetBase = Target->getBase();
			cout << "Target dead, switching to target with ID: [" << targetList.at(targetCount) << "]" << endl;

			targetCount++;
		} else if ( Target->isAlive() && targetCount == targetList.size()) // REACHED LAST PLAYER
		{
			cout << "Killing completed! Stopping kill thread." << endl;
			return;
		}
		
		auto			Munition = world->getMunition();
		auto			AmmonutionSize = Munition->getTableSize();
		auto			AmmonutionTable = Munition->getTable();

		for (SIZE_T i = 0; i < AmmonutionSize; i++) {
			auto Ammo = AmmonutionTable->getAmmoById(i);
			if (!Ammo->getBase())
				continue;

			auto BltOwner = Ammo->getStats();
			auto LocBtlOwner = world->getRealPlayer();

			if (BltOwner == LocBtlOwner) { // LOCAL PLAYERS BULLET
				int reference = m->read<int>( FramePtr );

				auto targetAccel = Target->getPlayer()->getAccerelation();
				D3DXVECTOR3 TA;
				auto targetPos = Target->getPlayer()->getPos();

				D3DXVec3Normalize(&targetAccel, &targetAccel);
				D3DXVec3Subtract(&targetPos, &targetPos, &targetAccel);

				if (targetAccel != D3DXVECTOR3(0, 0, 0)) {
					//D3DXVec3Scale(&TA, &targetAccel, 1.1f);
					TA = targetAccel;
					D3DXVec3Scale(&targetAccel, &targetAccel, 1000.0f);
				} else {
					TA = D3DXVECTOR3(0,1.0f,0);
					D3DXVec3Scale(&targetAccel, &D3DXVECTOR3(0, 1.5f, 0), 1000.0f);
				}

				Ammo->setAcceleration(targetAccel);
				Ammo->setPos(targetPos - TA + D3DXVECTOR3(0, 1.0f, 0));

				Ammo->setOwner(FramePtr);
				m->write(reference, (reference + 1));
			}
		}
	}
}

void hijackVehicle(INT targetID, INT timeOut)
{
	auto			world = World::Singleton();

	if (!world->getCameraOn()->getUnit()->getBase())
		return;

	auto			locPlayer	= world->getCameraOn()->getUnit()->getPlayer();
	auto			cameraOn	= world->getCameraOn();
	if (!locPlayer->getBase())
		return;

	auto			entityTablePtr = world->getEntityTable();
	auto			entityTable = entityTablePtr->getTable();
	auto			entityTableSize = entityTablePtr->getTableSize();

	auto			networkMgr = NetworkManager::Singleton();
	auto			scoreboard = networkMgr->getScoreboard();
	auto			scoreboardSize = scoreboard->getTableSize();
	auto			scoreboardTable = scoreboard->getScoreboardTable();

	auto TargetInfo = entityTable->getUnitInfoById(targetID);
	auto Target = TargetInfo->getUnit();
	auto TargetBase = Target->getBase();
	auto TargetID = Target->getID();

	if (TargetID == 1065353216) {
		auto Vehicle = TargetInfo->getVehicle();
		auto VehicleDriver = Vehicle->getDriver();

		m->write<DWORD>( Vehicle->getBase() + 0xBA4, cameraOn->getUnit()->getBase() );

		cout << "Vehicle hijacked. Waiting for " << timeOut << " seconds!" << endl;
		std::this_thread::sleep_for( std::chrono::seconds(timeOut) );

		m->write<DWORD>( Vehicle->getBase() + 0xBA4, NULL );

	} else {
		cout << "Target is not inside a car!" << endl;
	}

	cout << "Process complete!" << endl;
}

void getScriptConsole(INT eventID) {
	auto			world = World::Singleton();
	if (!world->getCameraOn()->getUnit()->getBase())
		return;

	auto			locUnit = world->getCameraOn()->getUnit();

	auto			HandlerTable = locUnit->getEventHandlerTable();
	auto			Handlers = HandlerTable->getHandlers();

	if (!locUnit->getBase())
		return;

	for (EventHandler e : Handlers)
	{
		if (eventID == 0) {
			cout << "EventID: " << e.getEvent() << endl;
			cout << "Text: " << e.getEventText()->getStringUnformated() << endl << endl;
		} else {
			if (e.getEvent() == eventID) {
				cout << "Event found!" << endl;
				cout << "Text: ";
				cout << e.getEventText()->getStringUnformated() << endl << endl;
			}
		}
	}

	cout << "Process completed" << endl;
}

void setScriptConsole(INT eventID, string text, INT timeOut )
{
	auto			world = World::Singleton();
	if (!world->getCameraOn()->getUnit()->getBase())
		return;

	auto			locUnit = world->getCameraOn()->getUnit();

	auto			HandlerTable = locUnit->getEventHandlerTable();
	auto			Handlers = HandlerTable->getHandlers();

	if (!locUnit->getBase())
		return;

	for (EventHandler e : Handlers)
	{
		if (e.getEvent() == eventID) {
			cout << "Event found!" << endl;
			cout << "Text: ";

			string oldText = e.getEventText()->getStringUnformated();
			cout << oldText << endl;

			cout << endl << endl;

			cout << "Replaced!" << endl;
			e.setEventText( text );

			cout << "Waiting for " << timeOut << " seconds." << endl;
			std::this_thread::sleep_for(std::chrono::seconds( timeOut ));

			if (e.getEvent() != eventID) {
				cout << "Old ID was invalid, reiterating again!" << endl;
				auto newHandlers = HandlerTable->getHandlers();

				for (EventHandler e : newHandlers)
				{
					if (e.getEvent() == eventID) {
						cout << "Replacing back with old version of script!" << endl;
						e.setEventText(oldText);
					}
				}
				return;
			}

			cout << "Replacing back with old version of script!" << endl;
			e.setEventText( oldText );
		}
	}

	cout << "Process completed!" << endl;
}

// These two functions are not working, the car stays in place for some reason.
void giveCarConsole(INT targetID, INT distance)
{
	auto			world = World::Singleton();

	if (!world->getCameraOn()->getUnit()->getBase())
		return;

	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();
	auto			cameraOn = world->getCameraOn();
	auto			transData = TransData::Singleton();
	transData->refresh();

	if (!locPlayer->getBase())
		return;

	auto			entityTablePtr = world->getEntityTable();
	auto			entityTable = entityTablePtr->getTable();
	auto			objects = entityTablePtr->getObjects();

	auto			networkMgr = NetworkManager::Singleton();
	auto			scoreboard = networkMgr->getScoreboard();
	auto			scoreboardSize = scoreboard->getTableSize();
	auto			scoreboardTable = scoreboard->getScoreboardTable();

	auto TargetInfo = entityTable->getUnitInfoById(targetID);
	auto Target = TargetInfo->getUnit();
	auto TargetBase = Target->getBase();
	auto TargetID = Target->getID();
	auto _targetPos = Target->getPlayer()->getPos();

	std::unique_ptr<A3::VehicleInfo> _vehicle = nullptr;

	for (auto& _obj : objects->emptyVehicles)
	{
		auto _vehPos = _obj->getVehicle()->getPos();

		if ((INT)transData->Distance3D(_targetPos, _vehPos) < distance) {
			_vehicle = std::move(_obj);

			cout << "Giving car [" << _vehicle->getEntityName()->getString() << "] to target." << endl;
			cout << "Distance: " << (INT)transData->Distance3D(_targetPos, _vehPos) << endl;
			break;
		}
	}

	if (!_vehicle)
	{
		cout << "Couldn't find car close enough!" << endl; return;
	}


	auto _vehPos = _vehicle->getVehicle()->getPos();
	auto _vehAcc = _vehicle->getVehicle()->getAccerelation();
	auto _targetAcc = Target->getPlayer()->getAccerelation();
	_targetPos = Target->getPlayer()->getPos();

	//_vehicle->getVehicle()->setPos( _targetPos + D3DXVECTOR3(20,20,20) );
	D3DXVec3Normalize(&_targetAcc, &_targetAcc);
	D3DXVec3Subtract(&_targetPos, &_targetPos, &_targetAcc);

	D3DXVECTOR3 _newPos;
	if (_targetAcc != D3DXVECTOR3(0, 0, 0)) {
		_newPos = D3DXVECTOR3(0, 2, 0);
		D3DXVec3Scale(&_targetAcc, &_targetAcc, 10.0f);
	} else {
		_targetAcc = D3DXVECTOR3(0, 0, 0);
		_newPos = D3DXVECTOR3(3.5f, 2, 0);
	}

	_vehicle->getVehicle()->setPos(_targetPos + _targetAcc + _newPos);
}

void accelerateCarConsole( INT targetID, INT distance, bool* run )
{
	auto			world = World::Singleton();

	if (!world->getCameraOn()->getUnit()->getBase())
		return;

	auto			locPlayer		= world->getCameraOn()->getUnit()->getPlayer();
	auto			cameraOn		= world->getCameraOn();
	auto			transData		= TransData::Singleton();
	transData->refresh();

	if (!locPlayer->getBase())
		return;

	auto			entityTablePtr	= world->getEntityTable();
	auto			entityTable		= entityTablePtr->getTable();
	auto			objects			= entityTablePtr->getObjects();

	auto			networkMgr = NetworkManager::Singleton();
	auto			scoreboard = networkMgr->getScoreboard();
	auto			scoreboardSize = scoreboard->getTableSize();
	auto			scoreboardTable = scoreboard->getScoreboardTable();

	auto TargetInfo		= entityTable->getUnitInfoById(targetID);
	auto Target			= TargetInfo->getUnit();
	auto TargetBase		= Target->getBase();
	auto TargetID		= Target->getID();
	auto _targetPos		= Target->getPlayer()->getPos();

	std::unique_ptr<A3::VehicleInfo> _vehicle	= nullptr;

	for (auto& _obj : objects->emptyVehicles)
	{
		auto _vehPos	= _obj->getVehicle()->getPos();

		if ( (INT)transData->Distance3D( _targetPos, _vehPos ) < distance ) {
			_vehicle = std::move( _obj );

			cout << "Launching car [" << _vehicle->getEntityName()->getString() << "] at target." << endl;
			cout << "Distance: " << (INT)transData->Distance3D( _targetPos, _vehPos ) << endl;
			break;
		}
	}

	if( !_vehicle )
	{ cout << "Couldn't find car close enough!" << endl; return; }

	while (*run)
	{
		auto _vehPos = _vehicle->getVehicle()->getPos();
		_targetPos = Target->getPlayer()->getPos();

		D3DXVECTOR3 _newAcc;

		D3DXVec3Subtract( &_newAcc, &_targetPos, &_vehPos );
		D3DXVec3Scale( &_newAcc, &_newAcc, 3.0f );
		_newAcc.y += 0.5f;

		_vehicle->getVehicle()->setAcceleration( _newAcc );
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}