#include <string.h>
#include <zip.h>

#include "textures.h"
#include "colors.h"

Texture2D beeTexture;
Texture2D beetleTexture;
Texture2D grasshopperTexture;
Texture2D soldierAntTexture;
Texture2D spiderTexture;

Image windowIcon;

// Player colors
Color player1Color = { 230, 230, 230, 255 };
Color player2Color = { 100, 100, 100, 255 };

// Menu colors
Color menuBgColor     = { 255, 255, 255, 230 };
Color menuBorderColor = { 40, 40, 40, 255 };

// Tile highlight
Color selectTileColor = { 0, 121, 241, 200 };
Color moveTileColor   = { 253, 249, 0, 200 };
Color menuTileColor   = BLUE;

Texture2D LoadTextureFromMemory(const char *fileType, const unsigned char *fileData, int dataSize)
{
	Texture2D texture;

	Image tmp = LoadImageFromMemory(fileType, fileData, dataSize);
	texture = LoadTextureFromImage(tmp);
	UnloadImage(tmp);

	return texture;
}

void LoadTexturepack(char *path)
{
  zip_t *za;
	zip_file_t *zf;
	zip_stat_t sb;
	unsigned char *buf;
	int errorp = 0;

	za = zip_open(path, 0, &errorp);

	switch (errorp)
	{
		case ZIP_ER_NOENT:
			TraceLog(LOG_ERROR, "The texturepack specified does not exist");
			return;
		
		case ZIP_ER_NOZIP:
			TraceLog(LOG_ERROR, "The texturepack specified is not a valid texturepack");
			return;
	}

	for (int i = 0; i < zip_get_num_entries(za, 0); i++)
	{ 
		zip_stat_index(za, i, 0, &sb);

		zf = zip_fopen_index(za, i, 0);

		buf = MemAlloc(sb.size);

		if (zip_fread(zf, buf, sb.size) != -1)
		{
			if (!strcmp(sb.name, "bee.png"))
				beeTexture = LoadTextureFromMemory(".png", buf, sb.size);
			else if (!strcmp(sb.name, "beetle.png"))
				beetleTexture  = LoadTextureFromMemory(".png", buf, sb.size);
			else if (!strcmp(sb.name, "grasshopper.png"))
				grasshopperTexture = LoadTextureFromMemory(".png", buf, sb.size);
			else if (!strcmp(sb.name, "soldier_ant.png"))
				soldierAntTexture = LoadTextureFromMemory(".png", buf, sb.size);
			else if (!strcmp(sb.name, "spider.png"))
				spiderTexture = LoadTextureFromMemory(".png", buf, sb.size);
			else if (!strcmp(sb.name, "icon.png"))
				windowIcon = LoadImageFromMemory(".png", buf, sb.size);
		}

		MemFree(buf);
		zip_fclose(zf);
	}

	zip_close(za);
}

void FreeTexturepack()
{	
	UnloadTexture(beeTexture);
	UnloadTexture(beetleTexture);
	UnloadTexture(grasshopperTexture);
	UnloadTexture(soldierAntTexture);
	UnloadTexture(spiderTexture);

	UnloadImage(windowIcon);
}
