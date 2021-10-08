#include "cbump.h"

extern inline cbump_box cbump_box_center(float x, float y, float w, float h);

/// Returns whether the two ranges `[amin; amin+awidth[` and `[bmin; bmin+bwidth[`
/// overlap (existence of a number common to both ranges).
static inline bool _range_overlap(float amin, float awidth, float bmin, float bwidth) {
    // Equivalent to !(bmax <= amin || bmin >= amax)
    return bmin+bwidth > amin && bmin < amin+awidth;
}

extern inline bool cbump_box_overlap(cbump_box const* a, cbump_box const* b) {
    // AABB testing: does X axis and Y axis overlap?
    return _range_overlap(a->x, a->w, b->x, b->w)
        && _range_overlap(a->y, a->h, b->y, b->h);
}

cbump_vec2 cbump_time_of_impact(cbump_box const* box, cbump_box const* other, cbump_vec2 move) {
    cbump_vec2 toi = { // Initializing toi with distance.
        (move.x > 0) ? (other->x - box->x - box->w) : (other->x + other->w - box->x),
        (move.y > 0) ? (other->y - box->y - box->h) : (other->y + other->h - box->y),
    };
    // Make 'toi' relative to 'move'.
    toi.x /= move.x;
    toi.y /= move.y;
    // Clamp 'toi' in range [0;1].
    toi.x = (toi.x < 0 || toi.x > 1) ? 1 : toi.x;
    toi.y = (toi.y < 0 || toi.y > 1) ? 1 : toi.y;
    return toi;

}

cbump_collision cbump_move_simple(cbump_box* box, cbump_vec2 move,
                                unsigned nb_obstacles, cbump_box* obstacles[])
{
    // Keeping track of the nearest collision.
    cbump_collision collision = {box, NULL, NULL, {0,0}, {0,0}, 1};
    
    for (unsigned i = 0; i < nb_obstacles; ++i) {
        cbump_box* obstacle = obstacles[i];
        // For each obstacle, determine its time of impact on both axis.
        cbump_vec2 toi = cbump_time_of_impact(box, obstacle, move);
        // Check if impact position would actually create collision.
        bool collide_x = toi.x < collision.time
            && _range_overlap(box->y + toi.x*move.y, box->h, obstacle->y, obstacle->h);
        bool collide_y = toi.y < collision.time 
            && _range_overlap(box->x + toi.y*move.x, box->w, obstacle->x, obstacle->w);
        
        if (!collide_x && !collide_y)
            continue;
        
        // Update collision to current obstacle.
        collision.index = i;
        if (toi.x < toi.y) {
            collision.time = toi.x;
            collision.normal.x = move.x > 0 ? -1 : 1;
            collision.normal.y = 0;
        } else {
            collision.time = toi.y;
            collision.normal.x = 0;
            collision.normal.y = move.y > 0 ? -1 : 1;
        }
    }
    // Update box position.
    box->x += collision.time * move.x;
    box->y += collision.time * move.y;

    collision.obstacle = obstacles[collision.index];
    collision.moveleft.x = (1 - collision.time) * move.x;
    collision.moveleft.y = (1 - collision.time) * move.y; 

    return collision;
}


cbump_vec2 cbump_resolve_touch (cbump_collision const* collision) {
    // The moving box stays at the touching position: remaining movement is null.
    cbump_vec2 remaining_movement = {0,0};
    return remaining_movement;
}

cbump_vec2 cbump_resolve_cross (cbump_collision const* collision) {
    // The moving box crosses the obstacle: remaining movement is movement left.
    return collision->moveleft;
}

cbump_vec2 cbump_resolve_slide (cbump_collision const* collision) {
    cbump_vec2 remaining_movement = collision->moveleft;
    if (collision->normal.x == 0) {
        // Horizontal is null, normal is up or down -> cancel the remaining vertical movement.
        remaining_movement.y = 0;
    } else {
        // Vertical is null, normal is left or right -> cancel the remaining horizontal movement.
        remaining_movement.x = 0;
    }
    return remaining_movement;
}

cbump_vec2 cbump_resolve_bounce(cbump_collision const* collision) {
    cbump_vec2 remaining_movement = collision->moveleft;
    if (collision->normal.x == 0) {
        // Horizontal is null, normal is up or down -> reverse the remaining vertical movement.
        remaining_movement.y = -remaining_movement.y;
    } else {
        // Vertical is null, normal is left or right -> reverse the remaining horizontal movement.
        remaining_movement.x = -remaining_movement.x;
    }
    return remaining_movement;
}


void cbump_move(cbump_box* box, cbump_vec2 move,
                unsigned nb_obstacles, cbump_box* obstacles[],
                cbump_resolver* resolver, void* userdata)
{
    // Default resolver is slide.
    if (!resolver) resolver = &cbump_resolve_slide;
    while (move.x != 0 || move.y != 0) {
        cbump_collision collision = cbump_move_simple(box, move, nb_obstacles, obstacles);
        collision.userdata = userdata;
        move = (*resolver)(&collision);
        // Remove colliding obstacle from future collisions.
        --nb_obstacles;
        obstacles[collision.index] = obstacles[nb_obstacles];
        obstacles[nb_obstacles] = collision.obstacle;
    }

}