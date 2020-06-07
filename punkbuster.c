#define _GNU_SOURCE
#include "ieth.h"

void eth_glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
    // Get the current values of our menus, visuals, and cams
    qboolean menu = eth.isMenuOpen;
    qboolean plist = eth.isPlistOpen;
    qboolean visuals = seth.value[VAR_VISUALS];
    qboolean mortarcam = seth.value[VAR_MORTARCAM];
    qboolean panzercam = seth.value[VAR_PANZERCAM];
    qboolean satchelcam = seth.value[VAR_SATCHELCAM];
    qboolean mirrorcam = seth.value[VAR_MIRRORCAM];
    qboolean riflecam = seth.value[VAR_RIFLECAM];
    qboolean binocsniperblack = seth.value[VAR_BINOCSNIPERBLACK];
    qboolean smoke = seth.value[VAR_SMOKE];

eth.pbss++;
    typedef void (*SCR_UpdateScreen_t)(void);
    SCR_UpdateScreen_t SCR_UpdateScreen = (SCR_UpdateScreen_t)sethET->SCR_UpdateScreen;

    // Turn off our menus, visuals, and cams
    eth.isMenuOpen = qfalse;
    eth.isPlistOpen = qfalse;
    seth.value[VAR_VISUALS]=qfalse;
    seth.value[VAR_MORTARCAM]=qfalse;
    seth.value[VAR_PANZERCAM]=qfalse;
    seth.value[VAR_SATCHELCAM]=qfalse;
    seth.value[VAR_MIRRORCAM]=qfalse;    
    seth.value[VAR_RIFLECAM]=qfalse;
    seth.value[VAR_BINOCSNIPERBLACK]=qfalse;
    seth.value[VAR_SMOKE]=qfalse;

    SCR_UpdateScreen();
    SCR_UpdateScreen();
    SCR_UpdateScreen();
    SCR_UpdateScreen();

    orig_glReadPixels(x,y,width,height,format,type,pixels);

    // Set our menus, visuals, and cams back to values before pbss call
    eth.isMenuOpen = menu;
    eth.isPlistOpen = plist;    
    seth.value[VAR_VISUALS] = visuals;
    seth.value[VAR_MORTARCAM] = mortarcam;
    seth.value[VAR_PANZERCAM] = panzercam;
    seth.value[VAR_SATCHELCAM] = satchelcam;
    seth.value[VAR_MIRRORCAM] = mirrorcam;    
    seth.value[VAR_RIFLECAM] = riflecam;
    seth.value[VAR_BINOCSNIPERBLACK] = binocsniperblack;
    seth.value[VAR_SMOKE] = smoke;
    return;
}
