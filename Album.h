//
//  Album.h
//  ComicFormat
//
//  Created by Thomas Mathews on 11-04-30.
//  Copyright 2011 Interactive TM. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ALBUM_ITEM_PATH_SIZE 10240

struct AlbumHeader {
    char title[256];
    char description[1024];
    char tags[1024];
    char author[256];
    char credits[1024];
    char copyright[512];
    int coverImageSize;
    int numberOfItems;
};

void AlbumCreate(char *filename, struct AlbumHeader album);
struct AlbumHeader AlbumCreateWithDetails(char *filename, const char title[256], const char description[1024], const char tags[1024], const char author[256], const char credits[1024], const char copyright[512], char *coverPath);
struct AlbumHeader AlbumOpen(char *filename);
char *AlbumGetCover(char *filename);
char *AlbumGetItemData(char *filename, int index, int *itemSize);
void AlbumSetCover(char *filename, char *coverPath);
void AlbumSetItems(char *filename, char items[][ALBUM_ITEM_PATH_SIZE], int totalItems);