/// \example pong.c 
/// \file pong.c Small Pong example for using cbump.
/// It can be played with W,S (player 1) and UP,DOWN (player 2)
/// keys.

#include <cbump.h>
/// Using SDL2 as graphic library.
#include <SDL2/SDL.h>

#include <math.h>
const float PI = 3.1415926;

/// Constants, in pixels.
enum {
    Width        = 800,
    Height       = 500,
    BallSize     = 20,
    BallSpeed    = 300,
    PaddleWidth  = 10,
    PaddleHeight = 100,
    PaddleSpeed  = 300, // in pixels per second.
};


int main() {
    // Initialisation of SDL.
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* w;
    SDL_Renderer* r;
    if (SDL_CreateWindowAndRenderer(Width, Height, 0, &w, &r) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return 1;
    }

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
    
    // Access to keyboard.
    Uint8 const* keyboard = SDL_GetKeyboardState(NULL);
    
    // Used to measure delta time.
    Uint32 t = SDL_GetTicks();
    for (;;) {
        // Event handling.
        SDL_Event e;
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT)
                goto end;
        if (keyboard[SDL_SCANCODE_ESCAPE])
            goto end;

        // Time update.
        Uint32 t2 = SDL_GetTicks();
        float dt = (t2 - t) * 0.001f; // Conversion from ms to seconds.
        t = t2;
        
        // Input processing for moving paddles.
        cbump_vec2 lmove = {0}, rmove = {0};
        if (keyboard[SDL_SCANCODE_W])    lmove.y -= 1;
        if (keyboard[SDL_SCANCODE_S])    lmove.y += 1;
        if (keyboard[SDL_SCANCODE_UP])   rmove.y -= 1;
        if (keyboard[SDL_SCANCODE_DOWN]) rmove.y += 1;
        lmove.y *= PaddleSpeed * dt;
        rmove.y *= PaddleSpeed * dt;

        // Paddle movement.
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

        // Rendering.
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderClear(r);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);

        SDL_RenderFillRectF(r, (SDL_FRect*)&ball);
        SDL_RenderFillRectF(r, (SDL_FRect*)&lpaddle);
        SDL_RenderFillRectF(r, (SDL_FRect*)&rpaddle);
        SDL_RenderPresent(r);

        SDL_Delay(10);
    }
end:

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    SDL_Quit();
    return 0;
}