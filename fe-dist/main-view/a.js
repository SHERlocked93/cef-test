document.write(location.href);

let eventer = {
    dic: {},
    emit(eventName, ...obj) {
        console.log("[eventer emit]", eventName, obj);
        if (!this.dic[eventName]) {
            console.warn(`没有找到该事件的监听函数：${eventName}`)
            return;
        }
        this.dic[eventName].forEach((func) => func(...obj))
    },
    on(eventName, callBack) {
        console.log("[eventer on]", eventName);
        if (!this.dic[eventName]) this.dic[eventName] = [callBack];
        else this.dic[eventName].push(callBack)
    },
    once(eventName, callBack) {
        console.log("[eventer once]", eventName);
        let callBackWrap = (...obj) => {
            let idx = this.dic[eventName].findIndex((v) => v == callBackWrap)
            if (idx >= 0) this.dic[eventName].splice(idx, 1)
            callBack(...obj)
        }
        if (!this.dic[eventName]) this.dic[eventName] = [callBackWrap];
        else this.dic[eventName].push(callBackWrap)
    },
    off(eventName, callBack) {
        console.log("[eventer off]", eventName);
        if (!this.dic[eventName]?.length < 1) return
        if (!callBack) {
            offAll(eventName)
            return
        }
        let index = this.dic[eventName].findIndex((v) => v == callBack);
        if (index >= 0) this.dic[eventName].splice(index, 1)
    },
    offAll(eventName) {
        console.log("[eventer offAll]", eventName);
        delete this.dic[eventName]
    }
}

let native = {
    randomNum_(len = 12) {
        return Math.floor(Math.pow(10, len) * Math.random());
    },
    call(msgName, ...params) {
        return new Promise((resolve, reject) => {
            let eventName = `${msgName}_${this.randomNum_()}`;
            eventer.once(eventName, resolve);
            window.globalBridge(eventName, ...params);
        });
    },
    callReuse(msgName, ...params) {
        return new Promise((resolve, reject) => {
            let eventName = `${msgName}_${this.randomNum_()}`;
            eventer.on(eventName, resolve);
            window.globalBridge(eventName, ...params);
        });
    },
    init() {
        window.globalBridge(`native_registe_callback`, (msgName, ...otherParams) => {
            console.log("native_registe_callback", msgName, otherParams)
            eventer.emit(msgName, ...otherParams);
        });
    },
};
native.init();


//==========================================================================

{
    let browserWindow = {
        getMsgName(args) {
            return `window_${args.callee.name}`
        },
        minimize() {
            let msgName = this.getMsgName(arguments);
            window.globalBridge(msgName);
        },
        maximize() {
            let msgName = this.getMsgName(arguments);
            window.globalBridge(msgName);
        },
        close() {
            let msgName = this.getMsgName(arguments);
            window.globalBridge(msgName);
        },
        restore() {
            let msgName = this.getMsgName(arguments);
            window.globalBridge(msgName);
        },
        isMaximized() {
            let hSpan = window.outerHeight - screen.availHeight
            let wSpan = window.outerWidth - screen.availWidth
            return Math.abs(hSpan) <= 2 && Math.abs(wSpan) <= 2
        },
        maximized: false,
        init() {
            window.addEventListener('resize', () => {
                let curState = this.isMaximized()
                let oldState = this.maximized
                this.maximized = curState
                if (oldState && !curState) eventer.emit(`window_unMaximize`);
                else if (!oldState && curState) eventer.emit(`window_maximize`)
            })
        }
    }


    const minimizeBtn = document.querySelector("#minimize-btn");
    const maximizeBtn = document.querySelector("#maximize-btn");
    const restoreBtn = document.querySelector("#restore-btn");
    const closeBtn = document.querySelector("#close-btn");

    minimizeBtn.addEventListener("click", () => browserWindow.minimize());
    closeBtn.addEventListener("click", () => browserWindow.close());
    maximizeBtn.addEventListener("click", () => browserWindow.maximize())
    restoreBtn.addEventListener("click", () => browserWindow.restore())
    browserWindow.init();

    eventer.on("window_maximize", () => {
        maximizeBtn.setAttribute("style", "display:none");
        restoreBtn.setAttribute("style", "display:block");
    })
    eventer.on("window_unMaximize", () => {
        restoreBtn.setAttribute("style", "display:none");
        maximizeBtn.setAttribute("style", "display:block");
    })
}

{
    let system = {
        getMsgName(args) {
            return `system_${args.callee.name}`;
        },
        async getOSVersion() {
            let msgName = this.getMsgName(arguments);
            let osVersion = await native.call(msgName);
            return osVersion;
        },
    };

    let getSystemInfoBtn = document.querySelector("#getSystemInfoBtn");
    getSystemInfoBtn.addEventListener("click", async () => {
        let result = await system.getOSVersion();
        console.log({result})
        alert(result)
    })
}


{
    let dialog = {
        getMsgName(args) {
            return `dialog_${args.callee.name}`;
        },
        async openFile(param) {
            let msgName = this.getMsgName(arguments);
            let resultStr = await native.call(msgName, JSON.stringify(param));
            return JSON.parse(resultStr);
        },
        async openFolder(param) {
            let msgName = this.getMsgName(arguments);
            let resultStr = await native.call(msgName, JSON.stringify(param));
            return JSON.parse(resultStr);
        },
    };

    let fileOpenBtn = document.querySelector("#fileOpenBtn");
    fileOpenBtn.addEventListener("click", async () => {
        let param = {
            title: "这是打开文件对话框的标题",
            defaultPath: "C:\\Program Files",
            filters: ["image/*", "text/*"],
            filterIndex: 1,
            multiSelections: true,
        };
        let files = await dialog.openFile(param);
        console.log(files);
    });

    let dirOpneBtn = document.querySelector("#dirOpneBtn");
    dirOpneBtn.addEventListener("click", async () => {
        let param = {
            title: "这是打开文件夹对话框的标题",
            defaultPath: "C:\\Program Files",
        };
        let files = await dialog.openFolder(param);
        console.log(files);
    });
}


let file = {
    getMsgName(args) {
        return `file_${args.callee.name}`
    },
    readFile(param) {
        let msgName = this.getMsgName(arguments);
        let onData = (obj) => {
            if (param.onData) {
                param.onData(obj);
            }
        }
        let onFinish = (obj) => {
            if (param.onFinish) {
                param.onFinish(obj);
            }
            eventer.off(`${msgName}_data`, onData)
        }
        eventer.on(`${msgName}_data`, onData);
        eventer.once(`${msgName}_finish`, onFinish);
        native.callReuse(msgName, JSON.stringify(param));
    }
}
let readFileBtn = document.querySelector("#readFileBtn");
readFileBtn.addEventListener("click", async () => {
    let result = "";
    let param = {
        filePath: "C:\\PandaProjects\\Codes\\cef_131_mytest\\README.md",
        onData(chunk) {
            let decoder = new TextDecoder('utf-8', {ignoreBOM: true})
            let str = decoder.decode(chunk)
            result += str;
            console.log('ondata', result)
        },
        onFinish(data) {
            console.log("文件读取完成")
            console.log(result);
            alert(result)
        }
    }
    file.readFile(param);
    console.log({eventer})
})

let db = {
    getMsgName(args) {
        return `db_${args.callee.name}`;
    },
    async open(param) {
        let msgName = this.getMsgName(arguments);
        let result = await native.call(msgName, JSON.stringify(param));
        return JSON.parse(result);
    },
    async close() {
        let msgName = this.getMsgName(arguments);
        let result = await native.call(msgName);
        return JSON.parse(result);
    },
    async execute(param) {
        let msgName = this.getMsgName(arguments);
        let result = await native.call(msgName, JSON.stringify(param));
        return JSON.parse(result);
    },
};

{
    let db = {
        getMsgName(args) {
            return `db_${args.callee.name}`;
        },
        async open(param) {
            let msgName = this.getMsgName(arguments);
            let result = await native.call(msgName, JSON.stringify(param));
            return JSON.parse(result);
        },
        async close() {
            let msgName = this.getMsgName(arguments);
            let result = await native.call(msgName);
            return JSON.parse(result);
        },
        async execute(param) {
            let msgName = this.getMsgName(arguments);
            let result = await native.call(msgName, JSON.stringify(param));
            return JSON.parse(result);
        },
    };


    let dbBtn = document.querySelector("#dbBtn");
    dbBtn.addEventListener("click", async () => {
        if (dbBtn.innerHTML === "打开数据库") {
            let result = await db.open({
                dbPath: "C:\\PandaProjects\\Codes\\cef_131_mytest\\data\\test.db",
            });
            console.log(result);
            dbBtn.innerHTML = "关闭数据库";
        } else if (dbBtn.innerHTML === "关闭数据库") {
            let result = await db.close();
            console.log(result);
            dbBtn.innerHTML = "打开数据库";
        }
    });

    let insertBtn = document.querySelector("#insertBtn");
    insertBtn.addEventListener("click", async () => {
        let msgs = [
            {
                message: `天接云涛连晓雾，星河欲转千帆舞。仿佛梦魂归帝所。闻天语，殷勤问我归何处。 我报路长嗟日暮，学诗谩有惊人句。九万里风鹏正举。风休住，蓬舟吹取三山去！`,
                fromUser: `李清照`,
                toUser: "辛弃疾",
            },
            {
                message: `醉里挑灯看剑，梦回吹角连营。八百里分麾下炙，五十弦翻塞外声。沙场秋点兵。 马作的卢飞快，弓如霹雳弦惊。了却君王天下事，赢得生前身后名。可怜白发生！`,
                fromUser: `辛弃疾`,
                toUser: "李清照",
            },
        ];
        let sqls = [];
        for (let i = 0; i < 60; i++) {
            let msg = msgs[i % 2];
            sqls.push(
                `insert into Message(Message, fromUser, toUser)
                 values ('${msg.message}', '${msg.fromUser}', '${msg.toUser}');`
            );
        }
        let result = await db.execute({sql: sqls.join("")});
        console.log(result);
    });

    let selectBtn = document.querySelector("#selectBtn");
    selectBtn.addEventListener("click", async () => {
        let sql = `select rowid, *
                   from Message limit 16;`;
        let result = await db.execute({sql});
        console.log(result);
    });

    let updateBtn = document.querySelector("#updateBtn");
    updateBtn.addEventListener("click", async () => {
        let obj = {
            message:
                "怒发冲冠，凭栏处、潇潇雨歇。抬望眼、仰天长啸，壮怀激烈。三十功名尘与土，八千里路云和月。莫等闲、白了少年头，空悲切。 靖康耻，犹未雪。臣子恨，何时灭。驾长车，踏破贺兰山缺。壮志饥餐胡虏肉，笑谈渴饮匈奴血。待从头、收拾旧山河，朝天阙。",
            fromUser: "岳飞",
            toUser: "辛弃疾",
        };
        let sql = `update Message
                   set Message  = '${obj.message}',
                       fromUser = '${obj.fromUser}',
                       toUser='${obj.toUser}'
                   where rowid in (select rowid from Message limit 1);`;
        let result = await db.execute({sql});
        console.log(result);
    });

    let deleteBtn = document.querySelector("#deleteBtn");
    deleteBtn.addEventListener("click", async () => {
        let sql = `delete
                   from Message
                   where rowid in (select rowid from Message limit 1);`;
        let result = await db.execute({sql});
        console.log(result);
    });

}
