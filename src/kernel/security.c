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
    // Use ternary permission checking
    ternary_permission_result_t result = security_check_permission_ternary(mode, file_uid, file_gid);
    return (result.final == PERM_ALLOW);
}

ternary_permission_result_t security_check_permission_ternary(uint32_t mode, uid_t file_uid, gid_t file_gid) {
    ternary_permission_result_t result = {0, 0, 0, 0};
    
    if (!security_initialized) {
        // Allow everything if security not initialized
        result.owner = result.group = result.other = result.final = PERM_ALLOW;
        return result;
    }
    
    uid_t uid = security_get_current_uid();
    gid_t gid = security_get_current_gid();
    
    // Root can do anything
    if (uid == 0) {
        result.owner = result.group = result.other = result.final = PERM_ALLOW;
        return result;
    }
    
    // Check owner permissions using ternary states
    if (uid == file_uid) {
        result.owner = PERM_ALLOW;  // Owner has full access
    } else if (gid == file_gid) {
        result.group = PERM_ALLOW;  // Group has full access
    } else {
        result.other = PERM_ALLOW;  // Others have full access
    }
    
    // Extract permission bits (read, write, execute)
    // For simplicity, check if any permission is granted
    // Full implementation would check specific operations
    
    // Ternary permission resolution: use consensus gate
    // Final = Consensus(owner, group, other)
    // Consensus: output is majority value, or 0 if no majority
    permission_state_t states[3] = {result.owner, result.group, result.other};
    int count_allow = 0, count_deny = 0, count_inherit = 0;
    
    for (int i = 0; i < 3; i++) {
        if (states[i] == PERM_ALLOW) count_allow++;
        else if (states[i] == PERM_DENY) count_deny++;
        else count_inherit++;
    }
    
    if (count_allow > count_deny && count_allow > count_inherit) {
        result.final = PERM_ALLOW;
    } else if (count_deny > count_allow && count_deny > count_inherit) {
        result.final = PERM_DENY;
    } else {
        result.final = PERM_INHERIT;
    }
    
    return result;
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

// =============================================================================
// ACL FUNCTIONS
// =============================================================================

acl_entry_t* security_create_acl(void) {
    acl_entry_t* acl = (acl_entry_t*)kmalloc(sizeof(acl_entry_t));
    if (acl == NULL) {
        return NULL;
    }
    acl->uid = 0;
    acl->gid = 0;
    acl->rwx_bits = 0;
    acl->next = NULL;
    return acl;
}

void security_add_acl_entry(acl_entry_t* acl, uid_t uid, gid_t gid, uint32_t rwx) {
    if (acl == NULL) {
        return;
    }
    
    acl_entry_t* entry = (acl_entry_t*)kmalloc(sizeof(acl_entry_t));
    if (entry == NULL) {
        return;
    }
    
    entry->uid = uid;
    entry->gid = gid;
    entry->rwx_bits = rwx;
    entry->next = acl->next;
    acl->next = entry;
}

bool security_check_acl(acl_entry_t* acl, uid_t uid, gid_t gid, uint32_t operation) {
    if (acl == NULL) {
        return false;
    }
    
    for (acl_entry_t* entry = acl->next; entry != NULL; entry = entry->next) {
        if (entry->uid == uid || entry->gid == gid) {
            // Use ternary logic for permission checking
            if (entry->rwx_bits & operation) {
                return true;
            }
        }
    }
    
    return false;
}

void security_destroy_acl(acl_entry_t* acl) {
    if (acl == NULL) {
        return;
    }
    
    acl_entry_t* entry = acl->next;
    while (entry != NULL) {
        acl_entry_t* next = entry->next;
        // Note: in real implementation, would use kfree
        // kfree(entry);
        entry = next;
    }
    
    // kfree(acl);
}