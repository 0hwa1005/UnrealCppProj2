
#include "CGameMode.h"
#include "Global.h"

ACGameMode::ACGameMode()
{
	CHelpers::GetClass<APawn>(&DefaultPawnClass, 
		"Blueprint'/Game/Player/BP_CPlayer.BP_CPlayer_C'"); 
	//CHelpers::GetClass<APawn>(&DefaultPawnClass,
	//	"Blueprint'/Game/Player/CPlayer_Test.CPlayer_Test_C'") ;
}