document.write(location.href);

let eventer = {
    dic: {},
    emit(eventName, ...obj) {
        if (!this.dic[eventName]) {
            console.warn(`没有找到该事件的监听函数：${eventName}`)
            return;
        }
        this.dic[eventName].forEach((func) => func(...obj))
    },
    on(eventName, callBack) {
        if (!this.dic[eventName]) this.dic[eventName] = [callBack];
        else this.dic[eventName].push(callBack)
    },
    once(eventName, callBack) {
        let callBackWrap = (...obj) => {
            let index = this.dic[eventName].findIndex((v) => v == callBackWrap)
            if (index >= 0) this.dic[eventName].splice(index, 1)
            callBack(...obj)
        }
        if (!this.dic[eventName]) this.dic[eventName] = [callBackWrap];
        else this.dic[eventName].push(callBackWrap)
    },
    off(eventName, callBack) {
        if (!this.dic[eventName]?.length < 1) return
        if (!callBack) {
            offAll(eventName)
            return
        }
        let index = this.dic[eventName].findIndex((v) => v == callBack);
        if (index >= 0) this.dic[eventName].splice(index, 1)
    },
    offAll(eventName) {
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
    init() {
        window.globalBridge(`native_registe_callback`, (msgName, ...otherParams) => {
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
        let result = await system['getOSVersion']();
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
