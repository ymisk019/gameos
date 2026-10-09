#ifndef VFS_H
#define VFS_H

#include "types.h"

#define MAX_FILENAME_LEN 64
#define MAX_FILES_PER_DIR 64
#define MAX_TOTAL_FILES 256

enum FileType {
    FS_FILE,
    FS_DIRECTORY,
    FS_GAME_PACKAGE
};

struct FileNode {
    char name[MAX_FILENAME_LEN];
    FileType type;
    uint32_t size;
    uint8_t* data;
    bool is_allocated;
    FileNode* parent;
    FileNode* children[MAX_FILES_PER_DIR];
    uint32_t child_count;
};

class VFS {
private:
    static FileNode* root_node;
    static FileNode file_pool[MAX_TOTAL_FILES];
    static uint32_t pool_index;

    static FileNode* allocate_node(const char* name, FileType type);

public:
    static void init();
    static FileNode* get_root();
    static FileNode* resolve_path(const char* path);
    static FileNode* create_file(const char* path, const uint8_t* data, uint32_t size, FileType type = FS_FILE);
    static FileNode* create_dir(const char* path);
    static bool write_file(const char* path, const uint8_t* data, uint32_t size);
    static uint8_t* read_file(const char* path, uint32_t* out_size);
    static bool delete_file(const char* path);
    static void list_directory(FileNode* dir, void (*callback)(const char* name, FileType type, uint32_t size));
};

#endif // VFS_H
