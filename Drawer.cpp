#include "Drawer.h"

using namespace A3;

void render( LPVOID args )
{
	D3D9_item*		item	= (D3D9_item*)args;
	Memory*			m		= (Memory*)item->arguments;

	auto			world		= World::Singleton();

	if( !world->getCameraOn()->getUnit()->getBase() )
		return;

	auto			locPlayer	= world->getCameraOn()->getUnit()->getPlayer();
	if( !locPlayer->getBase() )
		return;

	auto			transData	= TransData::Singleton();

	auto			entityTablePtr		= world->getEntityTable();
	auto			entityTable			= entityTablePtr->getTable();
	auto			entityTableSize		= entityTablePtr->getTableSize();

	auto			networkMgr			= NetworkManager::Singleton();
	auto			scoreboard			= networkMgr->getScoreboard();
	auto			scoreboardSize		= scoreboard->getTableSize();
	auto			scoreboardTable		= scoreboard->getScoreboardTable();

	auto			localID				= world->getCameraOn()->getUnit()->getTeam();
	string			plyrName			= "";
	transData->refresh();

	for( DWORD i = 0; i < entityTableSize; i++ ){

		auto UnitInfo = entityTable->getUnitInfoById( i );
		auto Unit	  = UnitInfo->getUnit();
		auto UnitBase = Unit->getBase();

		DWORD ID = Unit->getID();

		if( UnitBase ){
			if( ID > 1 && ID != 1065353216 && displayPlayers ){ // PLAYER
				auto TeamID		= Unit->getTeam();
				if( TeamID == localID && displayTeam )
					continue;

				auto Entity		= Unit->getPlayer();

				if( Entity->getBase() == locPlayer->getBase() )
					continue;
				 
				for (INT l = 0; l < scoreboardSize; l++) {
					auto scoreboardEntity = scoreboardTable->getEntryById(l);
					if (scoreboardEntity->getID() == ID) {
						plyrName = scoreboardEntity->getString()->getString();
						break;
					}
				}

				D3DXVECTOR3 screenCords = transData->WorldToScreen( Entity->getPos() );
				if( screenCords.z <= 0.01 )
					continue;

				float distance	= transData->Distance3D( Entity->getPos(), locPlayer->getPos() );
				if( distance > item->value )
					continue;

				plyrName += " [" + to_string( (int) distance ) + "m]";
				
				if( plyrName.length() > 200 )
					{continue;}

				if( distance > 700 ) distance = 700;
				if( distance < 5 ) distance = 15;

				float drawY		= ( 1050.0f  / distance ) / transData->Projection2.y;
				float drawX		= ( 800.0f  /  distance ) / transData->Projection1.x;

				float screenX = screenCords.x - ( drawX / 2 );
				float screenY = screenCords.y - drawY;
				
				d3d9.drawBox( (int)( screenX - ( distance / 100 ) ), (int)( screenY - ( distance / 90 ) ), (int)( drawX - ( distance / 100 ) ), (int)( drawY - ( distance / 90 ) ), D3DCOLOR_ARGB( 255, 17, 59, 245 ) ); // BOX
				d3d9.drawTextEx( plyrName.c_str(), (int)(screenCords.x - ( distance / 100.0f ) - plyrName.length( ) ), (int)( screenCords.y - ( ( drawY * 1.3f ) + ( distance / 90.0f ) ) ), D3DCOLOR_RGBA(255,0,0,255), font );

			} else if( ID == 1065353216 ){ // VEHICLE // FIX !!!

				auto VehicleInfo	= UnitInfo->getVehicle();
				auto Entity			= VehicleInfo->getDriver();
				auto Vehicle		= VehicleInfo->getVehicle();

				if( !Entity->getBase() && displayEmptyCars ){
					plyrName = VehicleInfo->getEntityName()->getString();

					D3DXVECTOR3 screenCords = transData->WorldToScreen( Vehicle->getPos( ) );
					if( screenCords.z <= 0.01 )
						continue;

					float distance = transData->Distance3D( Vehicle->getPos( ), locPlayer->getPos( ) );
					if( distance > item->value )
						continue;

					plyrName += " [" + to_string( (int) distance ) + "m]";

					if( distance > 700 ){
						distance = 700;
					} else if( distance < 5 ){
						distance = 5; }

					float drawY = ( 1050.0f / distance ) / transData->Projection2.y;
					float drawX = ( 800.0f / distance ) / transData->Projection1.x;

					float screenX = screenCords.x - ( drawX / 2 );
					float screenY = screenCords.y - drawY;

					d3d9.drawTextEx( plyrName.c_str( ), (int) ( screenCords.x - ( distance / 100.0f ) - plyrName.length( ) ), (int) ( screenCords.y - ( ( drawY * 1.3f ) + ( distance / 90.0f ) ) ), D3DCOLOR_RGBA(255,0,0,255), font ); // NAME
				} else if( Entity->getBase() && displayCars ){

					if( VehicleInfo->getDriver()->getBase() == locPlayer->getBase() )
						continue;

					ID = Entity->getID();
					if( ID == 1 ){
						plyrName = "AI";
					} else {
						for( INT l = 0; l < scoreboardSize; l++ ){
							auto scoreboardEntity = scoreboardTable->getEntryById( l );

							if( scoreboardEntity->getID() == ID ){
								plyrName = scoreboardEntity->getString()->getString();
								break;
							}
						}
					}
						

					D3DXVECTOR3 screenCords = transData->WorldToScreen( Vehicle->getPos() );
					if( screenCords.z <= 0.01 )
						continue;

					float distance = transData->Distance3D( Vehicle->getPos(), locPlayer->getPos() );
					if( distance > item->value )
						continue;

					plyrName += " [" + to_string( (int)distance ) + "m]";
					

					if( distance > 700 ) distance = 700;
					if( distance < 5 ) distance = 15;

					float drawY = ( 1050.0f / distance ) / transData->Projection2.y;
					float drawX = ( 800.0f / distance ) / transData->Projection1.x;

					float screenX = screenCords.x - ( drawX / 2 );
					float screenY = screenCords.y - drawY;


					string carType = VehicleInfo->getEntityName()->getString();

					d3d9.drawTextEx( plyrName.c_str(), (int) ( screenCords.x - ( distance / 100.0f ) - plyrName.length() ), (int) ( screenCords.y - ( ( drawY * 1.3f ) + ( distance / 90.0f )) ), D3DCOLOR_RGBA( 255, 0, 0, 255 ), font );
					d3d9.drawTextEx( carType.c_str( ), (int) ( screenCords.x - ( distance / 100 ) - carType.length( ) ), (int) ( screenCords.y - ( ( drawY * 1.3f ) + ( distance / 90.0f ) + 12 ) ), D3DCOLOR_RGBA( 0, 255, 0, 255 ), font );
				}
				
			}
		}
	}

	Sleep( 10 );
}

// Second render function that is cleaner but uses more CPU cycles (too lazy to fix it, look at Objects class in Engine.h to fix it).
void render2(LPVOID args)
{
	D3D9_item*		item = (D3D9_item*)args;
	Memory*			m = (Memory*)item->arguments;

	auto			world = World::Singleton();

	if (!world->getCameraOn()->getUnit()->getBase())
		return;

	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();
	if (!locPlayer->getBase())
		return;

	auto			transData = TransData::Singleton();

	auto			entityTable = world->getEntityTable();
	auto			_objects	= entityTable->getObjects();

	auto			networkMgr = NetworkManager::Singleton();
	auto			scoreboard = networkMgr->getScoreboard();
	auto			scoreboardSize = scoreboard->getTableSize();
	auto			scoreboardTable = scoreboard->getScoreboardTable();

	auto			localID = world->getCameraOn()->getUnit()->getTeam();
	transData->refresh();

	if (displayPlayers)
	{
		for(auto& _obj : _objects->players) {
			auto _name	= _obj->getName();
			auto _team	= _obj->getTeam();

			if( displayTeam && _team == localID ) // displayTeammates
				continue;
			
			D3DXVECTOR3 screenCords = transData->WorldToScreen( _obj->getPlayer()->getPos() );
			if (screenCords.z <= 0.01)
				continue;

			float distance = transData->Distance3D(_obj->getPlayer()->getPos(), locPlayer->getPos());
			if (distance > item->value)
				continue;

			_name += " [" + to_string((int)distance) + "m]";

			if (_name.length() > 200)
			{
				continue;
			}

			if (distance > 700) distance = 700;
			if (distance < 5) distance = 15;

			float drawY = (1050.0f / distance) / transData->Projection2.y;
			float drawX = (800.0f / distance) / transData->Projection1.x;

			float screenX = screenCords.x - (drawX / 2);
			float screenY = screenCords.y - drawY;

			d3d9.drawBox((int)(screenX - (distance / 100)), (int)(screenY - (distance / 90)), (int)(drawX - (distance / 100)), (int)(drawY - (distance / 90)), D3DCOLOR_ARGB(255, 17, 59, 245)); // BOX
			d3d9.drawTextEx( _name.c_str(), (int)(screenCords.x - (distance / 100.0f) - _name.length()), (int)(screenCords.y - ((drawY * 1.3f) + (distance / 90.0f))), D3DCOLOR_RGBA(255, 0, 0, 255), font );
		}
	}
	if (displayCars)
	{
		for (auto& _obj : _objects->vehPlayers) {
			auto _name	= _obj->getDriver()->getName();
			auto _type	= _obj->getEntityName()->getString();

			D3DXVECTOR3 screenCords = transData->WorldToScreen( _obj->getVehicle()->getPos() );
			if (screenCords.z <= 0.01)
				continue;

			float distance = transData->Distance3D( _obj->getVehicle()->getPos(), locPlayer->getPos() );
			if (distance > item->value)
				continue;

			_name += " [" + to_string((int)distance) + "m]";


			if (distance > 700) distance = 700;
			if (distance < 5) distance = 15;

			float drawY = (1050.0f / distance) / transData->Projection2.y;
			float drawX = (800.0f / distance) / transData->Projection1.x;

			float screenX = screenCords.x - (drawX / 2);
			float screenY = screenCords.y - drawY;

			d3d9.drawTextEx(_name.c_str(), (int)(screenCords.x - (distance / 100.0f) - _name.length()), (int)(screenCords.y - ((drawY * 1.3f) + (distance / 90.0f))), D3DCOLOR_RGBA(255, 0, 0, 255), font);
			d3d9.drawTextEx(_type.c_str(), (int)(screenCords.x - (distance / 100) - _type.length()), (int)(screenCords.y - ((drawY * 1.3f) + (distance / 90.0f) + 12)), D3DCOLOR_RGBA(0, 255, 0, 255), font);

		}
		for (auto& _obj : _objects->AIVehicles) {
			auto _name	= string("AI");
			auto _type	= _obj->getEntityName()->getString();

			D3DXVECTOR3 screenCords = transData->WorldToScreen(_obj->getVehicle()->getPos());
			if (screenCords.z <= 0.01)
				continue;

			float distance = transData->Distance3D(_obj->getVehicle()->getPos(), locPlayer->getPos());
			if (distance > item->value)
				continue;

			_name += " [" + to_string((int)distance) + "m]";


			if (distance > 700) distance = 700;
			if (distance < 5) distance = 15;

			float drawY = (1050.0f / distance) / transData->Projection2.y;
			float drawX = (800.0f / distance) / transData->Projection1.x;

			float screenX = screenCords.x - (drawX / 2);
			float screenY = screenCords.y - drawY;

			d3d9.drawTextEx(_name.c_str(), (int)(screenCords.x - (distance / 100.0f) - _name.length()), (int)(screenCords.y - ((drawY * 1.3f) + (distance / 90.0f))), D3DCOLOR_RGBA(255, 0, 0, 255), font);
			d3d9.drawTextEx(_type.c_str(), (int)(screenCords.x - (distance / 100) - _type.length()), (int)(screenCords.y - ((drawY * 1.3f) + (distance / 90.0f) + 12)), D3DCOLOR_RGBA(0, 255, 0, 255), font);

		}
	}
	if (displayEmptyCars)
	{
		for (auto& _obj : _objects->emptyVehicles) {
			auto _name = _obj->getEntityName()->getString();

			D3DXVECTOR3 screenCords = transData->WorldToScreen( _obj->getVehicle()->getPos() );
			if (screenCords.z <= 0.01)
				continue;

			float distance = transData->Distance3D( _obj->getVehicle()->getPos(), locPlayer->getPos() );
			if (distance > item->value)
				continue;

			_name += " [" + to_string((int)distance) + "m]";

			if (distance > 700) {
				distance = 700;
			} else if (distance < 5) {
				distance = 5;
			}

			float drawY = (1050.0f / distance) / transData->Projection2.y;
			float drawX = (800.0f / distance) / transData->Projection1.x;

			float screenX = screenCords.x - (drawX / 2);
			float screenY = screenCords.y - drawY;

			d3d9.drawTextEx( _name.c_str(), (int)(screenCords.x - (distance / 100.0f) - _name.length()), (int)(screenCords.y - ((drawY * 1.3f) + (distance / 90.0f))), D3DCOLOR_RGBA(255, 0, 0, 255), font); // NAME
		}
	}
}