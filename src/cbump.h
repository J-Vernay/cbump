/// @file cbump.h @brief Header file of cbump library.

#ifndef CBUMP_HEADER_GUARD
#define CBUMP_HEADER_GUARD

#include <stdbool.h>
#include <stddef.h>

/// Structure representing 2D coordinates, either point or vector.
typedef struct cbump_vec2 {
    float x; ///< X coordinate of the vector.
    float y; ///< Y coordinate of the vector.
} cbump_vec2;

/// Structure representing an axis-aligned box.
typedef struct cbump_box {
    float x; ///< X coordinate of the corner.
    float y; ///< Y coordinate of the corner.
    float w; ///< Width of the box, must be positive.
    float h; ///< Height of the box, must be positive.
} cbump_box;

/// Structure representing a collision occuring between two boxes.
typedef struct cbump_collision {
    cbump_box* moving;    ///< The box currently moving.
    cbump_box* obstacle;  ///< The box which collides with the moving box.
    void*      userdata;  ///< User data provided by caller of 'cbump_move'.
    cbump_vec2 normal;    ///< Direction opposite to collision side.
    cbump_vec2 moveleft;  ///< Remaining movement to be done.
    float      time;      ///< Time of impact.
    unsigned   index;     ///< Index of 'obstacle' in provided 'obstacles' list.
} cbump_collision;

/// Signature of function suitable to resolve collision between two boxes.
/// @param collision The collision to resolve.
/// @return Movement left to be done for `collision->moving` box.
typedef cbump_vec2 cbump_resolver(cbump_collision const* collision);

/// Constructor of a cbump_box specifying its center instead of the corner.
inline cbump_box cbump_box_center(float x, float y, float w, float h) {
    cbump_box b = { x - w/2, y - h/2, w, h };
    return b;
}

/// Checks whether two cbump_box currently overlap.
bool cbump_box_overlap(cbump_box const* a, cbump_box const* b);

/// Returns the time of impact on both axis clamped between 0 and 1.
/// \details For each axis, returns the factor by which multiplying
/// 'move' component will make 'box' and 'other' touch each other on this axis.
/// \param box   The box moving along \a move .
/// \param other The static box which may be collapsed with;
/// \param move  The direction of movement. Time of impact is relative to 'move'.
/// \retval Component is one if move can be done without overlapping, or if already overlapping.
/// \retval Component between 0 and 1 giving the time of impact.
cbump_vec2 cbump_time_of_impact(cbump_box const* box, cbump_box const* other, cbump_vec2 move);

/// Moves the given 'box' by the given 'move' until it collides with one of the 'obstacles'.
/// Returns the first collision occuring. 'box' has its position moved to the moment of impact.
cbump_collision cbump_move_simple(cbump_box* box, cbump_vec2 move,
                                  unsigned nb_obstacles, cbump_box* obstacles[]);

/// Collision resolver that stops the moving box.
cbump_vec2 cbump_resolve_touch (cbump_collision const* collision);
/// Collision resolver that makes the moving box crossing the obstacle.
cbump_vec2 cbump_resolve_cross (cbump_collision const* collision);
/// Collision resolver that makes the moving box slide along the obstacle.
cbump_vec2 cbump_resolve_slide (cbump_collision const* collision);
/// Collision resolver that makes the moving box bounce on the obstacle.
cbump_vec2 cbump_resolve_bounce(cbump_collision const* collision);

/// Makes @a box move along the given @a move vector, taking into account obstacles.
/// @param box           The box to move. Its position will be updated.
/// @param move          The vector representing the movement, relative to current @a box position.
/// @param nb_obstacles  Number of obstacles present in @a obstacles array.
/// @param obstacles     Array of pointers to potential obstacles which @a box may collide too.
///                      NULL-pointers are not allowed.
///                      It will be reordered in imprevisible ways (for internal use reasons):
///                      so you must not rely on order kept after calling `cbump_move()`.
/// @param resolver      The resolver to use. Can be `NULL`, in which case `cbump_slide` is used.
/// @param userdata      User data provided to resolver. Not used internally.
void cbump_move(cbump_box* box, cbump_vec2 move,
                unsigned nb_obstacles, cbump_box* obstacles[],
                cbump_resolver* resolver, void* userdata);

#endif // CBUMP_HEADER_GUARD
