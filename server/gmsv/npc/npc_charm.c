#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_charm.h"

//魅力代を導く計算式は

//レベル＊ＲＡＴＥ＊（現在の魅力/ＷＡＲＵ）

//回復量は　５です。
/*
#define RATE  4		//レート？
#define CHARMHEAL 5 //魅力の回復量
#define WARU	3	//魅力を割る値

*/

#define RATE  10	//レート？
#define CHARMHEAL 5 //魅力の回復量
#define WARU	3	//魅力を割る値


static void NPC_Charm_selectWindow( int meindex, int toindex, int num);
int NPC_CharmCost(int meindex,int talker);
void NPC_CharmUp(int meindex,int talker);


/*********************************
* 初期処理
*********************************/
BOOL NPC_CharmInit( int meindex )
{
	/*--キャラのタイプを設定--*/
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPECHARM );
	return TRUE;

}


/*********************************
* 話しかけられた時の処理
*********************************/
void NPC_CharmTalked( int meindex , int talkerindex , char *szMes ,int color )
{

    /* プレイヤーに対してだけ反応する */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    {
    	return;
    }
	
	/*--目の前にいるかどうか？--*/
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		/* １グリッド以内のみ */
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}

	NPC_Charm_selectWindow( meindex, talkerindex, 0);
}


/*
 * 各処理に分ける
 */
static void NPC_Charm_selectWindow( int meindex, int toindex, int num)
{

	char token[1024];
	char escapedname[1024];
	int fd = getfdFromCharaIndex( toindex);
	int buttontype = 0;
	int windowtype = 0;
	int windowno = 0;
	int cost = 0;
	int chartype;
	
	/*--ウインドウタイプメッセージがおおいので先に設定--*/
  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	switch( num) {
	  case 0:
  		/*--選択画面--*/
		sprintf(token,"4\n �@�@�@   = = ���e�v = = "
				     "\n\n �@�w�錺��{�Aさぱ�n圧ぐ賜�O�H"
				  "\n "
				  "\n\n�@�@�@�@�m�@宜и�y�O�@�n "
				  "\n\n�@�@�@�@�m ぐ賜�]ぃ圧 �n "
		);

	  	buttontype = WINDOW_BUTTONTYPE_NONE;
	  	windowtype = WINDOW_MESSAGETYPE_SELECT;
	  	windowno = CHAR_WINDOWTYPE_CHARM_START; 
	  	break;

	case 1:
		cost = NPC_CharmCost( meindex, toindex);
		if(cost == -1){
			sprintf(token,"�@�@�@�@     = = ���e�v = =  "
				      "\n\n�@�@�@�@�A�詐y�O�u�OЧ��"
				      "\n\n�@�@�@出ッ桟�O���A�詐y�O�髻C"
			);
		  	buttontype = WINDOW_BUTTONTYPE_OK;
		}else{
			sprintf(token,"�@�@�@�@     = = ���e�v = =  "
				 	    "\n�@�甓{�b,�d�H�S�f児濠�Iい��"
					  "\n\n �n�N�A�詐y�O�Wどき�I�左�"
					  "\n�@�@�@�@旨�n%6d��stone�髻I"
						"\n\n�@�@�@�Y�漏o舎�]�i�H芹�H",cost
			);
		  	buttontype = WINDOW_BUTTONTYPE_YESNO;

		}
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno = CHAR_WINDOWTYPE_CHARM_END; 

		break;

	case 2:
		cost = NPC_CharmCost( meindex, toindex);
		chartype = CHAR_getInt( toindex, CHAR_IMAGETYPE);
		
		if(cost > CHAR_getInt( toindex, CHAR_GOLD)) {
			sprintf(token,"�@�@�@�@     = = ���e�v = =  "
					  "\n\n�@�@ �u�r!�A�鎖�ぃ員�髻I"
				        "\n�@�@    ���h�s�n椙�UΩ�A�哥a�I"
			);

		}else{
			NPC_CharmUp( meindex, toindex);

			/*--キャラのタイプによってメッセージを変えてみた--*/
			switch( chartype) {
			  case CHAR_IMAGETYPE_GIRL:
				sprintf(token,"�@�@�@�@     = = ���e�v = =  "
					  "\n\n �@金,�o舎�A�詐y�O�N�Wど�o�I"
					  "\n\n�@�@�@�U�啖U�i�R�F�O�I"
				);

				break;
			  case CHAR_IMAGETYPE_BOY:
				sprintf(token,"�@�@�@�@     = = ���e�v = =  "
					  "\n\n �@金,�o舎�A�詐y�O�N�Wど�o�I"
					  "\n\n�@�@�Oぃ�Oゑ�茲~�n�哘h�F�O�H"
				);
			  	break;
			  	
			  case CHAR_IMAGETYPE_CHILDBOY:
			  case CHAR_IMAGETYPE_CHILDGIRL:
				sprintf(token,"�@�@�@�@     = = ���e�v = =  "
					  "\n\n �@金,�o舎�A�詐y�O�N�Wど�o�I"
					  "\n\n�@�@  �u�催椡o�n�i�R�髻I"
				);
			 	break;
			 	
			   case CHAR_IMAGETYPE_MAN:
				sprintf(token,"�@�@�@�@     = = ���e�v = =  "
					  "\n\n �@金,�o舎�A�詐y�O�N�Wど�o�I"
					  "\n\n �@�@�@怒�o�鷸鼎F�O�I"
				);
			 	break;
			 	
			   case CHAR_IMAGETYPE_WOMAN:
				sprintf(token,"�@�@�@�@     = = ���e�v = =  "
				  "\n\n �@金,�o舎�A�詐y�O�N�Wど�o�I"
				  "\n\n    �i�u�O怒�o�U�啖U���F�O�I"
				);
			 	break;
			 
			 }
		}

		buttontype = WINDOW_BUTTONTYPE_OK;
		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno = CHAR_WINDOWTYPE_CHARM_END; 
		break;
	}
	
	makeEscapeString( token, escapedname, sizeof( escapedname));
	/*-ここで送信する--*/
	lssproto_WN_send( fd, windowtype, 
					buttontype, 
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					escapedname);

}


/*-----------------------------------------
クライアントから返ってきた時に呼び出される。
-------------------------------------------*/
void NPC_CharmWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) return;

	switch( seqno){
	  case CHAR_WINDOWTYPE_CHARM_START:
	  	if(atoi( data) == 2) {
			NPC_Charm_selectWindow( meindex, talkerindex, 1 );
		}
		break;

	  case CHAR_WINDOWTYPE_CHARM_END:
	  	if(select == WINDOW_BUTTONTYPE_YES) {
			NPC_Charm_selectWindow( meindex, talkerindex, 2 );
		}
		break;
	}
	
}



/*--魅力ＵＰ--*/
void NPC_CharmUp(int meindex,int talker)
{
	int cost;
	int i;
	int petindex;
	char petsend[64];	

	/*--お金を減らしましょう--*/
	cost = NPC_CharmCost( meindex, talker);
	CHAR_setInt( talker, CHAR_GOLD,
			CHAR_getInt( talker, CHAR_GOLD) - cost);
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);

	/*--魅力が１００以上になる場合は強引に１００にする--*/
	if(CHAR_getInt( talker, CHAR_CHARM) + CHARMHEAL >= 100) {
		CHAR_setInt( talker, CHAR_CHARM, 100);
	}else{
		/*--魅力をセット--*/
		CHAR_setInt(talker, CHAR_CHARM,
	 			(CHAR_getInt( talker, CHAR_CHARM) + CHARMHEAL));
	}
	
	/*--ステータスの更新--*/
	CHAR_complianceParameter( talker );
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_CHARM);


	/*--ペットのパラメータを更新--*/
	for( i = 0 ; i < CHAR_MAXPETHAVE ; i++){
    	petindex = CHAR_getCharPet( talker, i);

		if( petindex == -1  )  continue;

	   /*  キャラの配列チェック    */
		if( !CHAR_CHECKINDEX( talker ) )  continue;

		/*--パラメータ調整--*/
		CHAR_complianceParameter( petindex );
		sprintf( petsend, "K%d", i );
		CHAR_sendStatusString( talker , petsend );
	}
}


/*--お金の計算--*/
int NPC_CharmCost(int meindex,int talker)
{
	int cost;
	int level;
	int charm;
	int trans;

	level = CHAR_getInt( talker, CHAR_LV);
	charm = CHAR_getInt( talker, CHAR_CHARM);
	trans = CHAR_getInt( talker, CHAR_TRANSMIGRATION);

	if(charm >= 100) return -1;
	
	if(charm <= 1) charm = WARU;
	
	/*-- 計算式 --*/
	cost = level * RATE * (charm / WARU) * (trans+1);

	return cost;

}
