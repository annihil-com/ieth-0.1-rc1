// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

/*
==============================
For all fx/informations show on screen.
==============================
*/

#include "ieth.h"

vec4_t gameBackColor = { 0.0, 0.0, 0.0, 0.75 };
vec4_t gameBorderColor = { 0.255f, 0.255f, 0.255f, 0.8f };
vec4_t gameFontColor = { 0.255f, 0.255f, 0.255f, 1.0f };

static ethPlayerColor_t playersColorsDef[] = {
	// Type 1
	{
		{ GFX_BLUE,		colorBlue },
		{ GFX_GREEN,		colorGreen },
		{ GFX_RED,		colorRed },
		{ GFX_YELLOW,		colorYellow },
	},
	// Type 2
	{
		{ GFX_BLUE,		colorBlue },
		{ GFX_GREEN,		colorGreen },
		{ GFX_YELLOW,		colorYellow },
		{ GFX_RED,		colorRed },
	},
	// Type 3
	{
		{ GFX_NONE,		colorWhite },
		{ GFX_NONE,		colorMdGrey },
		{ GFX_RED,		colorRed },
		{ GFX_TEAL,		colorMagenta }
	}
};

// Update colors from vars
void setColors() {
	// Sanity check
	if ((seth.value[VAR_COLORS] < 0) || (seth.value[VAR_COLORS] >= (sizeof(playersColorsDef) / sizeof(ethPlayerColor_t)))) {
		ethLog("visual: bad color %i. reset to default", seth.value[VAR_COLORS]);
		seth.value[VAR_COLORS] = seth.vars[VAR_COLORS].defaultValue;
	}

	// Sanity check
	if ((seth.value[VAR_CHAMS] < 0) || (seth.value[VAR_CHAMS] >= CHAMS_TOTAL)) {
		ethLog("visual: bad chams type %i. reset to default", seth.value[VAR_CHAMS]);
		seth.value[VAR_CHAMS] = seth.vars[VAR_CHAMS].defaultValue;
	}

	qhandle_t chams = seth.value[VAR_CHAMS];

	// Specials settings on some chams
	if (chams == CHAM_GLOWONLY)
		chams = CHAM_GLOW;
	
	ethPlayerColor_t *playerColor = &playersColorsDef[(int)seth.value[VAR_COLORS]];

	// Friend visible
	eth.friendVisibleShader = eth.shaders[playerColor->friendVisible.chams][chams];
	VectorCopy(playerColor->friendVisible.esp, eth.friendVisibleColor);

	// Enemy visible
	eth.enemyVisibleShader = eth.shaders[playerColor->enemyVisible.chams][chams];
	VectorCopy(playerColor->enemyVisible.esp, eth.enemyVisibleColor);

	// Specials settings on some chams
	if (chams == CHAM_QUAKEC)
		chams = CHAM_QUAKE;

	// Friend not visible
	eth.friendNotVisibleShader = eth.shaders[playerColor->friendNotVisible.chams][chams];
	VectorCopy(playerColor->friendNotVisible.esp, eth.friendNotVisibleColor);

	// Enemy not visible
	eth.enemyNotVisibleShader = eth.shaders[playerColor->enemyNotVisible.chams][chams];
	VectorCopy(playerColor->enemyNotVisible.esp, eth.enemyNotVisibleColor);
}
// For players and missile only
void addChams(refEntity_t* refEnt, qboolean visible) {
	// Specials settings on some chams
	if (seth.value[VAR_CHAMS] == CHAM_GLOW) {
		syscall_CG_R_AddRefEntityToScene(refEnt);
	} else if (seth.value[VAR_CHAMS] == CHAM_QUAKEC) {
	    refEntity_t original = *refEnt;
	    original.customShader = eth.whiteFlatShader;
		
		if ((original.entityNum < MAX_CLIENTS) && !IS_PLAYER_ENEMY(original.entityNum)) {
			original.shaderRGBA[2] = 255;
			original.shaderRGBA[3] = 200;           
		} else {
			// treat all non-players as enemy entities.
			original.shaderRGBA[0] = 255;
			original.shaderRGBA[3] = 200;           
		}
	    
	    syscall_CG_R_AddRefEntityToScene(&original);
	}
	
	float addRadius = 0;
	float addIntens = 0;
	float *dlightColor = 0;
	vec3_t dlightorg;
	
if (seth.value[VAR_CHAMS] == CHAM_PORTAL) {

        if (IS_PLAYER_ENEMY(refEnt->entityNum)) {
                if (visible)
                        refEnt->customShader = eth.quakeportalA;
                else
                        refEnt->customShader = eth.quakeportalB;

                        dlightColor = (float *)&eth.friendVisibleColor;
                if (seth.value[VAR_DLIGHT] == DLIGHT_ON) {
                        addRadius = 5.0;
                        addIntens = 5.0;
                }
        } else {
		// treat all non-players as enemy entities.
		if (visible) 
			refEnt->customShader = eth.friendVisibleShader;
		else
			refEnt->customShader = eth.friendNotVisibleShader;
			dlightColor = (float *)&eth.enemyVisibleColor;
		if (seth.value[VAR_DLIGHT] == DLIGHT_ON) {
			addRadius = 10.0;
			addIntens = 10.0;
		}
	}

} else {

	if (!IS_PLAYER_ENEMY(refEnt->entityNum)) {
		if (visible)
			refEnt->customShader = eth.friendVisibleShader;
		else
			refEnt->customShader = eth.friendNotVisibleShader;

			dlightColor = (float *)&eth.friendVisibleColor;
		if (seth.value[VAR_DLIGHT] == DLIGHT_ON) {
			addRadius = 5.0;
			addIntens = 5.0;
		}
	} else {
		// treat all non-players as enemy entities.
		if (visible) 
			refEnt->customShader = eth.enemyVisibleShader;
		else
			refEnt->customShader = eth.enemyNotVisibleShader;
			dlightColor = (float *)&eth.enemyVisibleColor;
		if (seth.value[VAR_DLIGHT] == DLIGHT_ON) {
			addRadius = 10.0;
			addIntens = 10.0;
		}
	}
}
	//main head-up dlight
	if (seth.value[VAR_DLIGHT] == DLIGHT_ON) {
	    VectorCopy(refEnt->origin,dlightorg);
	    dlightorg[ROLL] += 40;
	    syscall_R_AddLightToScene(dlightorg,100.0 + addRadius,50.0 + addIntens,dlightColor[0],dlightColor[1],dlightColor[2], refEnt->customShader,REF_FORCE_DLIGHT);
	}
	//legs dlight tune up
    	if (seth.value[VAR_MORE_DLIGHT] == MORE_DLIGHT_ON) {
	    VectorCopy(refEnt->origin,dlightorg);
	    dlightorg[ROLL] -= 30;
	    syscall_R_AddLightToScene(dlightorg,100.0 + addRadius,20.0 + addIntens,dlightColor[0],dlightColor[1],dlightColor[2], refEnt->customShader,REF_FORCE_DLIGHT);
	}
	// crazy dlight tune up	
	if (seth.value[VAR_MORE_DLIGHT] == MORE_DLIGHT_CRAZY) {
	    vec3_t dlaxis[3];
	    vec3_t dlangles[MAX_GENTITIES];
	    //dlights angle backup
	    static float lastangle[MAX_GENTITIES];
	    //get angles of current ent
	    AxisToAngles( refEnt->axis, dlangles[refEnt->entityNum] );
	    // increase deg using backup 		    
	    dlangles[refEnt->entityNum][1] = lastangle[refEnt->entityNum] + 2.0f;
	    // backup
	    lastangle[refEnt->entityNum] = dlangles[refEnt->entityNum][1];
	    if (lastangle[refEnt->entityNum] > 360.0f) lastangle[refEnt->entityNum] = -360.0f;
	    // create axis to move it forward
	    AnglesToAxis( dlangles[refEnt->entityNum],dlaxis );
            VectorMA(refEnt->origin, 50, dlaxis[0], dlightorg);
	    // draw rail and light
#ifdef ETH_DEBUG
	    if(seth.value[VAR_DLIGHT_CRAZY] == qtrue)
		eth.CG_RailTrail2(NULL, refEnt->origin, dlightorg);
#endif
	    syscall_R_AddLightToScene(dlightorg,80.0 + addRadius,540.0 + addIntens,dlightColor[0],dlightColor[1],dlightColor[2], refEnt->customShader,REF_FORCE_DLIGHT);
	}
	// simple draw "light shadow"  using decals
	if (seth.value[VAR_MORE_DLIGHT] == MORE_DLIGHT_DECAL) {
	    vec4_t projection; // facing direction 
	    vec4_t dlightCol; // color of decal 
	    vec3_t dlightOrigin; // origin - ground counted with cgtrace
	    trace_t dlightTrace; 

	    VectorCopy(refEnt->origin,dlightOrigin);
		    
	    dlightOrigin[ROLL] -= 666;
	    eth.CG_Trace(&dlightTrace,refEnt->origin,0,0,dlightOrigin,-1,MASK_MISSILESHOT);
	    VectorSet(projection, 0 , 0 , -1);
	    projection[3] = 70.0f;
	    VectorCopy(dlightColor,dlightCol);
	    dlightCol[3] = 0.4f;
	    syscall_R_ProjectDecal(eth.dlightShader[0],1,(vec3_t *)dlightTrace.endpos,projection,dlightCol,10,1);

	}
    
}

void addEspColors(int entityNum, qboolean visible) {
	if (!IS_PLAYER_ENEMY(entityNum)) {
		if (visible)
			eth.entities[entityNum].espColor = (float *)&eth.friendVisibleColor;
		else
			eth.entities[entityNum].espColor = (float *)&eth.friendNotVisibleColor;
	} else {
		if (visible)
			eth.entities[entityNum].espColor = (float *)&eth.enemyVisibleColor;
		else
			eth.entities[entityNum].espColor = (float *)&eth.enemyNotVisibleColor;
	}
}

eth_window_t advertwin;
void drawAdvert() {
	if( eth.isMenuOpen )
		return;	
	#define ADVERT_SIZE 0.38f
	#define ADVERT_Y 50
	char str[] = ETH_VERSION "-nix*";
	//create window
	createWindow(&advertwin, "advertwin");
// -- can't be defined as static even if stored in cfg	
	Vector4Copy(colorLtGrey, advertwin.bgColor);
	advertwin.bgColor[3] = 0.25f;
	advertwin.windowH = sizeTextHeight(ADVERT_SIZE,&eth.fontFixed);
	advertwin.windowW = sizeTextWidth(ADVERT_SIZE, str, &eth.fontFixed);
	if (!advertwin.posX) // this is called only once 
	    advertwin.posX = 320 - ( advertwin.windowW / 2);
// ---
	getWindowStatus(&advertwin);
	
	drawTextRect(advertwin.posX, advertwin.posY, ADVERT_SIZE, str, colorRed, qfalse, qtrue, qtrue, qtrue, advertwin.bgColor, colorBlack, &eth.fontFixed);
}

void drawBanner() {
	if( eth.isMenuOpen )
		return;
	
	vec4_t backColor = { 0.0f, 0.0f, 0.0f, 0.3f };
	vec4_t fontColor = { 0.625f, 0.625f, 0.6f, 1.0f };
	char gameMod[20];
	syscall_CG_Cvar_VariableStringBuffer("fs_game", gameMod, sizeof(gameMod));

	char stats[628];
	strncpy(stats,bnFormat,sizeof(stats));
	Format(stats);

	int w = eth_CG_Text_Width_Ext(stats, 0.20f, 0, &eth.fontFixed);
	drawFillRect(8, 8, w + 9, 16 + 2, backColor);
	eth_CG_Text_Paint_Ext(10, 12, 0.20f, 0.20f, fontColor, stats, 0, 0, 0, &eth.fontFixed);
}

// now it's window compat.
qboolean isRadar(int player, int *screen, float scale,int winX, int winY) {
	vec3_t vec,rot;
	float distance;

	if(IS_PLAYER_DEAD(player) || eth.clientInfo[player].team == TEAM_SPECTATOR || player == eth.cg_snap->ps.clientNum)
		return qfalse;

	if (!eth.entities[player].distance)
		return qfalse;

	VectorSubtract(eth.entities[player].origin, eth.cg_refdef.vieworg, vec);
	vec[2] = 0.0f;
	distance = VectorLength(vec) * 0.025;
	vectoangles(vec, rot);
	rot[1] = AngleNormalize180(rot[1] - eth.cg_refdefViewAngles[1]);
	AngleVectors(rot, vec, NULL, NULL);
	VectorScale(vec, distance, vec);

    if(vec[0] > scale)
       VectorScale(vec, scale / vec[0], vec);
    else if(vec[0] < -scale)
       VectorScale(vec, -scale / vec[0], vec);
    if(vec[1]>100.0f)
       VectorScale(vec, scale / vec[1], vec);
    else if(vec[1] < -scale)
       VectorScale(vec, -scale / vec[1], vec);

    screen[0] = ((int)-vec[1] + (winX + scale)) - 2;
    screen[1] = ((int)-vec[0] + (winY + scale)) - 2;
	return qtrue;
	
}

//windowed radar
eth_window_t radar2win;
void drawRadar(int scale) {
//         vec4_t radarcolor = { 0.0f, 0.0f, 0.0f, 0.35f };
	vec4_t colorBlackR = { 0.7f, 0.7f, 0.7f, 0.50f };

	createWindow(&radar2win,"radar2win");
	radar2win.windowW = scale;
	radar2win.windowH = scale;
	
	//dirty hax but easier this way 
	radar2win.posX -= scale;
	getWindowStatus(&radar2win);
	radar2win.posX += scale;
	

	drawFillRect(radar2win.posX-scale, radar2win.posY,scale, scale, radar2win.bgColor);
	drawFillRect(radar2win.posX-((scale/4)*3), radar2win.posY, 1, scale, colorBlackR);
	drawFillRect(radar2win.posX-(scale/4), radar2win.posY, 1, scale, colorBlackR);
	drawFillRect(radar2win.posX-scale, radar2win.posY + scale/4, scale, 1, colorBlackR);
	drawFillRect(radar2win.posX-scale, radar2win.posY + (scale/4)*3, scale, 1, colorBlackR);
	drawFillRect(radar2win.posX-(scale/2), radar2win.posY , 1, scale, colorBlack);
	drawFillRect(radar2win.posX-scale, radar2win.posY + scale/2, scale, 1, colorBlack);

	int screen[2];
	int player;
	for (player=0; player < MAX_CLIENTS; player++) {
		if (eth.clientInfo[player].infoValid && isRadar(player, screen,(float)scale/2,radar2win.posX - scale, radar2win.posY)) {
			vec4_t color;
			if (eth.entities[player].espColor)
				VectorCopy(eth.entities[player].espColor, color);
			
			color[3] = 1;
			// added in order not to cross radar area
			if (screen[0] < (radar2win.posX - scale) + 5)
				screen[0] = (radar2win.posX - scale) + 5;
			if (screen[0] > (radar2win.posX - 5))
				screen[0] = (radar2win.posX - 5);
			if (screen[1] > (radar2win.posY + scale) - 5)
				screen[1] = (radar2win.posY + scale) - 5;
			if (screen[1] < (radar2win.posY + 5))
				screen[1] = radar2win.posY + 5;

			syscall_CG_R_SetColor(color);
			drawPic(screen[0] - 2, screen[1] - 2, 10, 10, eth.classIcons[eth.clientInfo[player].cls]);
			syscall_CG_R_SetColor(NULL);
	    }
	}
}

//windowed radarNG
eth_window_t radarwin;
void drawRadarNg(int scale) { // no draw pic radar type - saw on etbot/ubot demos
// 	vec4_t radarcolor = { 0.0f, 0.0f, 0.0f, 0.6f };	
	vec4_t borderColor = { 0.16f, 0.17f, 0.17f, 0.8f };	

	//create window - auto return when created before
	createWindow(&radarwin,"radarwinNG");
	//store scale every time window is displayed	
	radarwin.windowW = scale;
	radarwin.windowH = scale;
	//dirty hax to support scale code in isRadar
	radarwin.posX -= scale;
	
	getWindowStatus(&radarwin);
	

	drawFillRect(radarwin.posX, radarwin.posY, scale, scale, radarwin.bgColor);
	drawRect(radarwin.posX , radarwin.posY, scale, scale, borderColor,2);
	int screen[2];
	int player;
	for (player=0; player < MAX_CLIENTS; player++) {
		if (eth.clientInfo[player].infoValid && isRadar(player, screen,(float)scale/2, radarwin.posX, radarwin.posY)) {
			vec4_t color;
			if (eth.entities[player].espColor)
				VectorCopy(eth.entities[player].espColor, color);
			color[3] = 1;

			// added in order not to cross radar area
            if (screen[0] < radarwin.posX + 5)
                screen[0] = radarwin.posX + 5;
            if (screen[0] > radarwin.posX + (scale - 5))
                screen[0] = radarwin.posX + (scale - 5);
            if (screen[1] > radarwin.posY + (scale - 5))
                screen[1] = radarwin.posY + (scale - 5);
            if (screen[1] < radarwin.posY + 5)
                screen[1] = radarwin.posY + 5;
		    drawFillRect(screen[0],screen[1],4,4,color);
		}
	}
	//dirty hax 
	radarwin.posX += scale;

}

// Speclist window
eth_window_t specwin;
void drawSpectators() {
	#define SPECBOX_X 10
	#define SPECBOX_Y 150
	#define SPECBOX_FONT_SIZE 0.19f
	#define SPECBOX_SPACE 5
	#define SPECBOX_BACKCOLOR (vec4_t){ 0.0f, 0.0f, 0.0f, 0.3f }
	#define SPECBOX_BORDERCOLOR colorBlack
	#define SPECBOX_TEXT "Spectator(s):"

	// initialize window
	createWindow(&specwin,"specwin");

	// If no spectator
	if (seth.specCount == 0)
		return;
	
	// Get max width
	int max = 0;
	int count = 0;
	for (; count < seth.specCount; count++) {
		int w = sizeTextWidth(SPECBOX_FONT_SIZE, seth.specNames[count], &eth.fontVariable);
		if (w > max)
			max = w;
	}
	if (sizeTextWidth(SPECBOX_FONT_SIZE, SPECBOX_TEXT, &eth.fontVariable) > max)
		max = sizeTextWidth(SPECBOX_FONT_SIZE, SPECBOX_TEXT, &eth.fontVariable);

	int width = max + (2 * SPECBOX_SPACE);
	specwin.windowW = width;
	int lineHeight = sizeTextHeight(SPECBOX_FONT_SIZE, &eth.fontVariable) + SPECBOX_SPACE;
	int height = ((lineHeight * (seth.specCount + 1)) + SPECBOX_SPACE);
	int y = specwin.posY + SPECBOX_SPACE;
	
	drawFillRect(specwin.posX, specwin.posY, specwin.windowW, specwin.windowH, specwin.bgColor);
	drawRect(specwin.posX, specwin.posY, width, height, SPECBOX_BORDERCOLOR, 1);
	drawText(SPECBOX_SPACE + specwin.posX, y, SPECBOX_FONT_SIZE, SPECBOX_TEXT, colorRed, qfalse, qtrue, &eth.fontVariable);
	y += lineHeight;
	
	for (count = 0; count < seth.specCount; count++) {
		drawText(SPECBOX_SPACE + specwin.posX, y, SPECBOX_FONT_SIZE, seth.specNames[count], colorWhite, qfalse, qtrue, &eth.fontVariable);
		y += lineHeight;
	}

	specwin.windowH = y - specwin.posY;
	getWindowStatus(&specwin);
}

void drawPS () {
	char str[250];
//	sprintf(str,"v1:[ %.4f ] v2:[ %.4f ] t1:[ %d ] t2:[ %d ] acc:[ %.4f ]",predict.speed_a,predict.speed_b,(int)predict.time_a,(int)predict.time_b,predict.acceleration);
	sprintf(str," [ %.4f ]",eth.predMissed);
	
	if(eth.predMissed < 100)
	    drawFillRect(5, 5, 20, eth.predMissed, colorGreen);
	else 
	    drawFillRect(5, 5, 20, 100, colorRed);

	drawTextRect(5, 5, 0.2f, str, colorWhite, qfalse, qtrue, qtrue, qtrue, colorBlack, colorBlack, &eth.fontVariable);
}
    
void drawEspEntities() {
	int entityNum;
	
	// Player esp
	for (entityNum = 0; entityNum < MAX_CLIENTS; entityNum++) {
		ethEntity_t* entity = &eth.entities[entityNum];
		
		if (!eth.clientInfo[entityNum].infoValid
				|| !entity->isInScreen
				|| (eth.cg_snap->ps.clientNum == entityNum))
			continue;

		// Esp dist
		if ((seth.value[VAR_ESP_DIST] != 0) && (entity->distance > seth.value[VAR_ESP_DIST]))
			continue;

		// Sanity check
		if (!entity->espColor) {
			// for now fix it this way, find out why entity 0 never gets esp color set
			addEspColors(entityNum, entity->isVisible);
			if (!entity->espColor)
				ethLog("error: esp: don't find color for entity %i", entityNum);
			continue;
		}

		// Define fade color for both name and icon
		float fadeColor;
		// Auto fade
		if (seth.value[VAR_ESP_FADE] == ESP_FADE_AUTO) {
			fadeColor = 800.0f / entity->distance;
			fadeColor = max(fadeColor, 0.2f);	// Max fade
			fadeColor = min(fadeColor, 1.0f);	// Min fade
		// Manual fade
		} else {
			fadeColor = seth.value[VAR_ESP_FADE_VALUE] / 100.0f;
		}

		// Name esp
		int y = entity->screenY;	// Keep y here so esp icon can know where to start display
		if( seth.value[VAR_ESPNAME] == NAMES_EVERYBODY ||
				  (seth.value[VAR_ESPNAME] == NAMES_TEAM && eth.clientInfo[entityNum].team == eth.clientInfo[eth.cg_clientNum].team ) ||
				  (seth.value[VAR_ESPNAME] == NAMES_ENEMY && eth.clientInfo[entityNum].team != eth.clientInfo[eth.cg_clientNum].team ) ||
				  (seth.value[VAR_ESPNAME] == NAMES_CUSTOM && eth.clientInfo[entityNum].targetType == PLIST_NAMES) ||
				  (seth.value[VAR_ESPNAME] == NAMES_FRIENDS && eth.clientInfo[entityNum].targetType == PLIST_FRIEND) ){

			char *str = eth.clientInfo[entityNum].name;
			
			float fontScale;
			// Static font size
			if (seth.value[VAR_ESPSIZE]) {
				fontScale = 0.15f;
			// Distance based font size
			} else {
				fontScale = 150.f / entity->distance;
				fontScale = max(fontScale, 0.15f);	// Min size
				fontScale = min(fontScale, 0.17f);	// Max size
			}

			// Position
			int x = entity->screenX - (sizeTextWidth(fontScale, str, &eth.fontVariable) / 2);
			y -= sizeTextHeight(fontScale, &eth.fontFixed);

			// Define name color	
			vec4_t nameColor;
			Vector4Copy(colorWhite, nameColor);
			nameColor[3] = fadeColor;

			if (seth.value[VAR_ESPNAME_BG]) {
				// Background color
				vec4_t bgColor;
				Vector4Copy(colorBlack, bgColor);
				bgColor[3] = fadeColor;
				// Border color
				vec4_t borderColor;
				Vector4Copy(colorLtGrey, borderColor);
				borderColor[3] = fadeColor;
				drawTextRect(x, y, fontScale, str, nameColor, seth.value[VAR_ESPNAME_COLORS] == 2 ? qtrue : qfalse, qfalse, qtrue, qtrue, bgColor, borderColor, &eth.fontFixed);
			} else {
				drawText(x, y, fontScale, str, nameColor, seth.value[VAR_ESPNAME_COLORS] == 2 ? qtrue : qfalse, qtrue, &eth.fontFixed);
			}
		}

		// Class and weapon icon esp
		if (seth.value[VAR_ESPICON] && (eth.mod->type != MOD_TCE)) {
			float iconSize;
			// Static icon size
			if (seth.value[VAR_ESPSIZE]) {
				iconSize = 7.0f;
			// Distance based icon size
			} else {
				iconSize = 10000.0f / entity->distance;
				iconSize = max(iconSize, 4.0f);		// Min size
				iconSize = min(iconSize, 5.0f);	// Max size
			}

			// Position
			int x = entity->screenX - (iconSize * 3) / 2;
			y -= iconSize + 4;

			// Icon color
			vec4_t iconColor;
			VectorCopy(entity->espColor, iconColor);
			iconColor[3] = fadeColor;

			// Background
			if (seth.value[VAR_ESPICON_BG]) {
				vec4_t bgColor;
				Vector4Copy(colorBlack, bgColor);
				bgColor[3] = fadeColor;
				drawFillRect(x, y, (iconSize * 3) + 2 , iconSize + 2, bgColor);
			}

			syscall_CG_R_SetColor(iconColor);
		    // Class
			drawPic(x, y, iconSize, iconSize, eth.classIcons[eth.clientInfo[entityNum].cls]);
			// Weapon
			drawPic(x + iconSize, y, iconSize * 2, iconSize, eth.weaponIcons[eth.cg_entities[entityNum].currentState->weapon]);
		    
			syscall_CG_R_SetColor(NULL);		
		}

		// Mortar trace
		if (seth.value[VAR_MORTARTRACE] && (eth.cg_entities[entityNum].currentState->weapon == WP_MORTAR_SET))
			drawMortarTrace(entityNum);
	}
	
	// Missile esp
	for (entityNum = MAX_CLIENTS; entityNum < MAX_GENTITIES && seth.value[VAR_ESPENT]; entityNum++) {

		ethEntity_t* entity = &eth.entities[entityNum];

		if (!*eth.cg_entities[entityNum].currentValid || !entity->isInScreen || !IS_MISSILE(entityNum))
			continue;

		// Esp dist
		if ((seth.value[VAR_ESP_DIST] != 0) && (entity->distance > seth.value[VAR_ESP_DIST]))
			continue;

		// Landmine esp
		if(IS_LANDMINE(entityNum) && IS_MISSILE_ARMED(entityNum)) {
			drawText(entity->screenX - 4 , entity->screenY, 0.20f, "L", colorYellow, qfalse, qtrue, &eth.fontFixed);
		// Dynamite esp
		} else if(IS_DYNAMITE(entityNum)) {
			char timeStr[8];
			int max, time;
		
			if (eth.mod->type == MOD_TCE)
				max = 45;
			else
				max = 30;
			
			time = max - ((eth.cg_time - eth.cg_entities[entityNum].currentState->effect1Time) / 1000);
			
			if (time > 0) {
				snprintf(timeStr, sizeof(timeStr), "D:%i", time);
				drawText(entity->screenX - 16, entity->screenY, 0.18f, timeStr, colorYellow,qfalse, qtrue, &eth.fontFixed);
			} else
			drawText(entity->screenX - 4, entity->screenY, 0.20f, "D", colorYellow, qfalse, qtrue, &eth.fontFixed);
		// Grenade esp
		} else if (IS_GRENADE(entityNum)) {
			drawText(entity->screenX - 4, entity->screenY, 0.20f, "G", colorYellow, qfalse, qtrue, &eth.fontFixed);
 		//Riffle nade esp
		} else if (IS_RIFFLE(entityNum)) {
			drawText(entity->screenX - 4, entity->screenY, 0.20f, "N", colorYellow, qfalse, qtrue, &eth.fontFixed);
		// Satchel esp
		} else if (IS_SATCHEL(entityNum)) {
			drawText(entity->screenX - 4, entity->screenY, 0.20f, "T", colorYellow, qfalse, qtrue, &eth.fontFixed);
		// Panzer esp
		} else if (IS_PANZER(entityNum)) {
			drawText(entity->screenX - 4, entity->screenY, 0.20f, "P", colorYellow, qfalse, qtrue, &eth.fontFixed);
		// Mortar esp
		} else if (IS_MORTAR(entityNum)) {
			drawText(entity->screenX - 4, entity->screenY, 0.20f, "M", colorYellow, qfalse, qtrue, &eth.fontFixed);
		// smoke esp
		} else if (IS_SMOKE(entityNum)) {
			drawText(entity->screenX - 4, entity->screenY, 0.20f, "S", colorYellow, qfalse, qtrue, &eth.fontFixed);
		}
	}


	// World entities esp
	if( seth.value[VAR_ESPITEMS] ){
		for (entityNum = 0; entityNum < eth.refEntitiesCount; entityNum++) {
			refEntity_t* refEnt = &eth.refEntities[entityNum];
			int x, y;
			if (worldToScreen(refEnt->origin, &x, &y)) {
				// AmmoPack esp
				if (refEnt->hModel == eth.hAmmoPack) {
					drawText(x - 4, y, 0.18f, "A", colorGreen, qfalse, qtrue, &eth.fontFixed);
				// MedicPack esp
				} else if (refEnt->hModel == eth.hMedicPack) {
					drawText(x - 4, y, 0.18f, "M", colorGreen, qfalse, qtrue, &eth.fontFixed);
				// Bullet esp
				//} else if (refEnt->hModel == eth.hBullet) {
	//				orig_CG_DrawStringExt(x - 2, y, "B", colorWhite, qtrue, qtrue, 4, 6, 50);
				}
				}
			}
		}
	
}

void drawSpawnTimerRight() {
	// No spanwtimer if not in a team
	if (getSpawntimer(qtrue) == -1)
		return;

	vec4_t timerBackground = { 0.16f, 0.2f,	0.17f, 0.8f	};
	vec4_t timerBorder = { 0.5f, 0.5f, 0.5f, 0.5f };
	
	char str[5];
    snprintf(str, sizeof(str), "%i", getSpawntimer(qtrue));
    int w = sizeTextWidth(0.19f, str, &eth.fontVariable);
    int y = 136 - 14;
    int x = 634 - w;
    drawFillRect(x - 2, y, w + 5, 12 + 2, timerBackground);
    drawRect(x - 2, y, w + 5, 12 + 2, timerBorder, 1);
    drawText(x, y + 3, 0.19f, str, colorRed, qtrue, qfalse, &eth.fontVariable);
}

void drawMortarTrace(int entityNum) {
	ethEntity_t* entity = &eth.entities[entityNum];

	vec3_t angles,forward;

	AxisToAngles(entity->headAxis, angles);

	angles[PITCH] -= 60.f;
	AngleVectors(angles, forward, NULL, NULL);
	forward[0] *= 3000 * 1.1f;
	forward[1] *= 3000 * 1.1f;
	forward[2] *= 1500 * 1.1f;

	trajectory_t mortarTrajectory;
	mortarTrajectory.trType = TR_GRAVITY;
	mortarTrajectory.trTime = eth.cg_time;
	VectorCopy(entity->head, mortarTrajectory.trBase);	 // TODO: Not precise need some muzzle
	VectorCopy(forward, mortarTrajectory.trDelta);
	
	int timeOffset = 0;
	trace_t mortarTrace;
	vec3_t mortarImpact;
	VectorCopy(mortarTrajectory.trBase, mortarImpact);
	#define TIME_STEPT 30
	while (timeOffset < 10000) {
		vec3_t nextPos;
		timeOffset += TIME_STEPT;
		eth.BG_EvaluateTrajectory(&mortarTrajectory, eth.cg_time + timeOffset, nextPos, qfalse, 0);
		eth.CG_Trace(&mortarTrace, mortarImpact, 0, 0, nextPos, entityNum, MASK_MISSILESHOT);
		if ((mortarTrace.fraction != 1)
				// Check for don't hit sky
				&& !((mortarTrace.surfaceFlags & SURF_NODRAW) || (mortarTrace.surfaceFlags & SURF_NOIMPACT))
				&& (mortarTrace.contents != 0)) {
			break;
		}
		VectorCopy(nextPos, mortarImpact);
		int x, y;
		if(worldToScreen(mortarImpact, &x, &y)) {
		    drawText(x, y, 0.19f, "*", colorGreen, qfalse, qtrue, &eth.fontVariable);
		}
	}
	
}

// Spree txt
void drawSpreeText () {
	//#define SPREE_FONT_SIZE 0.8f

    static char *spree[] = { "*", "*", "*","*","DOUBLE KILL", "EXCELLENT", "IMPRESSIVE", "KILLING SPREE", "KILLING SPREE!", "PERFECT", "GODLIKE!", "GODLIKE!!!", "HOLY SHIT!", "MULTIKILL!", "MULTIKILL!!!", "!!!MULTIKILL!!!", "!!!TRIPLE KILL!!!", "*!DOMINATING!*", "!!!DOMINATING!!!", "!ULTRA KILL!", "!!ULTRA KILL!!", "!!!ULTRA KILL!!!", "*MONSTERKILL*", "!!!MONSTERKILL!!!", "*!UNREAL!*", "!UNSTOPPABLE!", "!!! UNSTOPPABLE !!!", "!!! *WICKED*SICK* !!!"};
    static float spree_font_size;
    static int addor;
    
	
    vec4_t spree_c = { 1.0f, 0.0f, 0.0f, 1.0f};
    vec4_t spree_d = { 0.4f, 0.0f, 0.0f, 1.0f};
    if (spree_font_size == 0) spree_font_size = 0.6;

    if ((eth.startFadeTime) + 2 > eth.cg_time) {
	    addor = 1;
	    spree_font_size = 0.6;
    }
    float *spreecolor = eth_CG_FadeColor(eth.startFadeTime, 1000);	

    if (!spreecolor) 
	    return;
	

    int w = sizeTextWidth(spree_font_size, spree[eth.spreelevel], &eth.fontFixed);
    spree_c[3] = spreecolor[3];
    spree_d[3] = spreecolor[3];
    if( addor == 0) addor = 1;
    if(spree_font_size > 0.8) 
	addor += 1;
    
    int r_x = crandom() * (float)eth.s_level * 2.0f;
    int r_y = crandom() * (float)eth.s_level * 2.0f - addor;
    
    drawText((640 - w) / 2 + 2 + r_x, 200 + 2 + r_y, spree_font_size, spree[eth.spreelevel], spree_d, qtrue, qfalse, &eth.fontFixed);
    drawText((640 - w) / 2 + r_x, 200 + r_y, spree_font_size, spree[eth.spreelevel], spree_c, qtrue, qtrue, &eth.fontFixed);
    spree_font_size += 0.08;
    syscall_CG_R_SetColor(NULL);
}


//HUD stuff 

int isAimableWeapon() {
	int weapon = eth.cg_snap->ps.weapon;
	switch (weapon) {
		case WP_KNIFE:
		     return 0;
		     break;
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
		case WP_BINOCULARS:
		case WP_PANZERFAUST:
		case WP_MEDIC_SYRINGE:
		case WP_MEDIC_ADRENALINE:
		case WP_AMMO:
		case WP_MEDKIT:
		case WP_ARTY:
		case WP_DYNAMITE:
		case WP_MORTAR:
		case WP_MORTAR_SET:
		case WP_SATCHEL:
		case WP_LANDMINE:
			if (eth.mod->type != MOD_TCE)
			    return 0;
			return 1;
			break;
		default:
			return 1;
			break;
	    }
	return 1;
}

#define RELOAD_TIMER_X 5
#define RELOAD_TIMER_Y 420

#define KILL_X 5
#define KILL_Y 400 

#define CHAT_X 140
#define CHAT_Y 440 

#define WP_X 5
#define WP_Y 380
#define WP_H 30
#define WP_W 50
#define H_X 5
#define H_Y 340
#define H_W 40
#define H_H 40
#define XP_X 5
#define XP_Y 315

#define ES_X 5
#define ES_Y 290

#define FS_X 5
#define FS_Y 265

#define TXT_X 5
#define TXT_Y 5

#define WARNING_X 390
#define WARNING_Y 370

int consoleOrder[5] = {0,1,2,3,4};
int consoleQueue[5];
int chatOrder[3] = {0,1,2};
int chatQueue[3];
int kills;

// hud flashes
typedef struct fxphase_s {
    int clips_phase;
    int ammo_phase;
    int health_phase;
    vec4_t color[3];
} fxphase_t;
fxphase_t fxphase;

void initHUD() {
	VectorCopy(colorWhite,fxphase.color[0]);
	VectorCopy(colorRed,fxphase.color[1]);
	VectorCopy(colorMdRed,fxphase.color[2]);
	fxphase.color[0][3]= 1.0f;
	fxphase.color[1][3]= 1.0f;
	fxphase.color[2][3]= 1.0f;
}

void drawETHhud() {
    float *color;
    if (!kills && eth.killCount > 0)
    	kills = eth.killCount;
        
    if (eth.cg_snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR)
	    return;
    
	// check reload 
    if (eth.reloading && eth.reloadScale) {
	char relStr[10];
	snprintf(relStr, sizeof(relStr), "%d%%", ((eth.cg_snap->ps.weaponTime * 100) / eth.reloadScale));
	drawFillRect(RELOAD_TIMER_X, RELOAD_TIMER_Y, eth.cg_snap->ps.weaponTime / 10, 10, colorRed);
	drawText(eth.cg_snap->ps.weaponTime / 10 + RELOAD_TIMER_X, RELOAD_TIMER_Y, 0.19f, relStr, colorYellow,qfalse, qtrue, &eth.fontVariable);
        fxphase.clips_phase++;
        if (fxphase.clips_phase == 2) fxphase.clips_phase = 0;
        drawText (WARNING_X, WARNING_Y, 0.29f, "RELOADING...", fxphase.color[fxphase.clips_phase],qtrue, qfalse, &eth.fontVariable);
    }

	// draw weapon/ammo/xp/health
    int weapon = eth.cg_snap->ps.weapon; 
    int t_weapon = 0;
    switch(weapon) {
		case WP_FG42SCOPE:
			t_weapon = WP_FG42;
			break;
		case WP_GARAND_SCOPE:
			t_weapon = WP_GARAND;
			break;
		case WP_K43_SCOPE:
			t_weapon = WP_K43;
			break;
		case WP_SILENCED_COLT:
			t_weapon = WP_COLT;
			break;
		case WP_AKIMBO_SILENCEDLUGER:
			t_weapon = WP_AKIMBO_LUGER;
			break;
		case WP_AKIMBO_SILENCEDCOLT:
			t_weapon = WP_AKIMBO_COLT;
			break; 
		default:
			t_weapon = weapon;
			break;
	}

		
    int ammo = eth.cg_snap->ps.ammo[t_weapon];
    int clips = eth.cg_snap->ps.ammoclip[t_weapon];
    int winXP = eth.cg_snap->ps.stats[STAT_XP];
    int health = eth.cg_snap->ps.stats[STAT_HEALTH];
    int i = 0;

    char clipStr[3];
    char ammoStr[3];
    char xpStr[10];
    char healthStr[4];
    char spawnTimeE[3];
    char spawnTimeF[3];

    if (health < 0)
	    health = 0;
    
    snprintf(clipStr, sizeof(clipStr), "%d", clips);
    snprintf(ammoStr, sizeof(ammoStr), "%d", ammo);
    snprintf(xpStr, sizeof(xpStr), "%d", winXP);
    snprintf(healthStr, sizeof(healthStr), "%d", health);

    //draw current gun
    syscall_CG_R_SetColor(colorBlue);
    int addX = 0;
    if(eth.mod->type == MOD_TCE)
	addX = 45;
    
    if(health > 0) {
	if(eth.mod->type == MOD_TCE)
	    drawPic(WP_X + addX, WP_Y, WP_W, WP_H, eth.weaponIcons[WP_AMMO]);
	else 
	    drawPic(WP_X, WP_Y, WP_W, WP_H, eth.weaponIcons[weapon]);
    }
    
    if (isAimableWeapon()) {
    	if (clips < 5 ) {
	    	if (ammo > 0 ) { // fuck reload then 
				char wtext[100];
				if (!eth.reloading && health > 0) {
				    snprintf(wtext, sizeof(wtext), "RELOAD NEEDED!(%d)", clips);
				    fxphase.clips_phase++;
				    if (fxphase.clips_phase == 2)
				    	fxphase.clips_phase = 0;
				    drawText (WARNING_X, WARNING_Y, 0.29f,wtext, fxphase.color[fxphase.clips_phase],qtrue, qtrue, &eth.fontVariable);
				}
			}
			syscall_CG_R_SetColor(colorRed);
	    } else 
			syscall_CG_R_SetColor(colorYellow);
	    
	    if (clips > 0) {
			// draw ammo in cur clip
			for (i = 0; i < strlen(clipStr); i++) 
			    drawPic(WP_X + 5 + addX + (10 * i ), WP_Y + 20 , 10, 15, eth.number[clipStr[i]-48]);
	    }

	    if (ammo < 5 ) {
			if( health > 0 ) {
			    fxphase.ammo_phase++;
			    if (fxphase.ammo_phase == 2)
				fxphase.ammo_phase = 0;
			    drawText (WARNING_X, WARNING_Y + 30, 0.29f, "WARNING! LOW AMMO!", fxphase.color[fxphase.ammo_phase],qtrue, qfalse, &eth.fontVariable);
			}
			syscall_CG_R_SetColor(colorRed);
	    } else 
			syscall_CG_R_SetColor(colorGreen);
	
		// draw total ammo
	    if ( ammo > 0)  {
			for (i=0;i<strlen(ammoStr);i++) 
			    drawPic(WP_X + 5 + addX + (strlen(clipStr) * 10 + 10) + (10 * i ), WP_Y + 20, 10, 10, eth.number[ammoStr[i]-48]);
		}
	}

	//draw health icon 
	syscall_CG_R_SetColor(colorBlue);
	if (health > 40)
	    drawPic(H_X, H_Y, H_W, H_H, eth.weaponIcons[WP_MEDKIT]);

	// draw health value 
	if (health < 40 && health > 0) {
		fxphase.health_phase++;	
		
		if (fxphase.health_phase == 1)
		    drawPic(H_X - 1, H_Y - 1, H_W + 2, H_H + 2, eth.weaponIcons[WP_MEDKIT]);
		if (fxphase.health_phase == 0) {
		    syscall_CG_R_SetColor(colorRed);
		    drawPic(H_X + 3, H_Y + 3, H_W - 6, H_H - 6, eth.weaponIcons[WP_MEDKIT]);
		}
		if (fxphase.health_phase == 2) {
		    drawPic(H_X - 4, H_Y - 4, H_W + 8, H_H + 8, eth.weaponIcons[WP_MEDKIT]);
		    fxphase.health_phase = 0;
		}
		drawText (WARNING_X , WARNING_Y + 60 , 0.29f, "WARNING! LOW HEALTH!", fxphase.color[fxphase.health_phase],qtrue, qfalse, &eth.fontVariable);
		
		syscall_CG_R_SetColor(colorRed);
    } else 
		syscall_CG_R_SetColor(colorGreen);    

    if(health > 0)
	for (i=0;i<strlen(healthStr);i++) 
		drawPic(H_X + 5 + (10 * i ), H_Y + 20 , 10, 15, eth.number[healthStr[i]-48]);

	// draw XP 
    if(eth.mod->type != MOD_TCE) {
	drawText(XP_X, XP_Y, 0.19f, "XP:", colorBlue,qfalse, qtrue, &eth.fontVariable);
	syscall_CG_R_SetColor(colorGreen);
	for (i=0; i < strlen(xpStr); i++) 
		drawPic(XP_X + 5 + (10 * i ), XP_Y + 10 , 10, 10, eth.number[xpStr[i]-48]);
    }
	// draw enemy spawn time
    int enemyStime = getSpawntimer(qtrue);
    int warnAdd = 0;
    if (enemyStime > -1) {
		snprintf(spawnTimeE, sizeof(spawnTimeE), "%d", enemyStime);
		drawText(ES_X, ES_Y, 0.19f, "Enemy Spawn:", colorBlue,qfalse, qtrue, &eth.fontVariable);
		if (enemyStime > 3 )
		    syscall_CG_R_SetColor(colorYellow);	    
		else {
		    syscall_CG_R_SetColor(colorRed);
		    warnAdd = 4 - enemyStime;
		}
		for (i=0;i<strlen(spawnTimeE);i++) 
		    drawPic(ES_X + 5 + (10 * i ), ES_Y + 10 , 10, 10 + warnAdd, eth.number[spawnTimeE[i]-48]);
    }

	// draw team spawn time
    int teamStime = getSpawntimer(qfalse);
    if (teamStime > -1) {
		snprintf(spawnTimeF, sizeof(spawnTimeF), "%d", teamStime);
		drawText(FS_X, FS_Y, 0.19f, "Team Spawn:", colorBlue,qfalse, qtrue, &eth.fontVariable);
		syscall_CG_R_SetColor(colorGreen);
		for (i=0;i<strlen(spawnTimeF);i++) 
		    drawPic(FS_X + 5 + (10 * i ), FS_Y + 10 , 10, 10, eth.number[spawnTimeF[i]-48]);
    }
	
	// draw kills
    char killsC[50];
    snprintf(killsC, sizeof(killsC), "%d", eth.killCount);
    drawText(FS_X, FS_Y-25, 0.19f, "Kills:", colorBlue,qfalse, qtrue, &eth.fontVariable);
    syscall_CG_R_SetColor(colorGreen);
    for (i=0;i<strlen(killsC);i++) 
		drawPic(XP_X + 5 + (10 * i ), FS_Y - 25 + 10 , 10, 10, eth.number[killsC[i]-48]);

	//console txts 
    for (i=0 ; i < 5 ; i++) {
		consoleQueue[i] = consoleOrder[i] + eth.currentTxt;
		if (consoleQueue[i] > 4) 
		    consoleQueue[i] = consoleQueue[i] - 5;
    }

	//chat txts
    for (i=0 ; i < 3 ; i++) {
		chatQueue[i] = chatOrder[i] + eth.currentChat;
		if (chatQueue[i] > 2) 
		    chatQueue[i] = chatQueue[i] - 3;
    }

	//displaythem 
    for (i=0 ; i < 5 ; i++)  {
		if(strlen(console_p[consoleQueue[i]]) > 0)
			drawText(TXT_X, TXT_Y + (i * 10), 0.19f, console_p[consoleQueue[i]], colorWhite,qfalse, qfalse, &eth.fontVariable);
    }

    for (i=0 ; i < 3 ; i++)  {
		if(strlen(chat_p[chatQueue[i]]) > 0)
			drawText(CHAT_X, CHAT_Y + (i * 10), 0.19f, chat_p[chatQueue[i]], colorWhite,qfalse, qtrue, &eth.fontVariable);
    }

    color = eth_CG_FadeColor( eth.lastKillTime , 1000 * 5);
    if (color) {
		char killMsg[1024];
		snprintf(killMsg, sizeof(killMsg), "You killed %s!", eth.VictimName);
		int w = eth_CG_Text_Width_Ext(killMsg, 0.24f, 0, &eth.fontVariable);
		drawText((640 - w) / 2, KILL_Y , 0.24f, killMsg, color,qfalse, qtrue, &eth.fontVariable);
    }

    if ((eth.cg_time - eth.startFadeTime) < 2000)
		drawSpreeText();

	// finish here
    syscall_CG_R_SetColor(NULL);
}

/*
==============================
 Enemy/Friend player selector
==============================
*/

menuCat_t plistCats[] = {
	{ PLIST_UNKNOWN,	"--- UNMARKED ---" },
	{ PLIST_TEAMKILLER,	"--- TEAMKILLERS ---" },
	{ PLIST_FRIEND,		"--- FRIENDS ---" },
	{ PLIST_NAMES,		"--- NAMES TO DRAW ---"}
};

void drawPlayerList() {
	int itemHeight = sizeTextHeight(MENU_TEXT_SIZE, &eth.fontVariable) + (2 * 2);
	int maxItemWidth = 0;

	int count;
	for (count = 0; count < MAX_CLIENTS; count++) {
		int size = sizeTextWidth(MENU_TEXT_SIZE, eth.clientInfo[count].name, &eth.fontVariable);
		if (size > maxItemWidth)
			maxItemWidth = size;
	}
	for (count = 0; count < PLIST_MAX; count++) {
		int size = sizeTextWidth(MENU_TEXT_SIZE, plistCats[count].name, &eth.fontVariable);
		if (size > maxItemWidth)
			maxItemWidth = size;
	}
	maxItemWidth += itemHeight;

	static int openCategory = -1;
	int nextOpenCategory = openCategory;
	int y = MENU_SCREEN_BORDER;

	// Calculate how many players will be listed in one column
	count = 0;
	int lineHeight = itemHeight + MENU_ITEM_SPACE;
	int catHeight = 3 * itemHeight;
	int freeHeight = SCREEN_HEIGHT - (2 * MENU_SCREEN_BORDER + catHeight);
	int nicklines = freeHeight / lineHeight;

	int catCount = 0;
	int columns = 1;
	int timesdrawn = 0;
	int columnWidth = MENU_ITEM_SPACE + maxItemWidth;

	for (; catCount < PLIST_MAX; catCount++) {
		int X = MENU_SCREEN_BORDER;

		if (columns > 1) {
			y = MENU_SCREEN_BORDER + ((nicklines + catCount) * lineHeight);
		}

		qboolean isMouseOn = ((eth.mouseX > X)
				&& (eth.mouseX < (X + maxItemWidth))
				&& (eth.mouseY > y)
				&& (eth.mouseY < (y + itemHeight)));

		// Sanity check
		if (!plistCats[catCount].name)
			fatalError("Can't find text for a player list category.");

		// Draw category
		drawFillRect(MENU_SCREEN_BORDER, y, maxItemWidth, itemHeight, isMouseOn ? colorBlack : gameBackColor);
		drawRect(MENU_SCREEN_BORDER, y, maxItemWidth, itemHeight, isMouseOn ? gameBackColor : gameBorderColor, 1);
		drawText(MENU_SCREEN_BORDER + (itemHeight / 2), y + 2, MENU_TEXT_SIZE, plistCats[catCount].name, isMouseOn ? gameFontColor : colorWhite, qfalse, qfalse, &eth.fontVariable);

		y += lineHeight;

		// Category click
		if (((eth.mouseButton == Button1) || (eth.mouseButton == Button3)) && isMouseOn) {
			eth.mouseButton = 0;
			if (nextOpenCategory != catCount)
				syscall_CG_S_StartLocalSound(eth.cgs_media_sndLimboSelect, CHAN_LOCAL_SOUND);
			nextOpenCategory = catCount;
		}

		// Draw players only for this category
		if (openCategory != catCount)
			continue;

		// Search players in this category
		int client = 0;
		for (; client < MAX_CLIENTS; client++) {
			if (eth.clientInfo[client].targetType != openCategory)
				continue;

			if (client == eth.cg_clientNum)
				continue;

			if (!eth.clientInfo[client].infoValid)
				continue;

			X = MENU_SCREEN_BORDER + (columns-1)*columnWidth;

			qboolean isMouseOn = ((eth.mouseX > X)
					&& (eth.mouseX < (X + maxItemWidth))
					&& (eth.mouseY > y)
					&& (eth.mouseY < (y + itemHeight)));

			// Draw client
			drawFillRect(X, y, maxItemWidth, itemHeight, isMouseOn ? colorBlack : gameBackColor);
			drawRect(X, y, maxItemWidth, itemHeight, isMouseOn ? gameBackColor : gameBorderColor, 1);
			drawText(X + (itemHeight / 2) + sizeTextWidth(MENU_TEXT_SIZE, " ", &eth.fontVariable), y + 2, MENU_TEXT_SIZE, eth.clientInfo[client].name, colorLtGrey, qfalse, qfalse, &eth.fontVariable);

			y += itemHeight + MENU_ITEM_SPACE;
			timesdrawn++;

			if (timesdrawn == nicklines) {
				timesdrawn = 0;
				columns++;
				y = MENU_SCREEN_BORDER + (catCount + 1) * lineHeight;
			}

			// If not a player click
			if (((eth.mouseButton != Button1) && (eth.mouseButton != Button3)) || !isMouseOn)
				continue;

			// Use button1 to select friends, and mouse3 to cycle the rest
			if (eth.mouseButton == Button1) {
				if (openCategory == PLIST_UNKNOWN)
					eth.clientInfo[client].targetType = PLIST_FRIEND;
				else
					eth.clientInfo[client].targetType = PLIST_UNKNOWN;
			} else if (eth.mouseButton == Button3) {
				++openCategory;
				if( openCategory >= PLIST_MAX )
					openCategory = PLIST_UNKNOWN;
				eth.clientInfo[client].targetType = openCategory;
			}

			syscall_CG_S_StartLocalSound(eth.cgs_media_sndLimboSelect, CHAN_LOCAL_SOUND);
			eth.mouseButton = 0;
		}
	}
	openCategory = nextOpenCategory;
	eth.mouseButton = 0;
}

// Referee window
eth_window_t refwin;
void drawReferees() {
	if (seth.value[VAR_REFLIST] == REF_LIST) {

		#define REF_FONT_SIZE 0.19f
		#define REF_SPACE 5
		#define REF_BORDERCOLOR colorBlack
		#define REF_TEXT "Referee(s):"

		// initialize window
		createWindow(&refwin,"refwin");

		// If no spectator
		if (seth.refCount == 0)
				return;
		
		// Get max width
		int max = 0;
		int count = 0;
		for (; count < seth.refCount; count++) {
				int w = sizeTextWidth(REF_FONT_SIZE, seth.refNames[count], &eth.fontVariable);
				if (w > max)
						max = w;
		}
		if (sizeTextWidth(REF_FONT_SIZE, REF_TEXT, &eth.fontVariable) > max)
				max = sizeTextWidth(REF_FONT_SIZE, REF_TEXT, &eth.fontVariable);

		int width = max + (2 * REF_SPACE);
		refwin.windowW = width;
		int lineHeight = sizeTextHeight(REF_FONT_SIZE, &eth.fontVariable) + REF_SPACE;
		int height = ((lineHeight * (seth.refCount + 1)) + REF_SPACE);
		int y = refwin.posY + REF_SPACE;
		
		drawFillRect(refwin.posX, refwin.posY, refwin.windowW, refwin.windowH, refwin.bgColor);
		drawRect(refwin.posX, refwin.posY, width, height, REF_BORDERCOLOR, 1);
		drawText(REF_SPACE + refwin.posX, y, REF_FONT_SIZE, REF_TEXT, colorRed, qfalse, qtrue, &eth.fontVariable);
		y += lineHeight;
		
		for (count = 0; count < seth.refCount; count++) {
				drawText(REF_SPACE + refwin.posX, y, REF_FONT_SIZE, seth.refNames[count], colorWhite, qfalse, qtrue, &eth.fontVariable);
				y += lineHeight;
		}

		refwin.windowH = y - refwin.posY;
		getWindowStatus(&refwin);
	} else if (seth.value[VAR_REFLIST] == REF_NUM) {
		vec4_t refBackground = { 0.16f, 0.2f,	0.17f, 0.8f	};
		vec4_t refBorder = { 0.5f, 0.5f, 0.5f, 0.5f };
	
		char str[64];
    		snprintf(str, sizeof(str), "Referees: ^2%i", seth.refCount);
   		int w = sizeTextWidth(0.19f, str, &eth.fontVariable);
    		int y = 137;
    		int x = 634 - w;
    		drawFillRect(x - 2, y, w + 5, 12 + 2, refBackground);
    		drawRect(x - 2, y, w + 5, 12 + 2, refBorder, 1);
    		drawText(x, y + 3, 0.19f, str, colorRed, qfalse, qfalse, &eth.fontVariable);
	}
}

// Custom crosshair. Useful for tc:e because there isn't any
void drawCrosshair() {
	if (seth.value[VAR_CROSSHAIR] == XHAIR_TCE && eth.mod->type != MOD_TCE)
		return;
	drawFillRect(319.5, 235.0, 1, 10, colorRed);
	drawFillRect(315.0, 239.5, 10, 1, colorGreen);
}

#ifdef ETH_DEBUG

// Draw memdump
int getVarOffset(int var) {
	switch(var) {
		case 1:
			return (int)(void *)eth.CG_Trace;
		case 2:
			return (int)(void *)eth.BG_EvaluateTrajectory;
		case 3:
			return (int)(int *)eth.cg_time;
		case 4:
			return (int)(refdef_t *)eth.cg_refdef.vieworg;
		case 5:
			return (int)(float *)eth.cg_refdefViewAngles;
		case 6:
			return (int)(void *)eth_CG_vmMain;
		case 7:
			return 0x906c1c8; //CL_MOUSEX
		case 8:
			return (int)(void *)eth.cg_snap->ps.origin;
	    default:
			return 0;
	}
}

int retVChar( int memchar ) {
	if (memchar > 31 && memchar < 126 )
		return memchar;
	else
	    return 127;
}

#define MAX_FUNC 10
char *debugFName[MAX_FUNC] = { "NULL" ,"CG_Trace()", "BG_EvaluateTrajectory()", "cg.time", "cg.refdef", "cg_refdefViewAngles", "vmMain" , "CL_MOUSEDX_ADDR", "eth.cg_snap->ps.origin"};

void drawMemDump( int offset ) {
	#define DBOX_X 15
	#define DBOX_Y 150
	#define DBOX_FONT_SIZE 0.19f
	#define DBOX_SPACE 5
	#define DBOX_BACKCOLOR (vec4_t){ 0.02f, 0.3f, 0.07f, 0.65f }
	#define DBOX_BORDERCOLOR colorBlack
	#define DBOX_TEXT ""
	#define DBOX_TEXT_B "^2Offset keys: ^7[^3PgUp/PgDown^7] ^3-200/+200 ^2| ^7[^3Up/Down^7] -20/+20"
	#define DBOX_OUT_LINES 12
	#define DBOX_OUT_ROWS 110

	char d_text[100];
	char d_fhex[70];
	char d_hex[45];
	char d_ascii[60];
	char d_show[100];
	char d_temp[10];
	int i,j;

	int max = ((DBOX_OUT_ROWS) * (DBOX_SPACE));    
	int width = max;
	int lineHeight = sizeTextHeight(DBOX_FONT_SIZE, &eth.fontVariable) + DBOX_SPACE;
	int height = ((lineHeight * (DBOX_OUT_LINES)) + DBOX_SPACE);
	int y = DBOX_Y + DBOX_SPACE;
	int w = 0;

	drawFillRect(DBOX_X, DBOX_Y, width, height, DBOX_BACKCOLOR);
	drawRect(DBOX_X, DBOX_Y, width, height, DBOX_BORDERCOLOR, 1);
	bzero(d_text,sizeof(d_text));
	snprintf(d_text, sizeof(d_text), "Memdump for offset: ^3%s ^2( + 0x%.8x)",debugFName[(int)seth.value[VAR_MEMDUMP]],((eth.offsetMul*200)+(eth.offsetSub*20)));
	drawText(DBOX_X + DBOX_SPACE, y, DBOX_FONT_SIZE, d_text, colorRed, qfalse, qtrue, &eth.fontVariable);
	y += lineHeight;
    w = sizeTextWidth(DBOX_FONT_SIZE, "H", &eth.fontVariable);
    drawFillRect(DBOX_X + DBOX_SPACE , DBOX_Y + lineHeight , w*8 + DBOX_SPACE, ((lineHeight * (DBOX_OUT_LINES - 2)) + DBOX_SPACE), DBOX_BACKCOLOR);
    drawFillRect(DBOX_X + DBOX_SPACE + (w *10) , DBOX_Y + lineHeight , w*36 + DBOX_SPACE, ((lineHeight * (DBOX_OUT_LINES - 2)) + DBOX_SPACE), DBOX_BACKCOLOR);
    drawFillRect(DBOX_X + DBOX_SPACE + (w*10) + (w*37) , DBOX_Y + lineHeight , w*18, ((lineHeight * (DBOX_OUT_LINES - 2)) + DBOX_SPACE), DBOX_BACKCOLOR);

	for (j = 0 ; j < (DBOX_OUT_LINES - 2) ; j++) {
	    bzero(d_hex,sizeof(d_hex));
	    bzero(d_fhex,sizeof(d_fhex));
	    bzero(d_ascii,sizeof(d_ascii));
	    bzero(d_show,sizeof(d_show));
	    bzero(d_temp,sizeof(d_temp));

	    for (i = 0 ; i < 20 ; i++) {
			d_hex[i] = *(unsigned char *)(offset + i);
			d_ascii[i] = (unsigned char)retVChar((int)d_hex[i]);
	    }

	    for (i = 0 ; i < 20 ; i = i + 2 ) {
			snprintf(d_temp, 5 ,"%.2X%.2X ",d_hex[i],d_hex[i+1]);
			strncat(d_fhex, d_temp , 5);
			d_fhex[strlen(d_fhex)] = ' ';
	    }
	    snprintf(d_show, sizeof(d_show), "0x%.8X", offset);	
	    drawText(DBOX_X + DBOX_SPACE, y, DBOX_FONT_SIZE, d_show, colorGreen, qfalse, qfalse, &eth.fontVariable);
	    drawText(DBOX_X + DBOX_SPACE + (w*10), y, DBOX_FONT_SIZE, d_fhex, colorYellow, qfalse, qfalse, &eth.fontVariable);
	    drawText(DBOX_X + DBOX_SPACE + (w*10)+ (w*37), y, DBOX_FONT_SIZE, d_ascii, colorYellow, qfalse, qfalse, &eth.fontVariable);
	    y += lineHeight;
	    offset += 20;
	}
	drawText(DBOX_X + DBOX_SPACE, y, DBOX_FONT_SIZE, DBOX_TEXT_B, colorRed, qfalse, qtrue, &eth.fontVariable);
}

void crossRailTrail(vec3_t point) {
	#define CROSS_SIZE 40

	int count = 0;
	for (; count < 3; count++) {
		vec3_t start, end;
		VectorCopy(point, start);
		VectorCopy(point, end);
		start[count] += CROSS_SIZE;
		end[count] -= CROSS_SIZE;
		
		eth.CG_RailTrail2(NULL, start, end);
	}
}
/*
// Draw x, y and z prediction error stats bars. For debug only
void drawPredictStats(vec3_t origin, vec3_t real, vec3_t predict) {
	#define PSB_FONT_SIZE 0.19f
	#define PSB_SPACE 2
	#define PSB_BAR_WIDTH (sizeTextWidth(PSB_FONT_SIZE, "-000.0", &eth.fontVariable) + (PSB_SPACE * 2))
	#define PSB_BAR_HEIGHT 128
	#define PSB_X (320 - ((3 * (PSB_BAR_WIDTH + PSB_SPACE)) / 2))
	#define PSB_Y 32

	int x = PSB_X;
	float average = 0.0f;
	int count = 0;
	for (; count < 3; count++) {
		// Draw background
		drawFillRect(x, PSB_Y, PSB_BAR_WIDTH, PSB_BAR_HEIGHT, colorBlack);

		// Computing prediction coef error based on real delta
		float predictDelta = predict[count] - origin[count];
		float realDelta = real[count] - origin[count];
		realDelta = 5.0f;	// Just for a test with static delta value
		float coef;
		if ((predictDelta != 0.0f) && (realDelta != 0.0f))
			coef = predictDelta / realDelta;
		else
			coef = 0.0f;

		// Draw predict bar. A bar go from 200%(top) to -200%(bottom)
		vec4_t color;
		color[0] = min(abs(coef), 1.0f);	// FIXME: don't use abs() cause only support int
		color[1] = 1.0f - color[0];
		color[2] = 0;
		color[3] = 1;
		float barCoef = max(min(coef / 2.0f, 1.0f), -1.0f);
		int barY = PSB_Y + (PSB_BAR_HEIGHT / 2);
		float barH = ((float)PSB_BAR_HEIGHT / 2.0f) * barCoef;
		if (barCoef > 0)
			barY -= barH;
		drawFillRect(x, barY, PSB_BAR_WIDTH, abs(barH), color);

		// Draw the letter
		char *letter = NULL;
		if (count == 0)
			letter = "X";
		else if (count == 1)
			letter = "Y";
		else if (count == 2)
			letter = "Z";
		drawText(x + (PSB_BAR_WIDTH / 2) - (sizeTextWidth(PSB_FONT_SIZE, letter, &eth.fontVariable) / 2), PSB_Y + PSB_SPACE, PSB_FONT_SIZE, letter, colorGreen, qfalse, qtrue, &eth.fontVariable);	

		// Draw the coef
		char str[16];
		snprintf(str, sizeof(str), "%.1f", coef * 100.0f);
		drawText(x + PSB_BAR_WIDTH - sizeTextWidth(PSB_FONT_SIZE, str, &eth.fontVariable) - PSB_SPACE, PSB_Y + PSB_SPACE + sizeTextHeight(PSB_FONT_SIZE, &eth.fontVariable), PSB_FONT_SIZE, str, colorGreen, qfalse, qtrue, &eth.fontVariable);	

		// Draw border
		drawRect(x, PSB_Y, PSB_BAR_WIDTH, PSB_BAR_HEIGHT, colorLtGrey, 1);

		// Store current coef
		average = average + abs(coef);

		// Next bar
		x += PSB_BAR_WIDTH + PSB_SPACE;
	}

	ethDebug("prediction error: current average: %.1f%%", average / 3.0f * 100.0f);

	// Prediction error average
	static float value[200];
	static int valueCount = 0;

	// Computing average	
	value[valueCount] = (average / 3.0f);
	for (count = 0; count < (sizeof(value) / sizeof(float *)); count++)
		average += value[count];
	average /= (float)(sizeof(value) / sizeof(float *));

	if (valueCount < ((sizeof(value) / sizeof(float *)) - 1))
		valueCount++;
	else
		valueCount = 0;

	// Show average
	char str[32];
	snprintf(str, sizeof(str), "^3Average:^7 %.2f", average * 100.0f);
	drawText(PSB_X, PSB_Y + PSB_BAR_HEIGHT + PSB_SPACE, 0.19f, str, colorWhite, qfalse, qtrue, &eth.fontVariable);	

	// Show a 3d cross on the current target
	crossRailTrail(real);
}*/

#endif // DEBUG
