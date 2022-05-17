#include "excuse.h"

/* Return the type of method called. */
static enum func_type chk_func(DBusMessage *msg, const char *iface) {
    assert(msg);

    if (dbus_message_is_method_call(msg, iface, DBUS_BOFH_TST_FUNC))
        return TST;

    if (dbus_message_is_method_call(msg, iface, DBUS_BOFH_STP_FUNC))
        return STP;

    if (dbus_message_is_method_call(msg, iface, DBUS_BOFH_EXC_FUNC))
        return EXC;

    return ERR;
}

/* Run the server loop. */
void loop() {
    int ret = 0;
    DBusMessage *msg = NULL;
    DBusConnection *conn = NULL;
    DBusError err;
    enum func_type ft;

    printf("[%s]: inf: Listening for calls to \"%s%s%s.{%s,%s,%s}\"...\n"
           "[%s]: inf: The functions can be invoked via `dbus-send\' as: "
           "dbus-send --bus=\"%s\" --print-reply --dest=\"%s\" \"%s\" "
           "\"%s.{%s,%s,%s}\"\n",
           DBUS_BOFH_NAME, DBUS_BOFH_SERVICE, DBUS_BOFH_OBJECT, DBUS_BOFH_IFACE,
           DBUS_BOFH_EXC_FUNC, DBUS_BOFH_STP_FUNC, DBUS_BOFH_TST_FUNC,
           DBUS_BOFH_NAME, getenv(DBUS_ADDR_ENV), DBUS_BOFH_SERVICE,
           DBUS_BOFH_OBJECT, DBUS_BOFH_IFACE, DBUS_BOFH_EXC_FUNC,
           DBUS_BOFH_STP_FUNC, DBUS_BOFH_TST_FUNC);

    /* Set a seed for the random function. */
    srand(time(NULL));

    /* Initialize for errors. */
    dbus_error_init(&err);

    /* Establish connection with the bus. */
    conn = dbus_bus_get(DBUS_BUS_STARTER, &err);
    if (dbus_error_is_set(&err)) {
        printf("[%s]: err: Bus registation: %s.\n", DBUS_BOFH_NAME,
               err.message);
        dbus_error_free(&err);
    }

    /* Exit on error. */
    if (!conn) {
        printf("[%s]: err: Bus connection: %p.\n", DBUS_BOFH_NAME,
               (void *)conn);
        exit(1);
    }

    /* Request a name on the bus (like registration). */
    ret = dbus_bus_request_name(conn, DBUS_BOFH_SERVICE,
                                DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        printf("[%s]: err: Registration failed: %s.\n", DBUS_BOFH_NAME,
               err.message);
        dbus_error_free(&err);
    }

    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        printf("[%s]: err: Not a primary owner; abort (%d).\n", DBUS_BOFH_NAME,
               ret);
        exit(ret);
    }

    /* Now, loop waiting for new messages. */
    while (1) {
        /* Read (non-blocking) incoming messages. */
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);

        /* Otherwise, sleep for a second and try again. */
        if (!msg) {
            sleep(1);
            continue;
        }

        /*
         * Check if the message received is a valid
         * method call. If it is, send the reply.
         */
        ft = chk_func(msg, DBUS_BOFH_IFACE);

        switch (ft) {
        case TST:
        case STP:
            excuse(msg, conn, ft);
            dbus_message_unref(msg);
            goto ret;

        case EXC:
            /*
             * Generate a random excuse and
             * free the message buffer.
             */
            excuse(msg, conn, ft);
            dbus_message_unref(msg);
            break;

        default:
            dbus_message_unref(msg);
            break;
        }
    }

ret:
    /*
     * Free the connection (instead of closing;
     * shared connections should not be closed).
     */
    sleep(1);
    dbus_connection_flush(conn);
    dbus_connection_unref(conn);
}

int main(void) { loop(); }
