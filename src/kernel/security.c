/**
 * @file security.c
 * @brief Security and Access Control Implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "security.h"
#include "console.h"
#include "kmalloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_USERS 256

static user_t* users[MAX_USERS];
static int user_count = 0;
static uid_t current_uid = 0;  // Root by default
static gid_t current_gid = 0;

static bool security_initialized = false;

int security_init(void) {
    if (security_initialized) {
        return 0;
    }
    
    console_puts("SECURITY: Initializing security system...\n");
    
    // Initialize user array
    for (int i = 0; i < MAX_USERS; i++) {
        users[i] = NULL;
    }
    
    // Create root user
    user_t* root = (user_t*)kmalloc(sizeof(user_t));
    if (root == NULL) {
        console_puts("SECURITY: ERROR - Failed to allocate root user\n");
        return -1;
    }
    
    root->uid = 0;
    root->gid = 0;
    strcpy(root->username, "root");
    strcpy(root->home_dir, "/root");
    root->is_root = true;
    
    users[0] = root;
    user_count = 1;
    
    security_initialized = true;
    
    console_puts("SECURITY: Initialized\n");
    
    return 0;
}

uid_t security_get_current_uid(void) {
    return current_uid;
}

gid_t security_get_current_gid(void) {
    return current_gid;
}

user_t* security_get_current_user(void) {
    if (!security_initialized) {
        return NULL;
    }
    
    for (int i = 0; i < user_count; i++) {
        if (users[i] != NULL && users[i]->uid == current_uid) {
            return users[i];
        }
    }
    
    return NULL;
}

int security_set_uid(uid_t uid) {
    if (!security_initialized) {
        return -1;
    }
    
    // Check if user exists
    for (int i = 0; i < user_count; i++) {
        if (users[i] != NULL && users[i]->uid == uid) {
            current_uid = uid;
            current_gid = users[i]->gid;
            return 0;
        }
    }
    
    return -1;
}

bool security_check_permission(uint32_t mode, uid_t file_uid, gid_t file_gid) {
    if (!security_initialized) {
        return true;  // Allow if security not initialized
    }
    
    uid_t uid = security_get_current_uid();
    gid_t gid = security_get_current_gid();
    
    // Root can do anything
    if (uid == 0) {
        return true;
    }
    
    // Check owner permissions
    if (uid == file_uid) {
        if (mode & S_IRUSR) return true;  // Assuming read permission
    }
    
    // TODO: Implement full permission checking
    return false;
}

int security_add_user(const char* username, uid_t uid, gid_t gid) {
    if (!security_initialized || username == NULL) {
        return -1;
    }
    
    if (user_count >= MAX_USERS) {
        console_puts("SECURITY: ERROR - Maximum users reached\n");
        return -1;
    }
    
    // Check if user already exists
    for (int i = 0; i < user_count; i++) {
        if (users[i] != NULL && strcmp(users[i]->username, username) == 0) {
            return -1;
        }
        if (users[i] != NULL && users[i]->uid == uid) {
            return -1;
        }
    }
    
    // Allocate new user
    user_t* user = (user_t*)kmalloc(sizeof(user_t));
    if (user == NULL) {
        return -1;
    }
    
    user->uid = uid;
    user->gid = gid;
    strncpy(user->username, username, sizeof(user->username) - 1);
    user->username[sizeof(user->username) - 1] = '\0';
    user->is_root = false;
    
    // Create home directory path
    strcpy(user->home_dir, "/home/");
    strcat(user->home_dir, username);
    
    users[user_count++] = user;
    
    console_puts("SECURITY: Added user: ");
    console_puts(username);
    console_puts("\n");
    
    return 0;
}

user_t* security_get_user_by_name(const char* username) {
    if (!security_initialized || username == NULL) {
        return NULL;
    }
    
    for (int i = 0; i < user_count; i++) {
        if (users[i] != NULL && strcmp(users[i]->username, username) == 0) {
            return users[i];
        }
    }
    
    return NULL;
}

user_t* security_get_user_by_uid(uid_t uid) {
    if (!security_initialized) {
        return NULL;
    }
    
    for (int i = 0; i < user_count; i++) {
        if (users[i] != NULL && users[i]->uid == uid) {
            return users[i];
        }
    }
    
    return NULL;
}

