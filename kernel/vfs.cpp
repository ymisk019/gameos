#include "../include/vfs.h"
#include "../include/memory.h"

FileNode* VFS::root_node = NULL;
FileNode VFS::file_pool[MAX_TOTAL_FILES];
uint32_t VFS::pool_index = 0;

FileNode* VFS::allocate_node(const char* name, FileType type) {
    if (pool_index >= MAX_TOTAL_FILES) return NULL;
    FileNode* node = &file_pool[pool_index++];
    kmemset(node, 0, sizeof(FileNode));
    kstrncpy(node->name, name, MAX_FILENAME_LEN - 1);
    node->type = type;
    node->is_allocated = true;
    return node;
}

void VFS::init() {
    pool_index = 0;
    kmemset(file_pool, 0, sizeof(file_pool));

    root_node = allocate_node("/", FS_DIRECTORY);

    // Create system folders
    create_dir("/games");
    create_dir("/system");
    create_dir("/wallpapers");
    create_dir("/store");
    create_dir("/docs");

    // Create sample readme file
    const char* readme_content = 
        "Welcome to Yazan Game OS v1.0!\n"
        "A lightweight, custom operating system built from scratch.\n"
        "Features:\n"
        "- 5 Playable 2D games\n"
        "- Modern GUI Window Manager\n"
        "- Dynamic Game Store for external .ygp packages\n"
        "- Full Arabic and English language support\n"
        "- Zero Linux / standard OS dependencies\n";
    create_file("/docs/readme.txt", (const uint8_t*)readme_content, kstrlen(readme_content));

    const char* sys_info = 
        "OS: Yazan Game OS\n"
        "Arch: x86 Bare-Metal (32-bit)\n"
        "Kernel: Monolithic Micro-hybrid C++\n"
        "Graphics: VESA VBE Linear Framebuffer\n"
        "Sound: PC Speaker Synthesizer\n";
    create_file("/system/osinfo.cfg", (const uint8_t*)sys_info, kstrlen(sys_info));
}

FileNode* VFS::get_root() {
    return root_node;
}

FileNode* VFS::resolve_path(const char* path) {
    if (!path || path[0] == '\0') return NULL;
    if (kstrcmp(path, "/") == 0) return root_node;

    char temp_path[MAX_FILENAME_LEN * 2];
    kstrncpy(temp_path, path, sizeof(temp_path) - 1);

    FileNode* current = root_node;
    char* token = temp_path;
    if (token[0] == '/') token++;

    char part[MAX_FILENAME_LEN];
    while (*token != '\0') {
        int idx = 0;
        while (*token != '/' && *token != '\0' && idx < MAX_FILENAME_LEN - 1) {
            part[idx++] = *token++;
        }
        part[idx] = '\0';
        if (*token == '/') token++;

        bool found = false;
        for (uint32_t i = 0; i < current->child_count; i++) {
            if (kstrcmp(current->children[i]->name, part) == 0) {
                current = current->children[i];
                found = true;
                break;
            }
        }
        if (!found) return NULL;
    }
    return current;
}

FileNode* VFS::create_dir(const char* path) {
    if (!path) return NULL;
    
    // Find parent directory
    char parent_path[MAX_FILENAME_LEN * 2];
    char dir_name[MAX_FILENAME_LEN];
    kstrncpy(parent_path, path, sizeof(parent_path) - 1);

    int last_slash = -1;
    int len = kstrlen(parent_path);
    for (int i = len - 1; i >= 0; i--) {
        if (parent_path[i] == '/') {
            last_slash = i;
            break;
        }
    }

    if (last_slash == -1) return NULL;

    kstrcpy(dir_name, &parent_path[last_slash + 1]);
    if (last_slash == 0) {
        parent_path[1] = '\0';
    } else {
        parent_path[last_slash] = '\0';
    }

    FileNode* parent = resolve_path(parent_path);
    if (!parent || parent->type != FS_DIRECTORY) return NULL;
    if (parent->child_count >= MAX_FILES_PER_DIR) return NULL;

    FileNode* node = allocate_node(dir_name, FS_DIRECTORY);
    if (!node) return NULL;

    node->parent = parent;
    parent->children[parent->child_count++] = node;
    return node;
}

FileNode* VFS::create_file(const char* path, const uint8_t* data, uint32_t size, FileType type) {
    if (!path) return NULL;

    char parent_path[MAX_FILENAME_LEN * 2];
    char file_name[MAX_FILENAME_LEN];
    kstrncpy(parent_path, path, sizeof(parent_path) - 1);

    int last_slash = -1;
    int len = kstrlen(parent_path);
    for (int i = len - 1; i >= 0; i--) {
        if (parent_path[i] == '/') {
            last_slash = i;
            break;
        }
    }

    if (last_slash == -1) return NULL;

    kstrcpy(file_name, &parent_path[last_slash + 1]);
    if (last_slash == 0) {
        parent_path[1] = '\0';
    } else {
        parent_path[last_slash] = '\0';
    }

    FileNode* parent = resolve_path(parent_path);
    if (!parent || parent->type != FS_DIRECTORY) return NULL;
    if (parent->child_count >= MAX_FILES_PER_DIR) return NULL;

    FileNode* node = allocate_node(file_name, type);
    if (!node) return NULL;

    node->parent = parent;
    node->size = size;
    if (size > 0 && data != NULL) {
        node->data = (uint8_t*)kmalloc(size);
        if (node->data) {
            kmemcpy(node->data, data, size);
        }
    }

    parent->children[parent->child_count++] = node;
    return node;
}

bool VFS::write_file(const char* path, const uint8_t* data, uint32_t size) {
    FileNode* node = resolve_path(path);
    if (!node) {
        return create_file(path, data, size) != NULL;
    }

    if (node->type == FS_DIRECTORY) return false;

    if (node->data) {
        kfree(node->data);
    }
    node->size = size;
    node->data = (uint8_t*)kmalloc(size);
    if (node->data && data) {
        kmemcpy(node->data, data, size);
    }
    return true;
}

uint8_t* VFS::read_file(const char* path, uint32_t* out_size) {
    FileNode* node = resolve_path(path);
    if (!node || node->type == FS_DIRECTORY) return NULL;
    if (out_size) *out_size = node->size;
    return node->data;
}

bool VFS::delete_file(const char* path) {
    FileNode* node = resolve_path(path);
    if (!node || !node->parent) return false;

    FileNode* p = node->parent;
    for (uint32_t i = 0; i < p->child_count; i++) {
        if (p->children[i] == node) {
            if (node->data) kfree(node->data);
            node->is_allocated = false;
            // Shift left
            for (uint32_t j = i; j < p->child_count - 1; j++) {
                p->children[j] = p->children[j + 1];
            }
            p->child_count--;
            return true;
        }
    }
    return false;
}

void VFS::list_directory(FileNode* dir, void (*callback)(const char*, FileType, uint32_t)) {
    if (!dir || dir->type != FS_DIRECTORY || !callback) return;
    for (uint32_t i = 0; i < dir->child_count; i++) {
        callback(dir->children[i]->name, dir->children[i]->type, dir->children[i]->size);
    }
}
