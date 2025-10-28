/**
 * @file security.h
 * @brief Security and Access Control
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef SECURITY_H
#define SECURITY_H

#include <stdint.h>
#include <stdbool.h>

// User and Group IDs
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t pid_t;

// Permissions
#define S_IRUSR 0400  // Owner read
#define S_IWUSR 0200  // Owner write
#define S_IXUSR 0100  // Owner execute
#define S_IRGRP 0040  // Group read
#define S_IWGRP 0020  // Group write
#define S_IXGRP 0010  // Group execute
#define S_IROTH 0004  // Others read
#define S_IWOTH 0002  // Others write
#define S_IXOTH 0001  // Others execute

// File permissions
#define S_IFREG 0100000  // Regular file
#define S_IFDIR 0040000  // Directory
#define S_IFCHR 0020000  // Character device
#define S_IFBLK 0060000  // Block device

// User structure
typedef struct {
    uid_t uid;
    gid_t gid;
    char username[32];
    char home_dir[256];
    bool is_root;
} user_t;

// Ternary permission states (-1: deny, 0: inherit, +1: allow)
typedef int8_t permission_state_t;
#define PERM_DENY  -1
#define PERM_INHERIT 0
#define PERM_ALLOW  +1

// ACL entry
typedef struct acl_entry {
    uid_t uid;
    gid_t gid;
    uint32_t rwx_bits;  // Read/write/execute bits
    struct acl_entry* next;
} acl_entry_t;

// Ternary permission check result
typedef struct {
    permission_state_t owner;
    permission_state_t group;
    permission_state_t other;
    permission_state_t final;  // Final result using ternary gate
} ternary_permission_result_t;

// Security functions
int security_init(void);
uid_t security_get_current_uid(void);
gid_t security_get_current_gid(void);
user_t* security_get_current_user(void);
int security_set_uid(uid_t uid);
bool security_check_permission(uint32_t mode, uid_t file_uid, gid_t file_gid);
ternary_permission_result_t security_check_permission_ternary(uint32_t mode, uid_t file_uid, gid_t file_gid);
int security_add_user(const char* username, uid_t uid, gid_t gid);
user_t* security_get_user_by_name(const char* username);
user_t* security_get_user_by_uid(uid_t uid);

// ACL functions
acl_entry_t* security_create_acl(void);
void security_add_acl_entry(acl_entry_t* acl, uid_t uid, gid_t gid, uint32_t rwx);
bool security_check_acl(acl_entry_t* acl, uid_t uid, gid_t gid, uint32_t operation);
void security_destroy_acl(acl_entry_t* acl);

#endif // SECURITY_H

