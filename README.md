# cbump - 2D physics library for C/C++, loose adaptation of bump.lua

`cbump` is a simple C library for 2D collision-detection for axis-aligned rectangles. Here are its features:

- Only axis-aligned bounding boxes (AABB): no circles, polygons, etc.
- Simple API and integration.
- All moves are considered "bullets": no tunneling can happen.
- Minimal stack memory: no memory allocation.
- Detects collisions and provides basic collision responses.
- Non-realistic physics, ideal for 2D platformers, top-down games, fighting games...

# Examples

Complete example is available in `examples` subdirectory. Here is all physics code related to `cbump` to implement a Pong clone, as demonstrated in `examples/pong.c`:

```c
// Definition of all physic entities.
cbump_box
    ball = cbump_box_center(Width/2, Height/2, BallSize, BallSize),
    lpaddle = cbump_box_center(20, Height/2, PaddleWidth, PaddleHeight),
    rpaddle = cbump_box_center(Width-20, Height/2, PaddleWidth, PaddleHeight),
    topwall = {0, 0, Width, 0},
    leftwall = {0, 0, 0, Height},
    rightwall = {Width, 0, 0, Height},
    bottomwall = {0, Height, Width, 0};
cbump_box* entities[] = {
    &ball, &lpaddle, &rpaddle, &topwall, &leftwall, &rightwall, &bottomwall };
const unsigned nb_entities = 7;

// Variables to control ball movement.
float ball_angle = PI/5;
float ball_speed = BallSpeed;

// ...

// Paddle movement.
cbump_vec2 lmove = ..., rmove = ...;
cbump_move_simple(&lpaddle, lmove, nb_entities, entities);
cbump_move_simple(&rpaddle, rmove, nb_entities, entities);

// Ball movement.
cbump_vec2 ballmove = { cos(ball_angle), sin(ball_angle) };
ballmove.x *= ball_speed * dt;
ballmove.y *= ball_speed * dt;
cbump_collision collision = cbump_move_simple(&ball, ballmove, nb_entities, entities);

// Collision resolution.
if (collision.normal.x) { // Horizontal colliding (paddles).
    ball_angle = PI - ball_angle;
    ball_speed *= 1.1; // Make the ball faster at each paddle collision.
    cbump_resolve_bounce(&collision);
}
if (collision.normal.y) { // Vertical colliding (top and bottom walls).
    ball_angle = -ball_angle;
    cbump_resolve_bounce(&collision);
}
// Does the ball collide with left wall or right wall?
if (collision.obstacle == &leftwall || collision.obstacle == &rightwall) {
    // Reset the ball position.
    ball_angle = PI/5;
    ball_speed = BallSpeed;
    ball.x = Width/2;
    ball.y = Height/2;
}
```