#include "texturepack.h"
#include "render/TextureManager.h"

#include "ZipHelper.h"

#define MAX_FILENAME 256

bool texturepackUseDefaultIcons = true;
bool texturepackUseDefaultPlayer = true;
bool texturepackUseDefaultFont = true;
bool texturepackUseDefaultBottom = true;

void toLowerString(char *str) {
    int i;
    for (i = 0; str[i] != '\0'; i++)
        str[i] = (char)tolower((unsigned char)str[i]);
}

int getTexturePackComment(char *filename, char *cmmtBuf) {
    // Open the zip file
    unzFile *zipfile = unzOpen(filename);
    if (zipfile == NULL)
        return 1; // Error: ZipFile could not be opened.

    // Get info about the zip file
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK) {
        unzClose(zipfile);
        return 2; // Error: Could not read global info
    }

    unzGetGlobalComment(zipfile, cmmtBuf, 58);

    unzClose(zipfile);

    return 0;
}

int loadTexture(char *filename) {
    char lowerFilename[MAX_FILENAME];
    strcpy(lowerFilename, filename);
    toLowerString(lowerFilename);

    if (strcmp(lowerFilename, "icons.png") == 0) {
        if (imageIcons != NULL) {
            freeImage(imageIcons);
            imageIcons = NULL;
        }

        Image image = loadImage(filename);
        if (image == NULL) {
            return 0;
        }

        imageIcons = image;

        texturepackUseDefaultIcons = false;
    } else if (strcmp(lowerFilename, "player.png") == 0) {
        if (imagePlayerSprites != NULL) {
            freeImage(imagePlayerSprites);
            imagePlayerSprites = NULL;
        }

        Image image = loadImage(filename);
        if (image == NULL) {
            return 0;
        }

        imagePlayerSprites = image;

        texturepackUseDefaultPlayer = false;
    } else if (strcmp(lowerFilename, "font.png") == 0) {
        if (imageFont != NULL) {
            freeImage(imageFont);
            imageFont = NULL;
        }

        Image image = loadImage(filename);
        if (image == NULL) {
            return 0;
        }

        imageFont = image;

        texturepackUseDefaultFont = false;
    } else if (strcmp(lowerFilename, "bottombg.png") == 0) {
        if (imageBottombg != NULL) {
            freeImage(imageBottombg);
            imageBottombg = NULL;
        }

        Image image = loadImage(filename);
        if (image == NULL) {
            return 0;
        }

        imageBottombg = image;

        texturepackUseDefaultBottom = false;
    }

    return 0;
}

int loadTexturePack(char *filename) {
    texturepackUseDefaultIcons = true;
    texturepackUseDefaultPlayer = true;
    texturepackUseDefaultFont = true;
    texturepackUseDefaultBottom = true;

    if (filename != NULL && unzipAndLoad(filename, &loadTexture, NULL, ZIPHELPER_CLEANUP_FILES) != 0) {
        return 1;
    }

    if (texturepackUseDefaultIcons) {
        if (imageIcons != NULL) {
            freeImage(imageIcons);
            imageIcons = NULL;
        }
        imageIcons = loadImage("romfs:/icons.png");
    }
    if (texturepackUseDefaultPlayer) {
        if (imagePlayerSprites != NULL) {
            freeImage(imagePlayerSprites);
            imagePlayerSprites = NULL;
        }
        imagePlayerSprites = loadImage("romfs:/player.png");
    }
    if (texturepackUseDefaultFont) {
        if (imageFont != NULL) {
            freeImage(imageFont);
            imageFont = NULL;
        }
        imageFont = loadImage("romfs:/font.png");
    }
    if (texturepackUseDefaultBottom) {
        if (imageBottombg != NULL) {
            freeImage(imageBottombg);
            imageBottombg = NULL;
        }
        imageBottombg = loadImage("romfs:/bottombg.png");
    }
    textureManagerReload();

    return 0;
}
