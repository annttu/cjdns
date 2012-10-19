/* vim: set expandtab ts=4 sw=4: */
/*
 * You may redistribute this program and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef Admin_H
#define Admin_H

#include "benc/Dict.h"
#include "exception/ExceptionHandler.h"
#include "memory/Allocator.h"
#include "util/Log.h"
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif

#include <event2/event.h>
#include <stdbool.h>

#define Admin_FUNCTION(name) void (* name)(Dict* input, void* context, String* txid)

struct Admin;

struct Admin_FunctionArg
{
    char* name;
    char* type;
    bool required;
};

/**
 * @param arguments an array of struct Admin_FunctionArg specifying what functions are available
 *                  and of those, which are required.
 *        Example C code:
 *            struct Admin_FunctionArg adma[2] = {
 *                { .name = "password", .required = 1, .type = "String" },
 *                { .name = "authType", .required = 0, .type = "Int" }
 *            };
 *            Admin_registerFunction("AuthorizedPasswords_add", add, context, true, adma, admin);
 */
void Admin_registerFunctionWithArgCount(char* name,
                                        Admin_FUNCTION(callback),
                                        void* callbackContext,
                                        bool needsAuth,
                                        struct Admin_FunctionArg* arguments,
                                        int argCount,
                                        struct Admin* admin);
#define Admin_registerFunction(name, cb, ctx, needsAuth, args, admin) \
    Admin_registerFunctionWithArgCount(                                                           \
        name, cb, ctx, needsAuth, args, (sizeof(args) / sizeof(struct Admin_FunctionArg)), admin)

#define Admin_sendMessage_CHANNEL_CLOSED -1
int Admin_sendMessage(Dict* message, String* txid, struct Admin* admin);

struct Admin* Admin_new(int fromAngelFd,
                        int toAngelFd,
                        struct Allocator* alloc,
                        struct Log* logger,
                        struct event_base* eventBase,
                        String* password,
                        uint8_t pipeMagic[8]);

struct Admin* Admin_newProc(struct sockaddr_storage* addr,
                            int addrLen,
                            String* password,
                            char* user,
                            struct event_base* eventBase,
                            struct ExceptionHandler* eh,
                            struct Log* logger,
                            struct Allocator* allocator);

void Admin_getConnectInfo(struct sockaddr_storage** addrPtr,
                          int* addrLenPtr,
                          String** passwordPtr,
                          struct Admin* admin);
#endif
