#include "MenuSettingsTexture.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

#include <dirent.h>
#include <stdio.h>

#include "../texturepack.h"

char tpFileNames[1000][256];
char tpFileComment[1000][60];
int tpFileCount = 0;
int isLoadingTP = 0;

void readTPFiles() {
    memset(&tpFileNames, 0, sizeof(tpFileNames));     // reset tp fileNames
    memset(&tpFileComment, 0, sizeof(tpFileComment)); // reset zip comments
    tpFileCount = 1;                                  // 0 = default.
    DIR *d;
    struct dirent *dir;
    d = opendir("./texturepacks/");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".zip") != NULL) { // Check if filename contains ".zip"
                strncpy(tpFileNames[tpFileCount], dir->d_name, strlen(dir->d_name) - 4);

                char fullDirName[256];
                sprintf(fullDirName, "./texturepacks/%s", dir->d_name);
                // int err =
                getTexturePackComment(fullDirName, tpFileComment[tpFileCount]);
                /*
                if(err > 0){
                    char errorText[10];
                    sprintf(errorText,"err:%d",err);
                    strncpy(tpFileComment[tpFileCount], errorText, strlen(errorText));
                }*/
                ++tpFileCount;
            }
        }
        closedir(d);
    }
}

void menuSettingsTextureInit() {
    readTPFiles();
}

void menuSettingsTextureTick() {
    if (localInputs.k_up.clicked) {
        --currentSelection;
        if (currentSelection < 0)
            currentSelection = tpFileCount - 1;
    }
    if (localInputs.k_down.clicked) {
        ++currentSelection;
        if (currentSelection > tpFileCount - 1)
            currentSelection = 0;
    }
    if (localInputs.k_decline.clicked) {
        if (isLoadingTP < 1) {
            currentMenu = MENU_SETTINGS;
            currentSelection = 1;
        }
    }
    if (localInputs.k_accept.clicked) {
        if (currentSelection > 0) {
            isLoadingTP = 4;
        } else {
            loadTexturePack(NULL);

            currentMenu = MENU_SETTINGS;
            currentSelection = 1;
            remove("lastTP.bin");
        }
    }
}

void menuSettingsTextureRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        offsetX = 0;
        offsetY = (currentSelection * 40) - 48;

        renderTextCentered("Texture Packs", -16, width);
        for (int i = 0; i < tpFileCount; ++i) {
            int color = 0x323292FF;
            char *text = tpFileNames[i];
            char *cmtText = tpFileComment[i];
            if (i == 0) {
                text = "Default";
                cmtText = " Normal look of the  game ";
                color = 0x601092FF;
            }
            if (i != currentSelection)
                color &= 0xFFFFFF7F; // Darken color.

            renderFrame(1, i * 5, 24, (i * 5) + 5, color);
            renderTextCentered(text, i * 40 + 8, width);

            if (strlen(cmtText) > 20) {
                char cmtTxt1[30], cmtTxt2[30];
                strncpy(cmtTxt1, cmtText, 20);
                strncpy(cmtTxt2, cmtText + 20, strlen(cmtText) - 20);
                renderTextColor(cmtTxt1, (width / 2 - (strlen(cmtTxt1) * 8)) / 2, i * 40 + 18, 0xAFAFAFFF);
                renderTextColor(cmtTxt2, (width / 2 - (strlen(cmtTxt2) * 8)) / 2, i * 40 + 27, 0xAFAFAFFF);
            } else {
                renderTextColor(cmtText, (width / 2 - (strlen(cmtText) * 8)) / 2, i * 40 + 21, 0xAFAFAFFF);
            }
        }

        offsetX = 0;
        offsetY = 0;
        if (isLoadingTP > 0) {
            --isLoadingTP;
            renderFrame(1, 5, 24, 9, 0xFF666666);
            renderTextColor("Loading Texture pack...", (width / 2 - (23 * 8)) / 2, 54, 0xFFFF10FF);
            if (isLoadingTP == 0) {
                char fullDirName[256];
                sprintf(fullDirName, "./texturepacks/%s.zip", tpFileNames[currentSelection]);
                loadedtp = loadTexturePack(fullDirName);

                FILE *fs = fopen("lastTP.bin", "w");
                fprintf(fs, "%s", fullDirName);
                fclose(fs);

                currentMenu = MENU_SETTINGS;
                currentSelection = 1;
            }
        }
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextCentered("Press   to select", 40, width);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 55, 35);
        renderTextCentered("Press   to return", 65, width);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 55, 60);
    }
}
