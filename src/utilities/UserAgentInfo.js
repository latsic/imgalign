const singleton = Symbol();
const singletonEnforcer = Symbol();

export class UserAgentInfo {

  constructor(enforcer) {

    if(enforcer != singletonEnforcer) throw 'Cannot construct Singleton!';
  
      this._deviceOS = UserAgentInfo.deviceOS();
      this._renderingEngine = UserAgentInfo.renderingEngine();
  }

  isGeckoOnAndroid() {
    return this._deviceOS == 'android' && this._renderingEngine == 'gecko';
  }

  static get instance() {
    if(!this[singleton]) {
      this[singleton] = new UserAgentInfo(singletonEnforcer);
    }
    return this[singleton];
  }

  static deviceOS() {
    const useragent = navigator.userAgent;

    if(useragent.match(/Android/i)) {
      return 'android';
    }
    else if(useragent.match(/webOS/i)) {
      return 'webos';
    }
    else if(useragent.match(/iPhone/i)) {
      return 'iphone';
    }
    else if(useragent.match(/iPod/i)) {
      return 'ipod';
    }
    else if(useragent.match(/iPad/i)) {
      return 'ipad';
    }
    else if(useragent.match(/Windows Phone/i)) {
      return 'windows phone';
    }
    else if(useragent.match(/SymbianOS/i)) {
      return 'symbian';
    }
    else if(useragent.match(/RIM/i) || useragent.match(/BB/i)) {
      return 'blackberry';
    }
    else {
      return '';
    }
  }
  static renderingEngine() {
    const useragent = navigator.userAgent;
    if(useragent.match(/trident/i)){
      //Internet Explorer
      return 'trident';
    }
    else if(useragent.match(/webkit/i)){
      //Chrome, Safari
      return 'webkit';
    }
    // else if(useragent.search(/???/i)>0){ //replace ??? by the appropriate engine
    //   //others
    // }
    else if(useragent.match(/gecko/i)){
      //Firefox
      return 'gecko';
    }

    //Leave Gecko for the last condition since the userAgent property may
    // contain the expression "like Gecko", WebKit browsers do and IE as well.
  }

}

    