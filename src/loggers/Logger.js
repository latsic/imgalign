const singleton = Symbol();
const singletonEnforcer = Symbol();

export class Logger {

  constructor(enforcer) {

    if(enforcer != singletonEnforcer) throw 'Cannot construct Singleton!';
    
    this._infoOn = () => true;
    this._errorOn = () => true;
    this._assertOn = () => true;
    this._externOn = () => true;

    // eslint-disable-next-line no-console
    this._consoleStdLog = console.log;
    // eslint-disable-next-line no-console
    this._consoleStdInfo = console.info;
    // eslint-disable-next-line no-console
    this._consoleStdError = console.error;
    // eslint-disable-next-line no-console
    this._consoleStdAssert = console.assert;
    
    
    // eslint-disable-next-line no-console
    console.log = this.log.bind(this);
    // eslint-disable-next-line no-console
    console.info = this.info.bind(this);
    // eslint-disable-next-line no-console
    console.error = this.error.bind(this);
    // eslint-disable-next-line no-console
    console.assert = this.assert.bind(this);
  }

  static init(
    isInfoLogEnabledFn,
    isErrorLogEnabledFn,
    isAssertLogEnabledFn,
    isExternLogEnabled,
    otherLogFn,
    userInfoFn) {

    if(!this[singleton]) {
      this[singleton] = new Logger(singletonEnforcer);
    }
    this[singleton]._infoOn = isInfoLogEnabledFn;
    this[singleton]._errorOn = isErrorLogEnabledFn;
    this[singleton]._assertOn = isAssertLogEnabledFn;
    this[singleton]._externOn = isExternLogEnabled;
    this[singleton]._otherLogFn = otherLogFn;
    this[singleton]._userInfoFn = userInfoFn;
  }

  log() {

    if(this._userInfoFn) {
      const userInfoMsg = this.userInfo(...arguments);
      if(userInfoMsg) {
        this._userInfoFn(userInfoMsg);
      }
    }

    if(this._externOn() && this.isExtern(...arguments)) {
      this._consoleStdLog(...arguments);
      this.logOther(...arguments);
    }
    else if(this._infoOn()){
      this._consoleStdLog(...arguments);
      this.logOther(...arguments);
    }
  }
  error() {
    if(!this._errorOn()) return;
    if(!this._externOn() && this.isExtern(...arguments)) return;
    
    this._consoleStdError(...arguments);
    this.logOther(...arguments);
  }
  assert() {
    if(!this._assertOn()) return;
    if(!this._externOn() && this.isExtern(...arguments)) return;

    this._consoleStdAssert(...arguments);
    this.logOther(...arguments);
  }
  info() {
    if(this._externOn() && this.isExtern(...arguments)) {
      this._consoleStdInfo(...arguments);
      this.logOther(...arguments);
    }
    else if(this._infoOn()){
      this._consoleStdInfo(...arguments);
      this.logOther(...arguments);
    }
  }

  isExtern() {
    return arguments.length > 0 && arguments[0] == 'opencv';
  }
  
  userInfo() {

    if(arguments.length == 0) return null;

    let userInfoStr = ''
    for(let i = 0; i < arguments.length; ++i) {
      if(userInfoStr) {
        userInfoStr += ' ' + arguments[i];
      }
      else if(typeof arguments[i] == 'string' && arguments[i].startsWith('user-info ')) {
        userInfoStr = arguments[i].replace('user-info ', '');
      }
    }
    return userInfoStr;
  }

  logOther() {
    if(!this._otherLogFn) return;
    let messageLine = '';
    for(const arg of arguments) {
      messageLine += arg + ' ';
    }
    this._otherLogFn(messageLine);
  }

}