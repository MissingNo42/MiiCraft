
#include "engine/render/interface.h"
#include "engine/render/bloc.h"


void GUI::renderCursor(Player &player, Wiimote &wiimote){

    if (!player.inventory.open) {
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Start drawing

        GX_Position3f32(-x, y, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(15)); // Top left

        GX_Position3f32(x, y, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(15)); // Top right

        GX_Position3f32(x, -y, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(16)); // Bottom right

        GX_Position3f32(-x, -y, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(16)); // Bottom left

        GX_End();
    }

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

    a = 0, b = 0;
    auto wd = wiimote.wd;
    if (wd->ir.valid) {
        a = 2 * wd->ir.x / (f32) Renderer::rmode->fbWidth - 1;
        b = -2 * wd->ir.y / (f32) Renderer::rmode->xfbHeight + 1;
    }
    a -= x / 2;
    b += y / 2;

    GX_Position3f32(-x + a, y + b, 0);
    GX_Normal1x8(4);
    GX_Color1u32(white);
    GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(15)); // Top left

    GX_Position3f32(x + a, y + b, 0);
    GX_Normal1x8(4);
    GX_Color1u32(white);
    GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(15)); // Top right

    GX_Position3f32(x + a, -y + b, 0);
    GX_Normal1x8(4);
    GX_Color1u32(white);
    GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(16)); // Bottom right

    GX_Position3f32(-x + a, -y + b, 0);
    GX_Normal1x8(4);
    GX_Color1u32(white);
    GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(16)); // Bottom left

    GX_End();
}

void GUI::renderInventory(Player &player) const {
    if (player.inventory.open){
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Start drawing
        if (player.inventory.craftOpen) {

            GX_Position3f32(-0.8, 0.65, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(13)); // Top left

            GX_Position3f32(0.8, 0.65, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(27), BLOCK_COORD(13)); // Top right

            GX_Position3f32(0.8, -0.65, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(27), BLOCK_COORD(23)); // Bottom right

            GX_Position3f32(-0.8, -0.65, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(23)); // Bottom left
        }
        else{
            GX_Position3f32(-0.8, 0.65, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(2)); // Top left

            GX_Position3f32(0.8, 0.65, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(27), BLOCK_COORD(2)); // Top right

            GX_Position3f32(0.8, -0.65, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(27), BLOCK_COORD(12)); // Bottom right

            GX_Position3f32(-0.8, -0.65, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(12)); // Bottom left
        }
        GX_End();
        GX_Begin(GX_QUADS, GX_VTXFMT0, 108); // Start drawing
        for ( int i = 0; i <27 ; i++){
            GX_Position3f32(-0.682 + (i % 9) * 0.158 ,-0.02 - (i / 9) * 0.142,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[1+ player.inventory.currentPage * 3+i/9][i%9].item.type].x[2], blocData[player.inventory.inventory[1+ player.inventory.currentPage *3+i/9][i%9].item.type].y[2]); // Top left

            GX_Position3f32(-0.582+ i % 9 * 0.158,-0.02 - (i / 9) * 0.142,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[1+ player.inventory.currentPage*3+i/9][i%9].item.type].x[2] + OFFSET, blocData[player.inventory.inventory[1+ player.inventory.currentPage *3+i/9][i%9].item.type].y[2]); // Top right

            GX_Position3f32(-0.582 + i % 9 * 0.158,-0.12 - (i / 9) * 0.142,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[1+ player.inventory.currentPage*3+i/9][i%9].item.type].x[2] + OFFSET, blocData[player.inventory.inventory[1+ player.inventory.currentPage*3+i/9][i%9].item.type].y[2] + OFFSET); // Bottom right

            GX_Position3f32(-0.682 + i % 9 * 0.158,-0.12 - (i / 9) * 0.142,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[1+ player.inventory.currentPage*3+i/9][i%9].item.type].x[2],blocData[player.inventory.inventory[1+ player.inventory.currentPage*3+i/9][i%9].item.type].y[2] + OFFSET); // Bottom left
        }
        GX_End();
        GX_Begin(GX_QUADS, GX_VTXFMT0, 36);
        for (int i = 0; i < 9; i++){
            GX_Position3f32(-0.682 + i * 0.158 ,-0.475,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[0][i].item.type].x[2], blocData[player.inventory.inventory[0][i].item.type].y[2]); // Top left

            GX_Position3f32(-0.582+ i * 0.158,-0.475 ,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[0][i].item.type].x[2] + OFFSET, blocData[player.inventory.inventory[0][i].item.type].y[2]); // Top right

            GX_Position3f32(-0.582 + i  * 0.158,-0.575 ,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[0][i].item.type].x[2] + OFFSET, blocData[player.inventory.inventory[0][i].item.type].y[2] + OFFSET); // Bottom right

            GX_Position3f32(-0.682 + i  * 0.158,-0.575 ,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[0][i].item.type].x[2], blocData[player.inventory.inventory[0][i].item.type].y[2] + OFFSET); // Bottom left
        }
        GX_End();
        GX_Begin(GX_QUADS, GX_VTXFMT0, 36);
        if(player.inventory.craftOpen){
            for (int i = 0; i < 9; i++) {
                GX_Position3f32(-0.505 + i % 3 * 0.16, 0.515 - i / 3 * 0.148, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.inventory.craftSlots[i].item.type].x[2],
                                blocData[player.inventory.craftSlots[i].item.type].y[2]); // Top left

                GX_Position3f32(-0.405 + i % 3 * 0.16, 0.515 - i / 3 * 0.148, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.inventory.craftSlots[i].item.type].x[2] + OFFSET,
                                blocData[player.inventory.craftSlots[i].item.type].y[2]); // Top right

                GX_Position3f32(-0.405 + i % 3 * 0.16, 0.415 - i / 3 * 0.148, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.inventory.craftSlots[i].item.type].x[2] + OFFSET,
                                blocData[player.inventory.craftSlots[i].item.type].y[2] + OFFSET); // Bottom right

                GX_Position3f32(-0.505 + i % 3 * 0.16, 0.415 - i / 3 * 0.148, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.inventory.craftSlots[i].item.type].x[2],
                                blocData[player.inventory.craftSlots[i].item.type].y[2] + OFFSET); // Bottom left
            }
        }
        else {
            for (int i = 0; i < 9; i++) {
                GX_Position3f32(0.11 + i % 3 * 0.158, 0.5 - i / 3 * 0.148, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.inventory.craftSlots[i].item.type].x[2],
                                blocData[player.inventory.craftSlots[i].item.type].y[2]); // Top left

                GX_Position3f32(0.21 + i % 3 * 0.158, 0.5 - i / 3 * 0.148, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.inventory.craftSlots[i].item.type].x[2] + OFFSET,
                                blocData[player.inventory.craftSlots[i].item.type].y[2]); // Top right

                GX_Position3f32(0.21 + i % 3 * 0.158, 0.4 - i / 3 * 0.148, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.inventory.craftSlots[i].item.type].x[2] + OFFSET,
                                blocData[player.inventory.craftSlots[i].item.type].y[2] + OFFSET); // Bottom right

                GX_Position3f32(0.11 + i % 3 * 0.158, 0.4 - i / 3 * 0.148, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.inventory.craftSlots[i].item.type].x[2],
                                blocData[player.inventory.craftSlots[i].item.type].y[2] + OFFSET); // Bottom left
            }
        }
        GX_End();
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        if(player.inventory.craftOpen){
            GX_Position3f32(0.32, 0.39, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.craftSlots[9].item.type].x[2],
                            blocData[player.inventory.craftSlots[9].item.type].y[2]); // Top left

            GX_Position3f32(0.47, 0.39, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.craftSlots[9].item.type].x[2] + OFFSET,
                            blocData[player.inventory.craftSlots[9].item.type].y[2]); // Top right

            GX_Position3f32(0.47, 0.24, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.craftSlots[9].item.type].x[2] + OFFSET,
                            blocData[player.inventory.craftSlots[9].item.type].y[2] + OFFSET); // Bottom right

            GX_Position3f32(0.32, 0.24, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.craftSlots[9].item.type].x[2],
                            blocData[player.inventory.craftSlots[9].item.type].y[2] + OFFSET); // Bottom left
        }
        else {
            GX_Position3f32(0.6, 0.42, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.craftSlots[9].item.type].x[2],
                            blocData[player.inventory.craftSlots[9].item.type].y[2]); // Top left

            GX_Position3f32(0.7, 0.42, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.craftSlots[9].item.type].x[2] + OFFSET,
                            blocData[player.inventory.craftSlots[9].item.type].y[2]); // Top right

            GX_Position3f32(0.7, 0.32, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.craftSlots[9].item.type].x[2] + OFFSET,
                            blocData[player.inventory.craftSlots[9].item.type].y[2] + OFFSET); // Bottom right

            GX_Position3f32(0.6, 0.32, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.craftSlots[9].item.type].x[2],
                            blocData[player.inventory.craftSlots[9].item.type].y[2] + OFFSET); // Bottom left
        }
        GX_End();

        // chiffres

        if(player.inventory.craftOpen){
            if (player.inventory.craftSlots[9].quantity > 0) {
                GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
                GX_Position3f32(0.42, 0.28, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[9].quantity / 10),
                                BLOCK_COORD(12)); // Top left

                GX_Position3f32(0.46, 0.28, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[9].quantity / 10),
                                BLOCK_COORD(12)); // Top right

                GX_Position3f32(0.46, 0.24, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[9].quantity / 10),
                                BLOCK_COORD(13)); // Bottom right

                GX_Position3f32(0.42, 0.24, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[9].quantity / 10),
                                BLOCK_COORD(13)); // Bottom left





                GX_Position3f32(0.46, 0.28, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[9].quantity % 10),
                                BLOCK_COORD(12)); // Top left

                GX_Position3f32(0.5, 0.28, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[9].quantity % 10),
                                BLOCK_COORD(12)); // Top right

                GX_Position3f32(0.5, 0.24, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[9].quantity % 10),
                                BLOCK_COORD(13)); // Bottom right

                GX_Position3f32(0.46, 0.24, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[9].quantity % 10),
                                BLOCK_COORD(13)); // Bottom left
                GX_End();
            }
        }
        else {
            if (player.inventory.craftSlots[9].quantity > 0) {
                GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
                GX_Position3f32(0.64, 0.355, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[9].quantity / 10),
                                BLOCK_COORD(12)); // Top left

                GX_Position3f32(0.68, 0.355, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[9].quantity / 10),
                                BLOCK_COORD(12)); // Top right

                GX_Position3f32(0.68, 0.315, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[9].quantity / 10),
                                BLOCK_COORD(13)); // Bottom right

                GX_Position3f32(0.64, 0.315, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[9].quantity / 10),
                                BLOCK_COORD(13)); // Bottom left





                GX_Position3f32(0.68, 0.355, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[9].quantity % 10),
                                BLOCK_COORD(12)); // Top left

                GX_Position3f32(0.72, 0.355, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[9].quantity % 10),
                                BLOCK_COORD(12)); // Top right

                GX_Position3f32(0.72, 0.315, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[9].quantity % 10),
                                BLOCK_COORD(13)); // Bottom right

                GX_Position3f32(0.68, 0.315, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[9].quantity % 10),
                                BLOCK_COORD(13)); // Bottom left
                GX_End();
            }
        }


        for (int i = 0; i < 9; i++){
            if (player.inventory.craftOpen){
                if (player.inventory.craftSlots[i].quantity > 0) {
                    GX_Begin(GX_QUADS, GX_VTXFMT0, 8);

                    GX_Position3f32(-0.45 + i % 3 * 0.158, 0.44 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[i].quantity / 10),
                                    BLOCK_COORD(12)); // Top left

                    GX_Position3f32(-0.41 + i % 3 * 0.158, 0.44 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[i].quantity / 10),
                                    BLOCK_COORD(12)); // Top right

                    GX_Position3f32(-0.41 + i % 3 * 0.158, 0.40 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[i].quantity / 10),
                                    BLOCK_COORD(13)); // Bottom right

                    GX_Position3f32(-0.45 + i % 3 * 0.158, 0.40 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[i].quantity / 10),
                                    BLOCK_COORD(13)); // Bottom left

                    GX_Position3f32(-0.41 + i % 3 * 0.158, 0.44 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[i].quantity % 10),
                                    BLOCK_COORD(12)); // Top left

                    GX_Position3f32(-0.37 + i % 3 * 0.158, 0.44 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[i].quantity % 10),
                                    BLOCK_COORD(12)); // Top right

                    GX_Position3f32(-0.37 + i % 3 * 0.158, 0.40 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[i].quantity % 10),
                                    BLOCK_COORD(13)); // Bottom right

                    GX_Position3f32(-0.41 + i % 3 * 0.158, 0.40 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[i].quantity % 10),
                                    BLOCK_COORD(13)); // Bottom left
                    GX_End();
                }
            }
            else {
                if (player.inventory.craftSlots[i].quantity > 0) {
                    GX_Begin(GX_QUADS, GX_VTXFMT0, 8);

                    GX_Position3f32(0.15 + i % 3 * 0.158, 0.44 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[i].quantity / 10),
                                    BLOCK_COORD(12)); // Top left

                    GX_Position3f32(0.19 + i % 3 * 0.158, 0.44 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[i].quantity / 10),
                                    BLOCK_COORD(12)); // Top right

                    GX_Position3f32(0.19 + i % 3 * 0.158, 0.40 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[i].quantity / 10),
                                    BLOCK_COORD(13)); // Bottom right

                    GX_Position3f32(0.15 + i % 3 * 0.158, 0.40 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[i].quantity / 10),
                                    BLOCK_COORD(13)); // Bottom left

                    GX_Position3f32(0.19 + i % 3 * 0.158, 0.44 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[i].quantity % 10),
                                    BLOCK_COORD(12)); // Top left

                    GX_Position3f32(0.23 + i % 3 * 0.158, 0.44 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[i].quantity % 10),
                                    BLOCK_COORD(12)); // Top right

                    GX_Position3f32(0.23 + i % 3 * 0.158, 0.40 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.craftSlots[i].quantity % 10),
                                    BLOCK_COORD(13)); // Bottom right

                    GX_Position3f32(0.19 + i % 3 * 0.158, 0.40 - i / 3 * 0.148, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.craftSlots[i].quantity % 10),
                                    BLOCK_COORD(13)); // Bottom left
                    GX_End();
                }
            }

            for (int i = 0; i < 9; i++) {
                if (player.inventory.inventory[0][i].quantity > 0) {
                    GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
                    GX_Position3f32(-0.64 + i * 0.158, -0.535, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.inventory[0][i].quantity/10),
                                    BLOCK_COORD(12)); // Top left

                    GX_Position3f32(-0.6 + i * 0.158, -0.535, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.inventory[0][i].quantity/10),
                                    BLOCK_COORD(12)); // Top right

                    GX_Position3f32(-0.6 + i * 0.158, -0.575, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.inventory[0][i].quantity/10),
                                    BLOCK_COORD(13)); // Bottom right

                    GX_Position3f32(-0.64 + i * 0.158, -0.575, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.inventory[0][i].quantity/10),
                                    BLOCK_COORD(13)); // Bottom left



                    GX_Position3f32(-0.6 + i * 0.158, -0.535, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.inventory[0][i].quantity%10),
                                    BLOCK_COORD(12)); // Top left

                    GX_Position3f32(-0.56 + i * 0.158, -0.535, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.inventory[0][i].quantity%10),
                                    BLOCK_COORD(12)); // Top right

                    GX_Position3f32(-0.56 + i * 0.158, -0.575, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.inventory[0][i].quantity%10),
                                    BLOCK_COORD(13)); // Bottom right

                    GX_Position3f32(-0.6 + i * 0.158, -0.575, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.inventory[0][i].quantity%10),
                                    BLOCK_COORD(13)); // Bottom left

                    GX_End();
                }
            }

            for ( int i = 0; i <27 ; i++){
                if (player.inventory.inventory[1+ player.inventory.currentPage * 3+i/9][i%9].quantity > 0) {
                    GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
                    GX_Position3f32(-0.65 + (i % 9) * 0.158, -0.08 - (i / 9) * 0.142, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.inventory[1 + player.inventory.currentPage * 3 +
                                                                                i / 9][i % 9].quantity / 10),
                                    BLOCK_COORD(12)); // Top left

                    GX_Position3f32(-0.61 + i % 9 * 0.158, -0.08 - (i / 9) * 0.142, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.inventory[1 + player.inventory.currentPage * 3 +
                                                                                i / 9][i % 9].quantity / 10),
                                    BLOCK_COORD(12)); // Top right

                    GX_Position3f32(-0.61 + i % 9 * 0.158, -0.12 - (i / 9) * 0.142, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.inventory[1 + player.inventory.currentPage * 3 +
                                                                                i / 9][i % 9].quantity / 10),
                                    BLOCK_COORD(13)); // Bottom right

                    GX_Position3f32(-0.65 + i % 9 * 0.158, -0.12 - (i / 9) * 0.142, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.inventory[1 + player.inventory.currentPage * 3 +
                                                                                i / 9][i % 9].quantity / 10),
                                    BLOCK_COORD(13)); // Bottom left

                    GX_Position3f32(-0.61 + i % 9 * 0.158, -0.08 - (i / 9) * 0.142, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.inventory[1 + player.inventory.currentPage * 3 +
                                                                                i / 9][i % 9].quantity % 10),
                                    BLOCK_COORD(12)); // Top left

                    GX_Position3f32(-0.57 + i % 9 * 0.158, -0.08 - (i / 9) * 0.142, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.inventory[1 + player.inventory.currentPage * 3 +
                                                                                i / 9][i % 9].quantity % 10),
                                    BLOCK_COORD(12)); // Top right

                    GX_Position3f32(-0.57 + i % 9 * 0.158, -0.12 - (i / 9) * 0.142, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(17 + player.inventory.inventory[1 + player.inventory.currentPage * 3 +
                                                                                i / 9][i % 9].quantity % 10),
                                    BLOCK_COORD(13)); // Bottom right

                    GX_Position3f32(-0.61 + i % 9 * 0.158, -0.12 - (i / 9) * 0.142, 0);
                    GX_Normal1x8(4);
                    GX_Color1u32(white);
                    GX_TexCoord2f32(BLOCK_COORD(16 + player.inventory.inventory[1 + player.inventory.currentPage * 3 +
                                                                                i / 9][i % 9].quantity % 10),
                                    BLOCK_COORD(13)); // Bottom left

                    GX_End();
                }
            }
        }
    }

    else {


        GX_Begin(GX_QUADS, GX_VTXFMT0, 44); // Start drawing

        GX_Position3f32(-0.47, -0.6, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(0)); // Top left

        GX_Position3f32(0.47, -0.6, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(25), BLOCK_COORD(0)); // Top right

        GX_Position3f32(0.47, -0.7, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(25), BLOCK_COORD(1)); // Bottom right

        GX_Position3f32(-0.47, -0.7, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(1)); // Bottom left





        GX_Position3f32(-0.45 + 0.098 * player.inventory.selectedSlot, -0.6, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(1)); // Top left

        GX_Position3f32(-0.33 + 0.098 * player.inventory.selectedSlot, -0.6, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(17), BLOCK_COORD(1)); // Top right

        GX_Position3f32(-0.33 + 0.098 * player.inventory.selectedSlot, -0.7, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(17), BLOCK_COORD(2)); // Bottom right

        GX_Position3f32(-0.45 + 0.098 * player.inventory.selectedSlot, -0.7, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(2)); // Bottom left




        for (int i = 0; i < 9; i++) {
            GX_Position3f32(-0.42 + i * 0.098, -0.62, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[0][i].item.type].x[2], blocData[player.inventory.inventory[0][i].item.type].y[2]); // Top left

            GX_Position3f32(-0.36 + i * 0.098, -0.62, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[0][i].item.type].x[2] + OFFSET, blocData[player.inventory.inventory[0][i].item.type].y[2]); // Top right

            GX_Position3f32(-0.36 + i * 0.098, -0.68, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[0][i].item.type].x[2] + OFFSET,
                            blocData[player.inventory.inventory[0][i].item.type].y[2] + OFFSET); // Bottom right

            GX_Position3f32(-0.42 + i * 0.098, -0.68, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(blocData[player.inventory.inventory[0][i].item.type].x[2],
                            blocData[player.inventory.inventory[0][i].item.type].y[2] + OFFSET); // Bottom left
        }
    }

    GX_End();

    if (player.inventory.open && player.inventory.pickedItem.item.type != BlockType::Air) {
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Start drawing

        GX_Position3f32(-x + a, b -0.01, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(blocData[player.inventory.pickedItem.item.type].x[2], blocData[player.inventory.pickedItem.item.type].y[2]); // Top left

        GX_Position3f32( a -0.01,  b -0.01, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(blocData[player.inventory.pickedItem.item.type].x[2] + OFFSET, blocData[player.inventory.pickedItem.item.type].y[2]); // Top right

        GX_Position3f32( a -0.01, -y + b, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(blocData[player.inventory.pickedItem.item.type].x[2] + OFFSET, blocData[player.inventory.pickedItem.item.type].y[2] + OFFSET); // Bottom right

        GX_Position3f32(-x + a, -y + b, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(blocData[player.inventory.pickedItem.item.type].x[2], blocData[player.inventory.pickedItem.item.type].y[2] + OFFSET); // Bottom left

        GX_End();
    }
}