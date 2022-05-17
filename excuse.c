/*
 * dbus-excuse: A BOFH-style excuse server that runs on `dbus'.
 */

#include "excuse.h"

/* Return a random excuse. */
static const char *excuse_rand(void) {
    assert(excuses);
    return excuses[rand() % MAX_EXCUSES];
}

/* Construct and send a reply message for the remote procedure call. */
void excuse(DBusMessage *msg, DBusConnection *conn, enum func_type ft) {
    const char *ex = NULL;
    DBusMessage *reply = NULL;
    DBusMessageIter iter;
    dbus_uint32_t serial = 0;

    switch (ft) {
    case EXC:
        if (!(ex = excuse_rand())) {
            printf("[%s]: err: Oops. Failed to an get excuse. Sigh.\n",
                   DBUS_BOFH_NAME);
            exit(1);
        }
        break;

    case TST:
        assert(excuses);
        ex = excuses[0];
        break;

    case STP:
        ex = "Bye!";
        break;

    default:
        return;
    }

    /* Create a reply. */
    reply = dbus_message_new_method_return(msg);

    /* Create an iterator for appending the reply. */
    dbus_message_iter_init_append(reply, &iter);

    /* Add the excuse to the reply. */
    if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &ex)) {
        printf("[%s]: err: Out of memory: failed to construct a reply.\n",
               DBUS_BOFH_NAME);
        exit(1);
    }

    /* Send the reply and flush the connection. */
    if (!dbus_connection_send(conn, reply, &serial)) {
        printf("[%s]: err: Out of memory: failed to send message.\n",
               DBUS_BOFH_NAME);
        exit(1);
    }

    dbus_connection_flush(conn);

    /* Free the reply. */
    dbus_message_unref(reply);
}
