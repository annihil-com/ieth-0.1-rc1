#ifndef _TRACER_H
#define _TRACER_H

// stupid include hell workaround
#ifndef ETH_H_
#include "ieth.h"
#endif

#define RAILTRAIL_INTERVAL	33

void debug_head_box( int player );
qboolean trace_body( int player, vec3_t hitpoint );
qboolean trace_head( refEntity_t *head, vec3_t hitpoint, int player );

typedef enum {
	HITBOX_OFF,
	HITBOX_ETMAIN,
 	HITBOX_ETPUB,
 	HITBOX_ETPRO,
 	HITBOX_GENERIC,
 	HITBOX_CUSTOM
} hitbox_type_t;

typedef enum {
	MS_STANDING,
 	MS_CROUCHING,
 	MS_PRONE
} model_state_t;

typedef struct {
	vec3_t stand_offset;
	vec3_t crouch_offset;
	vec3_t prone_offset;

	vec3_t stand_offset_moving;
	vec3_t crouch_offset_moving;
	
	vec3_t size;

	// for body boxes
	vec3_t size2;
} hitbox_t;

/* these define the head hitboxes. format is { fwd, right, up }, relative to the tag's orientation
	matched them as good as possibly with server head hitboxes using hacked servercode

	hitboxes don't rotate, but their origin does! it rotates with tag_head

	etpub/etmain boxes are really based on centity position, not on tag_head position, so need
	different aimbot code to handle those mods... for now use everything tag_head based

	always use correct hitbox mode.. wrong vecs == suck4ge
*/

const static hitbox_t head_hitboxes[] =
{
    // stand               crouch         prone              stand moving      crouch moving    hitbox size (x,y,z)
 { { 0.0, 0.0, 0.0}, { 0.0, 0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0, 0.0, 0.0}, },	// OFF
 { { 0.2, -0.5, 5.8}, { 0.2, -0.5, 5.8}, { 2.0, -0.5, 5.8}, { 0.2, -0.5, 5.8}, { 0.3, -0.5, 4.8}, {12.0,12.0,12.0}, },	// ETMAIN
 { { 1.0, -1.0, 6.5}, { 1.0, -1.0, 6.0}, { 2.0,  0.0, 6.0}, { 1.0, -1.0, 6.5}, { 1.0,  -1.0, 6.5}, {12.0,12.0,12.0}, },	// ETPUB FIXED
 { { 0.3,-0.6, 4.9}, { 0.0, 0.6, 4.8}, { 1.5, -1.0, 4.0}, { 0.0,  0.0, 4.5}, { 0.9,  0.7, 4.8}, {11.5,11.5,12.0}, },	// ETPRO b_realhead 1
 { { 0.3, 0.3, 7.0}, {-0.3, 0.8, 7.0}, { 0.0,  0.3, 6.9}, { 0.5,  0.0, 6.5}, { 0.0, -0.7, 7.0}, {11.0,11.0,12.0}, },    // GENERIC realhead

};

// seems to be the same for every mod      stand              crouch              prone         stand moving       prone moving      stand bounds      prone bounds
const static hitbox_t body_hitbox = { { 0.0, 0.0, 0.0}, { 0.0, 0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0,  0.0, 0.0}, {36.0,36.0,72.0}, {36.0, 36.0,48.0} };

hitbox_t custom_headhitbox;

#endif
