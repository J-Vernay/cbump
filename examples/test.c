
#include <cbump.h>
#include <assert.h>
#include <stdio.h>

int main() {
    cbump_box ground = { 0, 0, 100, 0 }; // x, y, w, h
    cbump_box object = cbump_box_center(30, 30, 20, 20); // x,y of center
    cbump_box* entities[] = { &ground, &object };

    cbump_vec2 move = { 50, -50 };
    //cbump_move_step(&object, move, 2, entities);
    cbump_move(&object, move, 2, entities, NULL, NULL); // by default, sliding

    // point of impact was at [x=40, y=0], but with sliding, ending at x=70
    assert(object.x == 70 && object.y == 0);

    return 0;
}