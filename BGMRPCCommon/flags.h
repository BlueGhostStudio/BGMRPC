#ifndef FLAGS_H
#define FLAGS_H
#include <QtCore>

namespace NS_BGMRPC {
enum Ctrl {
    // CTRL_DAEMONCTRL = 0,
    CTRL_STOPSERVER,
    //    CTRL_CREATEOBJ,
    //    CTRL_GETSETTINGSFILE,
    CTRL_CONFIG,
    CTRL_SETTING,
    CTRL_DETACHOBJECT,
    CTRL_REGISTER,
    CTRL_CHECKOBJECT,
    CTRL_LISTOBJECTS
};
enum Config { CNF_PATH_ROOT, CNF_PATH_BIN, CNF_PATH_INTERFACES, CNF_PATH_LOGS };

enum Data {
    DATA_ERROR = 0,
    DATA_CLIENTID,
    DATA_LOCALCALL_CLIENTID,
    DATA_NONBLOCK_LOCALCALL
};

enum Call {
    CALL_UNDEFINED = -1,
    CALL_REMOTE = 0,
    CALL_INTERNAL = 1,
    CALL_INTERNAL_NOBLOCK = 2
};

enum Error { ERR_NOOBJ = 0, ERR_NOMETHOD = 1, ERR_ACCESS };
}  // namespace NS_BGMRPC

#endif  // FLAGS_H
