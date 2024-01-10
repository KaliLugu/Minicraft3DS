#include "Editor.h"

#include "EditorOptions.h"
#include "EditorTiles.h"

#include "../Globals.h"
#include "../Ingame.h"
#include "../Render.h"

#define EDITOR_COMPONENT_TILES 0
#define EDITOR_COMPONENT_ENTITIES 1
#define EDITOR_COMPONENT_SCRIPT 2
#define EDITOR_COMPONENT_OPTIONS 3

int editorComponent;

void editorInit() {
    // setup editor specific values
    playerCount = 0;
    bool load = false;
    char *filename = currentFileName;

    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        fclose(file);
        load = true;
    }

    // call game start
    // TODO: This currently works, because startGame with playerCount=0 does nothing but load the game, when that changes, this will break
    startGame(load, filename);

    // initialise editor components
    editorComponent = 0;

    editorTilesInit();
    editorOptionsInit();
}

void editorTick() {
    if (localInputs.k_menuPrev.clicked) {
        if (editorComponent > 0)
            editorComponent--;
    }
    if (localInputs.k_menuNext.clicked) {
        if (editorComponent < 3)
            editorComponent++;
    }

    switch (editorComponent) {
    case EDITOR_COMPONENT_TILES:
        editorTilesTick();
        break;
    case EDITOR_COMPONENT_ENTITIES:

        break;
    case EDITOR_COMPONENT_SCRIPT:

        break;
    case EDITOR_COMPONENT_OPTIONS:
        editorOptionsTick();
        break;
    }
}

char *editorGetComponentName(int comp) {
    switch (comp) {
    case EDITOR_COMPONENT_TILES:
        return "Tiles";
    case EDITOR_COMPONENT_ENTITIES:
        return "Entities";
    case EDITOR_COMPONENT_SCRIPT:
        return "Script";
    case EDITOR_COMPONENT_OPTIONS:
        return "Options";
    default:
        return "";
    }
}

void editorRender(int screen, int width, int height) {
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        // draw top bar
        renderFrame(0, 0, 25, 4, 0x109220FF);
        renderTextColor("Editor Mode:", (width / 2 - (12 * 8)) / 2 + 1, 8 + 1, 0x000000FF);
        renderTextColor("Editor Mode:", (width / 2 - (12 * 8)) / 2, 8, 0xFFFFFFFF);

        char *text;
        text = editorGetComponentName(editorComponent - 1);
        renderTextColor(text, 36 - (strlen(text) * 8) / 2 + 1, 20 + 1, 0x000000FF);
        renderTextColor(text, 36 - (strlen(text) * 8) / 2, 20, 0x404040FF);

        text = editorGetComponentName(editorComponent);
        renderTextColor(text, 100 - (strlen(text) * 8) / 2 + 1, 20 + 1, 0x000000FF);
        renderTextColor(text, 100 - (strlen(text) * 8) / 2, 20, 0xFFFFFFFF);

        text = editorGetComponentName(editorComponent + 1);
        renderTextColor(text, 164 - (strlen(text) * 8) / 2 + 1, 20 + 1, 0x000000FF);
        renderTextColor(text, 164 - (strlen(text) * 8) / 2, 20, 0x404040FF);

        renderButtonIcon(localInputs.k_menuPrev.input & -localInputs.k_menuPrev.input, 3, 3);
        renderButtonIcon(localInputs.k_menuNext.input & -localInputs.k_menuNext.input, 181, 3);

        // render component
        switch (editorComponent) {
        case EDITOR_COMPONENT_TILES:
            editorTilesRenderTop(screen, width, height);
            break;
        case EDITOR_COMPONENT_ENTITIES:

            break;
        case EDITOR_COMPONENT_SCRIPT:

            break;
        case EDITOR_COMPONENT_OPTIONS:
            editorOptionsRenderTop(screen, width, height);
            break;
        }
    }
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        // render component
        switch (editorComponent) {
        case EDITOR_COMPONENT_TILES:
            editorTilesRenderBottom(screen, width, height);
            break;
        case EDITOR_COMPONENT_ENTITIES:

            break;
        case EDITOR_COMPONENT_SCRIPT:

            break;
        case EDITOR_COMPONENT_OPTIONS:
            editorOptionsRenderBottom(screen, width, height);
            break;
        }
    }
}
