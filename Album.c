//
//  Album.c
//  ComicFormat
//

#include "Album.h"

void AlbumCreate(char *filename, struct AlbumHeader album) {
    FILE *albumFile = fopen(filename, "wb");
    if ( albumFile ) {
        fwrite(&album, sizeof(album), 1, albumFile);
        fclose(albumFile);
    }
}

struct AlbumHeader AlbumCreateWithDetails(
                                        char *filename,
                                        const char title[256],
                                        const char description[1024],
                                        const char tags[1024],
                                        const char author[256],
                                        const char credits[1024],
                                        const char copyright[512],
                                        char *coverPath
                                        ) {
    
    struct AlbumHeader album;
    strcpy(album.title, title);
    strcpy(album.description, description);
    strcpy(album.tags, tags);
    strcpy(album.author, author);
    strcpy(album.credits, credits);
    strcpy(album.copyright, copyright);
    album.numberOfItems = 0;
    album.coverImageSize = 0;
    
    AlbumCreate(filename, album);
    AlbumSetCover(filename, coverPath);
    return album;
}

struct AlbumHeader AlbumOpen(char *filename) {
    struct AlbumHeader album;
    FILE *albumFile = fopen(filename, "rb");
    
    if ( albumFile ) {
        fread(&album, sizeof(album), 1, albumFile);
        fclose(albumFile);
    }
    
    return album;
}

char *AlbumGetCover(char *filename) {
    struct AlbumHeader album;
    FILE *albumFile = fopen(filename, "rb");
    
    if ( !albumFile ) return NULL;
    
    int headerSize = sizeof(album);
    fread(&album, headerSize, 1, albumFile);
    
    char *cover = (char *)malloc(album.coverImageSize);
    fread(cover, album.coverImageSize, 1, albumFile);
    fclose(albumFile);
    
    return cover;
}

char *AlbumGetItemData(char *filename, int index, int *itemSize) {
    struct AlbumHeader album;
    FILE *albumFile = fopen(filename, "rb");
    
    if ( !albumFile ) return NULL;
    
    int headerSize = sizeof(album);
    fread(&album, headerSize, 1, albumFile);
    fseek(albumFile, album.coverImageSize, SEEK_CUR);
    
    for ( int i = 0; i < album.numberOfItems; i++ ) {
        int currentItemId, currentItemSize;
        fread(&currentItemId, sizeof(int), 1, albumFile);
        fread(&currentItemSize, sizeof(int), 1, albumFile);
        
        if ( currentItemId != index ) {
            fseek(albumFile, currentItemSize, SEEK_CUR);
            continue;
        }
        
        char *currentItem = (char *)malloc(currentItemSize + 1);
        fread(currentItem, currentItemSize, 1, albumFile);
        
        *itemSize = currentItemSize;
        fclose(albumFile);
        return currentItem;
    }
    
    fclose(albumFile);
    return 0;
}

void AlbumSetCover(char *filename, char *coverPath) {
    struct AlbumHeader album;
    FILE *albumFile = fopen(filename, "rb");
    if ( !albumFile ) return;
    
    fread(&album, sizeof(album), 1, albumFile);
    fclose(albumFile);
    
    FILE *file = fopen(coverPath, "rb");
    if ( !file ) return;
    
    fseek(file, 0, SEEK_END);
    int size = (int)ftell(file);
    rewind(file);
    
    char *fileData = (char *)malloc(size);
    fread(fileData, size, 1, file);
    fclose(file);
    
    album.coverImageSize = size;
    
    albumFile = fopen(filename, "wb");
    fwrite(&album, sizeof(album), 1, albumFile);
    fwrite(fileData, size, 1, albumFile);
    free(fileData);
    fclose(albumFile);
}

void AlbumSetItems(char *filename, char items[][ALBUM_ITEM_PATH_SIZE], int totalItems) {    
    struct AlbumHeader album;
    FILE *albumFile = fopen(filename, "rb");
    if ( !albumFile ) return;
    
    fread(&album, sizeof(album), 1, albumFile);
    
    char *cover = (char *)malloc(album.coverImageSize);
    fread(cover, album.coverImageSize, 1, albumFile);
    fclose(albumFile);
    
    album.numberOfItems = totalItems;
    
    albumFile = fopen(filename, "wb");
    fwrite(&album, sizeof(album), 1, albumFile);
    fwrite(cover, album.coverImageSize, 1, albumFile);
    
    for ( int i = 0; i < totalItems; i++ ) {
        int identity = i + 1;
        int size;
        
        fwrite(&identity, sizeof(int), 1, albumFile);
        
        FILE *file = fopen((const char *)items[i], "rb");
        if ( !file ) {
            size = 0;
            fwrite(&size, sizeof(int), 1, albumFile);
            continue;
        }
        
        fseek(file, 0, SEEK_END);
        size = (int)ftell(file);
        rewind(file);
        
        char *fileData = (char *)malloc(size);
        fread(fileData, size, 1, file);
        fclose(file);
        
        fwrite(&size, sizeof(int), 1, albumFile);
        fwrite(fileData, size, 1, albumFile);
        free(fileData);
    }
    
    fclose(albumFile);
    free(cover);
}
