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
    isExternLogEnabled) {

    if(!this[singleton]) {
      this[singleton] = new Logger(singletonEnforcer);
    }
    this[singleton]._infoOn = isInfoLogEnabledFn;
    this[singleton]._errorOn = isErrorLogEnabledFn;
    this[singleton]._assertOn = isAssertLogEnabledFn;
    this[singleton]._externOn = isExternLogEnabled;
  }

  log() {
    if(!this._infoOn()) return;
    if(!this._externOn() && this.isExtern(...arguments)) return;

    this._consoleStdLog(...arguments);
  }
  error() {
    if(!this._errorOn()) return;
    if(!this._externOn() && this.isExtern(...arguments)) return;
    
    this._consoleStdError(...arguments);
  }
  assert() {
    if(!this._assertOn()) return;
    if(!this._externOn() && this.isExtern(...arguments)) return;

    this._consoleStdAssert(...arguments);
  }
  info() {
    if(!this._infoOn()) return;
    if(!this._externOn() && this.isExtern(...arguments)) return;

    this._consoleStdInfo(...arguments);
  }

  isExtern() {
    return arguments.length > 0 && arguments[0] == 'extern';
  }

}